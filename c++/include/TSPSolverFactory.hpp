
#pragma once

#include "TSPSolver.hpp"
#include "AlgorithmSettings.hpp"

class TSPSolverFactory {
public:
    static std::unique_ptr<TSPSolver> create(Environment&, AlgorithmSettings&, TSP&, SolutionData&);
};