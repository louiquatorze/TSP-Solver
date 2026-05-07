
import ctypes
from ctypes import Structure

class SolutionData(Structure):
    _fields_ = [
        ("dimension", ctypes.c_int32),
        ("pathIndices", ctypes.POINTER(ctypes.c_int32)),
        ("pathLength", ctypes.c_int64),

        ("preprocessingTime_ns", ctypes.c_int64),
        ("preparationTime_ns", ctypes.c_int64),
        ("solutionTime_ns", ctypes.c_int64)
    ]
