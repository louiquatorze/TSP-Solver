
#pragma once

#include "types.h"

#include <iostream>
#include <iomanip>

extern "C" {
    struct TSP {
        i32 dimension;

        i32* edgeWeights;
        f32* heuristics;

        inline i32& ew(i32 i, i32 j) {
            return edgeWeights[j * dimension + i];
        }

        inline f32& he(i32 i, i32 j) {
            return heuristics[j * dimension + i];
        }

        void print() {
            std::cout << "[C++] --- TSP ---" << std::endl;
            std::cout << "[C++] Dimension: " << dimension << std::endl;
            
            if (edgeWeights != nullptr) {
                std::cout << "[C++] EdgeWeights: " << std::endl;

                i32 index = 0;
                for (i32 j = 0; j < dimension; j++) {
                    std::cout << std::setw(5) << j;
                    std::cout << ": ";

                    for (i32 i = 0; i < dimension; i++) {
                        std::cout << std::setw(4) << edgeWeights[index];
                        std::cout << " ";
                        index++;
                    }
                    std::cout << std::endl;
                }
            }

            if (heuristics != nullptr) {
                std::cout << "[C++] Heuristics: " << std::endl;

                i32 index = 0;
                for (i32 j = 0; j < dimension; j++) {
                    std::cout << std::setw(5) << j;
                    std::cout << ": ";

                    for (i32 i = 0; i < dimension; i++) {
                        std::cout << std::setw(4) << heuristics[index];
                        std::cout << " ";
                        index++;
                    }
                    std::cout << std::endl;
                }
            } 
        }
    };
}