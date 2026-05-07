
#pragma once

#include <functional>
#include <memory>

#include "AlgorithmSettings.h"
#include "TSP.h"
#include "SolutionData.h"
#include "Environment.h"
#include "ExitStatus.h"

class TSPSolver {
public:
    TSPSolver() = default;
    virtual ~TSPSolver() = default;

    ExitStatus solve(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData_out);

protected:
    virtual ExitStatus prepareCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) = 0;
    virtual ExitStatus prepareGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) = 0;

    virtual ExitStatus solveCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) = 0;
    virtual ExitStatus solveGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) = 0;
};