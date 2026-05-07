
#pragma once

#include "types.h"

extern "C" {
    struct TSP {
        i32 dimension;

        i32* edgeWeights;
        f32* heuristics;

        inline i32& ew(i32 i, i32 j) {
            return edgeWeights[j * dimension + i];
        }

        inline f32& he(i32 i, i32 j) {
            return heuristics[j * dimension + i];
        }
    };
}