
#include "ComputeTSP.h"
#include "DistanceFunctions.h"
#include "ExitStatus.h"
#include "TSP.h"
#include "TSPSetters.h"

#include <memory.h>
#include <cstring>
#include <iostream>
#include <iomanip>

ExitStatus ComputeTSP::fillTSPData(Environment& environment, AlgorithmSettings& algorithmSettings, TSPRaw& tspRaw, TSP& tsp_out) {
    tsp_out.dimension = tspRaw.dimension;

    // Dispatch the filler functions by how the data is to be set

    switch (algorithmSettings.algorithm) {
        case AlgorithmSettings::Algorithm::Iterative:
        case AlgorithmSettings::Algorithm::NearestNeighbour: {
            auto setter = SetEdgeWeight(tsp_out);

            ComputeTSP::clearDiagonal(tsp_out, setter);
            return ComputeTSP::dispatchBySetter(environment, tspRaw, tsp_out, setter);
        }

        case AlgorithmSettings::Algorithm::AntColony: {
            auto setter = SetBoth(tsp_out, algorithmSettings.beta);            
            
            ComputeTSP::clearDiagonal(tsp_out, setter);
            return ComputeTSP::dispatchBySetter(environment, tspRaw, tsp_out, setter);
        }

        default:
            return ExitStatus::ERROR_INVALID_INPUT;
    }
}

template <typename TSPDataSetter>
void ComputeTSP::clearDiagonal(TSP& tsp_out, TSPDataSetter setter) {
    i32 index = 0;

    for (i32 i = 0; i < tsp_out.dimension; i++) {
        setter(0, index);
        index += tsp_out.dimension + 1;
    }
}

template <typename TSPDataSetter>
ExitStatus ComputeTSP::dispatchBySetter(Environment& environment, TSPRaw& tspRaw, TSP& tsp_out, TSPDataSetter setter) {
    TSPRaw::EdgeWeightType type = tspRaw.edgeWeightType;
    
    if (tspRaw.edgeWeightType == TSPRaw::EdgeWeightType::EXPLICIT)
        return setTSPDataExplicit(environment, tspRaw, setter);

    const i32 _2D = 2;
    const i32 _3D = 3;

    switch (type) {
        case TSPRaw::EdgeWeightType::EUC_2D:
            return computeTSPData(environment, tspRaw, setter, AlgoEUC(_2D));
        case TSPRaw::EdgeWeightType::EUC_3D:
            return computeTSPData(environment, tspRaw, setter, AlgoEUC(_3D));
        case TSPRaw::EdgeWeightType::MAX_2D:
            return computeTSPData(environment, tspRaw, setter, AlgoMAX(_2D));
        case TSPRaw::EdgeWeightType::MAX_3D:
            return computeTSPData(environment, tspRaw, setter, AlgoMAX(_3D));
        case TSPRaw::EdgeWeightType::MAN_2D:
            return computeTSPData(environment, tspRaw, setter, AlgoMAN(_2D));
        case TSPRaw::EdgeWeightType::MAN_3D:
            return computeTSPData(environment, tspRaw, setter, AlgoMAN(_3D));
        case TSPRaw::EdgeWeightType::CEIL_2D:
            return computeTSPData(environment, tspRaw, setter, AlgoCEIL(_2D));
        case TSPRaw::EdgeWeightType::GEO:
            return computeTSPData(environment, tspRaw, setter, AlgoGEO());
        case TSPRaw::EdgeWeightType::ATT:
            return computeTSPData(environment, tspRaw, setter, AlgoATT());
        case TSPRaw::EdgeWeightType::XRAY1:
            return computeTSPData(environment, tspRaw, setter, AlgoXRAY1());
        case TSPRaw::EdgeWeightType::XRAY2:
            return computeTSPData(environment, tspRaw, setter, AlgoXRAY2());
        case TSPRaw::EdgeWeightType::SPECIAL:
            return computeTSPData(environment, tspRaw, setter, AlgoSPECIAL());

        default:
            return ExitStatus::ERROR_INVALID_INPUT;
    }
}

template <typename TSPDataSetter>
ExitStatus ComputeTSP::setTSPDataExplicit(Environment& environment, TSPRaw& tspRaw, TSPDataSetter setter) {
    TSPRaw::EdgeWeightFormat format = tspRaw.edgeWeightFormat;

    i32 dim = tspRaw.dimension;
    i32 index;

    switch (format) {
        case TSPRaw::EdgeWeightFormat::FUNCTION:
            std::cerr << "[C++ Error] Function edge weights not supported" << std::endl;
            
            return ExitStatus::ERROR_INVALID_INPUT;
        case TSPRaw::EdgeWeightFormat::FULL_MATRIX:
            index = 0;

            for (i32 j = 0; j < dim; j++) {
                for (i32 i = 0; i < j; i++) {
                    i32 val = tspRaw.edgeWeights[index];
                    setter(val, i, j);
                    
                    index++;
                }
                for (i32 i = j + 1; i < dim; i++) {
                    i32 val = tspRaw.edgeWeights[index];
                    setter(val, i, j);
                    
                    index++;
                }
            }

            break;
        case TSPRaw::EdgeWeightFormat::LOWER_ROW:
        case TSPRaw::EdgeWeightFormat::UPPER_COL:
            index = 0;

            for (i32 j = 0; j < dim; j++) {
                for (i32 i = 0; i < j; i++) {
                    i32 val = tspRaw.edgeWeights[index];
                    setter(val, i, j);
                    
                    index++;
                }
            }

            break;
        case TSPRaw::EdgeWeightFormat::LOWER_COL:
        case TSPRaw::EdgeWeightFormat::UPPER_ROW:
            index = 0;

            for (i32 j = 0; j < dim; j++) {
                for (i32 i = j + 1; i < dim; i++) {
                    i32 val = tspRaw.edgeWeights[index];
                    setter(val, i, j);
                    
                    index++;
                }
            }
            
            break;
        case TSPRaw::EdgeWeightFormat::LOWER_DIAG_ROW:
        case TSPRaw::EdgeWeightFormat::UPPER_DIAG_COL:
            index = 0;

            for (i32 j = 0; j < dim; j++) {
                for (i32 i = 0; i < j; i++) {
                    i32 val = tspRaw.edgeWeights[index];
                    setter(val, i, j);
                    
                    index++;
                }

                index++;
            }

            break;
        case TSPRaw::EdgeWeightFormat::LOWER_DIAG_COL:
        case TSPRaw::EdgeWeightFormat::UPPER_DIAG_ROW:
            index = 1;

            for (i32 j = 0; j < dim; j++) {
                for (i32 i = j + 1; i < dim; i++) {
                    i32 val = tspRaw.edgeWeights[index];
                    setter(val, i, j);
                    
                    index++;
                }

                index++;
            }
            
            break;
        default:
            return ExitStatus::ERROR_INVALID_INPUT; 
    }

    return ExitStatus::SUCCESS;
}

template <typename DistAlgo, typename TSPDataSetter>
ExitStatus ComputeTSP::computeTSPData(Environment& environment, TSPRaw& tspRaw, TSPDataSetter setter, DistAlgo formula) {
    environment.progress.store(0);

    i32 dim = tspRaw.dimension;
    f32* p1 = tspRaw.nodeCoords + formula.dim;

    for (i32 i = 1; i < dim; i++) {
        if (environment.interrupt)
            return ExitStatus::INTERRUPTED;

        environment.progress.store(100 * i / dim);
        f32* p2 = tspRaw.nodeCoords;
        
        for (i32 j = 0; j < i; j++) {
            i32 val = formula(p1, p2);
            setter(val, i, j);
            
            p2 += formula.dim;
        }

        p1 += formula.dim;
    }

    environment.progress.store(100);

    return ExitStatus::SUCCESS;
}

void ComputeTSP::printTSP(TSP& tsp) {
    std::cout << "[C++] --- TSP ---" << std::endl;
    std::cout << "[C++] Dimension: " << tsp.dimension << std::endl;
    
    if (tsp.edgeWeights != nullptr) {
        std::cout << "[C++] EdgeWeights: " << std::endl;

        i32 index = 0;
        for (i32 j = 0; j < tsp.dimension; j++) {
            std::cout << std::setw(5) << j;
            std::cout << ": ";

            for (i32 i = 0; i < tsp.dimension; i++) {
                std::cout << std::setw(4) << tsp.edgeWeights[index];
                std::cout << " ";
                index++;
            }
            std::cout << std::endl;
        }
    }

    if (tsp.heuristics != nullptr) {
        std::cout << "[C++] Heuristics: " << std::endl;

        i32 index = 0;
        for (i32 j = 0; j < tsp.dimension; j++) {
            std::cout << std::setw(5) << j;
            std::cout << ": ";

            for (i32 i = 0; i < tsp.dimension; i++) {
                std::cout << std::setw(4) << tsp.heuristics[index];
                std::cout << " ";
                index++;
            }
            std::cout << std::endl;
        }
    } 
}