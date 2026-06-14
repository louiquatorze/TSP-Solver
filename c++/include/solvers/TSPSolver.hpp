
#pragma once

#include "AlgorithmSettings.hpp"
#include "TSP.hpp"
#include "SolutionData.hpp"
#include "Environment.hpp"
#include "ExitStatus.hpp"
#include "Analytics.hpp"

#include <functional>
#include <memory>

class TSPSolver {
public:
    TSPSolver() = delete;
    TSPSolver(Environment&, AlgorithmSettings&, TSP&, SolutionData&, u32);
    virtual ~TSPSolver() = default;

    ExitStatus solve();
    virtual void print() = 0;

protected:
    const u32 analyticFlags;

    virtual ExitStatus prepareCPU() = 0;
    virtual ExitStatus prepareGPU() = 0;

    virtual ExitStatus solveCPU() = 0;
    virtual ExitStatus solveGPU() = 0;

    Environment& environment;
    AlgorithmSettings& algorithmSettings;
    TSP& tsp;
    SolutionData& solutionData_out;
};