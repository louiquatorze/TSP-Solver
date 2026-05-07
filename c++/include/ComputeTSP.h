
#pragma once

#include "TSP.h"
#include "TSPRaw.h"
#include "Environment.h"
#include "ExitStatus.h"
#include "AlgorithmSettings.h"

class ComputeTSP {
public:
    static ExitStatus fillTSPData(Environment& environment, AlgorithmSettings& algorithmSettings, TSPRaw& tspRaw, TSP& tsp_out);

    template <typename TSPDataSetter>
    static void clearDiagonal(TSP& tsp_out, TSPDataSetter setter);

    template <typename TSPDataSetter>
    static ExitStatus dispatchBySetter(Environment& environment, TSPRaw& tspRaw, TSP& tsp_out, TSPDataSetter setter);
 
    template <typename TSPDataSetter>
    static ExitStatus setTSPDataExplicit(Environment& environment, TSPRaw& tspRaw, TSPDataSetter setter);

    template <typename DistAlgo, typename TSPDataSetter>
    static ExitStatus computeTSPData(Environment& environment, TSPRaw& tspRaw, TSPDataSetter setter, DistAlgo formula);

    static void printTSP(TSP& tsp);
};