import threading

import pyqtgraph as pg
import numpy as np

from PySide6.QtWidgets import (QCheckBox, QWidget, QVBoxLayout, QGroupBox, QComboBox, 
                               QLabel, QStackedWidget, QPushButton, QFrame, 
                               QFormLayout, QSpinBox, QHBoxLayout, QSlider, QSizePolicy)
from PySide6.QtCore import QTimer, Qt

from src.solver.analytics import Analytics

class RingBuffer:
    def __init__(self, size=1024):
        self.size = size
        self.data = np.zeros(size, dtype=np.int32)
        self.reset()
    
    def reset(self):
        self.index = 0
        self.count = 0

    def append(self, value):
        self.data[self.index] = value
        self.count = min((self.count + 1), self.size) 
        self.index = (self.index + 1) % len(self.data)

    def get_values(self):
        print("Fetching values...")
        if self.count < self.size:
            return self.data[:self.count]
        
        return np.concatenate((self.data[self.index:], self.data[:self.index]))

class AnalyticsTab(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setMinimumWidth(220)  # Safe lower bounds for horizontal dragging
        
        self.layout = QVBoxLayout(self)
        self.layout.setAlignment(Qt.AlignmentFlag.AlignTop)
        self.layout.setSpacing(15)
        self.layout.setContentsMargins(10, 10, 10, 10)

        # Init plot data
        self.PLOT_DATA_SIZE = 1024
        
        self.lock = threading.Lock()

        self.cbl_ringbuffer = RingBuffer()
        self.lib_ringbuffer = RingBuffer()

        # Apply Global Dark Mode Theme
        self.setStyleSheet("""
            QWidget {
                background-color: #1e1e1e;
                color: #d4d4d4;
                font-family: 'Segoe UI', Helvetica, Arial, sans-serif;
            }
            QCheckBox {
                spacing: 8px;
            }
            QCheckBox::indicator {
                width: 14px;
                height: 14px;
                background-color: #2d2d2d;
                border: 1px solid #3e3e3e;
                border-radius: 3px;
            }
            QCheckBox::indicator:checked {
                background-color: #007acc;
                border: 1px solid #0098ff;
            }
            QFrame#SubContainer {
                background-color: #252526;
                border: 1px solid #2d2d2d;
                border-radius: 6px;
            }
            QLabel {
                background: transparent;
            }
        """)

        # --- Section: Select analytics ---
        self._add_section("Enabled analytics")
        self.progress_btn = QCheckBox("Progress")
        self.cbl_btn = QCheckBox("Current best length")
        self.cbp_btn = QCheckBox("Current best path")
        self.lib_btn = QCheckBox("Last iteration best")

        self.progress_btn.setChecked(True)

        self.layout.addWidget(self.progress_btn)
        self.layout.addWidget(self.cbl_btn)
        self.layout.addWidget(self.cbp_btn)
        self.layout.addWidget(self.lib_btn)

        # --- Section: Analytics Containers ---
        self._add_section("Analytics")
        
        self.analytics = QWidget()
        analytics_layout = QVBoxLayout(self.analytics)
        analytics_layout.setContentsMargins(0, 0, 0, 0)
        analytics_layout.setSpacing(15)

        # Create Subsection 1: Current Best Length
        self.cbl_sub, self.cbl_val_label, self.cbl_acc_label, self.cbl_plot, self.cbl_pct_chk = \
            self._create_analytic_subsection("Current Best Length")
            
        # Create Subsection 2: Last Iteration Best
        self.lib_sub, self.lib_val_label, self.lib_acc_label, self.lib_plot, self.lib_pct_chk = \
            self._create_analytic_subsection("Last Iteration Best")

        analytics_layout.addWidget(self.cbl_sub)
        analytics_layout.addWidget(self.lib_sub)

        self.layout.addWidget(self.analytics)
        
        # This stretch spring swallows all remaining vertical space, pushing panels upward
        self.layout.addStretch()

        # --- Toggle Visibility Logic ---
        # Back to standard setVisible, which eliminates empty gaps
        set_cbl_section_visible = lambda: self.cbl_sub.setVisible(self.cbl_btn.isChecked())
        self.cbl_btn.checkStateChanged.connect(set_cbl_section_visible)

        set_lib_section_visible = lambda: self.lib_sub.setVisible(self.lib_btn.isChecked())
        self.lib_btn.checkStateChanged.connect(set_lib_section_visible)

        set_cbl_section_visible()  
        set_lib_section_visible()

        self.clear_plots()
        
        # Start refresh cycle
        self.refresh_timer = QTimer()
        self.refresh_timer.timeout.connect(self.refresh_plots)
        self.refresh_timer.start(66) # Refresh at ~15 FPS

    def _create_analytic_subsection(self, title_text):
        """Helper method to construct identical, neat data subsections with rigid vertical rules."""
        
        # Container Frame
        sub_widget = QFrame()
        sub_widget.setObjectName("SubContainer")
        sub_layout = QVBoxLayout(sub_widget)
        sub_layout.setContentsMargins(8, 8, 8, 8)
        sub_layout.setSpacing(6)

        # It can expand horizontally as wide as it wants, but its vertical height is locked.
        sub_widget.setSizePolicy(QSizePolicy.Policy.Expanding, QSizePolicy.Policy.Fixed)
        sub_widget.setFixedHeight(280) 

        # Subtitle
        title = QLabel(f"<b>{title_text}</b>")
        title.setStyleSheet("color: #569cd6; font-size: 12px;") 
        sub_layout.addWidget(title)
        
        # Stats Row (Value & Accuracy side-by-side)
        stats_widget = QWidget()
        stats_layout = QHBoxLayout(stats_widget)
        stats_layout.setContentsMargins(0, 0, 0, 0)
        
        val_label = QLabel("Value: 0")
        acc_label = QLabel("Accuracy: 0.00%")
        val_label.setStyleSheet("color: #b5cea8;") 
        acc_label.setStyleSheet("color: #b5cea8;")
        
        stats_layout.addWidget(val_label)
        stats_layout.addWidget(acc_label)
        sub_layout.addWidget(stats_widget)

        # Plot Controls Row
        control_widget = QWidget()
        control_layout = QHBoxLayout(control_widget)
        control_layout.setContentsMargins(0, 0, 0, 0)
        control_layout.setAlignment(Qt.AlignmentFlag.AlignRight)
        
        pct_chk = QCheckBox("%")
        pct_chk.setStyleSheet("font-size: 11px; color: #888888;")
        control_layout.addWidget(pct_chk)
        sub_layout.addWidget(control_widget)

        # Graph Plot
        plot_widget = pg.PlotWidget()
        plot_widget.setFixedHeight(200) # Locked graph height inside the panel

        # Create plot line
        plot_line = plot_widget.plot(pen='#b5cea8') 
        
        # Dark Theme Configuration for pyqtgraph
        plot_widget.setBackground('#1e1e1e')
        plot_widget.getAxis('left').setPen('#3e3e3e')
        plot_widget.getAxis('bottom').setPen('#3e3e3e')
        
        sub_layout.addWidget(plot_widget)

        return sub_widget, val_label, acc_label, plot_line, pct_chk

    def _add_section(self, text):
        header = QLabel(text.upper())
        header.setStyleSheet("color: #888888; font-size: 10px; font-weight: bold; margin-top: 10px; letter-spacing: 1px;")
        self.layout.addWidget(header)
    
    def set_buttons_enabled(self, enabled):
        with self.lock:
            self.progress_btn.setEnabled(enabled)
            self.cbl_btn.setEnabled(enabled)
            self.cbp_btn.setEnabled(enabled)
            self.lib_btn.setEnabled(enabled)

    def get_analytics_flags(self):
        with self.lock:
            flags = 0

            flags |= Analytics.Progress.value          if self.progress_btn.isChecked() else 0
            flags |= Analytics.CurrentBestLength.value if self.cbl_btn.isChecked()      else 0
            flags |= Analytics.CurrentBestPath.value   if self.cbp_btn.isChecked()      else 0
            flags |= Analytics.LastIterationBest.value if self.lib_btn.isChecked()      else 0

        return flags
    
    def refresh_plots(self):
        with self.lock:
            if self.cbl_btn.isChecked():
                self.cbl_plot.setData(self.cbl_ringbuffer.get_values())
                
            if self.lib_btn.isChecked():
                self.lib_plot.setData(self.lib_ringbuffer.get_values())

    def update_cbl_plot(self, value):
        with self.lock:
            self.cbl_ringbuffer.append(value)

    def update_lib_plot(self, value):
        with self.lock:
            self.lib_ringbuffer.append(value)

    def clear_plots(self):
        with self.lock:
            self.cbl_ringbuffer.reset()
            self.lib_ringbuffer.reset()
            
            self.cbl_plot.setData(np.array([], dtype=np.int32))
            self.lib_plot.setData(np.array([], dtype=np.int32))

