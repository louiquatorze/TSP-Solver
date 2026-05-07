
#pragma once

#include "TSP.h"

#include <iostream>

struct SetEdgeWeight {
    TSP& tsp;

    SetEdgeWeight(TSP& tsp) : tsp(tsp) { }

    inline void operator()(i32 value, i32 i) {
        tsp.edgeWeights[i] = value;
    }
    
    inline void operator()(i32 edgeWeight, i32 i, i32 j) {
        tsp.ew(i, j)= edgeWeight;
        tsp.ew(j, i)= edgeWeight;
    }
};

struct SetHeuristic {
    TSP& tsp;
    f32 beta;

    SetHeuristic(TSP& tsp, f32 beta) : tsp(tsp), beta(beta) { }

    inline void operator()(i32 value, i32 i) {
        tsp.heuristics[i] = value;
    }
    
    inline void operator()(i32 edgeWeight, i32 i, i32 j) {
        f32 heuristic = 1.0f / std::pow(edgeWeight, beta);
        tsp.he(i, j) = heuristic;
        tsp.he(j, i) = heuristic;
    }
};

struct SetBoth {
    TSP& tsp;
    f32 beta;

    SetBoth(TSP& tsp, f32 beta) : tsp(tsp), beta(beta) { }

    inline void operator()(i32 value, i32 i) {
        tsp.edgeWeights[i] = value;
        tsp.heuristics[i] = value;
    }

    inline void operator()(i32 edgeWeight, i32 i, i32 j) {
        f32 heuristic = 1.0f / std::pow(edgeWeight, beta);

        std::cout << edgeWeight << " " << beta << " " << heuristic << std::endl;

        i32 i1 = i * tsp.dimension + j;
        i32 i2 = j * tsp.dimension + i;

        tsp.edgeWeights[i1] = edgeWeight;
        tsp.edgeWeights[i2] = edgeWeight;

        tsp.heuristics[i1] = heuristic;
        tsp.heuristics[i2] = heuristic;
    }
};