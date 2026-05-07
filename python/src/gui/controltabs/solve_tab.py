
from PySide6.QtWidgets import (QCheckBox, QWidget, QVBoxLayout, QGroupBox, QComboBox, 
                             QLabel, QStackedWidget, QPushButton, QFrame, 
                             QFormLayout, QSpinBox, QDoubleSpinBox)
from PySide6.QtCore import Signal

from src.solver.structs.algorithm_settings import AlgorithmSettings

class SolveTab(QWidget):
    solveRequested = Signal(AlgorithmSettings)
    interruptRequested = Signal()

    def __init__(self, parent=None):
        super().__init__(parent)
        layout = QVBoxLayout(self)
        
        # 1. Algorithm settings
        algo_group = QGroupBox("Optimization Strategy")
        algo_layout = QVBoxLayout(algo_group)

        # 1.1. Algorith selection
        self.algo_combo = QComboBox()

        # Must align with SolveParams.Algorithm !
        self.algo_combo.addItem("Iterative", AlgorithmSettings.Algorithm.Iterative)
        self.algo_combo.addItem("Nearest Neighbor", AlgorithmSettings.Algorithm.NearestNeighbour)
        self.algo_combo.addItem("Ant Colony Optimization", AlgorithmSettings.Algorithm.AntColony)

        self.algo_combo.currentIndexChanged.connect(lambda idx: self.param_stack.setCurrentIndex(idx))
        
        algo_layout.addWidget(QLabel("Select Algorithm:"))
        algo_layout.addWidget(self.algo_combo)

        # 1.2. GPU acceleration
        self.use_gpu_checkbox = QCheckBox("GPU")
        algo_layout.addWidget(self.use_gpu_checkbox)

        # 2. Dynamic Parameter Section
        self.param_group = QGroupBox("Algorithm Parameters")
        param_layout = QVBoxLayout(self.param_group)
        self.param_stack = QStackedWidget()

        layout.addWidget(algo_group)
        
        # Pages (Keep references to access values later)
        self.it_page = self._create_it_params()
        self.nn_page = self._create_nn_params()
        self.ac_page = self._create_ac_params()

        self.param_stack.addWidget(self.it_page)
        self.param_stack.addWidget(self.nn_page)
        self.param_stack.addWidget(self.ac_page)
        
        param_layout.addWidget(self.param_stack)
        layout.addWidget(self.param_group)
        layout.addStretch()

        self.btn_solve = QPushButton("Solve")
        self.btn_solve.clicked.connect(self._on_solve_clicked)

        self.btn_interrupt = QPushButton("Interrupt")
        self.btn_interrupt.clicked.connect(self.interruptRequested.emit)

        layout.addWidget(self.btn_solve)
        layout.addWidget(self.btn_interrupt)

    # Params for iterative algorithm
    def _create_it_params(self):
        page = QFrame()
        layout = QFormLayout(page)

        return page
    
    # Params for next neighbour algorithm
    def _create_nn_params(self):
        page = QFrame()
        layout = QFormLayout(page)
        self.nn_start_node = QSpinBox()
        layout.addRow("Start Node:", self.nn_start_node)

        return page
    
    # Params for ant colony optimization algorithm
    def _create_ac_params(self):
        page = QFrame()
        layout = QFormLayout(page)

        return page

    def _on_solve_clicked(self):
        algorithm_settings = AlgorithmSettings()
        algorithm_settings.algorithm = AlgorithmSettings.Algorithm(self.algo_combo.currentIndex())
        algorithm_settings.gpu = self.use_gpu_checkbox.isChecked()

        if algorithm_settings.algorithm == AlgorithmSettings.Algorithm.Iterative:
            pass
        elif algorithm_settings.algorithm == AlgorithmSettings.Algorithm.NearestNeighbour:
            algorithm_settings.startIndex = self.nn_start_node.value()
        elif algorithm_settings.algorithm == AlgorithmSettings.Algorithm.AntColony:
            algorithm_settings.beta = 1.0
            pass

        self.solveRequested.emit(algorithm_settings)