
#include "IterativeSolver.h"

#include <iostream>
#include <iomanip>
#include <cmath>
#include <Util.h>

IterativeSolver::~IterativeSolver() {
    delete[] indices;
    delete[] swaps;
}

ExitStatus IterativeSolver::prepareCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    if (tsp.dimension > 35) 
        // Dimension > 35 -> Permutable elements n >= 35
        // 2^128 = 3.4e38 -> Max permutation count that fits in a u128 (for progress)
        // n = 34: 34! = 3e38 < max
        // n = 35: 35! = 1e40 > max
        return ExitStatus::ERROR_MEMORY_LIMIT;
    
    indices = new u16[tsp.dimension + 1];
    swaps = new u16[tsp.dimension, 1];
    
    return ExitStatus::SUCCESS;
}

ExitStatus IterativeSolver::prepareGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    return ExitStatus::SUCCESS;
}

ExitStatus IterativeSolver::solveCPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    environment.progress.store(0);

    i64 length = 0;
    i64 minLength = 0;
    i32 n = tsp.dimension - 1;   

    indices[0] = 0;
    indices[tsp.dimension] = 0;

    solutionData.pathIndices[0] = 0;

    for (i32 i = 1; i < tsp.dimension; i++) {
        indices[i] = i;
        solutionData.pathIndices[i] = i;
        
        length += tsp.ew(indices[i - 1], indices[i]);
    }

    length += tsp.ew(0, indices[n]);
    minLength = length;

    // Heaps algorithm

    i32 its = 1e8 / tsp.dimension;
    u128 permutations = 1;
    u128 totalPermutations = Util::factorial<u128>(n);

    /*
    u64* totalPermutations_u64s = reinterpret_cast<u64*>(&totalPermutations);
    std::cout << std::hex << totalPermutations_u64s[1];
    std::cout << std::hex << totalPermutations_u64s[0] << std::endl;
    */
    
    i32 pos = 2;
    while (permutations < totalPermutations) {
        if (environment.interrupt)
            return ExitStatus::INTERRUPTED;
        
        double progress = 100 * static_cast<double>(permutations) / static_cast<double>(totalPermutations);
        environment.progress.store(static_cast<i32>(progress));

        u128 remaining = totalPermutations - permutations;
        if (its > remaining)
            its = remaining;
        
        for (i32 it = 0; it < its; it++) {
            if (swaps[pos] < pos) {
                if (pos % 2 == 1) {
                    length -= tsp.ew(indices[1], 0);
                    length -= tsp.ew(indices[1], indices[2]);

                    length -= tsp.ew(indices[pos], indices[pos - 1]);
                    length -= tsp.ew(indices[pos], indices[pos + 1]);
                    
                    std::swap(indices[1], indices[pos]);
                    
                    length += tsp.ew(indices[1], 0);
                    length += tsp.ew(indices[1], indices[2]);
                    
                    length += tsp.ew(indices[pos], indices[pos - 1]);
                    length += tsp.ew(indices[pos], indices[pos + 1]);
                } else {
                    i32 sw = swaps[pos];

                    length -= tsp.ew(indices[sw], indices[sw - 1]);
                    length -= tsp.ew(indices[sw], indices[sw + 1]);
                    
                    length -= tsp.ew(indices[pos], indices[pos - 1]);
                    length -= tsp.ew(indices[pos], indices[pos + 1]);
                    
                    std::swap(indices[sw], indices[pos]);

                    length += tsp.ew(indices[sw], indices[sw - 1]);
                    length += tsp.ew(indices[sw], indices[sw + 1]);

                    length += tsp.ew(indices[pos], indices[pos - 1]);
                    length += tsp.ew(indices[pos], indices[pos + 1]);
                }
                
                if (length < minLength) {
                    minLength = length;
                    
                    for (i32 i = 1; i < tsp.dimension; i++)
                        solutionData.pathIndices[i] = indices[i];
                }
                
                swaps[pos]++; 
                pos = 2;
            } else {
                swaps[pos] = 1;
                pos++;
            }
        }

        permutations += its;
    }

    environment.progress.store(100);
    solutionData.pathLength = minLength;
    
    return ExitStatus::SUCCESS;
}

ExitStatus IterativeSolver::solveGPU(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData) {
    return ExitStatus::ERROR_NOT_IMPLEMENTED;
}