
#pragma once

#include "AlgorithmSettings.h"
#include "TSP.h"
#include "TSPRaw.h"
#include "SolutionData.h"
#include "types.h"
#include "Context.h"

extern "C" {
    void* createContext();
    void destroyContext(void* handle);
    void setInterrupt(void* handle, bool interrupt);
    i32 getProgress(void* handle);

    i32 computeTSPData(void* handle, AlgorithmSettings* algorithmSettings, TSPRaw* tspRaw, TSP* tsp_out, SolutionData* solutionData_out);
    i32 solveTSP(void* handle, AlgorithmSettings* algorithmSettings, TSP* tsp, SolutionData* solutionData_out);
}