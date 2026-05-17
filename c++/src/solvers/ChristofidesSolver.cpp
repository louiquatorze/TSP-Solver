
#include "ChristofidesSolver.h"
#include "GraphUtil.h"

#include <vector>
#include <set>
#include <limits>

ChristofidesSolver::ChristofidesSolver(Environment& environment, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData_out)
    : TSPSolver(environment, algorithmSettings, tsp, solutionData_out)
{ }

ChristofidesSolver::~ChristofidesSolver() { }

ExitStatus ChristofidesSolver::prepareCPU() {
    return ExitStatus::SUCCESS;
}

ExitStatus ChristofidesSolver::prepareGPU() {
    return ExitStatus::SUCCESS;
}

ExitStatus ChristofidesSolver::solveCPU() {
    // Find minimum spanning tree
    std::vector<i32> mst = GraphUtil::minimumSpanningTree(tsp.edgeWeights, tsp.dimension);

    // Compute all node degrees from MST edges
    std::vector<i32> nodeDegrees(tsp.dimension, 0);
    for (i32 i = 1; i < tsp.dimension; i++) {
        nodeDegrees[i]++;
        nodeDegrees[mst[i]]++;
    }

    // Isolate the indices of all odd-degree nodes
    std::vector<i32> oddNodes;
    oddNodes.reserve(tsp.dimension);
    for (i32 i = 0; i < tsp.dimension; i++) {
        if (nodeDegrees[i] % 2 != 0) {
            oddNodes.push_back(i);
        }
    }
    i32 oddNodesCount = static_cast<i32>(oddNodes.size());

    // Match the odd nodes
    struct Edge {
        i32 i, j;
    };
    i32 edgeCount = (tsp.dimension - 1) + (oddNodesCount / 2);
    std::vector<Edge> edges;
    edges.reserve(edgeCount);

    // Add MST edges to our combined edge group
    for (i32 i = 1; i < tsp.dimension; i++) {
        edges.push_back(Edge{ i, mst[i] });
    }

    // Greedy Matching over remaining unmatched pools to guarantee valid even-degree geometry
    std::vector<bool> matched(oddNodesCount, false);
    for (i32 i = 0; i < oddNodesCount; i++) {
        if (matched[i]) continue;
        matched[i] = true;

        i32 u = oddNodes[i];
        i32 bestNeighborIdx = -1;
        i32 minWeight = INT32_MAX;

        for (i32 j = i + 1; j < oddNodesCount; j++) {
            if (matched[j]) continue;
            
            i32 v = oddNodes[j];
            i32 weight = tsp.edgeWeights[u * tsp.dimension + v];
            if (weight < minWeight) {
                minWeight = weight;
                bestNeighborIdx = j;
            }
        }

        if (bestNeighborIdx != -1) {
            matched[bestNeighborIdx] = true;
            edges.push_back(Edge{ u, oddNodes[bestNeighborIdx] });
        }
    }
    
    // Create our adjacency lists for the multigraph
    std::vector<std::vector<std::pair<i32, i32>>> adjacent(tsp.dimension);
    for (size_t i = 0; i < edges.size(); i++) {
        Edge& e = edges[i];
        adjacent[e.i].push_back({static_cast<i32>(i), e.j});
        adjacent[e.j].push_back({static_cast<i32>(i), e.i});
    }
    
    // Generate the Eulerian circuit (Hierholzer's algorithm)
    std::vector<bool> edgeUsed(edges.size(), false);
    std::vector<i32> stack;
    std::vector<i32> eulerianPath;
    eulerianPath.reserve(edges.size() + 1);

    stack.push_back(0);
    while (!stack.empty()) {
        i32 u = stack.back();

        bool foundEdge = false;
        while (!adjacent[u].empty()) {
            std::pair<i32, i32> adj = adjacent[u].back();
            adjacent[u].pop_back();

            if (!edgeUsed[adj.first]) {
                edgeUsed[adj.first] = true;
                stack.push_back(adj.second);
                foundEdge = true;
                break;
            }
        }

        if (!foundEdge) {
            eulerianPath.push_back(u);
            stack.pop_back();
        }
    }

    // Orient path back to its proper forward direction
    std::reverse(eulerianPath.begin(), eulerianPath.end());

    // Perform short-cutting to finalize our Hamiltonian tour
    std::vector<bool> visited(tsp.dimension, false);
    i32 outIdx = 0;
    for (i32 ep : eulerianPath) {
        if (visited[ep]) continue;
        
        solutionData_out.pathIndices[outIdx++] = ep;
        visited[ep] = true;
    }

    environment.progress.store(100);
    return ExitStatus::SUCCESS;
}

ExitStatus ChristofidesSolver::solveGPU() {
    return ExitStatus::ERROR_NOT_IMPLEMENTED;
}

void ChristofidesSolver::print() {
    std::cout << "[C++] --- ChristofidesSolver ---" << std::endl;
}