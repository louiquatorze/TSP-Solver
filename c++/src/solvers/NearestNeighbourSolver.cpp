
#include "NearestNeighbourSolver.h"

#include <iostream>
#include <random>
#include <vector>

NearestNeighbourSolver::NearestNeighbourSolver(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData_out)
    : TSPSolver(environment, algorithmSettings, tsp, solutionData_out)
{ }

ExitStatus NearestNeighbourSolver::getPathLength(Environment& environment, TSP& tsp, i64& length_out) {
    length_out = 0;
    
    i32 visited[tsp.dimension];
    std::iota(visited, visited + tsp.dimension, 0);
    std::swap(visited[0], visited[0]);

    i32 visitedCount = 1;
    i32 its = 1e9 / tsp.dimension;

    while (visitedCount < tsp.dimension) {
        if (environment.interrupt)
            return ExitStatus::INTERRUPTED;

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
            length_out += minWeight;
            visitedCount++;
        }
    }

    length_out += tsp.ew(visited[0], visited[tsp.dimension - 1]);
    
    return ExitStatus::SUCCESS;
}

ExitStatus NearestNeighbourSolver::prepareCPU() {
    std::iota(solutionData_out.pathIndices, solutionData_out.pathIndices + tsp.dimension, 0);

    return ExitStatus::SUCCESS;
}

ExitStatus NearestNeighbourSolver::prepareGPU() {
    return ExitStatus::SUCCESS;
}

ExitStatus NearestNeighbourSolver::solveCPU() {
    i32* visited = solutionData_out.pathIndices;
    solutionData_out.pathLength = 0;

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
            solutionData_out.pathLength += minWeight;
            visitedCount++;
        }
    }

    solutionData_out.pathLength += tsp.ew(solutionData_out.pathIndices[0], solutionData_out.pathIndices[tsp.dimension - 1]);
    environment.progress.store(100);
    
    return ExitStatus::SUCCESS;
}

ExitStatus NearestNeighbourSolver::solveGPU() {
    return ExitStatus::NOT_SUPPORTED;
}

void NearestNeighbourSolver::print() {
    std::cout << "[C++] --- NearestNeighbourSolver ---" << std::endl;
    std::cout << "[C++] Start index: " << algorithmSettings.startIndex << std::endl;
}