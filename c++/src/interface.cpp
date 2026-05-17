
#include "interface.h"
#include "ComputeTSP.h"
#include "TSPSolverFactory.h"
#include "ExitStatus.h"
#include "Timer.h"
#include "GraphUtil.h"

#include "NearestNeighbourSolver.h"

#include <iostream>
#include <cstring>
#include <chrono>

extern "C" {
    void* createContext() {
        return static_cast<void*>(new Context);
    }

    void destroyContext(void* handle) {
        auto context = static_cast<Context*>(handle);
        delete context;
    }

    void setInterrupt(void* handle, bool interrupt) {
        auto context = static_cast<Context*>(handle);
        context->environment.interrupt.store(interrupt);
    }

    i32 getProgress(void* handle) {
        auto context = static_cast<Context*>(handle);
        return context->environment.progress.load();
    }

    i32 computeTSPData(void* handle, AlgorithmSettings* algorithmSettings, TSPRaw* tspRaw, TSP* tsp_out, SolutionData* solutionData_out) {
        std::cout << "[C++] Computing TSP data" << std::endl;
        
        auto context = static_cast<Context*>(handle);

        ExitStatus exitStatus = Timer::time(
                [&]() { 
                    return ComputeTSP::fillTSPData(context->environment, *algorithmSettings, *tspRaw, *tsp_out) ;
                },
                solutionData_out->preprocessingTime_ns
            );

        // ComputeTSP::printTSP(*tsp_out);
        
        std::cout << "[C++] Done." << std::endl;

        return exitStatus;
    }

    i32 solveTSP(void* handle, AlgorithmSettings* algorithmSettings, TSP* tsp, SolutionData* solutionData_out){
        std::cout << "[C++] ---------------------" << std::endl;
        std::cout << "[C++]        Solving       "<< std::endl;
        std::cout << "[C++] ---------------------" << std::endl;
        std::cout << "[C++] Algorithm: " << algorithmSettings->algorithm << std::endl;
        std::cout << "[C++] GPU: " << algorithmSettings->gpu << std::endl;
        std::cout << "[C++] Dimension: " << tsp->dimension << std::endl;

        auto context = static_cast<Context*>(handle);

        context->solver = TSPSolverFactory::create(context->environment, *algorithmSettings, *tsp, *solutionData_out);

        context->solver->print();
        ExitStatus exitStatus = context->solver->solve();

        // Delete the solver
        context->solver = nullptr;

        std::cout << "[C++] Done." << std::endl;
        
        return exitStatus;
    }
}