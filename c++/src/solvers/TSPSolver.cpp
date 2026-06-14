
#include "TSPSolver.hpp"

#include "IterativeSolver.hpp"
#include "NearestNeighbourSolver.hpp"
#include "AntColonySolver.hpp"
#include "Timer.hpp"

#include <chrono>
#include <functional>
#include <iostream>
#include <vulkan/vulkan.h>

TSPSolver::TSPSolver(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData_out, u32 analyticFlags) :
    environment(environment),
    algorithmSettings(algorithmSettings),
    tsp(tsp),
    solutionData_out(solutionData_out),
    analyticFlags(analyticFlags)
{ }

ExitStatus TSPSolver::solve() {
    solutionData_out.dimension = tsp.dimension;
    std::function<ExitStatus()> prepare, solve;

    if (algorithmSettings.gpu) {
        prepare = [&]() { return prepareGPU(); };
        solve = [&](){ return solveGPU(); };
    } else {
        prepare = [&](){ return prepareCPU(); };
        solve = [&](){ return solveCPU(); };
    }

    ExitStatus exitStatus = Timer::time(prepare, solutionData_out.preparationTime_ns);

    if (exitStatus != ExitStatus::SUCCESS) {        
        std::cerr << "[C++ Error] Preparation failed" << std::endl;
        return exitStatus;
    }
    
    return Timer::time(solve, solutionData_out.solutionTime_ns);
}