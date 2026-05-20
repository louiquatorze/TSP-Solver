
#include "GraphUtil.h"

#include <vector>
#include <random>
#include <iostream>

std::vector<i32> GraphUtil::minimumSpanningTree(u32* edgeWeights, i32 dimension) {
    std::vector<i32> parent(dimension);

    // The visited/not visited nodes
    std::vector<i32> visited(dimension);

    // The current minimum dist of the nodes from the root
    std::vector<i32> dists(dimension);

    // Init all nodes
    i32 visitedCount = 1;
    parent[0] = -1;
    visited[0] = 0;
    dists[0] = 0;

    for (i32 i = 1; i < dimension; i++) {
        visited[i] = i;
        dists[i] = edgeWeights[i];
        parent[i] = 0;
    }

    while (visitedCount < dimension) {
        // Pick node with min distance

        i32 min = visitedCount;
        i32 minDist = dists[visited[visitedCount]];
        for (i32 nv = visitedCount + 1; nv < dimension; nv++) {
            i32 dist = dists[visited[nv]];

            if (dist < minDist) {
                min = nv;
                minDist = dist;
            }
        }

        std::swap(visited[visitedCount], visited[min]);

        i32 minIndex = visited[visitedCount];
        i32 row = minIndex * dimension;

        visitedCount++;

        for (i32 nv = visitedCount; nv < dimension; nv++) {
            i32 index = visited[nv];
            i32 dist = edgeWeights[row + index];
            
            if (dist < dists[index]) {
                dists[index] = dist;
                parent[index] = minIndex;
            }
        }
    }

    return parent;
}