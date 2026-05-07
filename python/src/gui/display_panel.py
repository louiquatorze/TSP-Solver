
from PySide6.QtWidgets import QWidget, QVBoxLayout

from src.gui.display.tsp_display import TSPDisplay
from src.gui.display.progress_display import ProgressDisplay

class DisplayPanel(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)

        layout = QVBoxLayout(self)
        self.progress_display = ProgressDisplay()
        self.tsp_display = TSPDisplay()
        
        layout.addWidget(self.progress_display)
        layout.addWidget(self.tsp_display)

    def set_status(self, status):
        self.progress_display.set_status(status)
    
    def set_progress(self, progress):
        self.progress_display.set_progress(progress)
    
    def set_range(self, range):
        if range is None:
            return
        
        x_range, y_range = range
        self.tsp_display.setRange(xRange=x_range, yRange=y_range, padding=0.08)

    def set_cities(self, cities):
        self.tsp_display.set_cities(cities)

    def set_path(self, path):
        self.tsp_display.set_path(path)

    def set_opt_path(self, path):
        self.tsp_display.set_opt_path(path)