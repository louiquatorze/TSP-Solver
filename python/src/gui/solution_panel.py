
from PySide6.QtWidgets import QLabel, QWidget, QVBoxLayout
from pyqtgraph.Qt.QtCore import Qt

from src.solver.exit_status import ExitStatus
from src.solver.structs.solution_data import SolutionData

class SolutionPanel(QWidget):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setFixedWidth(250)
        
        self.layout = QVBoxLayout(self)
        self.layout.setAlignment(Qt.AlignmentFlag.AlignTop)
        self.layout.setSpacing(15)

        title = QLabel("Solution")
        title.setStyleSheet("font-weight: bold; font-size: 16px; color: white;")
        self.layout.addWidget(title)

        # --- Section: Execution Summary ---
        self._add_section("Execution Summary")
        self.exit_status_label = self._add_data_field("Exit Status:", "")
        
        # New Granular Timing Fields
        self.prep_time_label = self._add_data_field("Preprocessing Time:", "")
        self.ready_time_label = self._add_data_field("Preparation Time:", "")
        self.solve_time_label = self._add_data_field("Solution Time:", "")
        self.total_time_label = self._add_data_field("Total Time:", "")

        # --- Section: Final Path Results ---
        self._add_section("Final Metrics")
        self.final_len_label = self._add_data_field("Final Length:", "")
        self.optimal_len_label = self._add_data_field("Optimal Length:", "")
        self.accuracy_label = self._add_data_field("Accuracy:", "") 

        self.final_len_label.setStyleSheet("color: #00d4ff; font-weight: bold; font-size: 14px;")
        self.total_time_label.setStyleSheet("color: #f1c40f; font-weight: bold;") # Highlight total
        
        self.clear_all()

    def _add_section(self, text):
        header = QLabel(text.upper())
        header.setStyleSheet("color: #666; font-size: 10px; font-weight: bold; margin-top: 5px;")
        self.layout.addWidget(header)

    def _add_data_field(self, label_text, initial_value):
        container = QVBoxLayout()
        container.setSpacing(2)
        
        lbl = QLabel(label_text)
        lbl.setStyleSheet("color: #aaa; font-size: 11px;")
        
        val = QLabel(initial_value)
        val.setStyleSheet("font-family: 'Consolas'; font-size: 13px; color: #eee;")
        
        container.addWidget(lbl)
        container.addWidget(val)
        self.layout.addLayout(container)
        return val
    
    def set_exit_status(self, exit_status):
        self.exit_status_label.setText(exit_status.name)

        if exit_status == ExitStatus.SUCCESS:
            self.exit_status_label.setStyleSheet("color: #00ff88")
        elif exit_status == ExitStatus.INTERRUPTED:
            self.exit_status_label.setStyleSheet("color: #f1c40f")
        else:
            self.exit_status_label.setStyleSheet("color: #e74c3c")


    def set_solution_data(self, solution_data):
        """Processes the SolutionData struct from C++ DLL."""
        # Convert ns to s for all metrics
        p_time = solution_data.preprocessingTime_ns / 1e9
        r_time = solution_data.preparationTime_ns / 1e9
        s_time = solution_data.solutionTime_ns / 1e9
        total = p_time + r_time + s_time

        self.prep_time_label.setText(f"{p_time:.4f}s")
        self.ready_time_label.setText(f"{r_time:.4f}s")
        self.solve_time_label.setText(f"{s_time:.4f}s")
        self.total_time_label.setText(f"{total:.4f}s")

        self.final_len_label.setText(f"{solution_data.pathLength}")
        
        # Logic for accuracy if optimal exists
        if hasattr(solution_data, 'optimalLength') and solution_data.optimalLength > 0:
            acc = (solution_data.optimalLength / solution_data.pathLength) * 100
            self.accuracy_label.setText(f"{acc:.2f}%")

    def clear_all(self):
        self.exit_status_label.setText("-")
        self.prep_time_label.setText("0.000s")
        self.ready_time_label.setText("0.000s")
        self.solve_time_label.setText("0.000s")
        self.total_time_label.setText("0.000s")
        self.final_len_label.setText("-")
        self.optimal_len_label.setText("-")
        self.accuracy_label.setText("0.0%")
