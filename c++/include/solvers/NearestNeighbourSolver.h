
#pragma once

#include "TSPSolver.h"

class NearestNeighbourSolver : public TSPSolver {
public:
    NearestNeighbourSolver() = default;
    ~NearestNeighbourSolver() = default;

protected:
    ExitStatus prepareCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;
    ExitStatus prepareGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;

    ExitStatus solveCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;
    ExitStatus solveGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;
};