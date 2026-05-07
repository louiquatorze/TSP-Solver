
#pragma once

enum ExitStatus {
    SUCCESS                 = 0,
    INTERRUPTED             = 1,
    ERROR_INVALID_INPUT     = -1,
    ERROR_MEMORY_LIMIT      = -2,
    ERROR_NOT_IMPLEMENTED   = -3,
    NOT_SUPPORTED           = -4
};