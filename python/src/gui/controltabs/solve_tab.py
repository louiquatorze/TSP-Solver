
from PySide6.QtWidgets import (QCheckBox, QWidget, QVBoxLayout, QGroupBox, QComboBox, 
                             QLabel, QStackedWidget, QPushButton, QFrame, 
                             QFormLayout, QSpinBox, QHBoxLayout, QSlider)
from PySide6.QtCore import Signal
from PySide6.QtCore import Qt

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
    
    def _create_ac_params(self):
        page = QFrame()
        layout = QVBoxLayout(page)  # Vertical layout for the whole page
        layout.setSpacing(15)       # Add space between parameter blocks

        def create_slider_block(name, default=0.5, scale=100, min_val=0.0, max_val=1.0):
            # Container for the whole "block"
            block = QWidget()
            block_layout = QVBoxLayout(block)
            block_layout.setContentsMargins(0, 0, 0, 0)
            block_layout.setSpacing(4)

            # Header row: Name on left, Value on right
            header_layout = QHBoxLayout()
            name_label = QLabel(f"<b>{name}</b>")
            val_label = QLabel(f"{default:.2f}")
            header_layout.addWidget(name_label)
            header_layout.addStretch()
            header_layout.addWidget(val_label)

            # The Slider
            slider = QSlider(Qt.Orientation.Horizontal)
            slider.setRange(int(min_val * scale), int(max_val * scale))
            slider.setValue(int(default * scale))
            
            # Update value label on move
            slider.valueChanged.connect(
                lambda v: val_label.setText(f"{(v / scale):.2f}")
            )
            
            block_layout.addLayout(header_layout)
            block_layout.addWidget(slider)

            # Store references for later retrieval
            block.slider = slider
            block.scale = scale
            return block

        def create_int_block(name, value=10, maximum=1000):
            block = QWidget()
            block_layout = QVBoxLayout(block)
            block_layout.setContentsMargins(0, 0, 0, 0)
            
            label = QLabel(f"<b>{name}</b>")
            sb = QSpinBox()
            sb.setRange(1, maximum)
            sb.setValue(value)
            
            block_layout.addWidget(label)
            block_layout.addWidget(sb)
            
            block.spinbox = sb
            return block

        # 1. Integer Blocks
        self.ac_ant_count_input = create_int_block("Ant Count", 20)
        self.ac_iterations_input = create_int_block("Iterations", 100)

        # 2. Slider Blocks (Floats)
        self.ac_beta_input = create_slider_block("Beta", 2.0, scale=100, min_val=0.0, max_val=5.0)
        self.ac_pheromone_power_input = create_slider_block("Pheromone Power", 1.0, scale=100, min_val=0.0, max_val=5.0)
        self.ac_exploitation_prob_input = create_slider_block("Exploitation Probability", 0.9)
        self.ac_local_evap_input = create_slider_block("Local Evaporation Rate", 0.1)
        self.ac_global_evap_input = create_slider_block("Global Evaporation Rate", 0.1)

        widgets = [
            self.ac_ant_count_input, 
            self.ac_iterations_input, 
            self.ac_beta_input, 
            self.ac_pheromone_power_input,
            self.ac_exploitation_prob_input, 
            self.ac_local_evap_input, 
            self.ac_global_evap_input, 
        ]

        for widget in widgets:
            layout.addWidget(widget)

        layout.addStretch() # Pushes everything to the top
        return page

    def _on_solve_clicked(self):
        algorithm_settings = AlgorithmSettings()
        algorithm_settings.algorithm = AlgorithmSettings.Algorithm(self.algo_combo.currentIndex())
        algorithm_settings.gpu = self.use_gpu_checkbox.isChecked()

        if algorithm_settings.algorithm == AlgorithmSettings.Algorithm.Iterative:
            pass
        elif algorithm_settings.algorithm == AlgorithmSettings.Algorithm.NearestNeighbour:
            algorithm_settings.startIndex = int(self.nn_start_node.value())
        elif algorithm_settings.algorithm == AlgorithmSettings.Algorithm.AntColony:
            algorithm_settings.beta = int(self.ac_beta_input.slider.value()) / self.ac_beta_input.scale

            algorithm_settings.antCount = int(self.ac_ant_count_input.spinbox.value())
            algorithm_settings.iterations = int(self.ac_iterations_input.spinbox.value())

            algorithm_settings.pheromonePower = int(self.ac_pheromone_power_input.slider.value()) / self.ac_pheromone_power_input.scale
            algorithm_settings.exploitationProbability = int(self.ac_exploitation_prob_input.slider.value()) / self.ac_exploitation_prob_input.scale
            algorithm_settings.localEvaporationRate = int(self.ac_local_evap_input.slider.value()) / self.ac_local_evap_input.scale
            algorithm_settings.globalEvaporationRate = int(self.ac_global_evap_input.slider.value()) / self.ac_global_evap_input.scale

        self.solveRequested.emit(algorithm_settings)