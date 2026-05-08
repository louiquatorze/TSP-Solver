
#include "AntColonySolver.h"

#include <iostream>
#include <iomanip>
#include <random>
#include <vector>

AntColonySolver::AntColonySolver(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData_out)
    : TSPSolver(environment, algorithmSettings, tsp, solutionData_out)
{ }

AntColonySolver::~AntColonySolver() {
    delete[] visited;
    delete[] pheromones;
    delete[] probabilisticWeights;
    delete[] pathLenghts;
}

ExitStatus AntColonySolver::prepareCPU() {
    minPathLength = 9223372036854775807;
    
    // Init random
    std::random_device rd;
    gen = std::mt19937(rd());
    nodeGen = std::uniform_int_distribution<i32>(1, tsp.dimension - 1);
    realGen = std::uniform_real_distribution<f32>(0.0f, 1.0f);
    
    // Init ants, pheromones, probabilisticWeights and pathLengths
    try {
        visited = new i32[antCount * tsp.dimension];
        pheromones = new f32[tsp.dimension * tsp.dimension];

        probabilisticWeights = new f32[tsp.dimension];
        pathLenghts = new i64[antCount];
    } catch (std::bad_alloc e) {
        std::cerr << "[C++ Error] Can't allocate memory" << std::endl;
        return ExitStatus::ERROR_MEMORY_LIMIT;
    }

    // Fill not visited with city indices, clear pathlengths
    i32* visitedStart = visited;
    i32* visitedEnd = visitedStart + tsp.dimension;
    for (i32 i = 0; i < antCount; i++) {
        std::iota(visitedStart, visitedEnd, 0);
        visitedStart = visitedEnd;
        visitedEnd += tsp.dimension;

        pathLenghts[i] = 0;
    }

    // Set ants starting positions
    i32 antsPerNode = antCount / tsp.dimension;
    i32 antIndex = 0;
    for (i32 i = 0; i < antsPerNode; i++) {
        for (i32 n = 0; n < tsp.dimension; n++)  {
            i32 index = antIndex++ * tsp.dimension;

            // Swap visited[0] of ant i with visited[n] which equals n 
            std::swap(visited[index], visited[index + n]);
        }
    }

    // Assign remaining ants to distinct, random cities
    std::vector<i32> nodeIndices(tsp.dimension);
    std::iota(nodeIndices.begin(), nodeIndices.end(), 0);
    std::shuffle(nodeIndices.begin(), nodeIndices.end(), gen);

    i32 remaining = antCount % tsp.dimension;
    i32 distributed = antCount - remaining;

    for (i32 i = 0; i < remaining; i++) {
        i32 index = (distributed + i) * tsp.dimension;
        std::swap(visited[index], visited[index + nodeIndices[i]]);
    }
    
    // Set initial pheromone strength
    initialPheromoneStrength = 1.0f / tsp.dimension;
    for (i32 i = 0; i < tsp.dimension * tsp.dimension; i++)
        pheromones[i] = initialPheromoneStrength;

    return ExitStatus::SUCCESS;
}

ExitStatus AntColonySolver::solveCPU() {
    environment.progress.store(0);

    printVisited();
    printPheromones();

    // Solve TSP
    for (i32 i = 0; i < iterations; i++) {
        if (environment.interrupt)
            return ExitStatus::INTERRUPTED;
        
        environment.progress.store(100 * i / iterations);

        runAntColonyIteration();
        evaluateBestPathAndResetAnts();
    }

    solutionData_out.pathLength = minPathLength;
    environment.progress.store(100);

    return ExitStatus::SUCCESS;
}

void AntColonySolver::runAntColonyIteration() {
    // Iterate through all steps
    for (i32 step = 1; step < tsp.dimension; step++) {

        // Iterate through ants
        for (i32 ant = 0; ant < antCount; ant++) {
            // Pick next node

            // Index of the start of the row in indices corresponding to the current ant
            i32 visitedRow = ant * tsp.dimension;

            // The current visited node
            i32 current = visited[visitedRow + step - 1];
            
            // Index of the start of the row in edgeWeights corresponding to the currently visited node
            i32 row = current * tsp.dimension;

            i32 nextVisitedIndex = -1;
            f32 q = realGen(gen);
            
            if (q < exploitationProbability) {
                // Pick Greedy

                f32 maxScore = -1.0f;

                for (i32 notVisited = step; notVisited < tsp.dimension; notVisited++) {
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

                f32 sumScores = 0.0f;

                for (i32 notVisited = step; notVisited < tsp.dimension; notVisited++) {
                    i32 notVisitedIndex = visitedRow + notVisited;
                    i32 index = row + visited[visitedRow + notVisited];
                    f32 score = tsp.heuristics[index] * pheromones[index];
                    
                    sumScores += score;
                    probabilisticWeights[notVisited] = score;
                }

                f32 r = realGen(gen);

                i32 nextIndex = step;
                while (nextIndex < tsp.dimension) {
                    r -= probabilisticWeights[nextIndex];
                    
                    if (r <= 0) 
                        break;
                        
                    nextIndex++;
                }

                nextVisitedIndex = visitedRow + nextIndex;
            }

            // Get next node to visit up front
            // Visited looks like
            // 10 5 2 8 6 1 4 3 7 9 11 50 21 ...
            // < visited | not visited >
            std::swap(visited[visitedRow + step], visited[nextVisitedIndex]);
            i32 next = visited[visitedRow + step];

            // Update local pheromone

            i32 i1 = current + next * tsp.dimension;
            i32 i2 = next + current * tsp.dimension;
            
            f32 p = (1.0f - localEvaporationRate) * pheromones[i1] + localEvaporationRate * initialPheromoneStrength;
            pheromones[i1] = p;
            pheromones[i2] = p;

            pathLenghts[ant] += tsp.edgeWeights[i1];
        }
    }
}

void AntColonySolver::evaluateBestPathAndResetAnts() {
    i32 minPathLengthAnt = 0;
    for (i32 i = 1; i < antCount; i++) {
        i32 worseAnt;

        if (pathLenghts[i] < pathLenghts[minPathLengthAnt]) {
            minPathLengthAnt = i;
            worseAnt = minPathLengthAnt;
        } else {
            worseAnt = i;
        }
    
        pathLenghts[worseAnt] = 0;

        i32 swapIndex = nodeGen(gen);
        std::swap(visited[0], visited[swapIndex]);
    }

    f32 pheromoneSecretion = 1.0f / pathLenghts[minPathLengthAnt];

    i32 index = tsp.dimension * minPathLengthAnt;

    auto setPheromones = [&](i32 l, i32 c) {
        i32 i1 = l * tsp.dimension + c;
        i32 i2 = c * tsp.dimension + l;

        i32 p = (1.0f - globalEvaporationRate) * pheromones[i1] + globalEvaporationRate * pheromoneSecretion;

        pheromones[i1] = p;
        pheromones[i2] = p;
    };

    if (pathLenghts[minPathLengthAnt] < minPathLength) {
        for (i32 i = 1; i < tsp.dimension; i++) {
            solutionData_out.pathIndices[i] = visited[index];

            setPheromones(visited[index - 1], visited[index]);
            index++;
        } 
    } else {
        for (i32 i = 1; i < tsp.dimension; i++) {
            setPheromones(visited[index - 1], visited[index]);
            index++;
        } 
    }
}

ExitStatus AntColonySolver::prepareGPU() {
    return ExitStatus::SUCCESS;
}

ExitStatus AntColonySolver::solveGPU() {
    return ExitStatus::ERROR_NOT_IMPLEMENTED;
}

void AntColonySolver::print() {
    std::cout << "[C++] --- AntColonySolver ---" << std::endl;
    
    std::cout << "[C++] Number of ants: " << algorithmSettings.antCount << std::endl;
    std::cout << "[C++] Number of iterations: " << algorithmSettings.iterations << std::endl;
    
    std::cout << std::endl;

    std::cout << "[C++] Beta: " << algorithmSettings.beta << std::endl;
    std::cout << "[C++] Pheromone power: " << algorithmSettings.pheromonePower << std::endl;
    std::cout << "[C++] Exploitation probability: " << algorithmSettings.exploitationProbability << std::endl;
    std::cout << "[C++] Local pheromone evaporation coefficient: " << algorithmSettings.localEvaporationRate << std::endl;
    std::cout << "[C++] Global pheromone evaporation coefficient: " << algorithmSettings.globalEvaporationRate << std::endl;
}

void AntColonySolver::printVisited() {
    std::cout << "[C++] Visited:" << std::endl;

    i32 index = 0;
    for (i32 i = 0; i < antCount; i++) {
        std::cout << "[C++] Ant " << std::setw(3) << i << ": ";
        for (i32 j = 0; j < tsp.dimension; j++)
            std::cout << std::setw(3) << visited[index++] << " ";
        std::cout << std::endl;
    }
}

void AntColonySolver::printPheromones() {
    std::cout << "[C++] Pheromones:" << std::endl;

    i32 index = 0;
    for (i32 i = 0; i < tsp.dimension; i++) {
        std::cout << "[C++] Node " << std::setw(3) << i << ": ";
        for (i32 j = 0; j < tsp.dimension; j++)
            std::cout << std::setw(3) << pheromones[index++] << " ";
        std::cout << std::endl;
    }
}
