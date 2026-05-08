
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

        // Heuristics
        f32 beta;

        // Nearest Neighbour
        i32 startIndex;

        // Ant Colony System
        i32 antCount;
        i32 iterations;

        f32 exploitationProbability;
        f32 localEvaporationRate;
        f32 globalEvaporationRate;
        f32 pheromonePower;
    };
}