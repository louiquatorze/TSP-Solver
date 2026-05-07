
#pragma once

#include "TSPSolver.h"

class IterativeSolver : public TSPSolver {
public:
    IterativeSolver() = default;
    ~IterativeSolver();

protected:
    ExitStatus prepareCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;
    ExitStatus prepareGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;

    ExitStatus solveCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;
    ExitStatus solveGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) override;

private:
    u16* indices;
    u16* swaps;
};