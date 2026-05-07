
#include "TSPSolverFactory.h"

#include "IterativeSolver.h"
#include "NearestNeighbourSolver.h"
#include "AntColonySolver.h"

std::unique_ptr<TSPSolver> TSPSolverFactory::create(AlgorithmSettings& algorithmSettings) {
    std::unique_ptr<TSPSolver> solver;

    switch (algorithmSettings.algorithm) {
        case AlgorithmSettings::Algorithm::Iterative:
            solver = std::make_unique<IterativeSolver>();   
            break;
        case AlgorithmSettings::Algorithm::NearestNeighbour:
            solver = std::make_unique<NearestNeighbourSolver>();
            break;
        case AlgorithmSettings::Algorithm::AntColony:
            solver = std::make_unique<AntColonySolver>();
            break;
        default:
            solver = nullptr;
    }

    return solver;
}