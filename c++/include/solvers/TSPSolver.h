
#pragma once

#include "AlgorithmSettings.h"
#include "TSP.h"
#include "SolutionData.h"
#include "Environment.h"
#include "ExitStatus.h"
#include "VulkanContext.h"

#include <functional>
#include <memory>

class TSPSolver {
public:
    TSPSolver() = delete;
    TSPSolver(Environment&, VulkanContext&, AlgorithmSettings&, TSP&, SolutionData&);
    virtual ~TSPSolver() = default;

    ExitStatus solve();
    virtual void print() = 0;

protected:
    virtual ExitStatus prepareCPU() = 0;
    virtual ExitStatus prepareGPU() = 0;

    virtual ExitStatus solveCPU() = 0;
    virtual ExitStatus solveGPU() = 0;

    Environment& environment;
    VulkanContext& vulkanContext;

    AlgorithmSettings& algorithmSettings;
    TSP& tsp;

    SolutionData& solutionData_out;
};