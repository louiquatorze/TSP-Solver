
#pragma once

#include "TSPSolver.h"

#include <random>

class AntColonySolver : public TSPSolver {    
public:
    ~AntColonySolver();

protected:
    ExitStatus prepareCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;
    ExitStatus prepareGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;

    ExitStatus solveCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;
    ExitStatus solveGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;

private:
    const i32 antCount = 4;
    const i32 iterations = 10;

    const f32 q0 = 0.9;
    const f32 p0 = 0.2;
    const f32 beta = 0.8;

    f32 initialPheromoneStrength = 0.0f;

    i32* visited    = nullptr;
    f32* pheromones = nullptr;

    std::mt19937 gen;
    std::uniform_real_distribution<f32> realGen;

    void print(i32 dim);
};