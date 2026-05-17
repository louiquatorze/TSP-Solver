
import ctypes

from ctypes import Structure, Array
from src.solver.structs.algorithm_settings import AlgorithmSettings

Algo = AlgorithmSettings.Algorithm

class TSP(Structure):
    has_edge_weights = {
        Algo.Iterative,
        Algo.NearestNeighbour,
        Algo.AntColony,
        Algo.Christofides
    }

    has_heuristics = {
        # Algo.Iterative,
        # Algo.NearestNeighbour,
        Algo.AntColony,
        # Algo.Christofides
    }

    requires_beta = {
        # Algo.Iterative,
        # Algo.NearestNeighbour,
        Algo.AntColony,
        # Algo.Christofides
    }

    edge_weights_np: Array
    heuristics_np: Array

    _fields_ = [
        ("dimension", ctypes.c_uint32),
        ("edgeWeights", ctypes.POINTER(ctypes.c_uint32)),
        ("heuristics", ctypes.POINTER(ctypes.c_float))
    ]
