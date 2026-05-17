
import numpy as np
import ctypes

from PySide6.QtCore import QObject, Signal, Slot, QMetaObject, Qt, Q_ARG

from definitions import PROJECT_ROOT_DIR
from src.solver.structs.solution_data import SolutionData
from src.solver.structs.tsp import TSP
from src.solver.structs.tsp_raw import TSPRaw
from src.solver.exit_status import ExitStatus
from src.solver.work_status import WorkStatus
from src.solver.structs.algorithm_settings import AlgorithmSettings
from src.solver.tsp_cache import TSPCache

class SolverWorker(QObject):
    finished = Signal(SolutionData, ExitStatus)
    status = Signal(WorkStatus)
    
    def __init__(self, lib, context):
        super().__init__()

        self.lib = lib
        self.context = context

        self.tsp_cache = TSPCache()

    @Slot(dict)
    def solve(self, settings):
        algorithm_settings = settings["algorithm_settings"]
        tsp_parsed = settings["tsp_parsed"]

        # Create solution data output buffer

        solution_data_buffer = SolutionData()

        path_indices = (ctypes.c_int32 * tsp_parsed.dimension)()
        solution_data_buffer.pathIndices = path_indices
        solution_data_buffer._keep_alive = path_indices

        ##################
        ### Preprocess ###
        ##################
        
        self.status.emit(WorkStatus.Preprocessing)

        algorithm = AlgorithmSettings.Algorithm(algorithm_settings.algorithm)
        
        if self.tsp_cache.is_cached(tsp_parsed.id, algorithm_settings.beta, algorithm):
            tsp_buffer = self.tsp_cache.tsp

        else:
            tsp_raw_buffer = TSPRaw()

            edge_weight_type = tsp_parsed.edge_weight_type if tsp_parsed.edge_weight_type else 0
            edge_weight_format = tsp_parsed.edge_weight_format if tsp_parsed.edge_weight_format else 0
            node_coord_type = tsp_parsed.node_coord_type if tsp_parsed.node_coord_type else 0

            edge_weights_ref = (ctypes.c_uint32 * len(tsp_parsed.edge_weights))(*tsp_parsed.edge_weights) if tsp_parsed.edge_weights else None
            node_coords_ref = (ctypes.c_float * len(tsp_parsed.node_coords))(*tsp_parsed.node_coords) if tsp_parsed.node_coords else None
            
            tsp_raw_buffer.dimension = tsp_parsed.dimension
            tsp_raw_buffer.edgeWeightType = ctypes.c_int32(edge_weight_type)
            tsp_raw_buffer.edgeWeightFormat = ctypes.c_int32(edge_weight_format)
            tsp_raw_buffer.nodeCoordType = ctypes.c_int32(node_coord_type)
            tsp_raw_buffer.edgeWeights = ctypes.cast(edge_weights_ref, ctypes.POINTER(ctypes.c_uint32))
            tsp_raw_buffer.nodeCoords = ctypes.cast(node_coords_ref, ctypes.POINTER(ctypes.c_float))

            # Create TSP output buffer

            tsp_buffer = TSP()
            algorithm = AlgorithmSettings.Algorithm(algorithm_settings.algorithm)

            tsp_buffer.dimension = tsp_parsed.dimension
            tsp_buffer.edgeWeights = None
            tsp_buffer.heuristics = None

            data_count = tsp_raw_buffer.dimension * tsp_raw_buffer.dimension

            if algorithm in TSP.has_edge_weights:
                tsp_buffer.edge_weights_np = np.empty(data_count, dtype=np.uint32)
                tsp_buffer.edgeWeights = tsp_buffer.edge_weights_np.ctypes.data_as(ctypes.POINTER(ctypes.c_uint32))

            if algorithm in TSP.has_heuristics:
                tsp_buffer.heuristics_np = np.empty(data_count, dtype=np.float32)
                tsp_buffer.heuristics = tsp_buffer.heuristics_np.ctypes.data_as(ctypes.POINTER(ctypes.c_float))

            # Turn raw tsp data to usable tsp data
            exit_status = self.lib.computeTSPData(
                self.context, 
                ctypes.byref(algorithm_settings), 
                ctypes.byref(tsp_raw_buffer), 
                ctypes.byref(tsp_buffer),
                ctypes.byref(solution_data_buffer)
            )
            exit_status = ExitStatus(exit_status)

            if exit_status != ExitStatus.SUCCESS:
                print(f"[SolverWorker] Compute TSP data failed: { exit_status }") 

                self.finished.emit(None, exit_status)
                self.status.emit(WorkStatus.Idle)
                return
            
            self.tsp_cache.set(tsp_buffer, tsp_parsed.id, algorithm_settings.beta, algorithm)  

        #################
        ### Solve TSP ###
        #################
        
        self.status.emit(WorkStatus.Solving)

        exit_status = self.lib.solveTSP(
            self.context, 
            ctypes.byref(algorithm_settings), 
            ctypes.byref(tsp_buffer),       
            ctypes.byref(solution_data_buffer)
        )        
        exit_status = ExitStatus(exit_status)

        if exit_status != ExitStatus.SUCCESS:
            print(f"[SolverWorker] Solve TSP failed: { exit_status }") 

            self.finished.emit(None, exit_status)
            self.status.emit(WorkStatus.Idle)
            return
        
        self.finished.emit(solution_data_buffer, exit_status)
        self.status.emit(WorkStatus.Idle)