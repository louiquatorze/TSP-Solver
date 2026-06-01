
#pragma once

#include "TSPSolver.h"

#include <random>

class AntColonySolver : public TSPSolver {    
public:
    AntColonySolver() = delete;
    AntColonySolver(Environment&, AlgorithmSettings&, TSP&, SolutionData&);
    ~AntColonySolver();

    void print() override;
    void printVisited();
    void printPheromones();

protected:
    ExitStatus prepareCPU() override;
    ExitStatus prepareGPU() override;

    ExitStatus solveCPU() override;
    ExitStatus solveGPU() override;

private:
    f32 initialPheromoneStrength = 0.0f;

    i32* visited    = nullptr;
    f32* pheromones = nullptr;

    f32* probabilisticWeights   = nullptr;
    i64* pathLenghts            = nullptr;

    i64 minPathLength;

    std::mt19937 gen;
    std::uniform_int_distribution<i32> nodeGen;
    std::uniform_real_distribution<f32> realGen;

    void runAntColonyIteration();
    void evaluateBestPathAndResetAnts();
};