
import ctypes
import os
import random

from definitions import PROJECT_ROOT_DIR
from PySide6.QtCore import QThread, QObject, Signal, QTimer

from src.solver.solver_worker import SolverWorker
from src.solver.structs.algorithm_settings import AlgorithmSettings
from src.solver.structs.tsp import TSP
from src.solver.structs.tsp_raw import TSPRaw
from src.solver.structs.solution_data import SolutionData
from src.solver.work_status import WorkStatus
from src.solver.exit_status import ExitStatus
from src.solver.analytics import Analytics

class SolverHandler(QObject):
    solve = Signal(dict)
    solved = Signal(SolutionData, ExitStatus)
    status = Signal(WorkStatus)

    poll_state = Signal()

    progress = Signal(int)
    cbl_val = Signal(int)
    lib_val = Signal(int)
    cbp_ind = Signal([int])
    
    def __init__(self):
        super().__init__()

        self.solving = False
        self.active_poll_tasks = []

        self._init_lib()
        self._init_context()
        self._init_solver_worker_thread()

        self.poll_state.connect(self._on_poll_state)

    def _init_lib(self):
        lib_path = os.path.join(PROJECT_ROOT_DIR, "lib/libTSPSolver.so")

        if not os.path.exists(lib_path):
            raise RuntimeError(f"No DLL found at { lib_path }")

        self.lib = ctypes.CDLL(lib_path)

        self.lib.createContext.argtypes = []
        self.lib.createContext.restype = ctypes.c_void_p

        self.lib.destroyContext.argtypes = [ctypes.c_void_p]
        self.lib.destroyContext.restype = None

        self.lib.setInterrupt.argtypes = [ctypes.c_void_p, ctypes.c_bool]
        self.lib.setInterrupt.restype = None

        self.lib.getProgress.argtypes = [ctypes.c_void_p]
        self.lib.getProgress.restype = ctypes.c_int32

        self.lib.computeTSPData.argtypes = [ctypes.c_void_p, ctypes.POINTER(AlgorithmSettings), ctypes.POINTER(TSPRaw), ctypes.POINTER(TSP), ctypes.POINTER(SolutionData)]
        self.lib.computeTSPData.restype = ctypes.c_int32

        self.lib.solveTSP.argtypes = [ctypes.c_void_p, ctypes.POINTER(AlgorithmSettings), ctypes.POINTER(TSP), ctypes.POINTER(SolutionData)]
        self.lib.solveTSP.restype = ctypes.c_int32

    def _init_context(self):
        self.context = self.lib.createContext()

    def _init_solver_worker_thread(self):
        self.solver_worker = SolverWorker(self.lib, self.context)
        self.solver_thread = QThread()
        self.solver_worker.moveToThread(self.solver_thread)
        self.solver_thread.start()
        
        self.solve.connect(self.solver_worker.solve)
        self.solver_worker.status.connect(self.status.emit)
        self.solver_worker.finished.connect(self.on_worker_finished)

    def handle_solve(self, settings):
        self.settings = settings

        if self.solving:
            self.interrupt()
            self.solve_queued = True
            return
        
        self._execute_solve()

    def _execute_solve(self):
        self.lib.setInterrupt(self.context, ctypes.c_bool(False))

        self.progress.emit(0)

        self.solving = True
        self.solve_queued = False
        self.solve.emit(self.settings)

        self.update_active_poll_tasks(self.settings["analytics_flags"])
        self.poll_state.emit()

    def update_active_poll_tasks(self, analytics_flags):
        self.active_poll_tasks = []

        if analytics_flags & Analytics.Progress.value:
            self.active_poll_tasks.append(self._poll_progress)
        if analytics_flags & Analytics.CurrentBestLength.value:
            self.active_poll_tasks.append(self._poll_cbl)
        if analytics_flags & Analytics.CurrentBestPath.value:
            self.active_poll_tasks.append(self._poll_cbp)
        if analytics_flags & Analytics.LastIterationBest.value:
            self.active_poll_tasks.append(self._poll_lib)

    def _on_poll_state(self):
        if not self.solving:
            return
        
        for task in self.active_poll_tasks:
            task()
        
        QTimer.singleShot(80, self._on_poll_state)

    def _poll_progress(self):
        self.progress.emit(int(self.lib.getProgress(self.context)))

    def _poll_cbl(self):
        self.cbl_val.emit(12)
        return
    
        cbl_value = int(self.lib.getCBL(self.context));

        if cbl_value >= 0:
            self.cbl_val.emit(cbl_value)

    def _poll_lib(self):
        self.lib_val.emit(5)
        return

        lib_value = int(self.lib.getLIB(self.context));

        if lib_value >= 0:
            self.lib_val.emit(lib_value)

    def _poll_cbp(self):
        # TODO
        pass

    def interrupt(self):
        self.lib.setInterrupt(self.context, ctypes.c_bool(True))

    def on_worker_finished(self, solution_data, exit_status):
        self.solving = False
        self.solved.emit(solution_data, exit_status)

        if solution_data is not None:
            self.progress.emit(100)
        else:
            self.progress.emit(0)
        
        if self.solve_queued:
            self._execute_solve()

    def cleanup(self):
        def cleanup_solver_thread():
            if not self.solver_thread or not self.solver_thread.isRunning():
                return
            
            try:
                self.solver_worker.finished.disconnect(self._on_worker_finished)
            except Exception:
                pass

            self.interrupt()
            self.solver_thread.quit()

            if not self.solver_thread.wait(3000):
                print("Thread is stuck, terminating.")
                self.solver_thread.terminate()
                self.solver_thread.wait()

            self.solver_thread = None

        def cleanup_solver_worker():
            if not  self.solver_worker:
                self.solver_worker.deleteLater()
                self.solver_worker = None
            
        def cleanup_context():
            if not self.context:
                return
            
            self.lib.destroyContext(self.context)
            self.context = None
        
        print("Cleaning up handler")

        cleanup_solver_thread()
        cleanup_solver_worker()
        cleanup_context()



        

    