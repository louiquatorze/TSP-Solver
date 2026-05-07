
from PySide6.QtWidgets import QWidget, QVBoxLayout, QTabWidget
from PySide6.QtCore import Signal

from src.gui.controltabs.select_tab import SelectTab
from src.gui.controltabs.create_tab import CreateTab
from src.gui.controltabs.solve_tab import SolveTab
from src.gui.metadata_section import MetadataSection

from src.solver.structs.algorithm_settings import AlgorithmSettings

class ControlPanel(QWidget):
    fileSelected = Signal(str)
    solveRequested = Signal(AlgorithmSettings)
    interruptRequested = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.setFixedWidth(250)

        main_layout = QVBoxLayout(self)
        self.tabs = QTabWidget()
        
        # Initialize Modular Tabs
        self.select_tab = SelectTab()
        self.create_tab = CreateTab()
        self.solve_tab = SolveTab()
        
        # Add to TabWidget
        self.tabs.addTab(self.select_tab, "Select")
        self.tabs.addTab(self.create_tab, "Create")
        self.tabs.addTab(self.solve_tab, "Solve")
        
        # Connect internal signals to external ControlPanel signals
        self.select_tab.fileSelected.connect(self.fileSelected.emit)
        self.solve_tab.solveRequested.connect(self.solveRequested.emit)
        self.solve_tab.interruptRequested.connect(self.interruptRequested.emit)
        
        # Metadata Section
        self.metadata = MetadataSection()

        main_layout.addWidget(self.tabs)
        main_layout.addWidget(self.metadata)

    def get_algorithm_settings(self):
        return self.solve_tab.get_algorithm_settings()
    
    def set_control_tabs_enabled(self, select, create, solve):
        self.tabs.setTabEnabled(0, select)
        self.tabs.setTabEnabled(1, create)
        self.tabs.setTabEnabled(2, solve)