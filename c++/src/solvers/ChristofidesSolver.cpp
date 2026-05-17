
#include "ChristofidesSolver.h"
#include "GraphUtil.h"

#include <vector>
#include <set>

ChristofidesSolver::ChristofidesSolver(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData_out)
    : TSPSolver(environment, algorithmSettings, tsp, solutionData_out)
{ }

ExitStatus ChristofidesSolver::prepareCPU() {
    return ExitStatus::SUCCESS;
}

ExitStatus ChristofidesSolver::prepareGPU() {
    return ExitStatus::SUCCESS;
}

ExitStatus ChristofidesSolver::solveCPU() {
    // Find minimum spanning tree
    std::vector<i32> mst = GraphUtil::minimumSpanningTree(tsp.edgeWeights, tsp.dimension);

    // Get all nodes with odd degree
    std::vector<i32> nodeDegrees(tsp.dimension, 1);
    nodeDegrees[0] = 0;

    for (i32 i = 1; i < tsp.dimension; i++)
        nodeDegrees[mst[i]]++;

    // Reuse nodeDegrees for indices of odd nodes
    i32 oddNodesCount = 0;
    for (i32 i = 0; i < tsp.dimension; i++) {
        if (nodeDegrees[i] % 2 != 0) 
            nodeDegrees[oddNodesCount++] = i;
    }

    // Swap in place so that nodeDegrees is ordered by pairs
    for (i32 i = 0; i < oddNodesCount - 2; i += 2) {
        i32 idx1 = nodeDegrees[i];
        i32 row = idx1 * tsp.dimension;

        i32 min = i + 1;
        i32 minWeight = tsp.edgeWeights[row + nodeDegrees[min]];

        for (i32 j = i + 2; j < oddNodesCount; j++) {
            i32 weight = tsp.edgeWeights[row + nodeDegrees[j]];

            if (weight < minWeight) {
                minWeight = weight;
                min = j;
            }
        }

        std::swap(nodeDegrees[i + 1], nodeDegrees[min]);
    }

    // Merge nodeDegrees with minimum spanning tree

    struct Edge {
        i32 i, j;
    };

    i32 edgeCount = tsp.dimension - 1 + oddNodesCount / 2;
    std::vector<Edge> edges(edgeCount);

    i32 ie = 0;

    for (i32 i = 1; i < tsp.dimension; i++)
        edges[ie++] = Edge{ i, mst[i] };

    for (i32 i = 0; i < oddNodesCount; i += 2)
        edges[ie++] = Edge{ nodeDegrees[i], nodeDegrees[i + 1] };
    
    // Create eulerian path

    std::vector<std::vector<i32>> adjacent(tsp.dimension);
    
    for (Edge e : edges) {
        adjacent[e.i].push_back(e.j);
        adjacent[e.j].push_back(e.i);
    }
    
    std::vector<bool> edgeUsed(edges.size(), false);
    std::vector<i32> eulerianPath;
    std::vector<i32> stack;
    stack.push_back(0);

    while (!stack.empty()) {
        i32 u = stack.back();
        bool foundEdge = false;

        for (i32 i : adjacent[u]) {
                
        }
    }

    environment.progress.store(100);

    return ExitStatus::ERROR_NOT_IMPLEMENTED;
}

ExitStatus ChristofidesSolver::solveGPU() {
    return ExitStatus::ERROR_NOT_IMPLEMENTED;
}

void ChristofidesSolver::print() {
    std::cout << "[C++] --- ChristofidesSolver ---" << std::endl;
}