
from PySide6.QtWidgets import QWidget, QVBoxLayout, QTabWidget
from PySide6.QtCore import Signal

from src.gui.solutiontabs.solution_tab import SolutionTab
from src.gui.solutiontabs.analytics_tab import AnalyticsTab

class SolutionPanel(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.setMinimumWidth(180)

        main_layout = QVBoxLayout(self)
        self.tabs = QTabWidget()
        
        # Initialize Modular Tabs
        self.solution_tab = SolutionTab()
        self.analytics_tab = AnalyticsTab()
        
        # Add to TabWidget
        self.tabs.addTab(self.solution_tab, "Solution")
        self.tabs.addTab(self.analytics_tab, "Analytics")

        main_layout.addWidget(self.tabs)
    
    def set_analytics_buttons_enabled(self, enabled):
        self.analytics_tab.set_buttons_enabled(enabled)
    
    def set_exit_status(self, exit_status):
        self.solution_tab.set_exit_status(exit_status)

    def set_solution_data(self, solution_data):
        self.solution_tab.set_solution_data(solution_data)

    def set_optimal_path_length(self, opt_path_length):
        self.solution_tab.set_optimal_path_length(opt_path_length)

    def set_accuracy(self, accuracy):
        self.solution_tab.set_accuracy(accuracy)

    def clear_all(self):
        self.solution_tab.clear_all()
        
    def get_analytics_flags(self):
        return self.analytics_tab.get_analytics_flags()