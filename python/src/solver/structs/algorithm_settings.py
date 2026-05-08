
import ctypes

from ctypes import Structure
from enum import IntEnum

class AlgorithmSettings(Structure):
    class Algorithm(IntEnum):
        Iterative = 0
        NearestNeighbour = 1
        AntColony = 2

    _fields_ = [
        ("algorithm", ctypes.c_int32),
        ("gpu", ctypes.c_bool),
        
        # Heuristics
        ("beta", ctypes.c_float),

        # NearestNeighbour
        ("startIndex", ctypes.c_int32),

        # AntColony
        ("antCount", ctypes.c_int32),
        ("iterations", ctypes.c_int32),

        ("pheromonePower", ctypes.c_float),
        ("exploitationProbability", ctypes.c_float),
        ("localEvaporationRate", ctypes.c_float),
        ("globalEvaporationRate", ctypes.c_float),
    ]
