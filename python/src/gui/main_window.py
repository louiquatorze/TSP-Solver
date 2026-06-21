import sys
import numpy as np

from PySide6.QtWidgets import QSplitter, QMainWindow, QWidget, QHBoxLayout
from PySide6.QtCore import Qt

from src.gui.control_panel import ControlPanel
from src.gui.display_panel import DisplayPanel
from src.gui.solution_panel import SolutionPanel

class MainWindow(QMainWindow):
    def __init__(self, title="Main window"):
        super().__init__()
        self.setWindowTitle(title)
        self.resize(1300, 800)
        
        # 1. Instantiate modules
        self.controls = ControlPanel()
        self.display = DisplayPanel()
        self.solution = SolutionPanel()

        # 2. Layout
        splitter = QSplitter(Qt.Orientation.Horizontal)
        layout = QHBoxLayout(splitter)

        layout.addWidget(self.controls)
        layout.addWidget(self.display)
        layout.addWidget(self.solution)

        splitter.setSizes([250, 800, 250])
        splitter.setLayout(layout)
        self.setCentralWidget(splitter)
        
        # Set style
        self.setStyleSheet("""
            /* The main container background */
            QMainWindow, QWidget#ControlPanel {
                background-color: #141414;
            }

            /* Make the File List and Inputs stand out */
            QListView, QTreeView, QLineEdit, QSpinBox, QComboBox {
                background-color: #2d2d2d;
                border: 1px solid #3f3f3f;
                border-radius: 2px;
                color: #efefef;
                padding: 4px;
            }

            /* Brighter Tab Bar to distinguish selection area */
            QTabWidget::pane { 
                border: 1px solid #3d3d3d; 
                background-color: #242424; /* Subtle middle ground */
            }
            
            QTabBar::tab {
                background: #1a1a1a;
                color: #888;
                padding: 10px 15px;
                border: 1px solid #2d2d2d;
            }

            QTabBar::tab:selected {
                background: #2d2d2d; /* Matches the brighter file list */
                color: white;
                border-bottom: 2px solid #3498db;
            }

            /* Buttons sit on the dark background, so make them distinct */
            QPushButton {
                background-color: #3d3d3d;
                border: 1px solid #555;
                color: white;
                font-weight: bold;
            }

            QPushButton:hover {
                background-color: #4d4d4d;
            }
        """)

    def clear_solution_panel(self):
        self.solution.clear_all()

    def set_solution_data(self, solution_data):
        self.solution.set_solution_data(solution_data)

    def set_progress(self, progress):
        self.display.set_progress(progress)

    def set_status(self, status):
        self.display.set_status(status)

    def set_control_tabs_enabled(self, select=None, create=None, solve=None):
        self.controls.set_control_tabs_enabled(select, create, solve)

    def set_analytics_buttons_enabled(self, enabled):
        self.solution.set_analytics_buttons_enabled(enabled)

    def set_range(self, range):
        self.display.set_range(range)

    def set_metadata(self, name, dimension, has_opt, comment):
        self.controls.metadata.set_data(name, dimension, has_opt, comment)
    
    def set_cities(self, cities):
        self.display.set_cities(cities)

    def set_path(self, path):
        self.display.set_path(path)

    def set_opt_path(self, opt_path):
        self.display.set_opt_path(opt_path)

    def set_exit_status(self, exit_status):
        self.solution.set_exit_status(exit_status)
        
    def get_analytics_flags(self):
        return self.solution.get_analytics_flags()
    
    def update_cbl_plot(self, cbl):
        self.solution.update_cbl_plot(cbl)

    def update_lib_plot(self, lib):
        self.solution.update_lib_plot(lib)

    def clear_analytic_plots(self):
        self.solution.clear_analytic_plots()