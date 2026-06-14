
#pragma once

#include "AlgorithmSettings.hpp"
#include "TSP.hpp"
#include "TSPRaw.hpp"
#include "SolutionData.hpp"
#include "types.hpp"
#include "Context.hpp"

extern "C" {
    void* createContext();
    void destroyContext(void* handle);
    void setInterrupt(void* handle, bool interrupt);
    i32 getProgress(void* handle);
    
    i32 computeTSPData(void* handle, AlgorithmSettings* algorithmSettings, TSPRaw* tspRaw, TSP* tsp_out, SolutionData* solutionData_out);
    i32 solveTSP(void* handle, AlgorithmSettings* algorithmSettings, TSP* tsp, SolutionData* solutionData_out, u32 analysisFlags);
}