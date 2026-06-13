
#include "TSPSolverFactory.hpp"

#include "IterativeSolver.hpp"
#include "NearestNeighbourSolver.hpp"
#include "AntColonySolver.hpp"
#include "ChristofidesSolver.hpp"

std::unique_ptr<TSPSolver> TSPSolverFactory::create(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData_out) {
    std::unique_ptr<TSPSolver> solver;

    switch (algorithmSettings.algorithm) {
        case AlgorithmSettings::Algorithm::Iterative:
            solver = std::make_unique<IterativeSolver>(environment, algorithmSettings, tsp, solutionData_out);   
            break;
        case AlgorithmSettings::Algorithm::NearestNeighbour:
            solver = std::make_unique<NearestNeighbourSolver>(environment, algorithmSettings, tsp, solutionData_out);
            break;
        case AlgorithmSettings::Algorithm::AntColony:
            solver = std::make_unique<AntColonySolver>(environment, algorithmSettings, tsp, solutionData_out);
            break;
        case AlgorithmSettings::Algorithm::Christofides:
            solver = std::make_unique<ChristofidesSolver>(environment, algorithmSettings, tsp, solutionData_out);
            break;
        default:
            solver = nullptr;
    }

    return solver;
}