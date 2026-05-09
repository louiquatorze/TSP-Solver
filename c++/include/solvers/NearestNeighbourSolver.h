
#pragma once

#include "TSPSolver.h"

class NearestNeighbourSolver : public TSPSolver {
public:
    NearestNeighbourSolver() = delete;
    NearestNeighbourSolver(Environment&, AlgorithmSettings&, TSP&, SolutionData&);
    ~NearestNeighbourSolver() = default;

    void print() override;

    static ExitStatus getPathLength(Environment& environment, TSP& tsp, i64& length_out);
    
protected:
    ExitStatus prepareCPU() override;
    ExitStatus prepareGPU() override;

    ExitStatus solveCPU() override;
    ExitStatus solveGPU() override;
};