
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

        ("startIndex", ctypes.c_int32),

        ("beta", ctypes.c_float)
    ]
