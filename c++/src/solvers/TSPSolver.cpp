
#include "TSPSolver.h"

#include "IterativeSolver.h"
#include "NearestNeighbourSolver.h"
#include "AntColonySolver.h"
#include "Timer.h"

#include <chrono>
#include <functional>
#include <iostream>

ExitStatus TSPSolver::solve(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData_out) {
    solutionData_out.dimension = tsp.dimension;
    std::function<ExitStatus()> prepare, solve;

    if (algorithmSettings.gpu) {
        prepare = [&] {
            return prepareGPU(environment, algorithmSettings, tsp, solutionData_out);
        };
        solve = [&] {
            return solveGPU(environment, algorithmSettings, tsp, solutionData_out);
        };
    } else {
        prepare = [&] {
            return prepareCPU(environment, algorithmSettings, tsp, solutionData_out);
        };
        solve = [&] {
            return solveCPU(environment, algorithmSettings, tsp, solutionData_out);
        };
    }

    ExitStatus exitStatus = Timer::time(prepare, solutionData_out.preparationTime_ns);

    if (exitStatus != ExitStatus::SUCCESS) {
        std::cerr << "[C++ Error] Preparation failed" << std::endl;
        return exitStatus;
    }
    
    return Timer::time(solve, solutionData_out.solutionTime_ns);
}