
import ctypes
from ctypes import Structure

class TSP(Structure):
    _fields_ = [
        ("dimension", ctypes.c_uint32),
        ("edgeWeights", ctypes.POINTER(ctypes.c_int32)),
        ("heuristics", ctypes.POINTER(ctypes.c_float))
    ]
