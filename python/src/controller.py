
import numpy as np

from PySide6.QtCore import QObject

from src.gui.main_window import MainWindow
from src.solver.solver_handler import SolverHandler
from src.tsp_parser import TSPParser
from src.solver.work_status import WorkStatus

class Controller(QObject):
    def __init__(self):
        super().__init__()

        self.tsp_parsed = None

        self.view = MainWindow(title="TSP Solver")
        self.view.show()

        self.handler = SolverHandler()
        self._setup_connections()

    def _setup_connections(self):
        self.view.controls.fileSelected.connect(self.on_file_picked)
        self.view.controls.solveRequested.connect(self.solve)
        self.view.controls.interruptRequested.connect(self.handler.interrupt)
        
        self.handler.status.connect(self.view.set_status)
        self.handler.progress.connect(self.view.set_progress)
        self.handler.solved.connect(self.on_solved)

    def solve(self, algorithm_settings):
        if self.tsp_parsed is None:
            return
        
        data = {
            "algorithm_settings" : algorithm_settings,
            "tsp_parsed" : self.tsp_parsed
        }

        self.view.set_path(None)
        self.view.set_control_tabs_enabled(False, False, True)
        self.handler.handle_solve(data)

    def on_solved(self, solution_data, exit_status):
        self.view.set_control_tabs_enabled(True, True, True)
        self.view.set_exit_status(exit_status)

        if solution_data is None:
            return
        
        self.view.set_solution_data(solution_data)
        
        path_indices = np.ctypeslib.as_array(solution_data.pathIndices, shape=(solution_data.dimension,))
        display_data = self.tsp_parsed.get_display_data()

        path = np.ndarray((solution_data.dimension + 1, 2), dtype=np.float32)
        path[0 : solution_data.dimension] = display_data[path_indices]
        path[solution_data.dimension] = path[0]

        self.view.set_path(path)

    def on_file_picked(self, path):
        self.tsp_parsed = TSPParser.parse(path)

        display_data = self.tsp_parsed.get_display_data()
        range = self.tsp_parsed.get_display_range(display_data)
        opt_path = self.tsp_parsed.get_opt_path(display_data)

        self.view.set_progress(0)
        self.view.set_status(WorkStatus.Idle)
        self.view.clear_solution_panel()

        self.view.set_cities(display_data)
        self.view.set_path(None)
        self.view.set_metadata(
            self.tsp_parsed.name,
            self.tsp_parsed.dimension,
            opt_path is not None,
            self.tsp_parsed.comment
        )
        self.view.set_range(range)
        self.view.set_opt_path(opt_path)

    def cleanup(self):
        self.handler.cleanup()