
#pragma once

#include "TSPSolver.h"

class IterativeSolver : public TSPSolver {
public:
    IterativeSolver() = delete;
    IterativeSolver(Environment&, AlgorithmSettings&, TSP&, SolutionData&);
    ~IterativeSolver();

    void print() override;

protected:
    ExitStatus prepareCPU() override;
    ExitStatus prepareGPU() override;

    ExitStatus solveCPU() override;
    ExitStatus solveGPU() override;

private:
    u16* indices;
    u16* swaps;
};