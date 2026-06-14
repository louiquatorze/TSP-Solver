
#include "interface.hpp"
#include "ComputeTSP.hpp"
#include "TSPSolverFactory.hpp"
#include "ExitStatus.hpp"
#include "Timer.hpp"
#include "GraphUtil.hpp"

#include <iostream>
#include <cstring>
#include <chrono>
#include <magic_enum.hpp>

extern "C" {
    void* createContext() {
        try {
            return static_cast<void*>(new Context);
        } catch (const std::exception& e) {
            std::cerr << "[C++] Context initialization failed: " << e.what() << std::endl;
        }
        
        return nullptr;
    }

    void destroyContext(void* handle) {
        if (handle == nullptr)
            return;

        auto context = static_cast<Context*>(handle);
        delete context;
    }

    void setInterrupt(void* handle, bool interrupt) {
        if (handle == nullptr)
            return;
            
        auto context = static_cast<Context*>(handle);
        context->environment.interrupt.store(interrupt);
    }

    i32 getProgress(void* handle) {
        if (handle == nullptr)
            return ExitStatus::ERROR_INVALID_HANDLE;
            
        auto context = static_cast<Context*>(handle);
        return context->environment.progress.load();
    }

    i32 computeTSPData(void* handle, AlgorithmSettings* algorithmSettings, TSPRaw* tspRaw, TSP* tsp_out, SolutionData* solutionData_out) {
        if (handle == nullptr)
            return ExitStatus::ERROR_INVALID_HANDLE;
            
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

    i32 solveTSP(void* handle, AlgorithmSettings* algorithmSettings, TSP* tsp, SolutionData* solutionData_out, u32 analyticFlags){
        if (handle == nullptr)
            return ExitStatus::ERROR_INVALID_HANDLE;
            
        std::cout << "[C++] ---------------------" << std::endl;
        std::cout << "[C++]        Solving       "<< std::endl;
        std::cout << "[C++] ---------------------" << std::endl;
        std::cout << "[C++] Algorithm: " << algorithmSettings->algorithm << std::endl;
        std::cout << "[C++] GPU: " << algorithmSettings->gpu << std::endl;
        std::cout << "[C++] Dimension: " << tsp->dimension << std::endl;

        std::cout << "[C++] Analytics: ";
        
        u32 pow = 1;
        while (pow <= Analytics::All) {
            if (pow & analyticFlags)
                std::cout << magic_enum::enum_name(static_cast<Analytics>(pow)) << ", ";

            pow <<= 1;
        }

        std::cout << std::endl;

        auto context = static_cast<Context*>(handle);

        context->solver = TSPSolverFactory::create(context->environment, *algorithmSettings, *tsp, *solutionData_out, analyticFlags);

        context->solver->print();
        ExitStatus exitStatus = context->solver->solve();

        // Delete the solver
        context->solver = nullptr;

        std::cout << "[C++] Done." << std::endl;
        
        return exitStatus;
    }
}