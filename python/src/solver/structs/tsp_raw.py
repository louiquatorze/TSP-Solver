
import ctypes
from ctypes import Structure

class TSPRaw(ctypes.Structure):
    _fields_ = [
        ("dimension", ctypes.c_int32),
        ("edgeWeightType", ctypes.c_int32),
        ("edgeWeightFormat", ctypes.c_int32),
        ("nodeCoordType", ctypes.c_int32),
        ("edgeWeights", ctypes.POINTER(ctypes.c_int32)),
        ("nodeCoords", ctypes.POINTER(ctypes.c_float))
    ]