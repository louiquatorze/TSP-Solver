
#include "AntColonySolver.h"

#include <iostream>
#include <iomanip>
#include <random>
#include <vector>

AntColonySolver::~AntColonySolver() {
    delete[] visited;
    delete[] pheromones;
}

ExitStatus AntColonySolver::prepareCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    i32 dim = tsp.dimension;
    
    // Init random
    std::random_device rd;
    gen = std::mt19937(rd());
    realGen = std::uniform_real_distribution<f32>(0.0f, 1.0f);
    
    // Init ants and pheromones
    try {
        visited = new i32[antCount * dim];
        pheromones = new f32[dim * dim];
    } catch (std::bad_alloc e) {
        std::cerr << "[C++ Error] Can't allocate memory" << std::endl;
        return ExitStatus::ERROR_MEMORY_LIMIT;
    }

    // Fill not visited with city indices
    i32* visitedStart = visited;
    i32* visitedEnd = visitedStart + dim;
    for (i32 i = 0; i < antCount; i++) {
        std::iota(visitedStart, visitedEnd, 0);
        visitedStart = visitedEnd;
        visitedEnd += dim;
    }

    // Set ants starting positions
    i32 antsPerNode = antCount / dim;
    i32 antIndex = 0;
    for (i32 i = 0; i < antsPerNode; i++) {
        for (i32 n = 0; n < dim; n++)  {
            i32 index = antIndex++ * dim;

            // Swap visited[0] of ant i with visited[n] which equals n 
            std::swap(visited[index], visited[index + n]);
        }
    }

    // Assign remaining ants to distinct, random cities
    std::vector<i32> nodeIndices(dim);
    std::iota(nodeIndices.begin(), nodeIndices.end(), 0);
    std::shuffle(nodeIndices.begin(), nodeIndices.end(), gen);

    i32 remaining = antCount % dim;
    i32 distributed = antCount - remaining;

    for (i32 i = 0; i < remaining; i++) {
        i32 index = (distributed + i) * dim;
        std::swap(visited[index], visited[index + nodeIndices[i]]);
    }
    
    // Set initial pheromone strength
    initialPheromoneStrength = 1.0f / dim;
    for (i32 i = 0; i < dim * dim; i++)
        pheromones[i] = initialPheromoneStrength;

    print(dim);

    return ExitStatus::SUCCESS;
}

ExitStatus AntColonySolver::prepareGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    return ExitStatus::SUCCESS;
}

ExitStatus AntColonySolver::solveCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    environment.progress.store(0);

    i32 dim = tsp.dimension;

    f32 probabilisticWeights[dim];
    i64 pathScores[antCount] = { 0 };

    f32 minPathScore = -1.0;

    // Solve TSP
    for (i32 i = 0; i < iterations; i++) {
        if (environment.interrupt)
            return ExitStatus::INTERRUPTED;
        
        environment.progress.store(100 * i / iterations);

        // Iterate through all steps
        for (i32 step = 1; step < dim; step++) {

            // Iterate through ants
            for (i32 ant = 0; ant < antCount; ant++) {
                // Pick next node

                // Index of the start of the row in indices corresponding to the current ant
                i32 visitedRow = ant * dim;

                // The current visited node
                i32 current = visited[visitedRow + step - 1];
                
                // Index of the start of the row in edgeWeights corresponding to the currently visited node
                i32 row = current * dim;

                i32 nextVisitedIndex = -1;
                f32 q = realGen(gen);
                
                //if (q <= q0) {
                if (true) {
                    // Pick Greedy

                    f32 maxScore = -1.0f;

                    for (i32 notVisited = step; notVisited < dim; notVisited++) {
                        i32 notVisitedIndex = visitedRow + notVisited;
                        i32 index = row + visited[notVisitedIndex];
                        f32 score = tsp.heuristics[index] * pheromones[index];

                        if (maxScore < score) {
                            maxScore = score;
                            nextVisitedIndex = notVisitedIndex;
                        }
                    }
                } else {
                    // Pick probabilistically

                    f32 sumWeights = 0.0f;

                    for (i32 notVisited = step; notVisited < dim; notVisited++) {
                        i32 notVisitedIndex = visitedRow + notVisited;
                        i32 index = row + visited[visitedRow + notVisited];
                        f32 weight = tsp.heuristics[index] * pheromones[index];
                        
                        sumWeights += weight;
                        probabilisticWeights[notVisited] = weight;
                    }

                    f32 r = realGen(gen);

                    i32 nextIndex = step;
                    while (nextIndex < dim) {
                        r -= probabilisticWeights[nextIndex];
                        
                        if (r <= 0) 
                            break;
                            
                        nextIndex++;
                    }

                    nextVisitedIndex = visitedRow + nextIndex;
                }

                // Get next visited node to front
                // Visited looks like
                // 10 5 2 8 6 1 4 3 7 9 11 50 21 ...
                // < visited | not visited >
                std::swap(visited[visitedRow + step], visited[nextVisitedIndex]);
                i32 next = visited[visitedRow + step];

                // Update local pheromone

                i32 i1 = current + next * dim;
                i32 i2 = next + current * dim;
                
                f32 p = (1.0f - p0) * pheromones[i1] + p0 * initialPheromoneStrength;
                pheromones[i1] = p;
                pheromones[i2] = p;

                pathScores[ant] += 1.0f / tsp.heuristics[i1];
            }
        }

        i32 minPathScoreIndex = 0;
        for (i32 i = 1; i < antCount; i++) {
            if (pathScores[i] < pathScores[minPathScoreIndex]) {
                minPathScoreIndex = i;
                pathScores[minPathScoreIndex] = 0;
            } else {
                pathScores[i] = 0;
            }
        }

        i32 index = dim * minPathScoreIndex;
        i32 last = visited[index++];
        for (i32 i = 1; i < dim; i++) {
            i32 current = visited[index++];
        }

        if (pathScores[minPathScoreIndex] < minPathScore) {
            for (i32 i = 0; i < dim; i++)
                solutionData.pathIndices[i] = 0;
        }
    }

    environment.progress.store(100);

    return ExitStatus::SUCCESS;
}

ExitStatus AntColonySolver::solveGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    return ExitStatus::ERROR_NOT_IMPLEMENTED;
}

void AntColonySolver::print(i32 dim) {
    std::cout << "[C++] ### AntColonySolver ###" << std::endl;
    
    std::cout << "[C++] Number of ants: " << antCount << std::endl;
    std::cout << "[C++] Number of iterations: " << iterations << std::endl;

    std::cout << "[C++] Exploitation probability: " << q0 << std::endl;
    std::cout << "[C++] Pheromone dissipation coefficient: " << p0 << std::endl;
    std::cout << "[C++] Initial pheromone strength: " << initialPheromoneStrength << std::endl;

    std::cout << "[C++] Visited: " << initialPheromoneStrength << std::endl;

    i32 index = 0;
    for (i32 i = 0; i < antCount; i++) {
        std::cout << "Ant " << std::setw(3) << i << ": ";
        for (i32 j = 0; j < dim; j++)
            std::cout << std::setw(3) << visited[index++] << " ";
        std::cout << std::endl;
    }
    
    index = 0;
    for (i32 i = 0; i < dim; i++) {
        std::cout << "Node " << std::setw(3) << i << ": ";
        for (i32 j = 0; j < dim; j++)
            std::cout << std::setw(3) << pheromones[index++] << " ";
        std::cout << std::endl;
    }
}