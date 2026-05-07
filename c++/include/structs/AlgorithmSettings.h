
#pragma once

#include "types.h"

extern "C" {
    struct AlgorithmSettings {
        enum Algorithm : i32 {
            Iterative = 0,
            NearestNeighbour,
            AntColony
        };

        Algorithm algorithm;
        bool gpu;

        // Nearest Neighbour
        i32 startIndex;

        // Ant Colony System
        f32 beta;
    };
}