
#pragma once

#include "TSPSolver.h"

class ChristofidesSolver : TSPSolver {
public:
    ChristofidesSolver() = delete;
    ChristofidesSolver(Environment&, AlgorithmSettings&, TSP&, SolutionData&);

    void print() override;

protected:
    ExitStatus prepareCPU() override;
    ExitStatus prepareGPU() override;

    ExitStatus solveCPU() override;
    ExitStatus solveGPU() override;
};