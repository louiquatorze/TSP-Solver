
#include "NearestNeighbourSolver.h"

#include <iostream>
#include <random>
#include <vector>

ExitStatus NearestNeighbourSolver::prepareCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    std::iota(solutionData.pathIndices, solutionData.pathIndices + tsp.dimension, 0);

    return ExitStatus::SUCCESS;
}

ExitStatus NearestNeighbourSolver::prepareGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    return ExitStatus::SUCCESS;
}

ExitStatus NearestNeighbourSolver::solveCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    std::cout << "[C++] Start index:" << algorithmSettings.startIndex << std::endl;

    i32* visited = solutionData.pathIndices;
    solutionData.pathLength = 0;

    environment.progress.store(0);

    // Make sure first index is at the start
    std::swap(visited[0], visited[algorithmSettings.startIndex]);

    i32 visitedCount = 1;
    i32 its = 1e7 / tsp.dimension;

    while (visitedCount < tsp.dimension) {
        if (environment.interrupt)
            return ExitStatus::INTERRUPTED;

        environment.progress.store(100 * visitedCount / tsp.dimension);

        i32 remaining = tsp.dimension - visitedCount;
        if (its > remaining)
            its = remaining;
        
        for (i32 it = 0; it < its; it++) {
            i32 minIndex = -1;
            i32 minWeight = 2147483647;
            i32 row = tsp.dimension * visited[visitedCount - 1];

            // Go through all unvisited nodes and pick the minimum
            for (i32 j = visitedCount; j < tsp.dimension; j++) {
                i32 weight = tsp.edgeWeights[row + visited[j]];
                if (weight < minWeight) {
                    minIndex = j;
                    minWeight = weight;
                } 
            }
            
            std::swap(visited[visitedCount], visited[minIndex]);
            solutionData.pathLength += minWeight;
            visitedCount++;
        }
    }

    solutionData.pathLength += tsp.ew(solutionData.pathIndices[0], solutionData.pathIndices[tsp.dimension - 1]);
    environment.progress.store(100);
    
    return ExitStatus::SUCCESS;
}

ExitStatus NearestNeighbourSolver::solveGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    return ExitStatus::NOT_SUPPORTED;
}