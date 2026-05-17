
#pragma once

#include "types.h"

extern "C" {
    struct SolutionData {
        i32 dimension;

        // Path, not cycle: return to node 0 is implicit
        i32* pathIndices;
        i64 pathLength = -1; // Doesn't have to be assigned: if -1, must be calculated by caller

        i64 preprocessingTime_ns;
        i64 preparationTime_ns;
        i64 solutionTime_ns;
    };
}