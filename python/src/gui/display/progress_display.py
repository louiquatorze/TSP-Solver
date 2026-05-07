
from PySide6.QtWidgets import QWidget, QLabel, QHBoxLayout, QProgressBar, QRadioButton
from PySide6.QtCore import Signal

from src.solver.work_status import WorkStatus

class ProgressDisplay(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        # Increased height slightly to 40px to comfortably fit labels/indicators, 
        # but you can keep it at 30px if the layout is tight.
        self.setFixedHeight(40) 
        
        # Main Layout (Horizontal to fit everything in a thin bar)
        self.main_layout = QHBoxLayout(self)
        self.main_layout.setContentsMargins(5, 0, 5, 0)
        self.main_layout.setSpacing(10)

        # 1. Progress field
        self.progress_label = QLabel("  0%")
        self.main_layout.addWidget(self.progress_label)

        # 2. Progress Bar
        self.progress_bar = QProgressBar()
        self.progress_bar.setRange(0, 100)
        self.progress_bar.setValue(0)
        self.progress_bar.setTextVisible(False) # Keeps it clean for a 30px height
        self.main_layout.addWidget(self.progress_bar)

        # 3. Status Buttons (Radio Buttons)
        self.status_group_layout = QHBoxLayout()
        self.status_group_layout.setSpacing(5)

        # Styling for "Green Light" effect
        # We target the indicator specifically
        green_light_style = """
            QRadioButton::indicator:disabled {
                width: 12px;
                height: 12px;
                border-radius: 6px;
                border: 1px solid gray;
                background-color: white;
            }
            QRadioButton::indicator:checked:disabled {
                background-color: #2ecc71; /* Emerald Green */
                border: 1px solid #27ae60;
            }
        """
        
        self.idle_light = QRadioButton("Idle")
        self.preprocessing_light = QRadioButton("Preprocessing")
        self.solving_light = QRadioButton("Solving")

        for light in [self.idle_light, self.preprocessing_light, self.solving_light]:
            light.setEnabled(False)
            light.setStyleSheet(green_light_style)
            self.status_group_layout.addWidget(light)
        
        self.idle_light.setChecked(True)
        self.main_layout.addLayout(self.status_group_layout)

    def set_progress(self, progress):
        self.progress_label.setText(str(progress).rjust(3) + "%")
        self.progress_bar.setValue(progress)

    def set_status(self, status):
        if status == WorkStatus.Idle:
            self.idle_light.setChecked(True)
        elif status == WorkStatus.Preprocessing:
            self.preprocessing_light.setChecked(True)
        elif status == WorkStatus.Solving:
            self.solving_light.setChecked(True)
        