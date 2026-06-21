
#include "TSPSolverFactory.hpp"

#include "IterativeSolver.hpp"
#include "NearestNeighbourSolver.hpp"
#include "AntColonySolver.hpp"
#include "ChristofidesSolver.hpp"

std::unique_ptr<TSPSolver> TSPSolverFactory::create(Environment& environment, VulkanContext& vulkanContext, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData_out, u32 analyticFlags) {
    std::unique_ptr<TSPSolver> solver;

    switch (algorithmSettings.algorithm) {
        case AlgorithmSettings::Algorithm::Iterative:
            solver = std::make_unique<IterativeSolver>(environment, vulkanContext, algorithmSettings, tsp, solutionData_out, analyticFlags);   
            break;
        case AlgorithmSettings::Algorithm::NearestNeighbour:
            solver = std::make_unique<NearestNeighbourSolver>(environment, vulkanContext, algorithmSettings, tsp, solutionData_out, analyticFlags);
            break;
        case AlgorithmSettings::Algorithm::AntColony:
            solver = std::make_unique<AntColonySolver>(environment, vulkanContext, algorithmSettings, tsp, solutionData_out, analyticFlags);
            break;
        case AlgorithmSettings::Algorithm::Christofides:
            solver = std::make_unique<ChristofidesSolver>(environment, vulkanContext, algorithmSettings, tsp, solutionData_out, analyticFlags);
            break;
        default:
            solver = nullptr;
    }

    return solver;
}