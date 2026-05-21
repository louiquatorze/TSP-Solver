
from enum import Enum

class ExitStatus(Enum):
    SUCCESS                 = 0
    INTERRUPTED             = 1
    ERROR_INVALID_INPUT     = -1
    ERROR_INVALID_HANDLE    = -2
    ERROR_MEMORY_LIMIT      = -10
    ERROR_NOT_IMPLEMENTED   = -40
    ERROR_NOT_SUPPORTED     = -100
