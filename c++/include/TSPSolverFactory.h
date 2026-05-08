
#pragma once

#include "TSPSolver.h"
#include "AlgorithmSettings.h"

class TSPSolverFactory {
public:
    static std::unique_ptr<TSPSolver> create(Environment&, AlgorithmSettings&, TSP&, SolutionData&);
};