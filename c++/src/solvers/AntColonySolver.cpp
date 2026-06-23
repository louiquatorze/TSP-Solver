
#include "AntColonySolver.hpp"
#include "NearestNeighbourSolver.hpp"
#include "PushConstants.hpp"

#include <iostream>
#include <iomanip>
#include <random>
#include <vector>
#include <chrono>
#include <cstring>

AntColonySolver::AntColonySolver(Environment& environment, VulkanContext& vulkanContext, AlgorithmSettings& algorithmSettings, TSP& tsp, SolutionData& solutionData_out, u32 analyticFlags)
    : TSPSolver(environment, vulkanContext, algorithmSettings, tsp, solutionData_out, analyticFlags)
{ }

AntColonySolver::~AntColonySolver() {
    delete[] visited;
    delete[] pheromones;
    delete[] probabilisticWeights;
    delete[] pathLenghts;
}

ExitStatus AntColonySolver::prepareCPU() {
    minPathLength = INT64_MAX;
    
    // Init random
    std::random_device rd;
    gen = std::mt19937(rd());
    nodeGen = std::uniform_int_distribution<i32>(1, tsp.dimension - 1);
    realGen = std::uniform_real_distribution<f32>(0.0f, 1.0f);
    
    // Init ants, pheromones, probabilisticWeights and pathLengths
    try {
        visited = new i32[algorithmSettings.antCount * tsp.dimension];
        pheromones = new f32[tsp.dimension * tsp.dimension];
        
        probabilisticWeights = new f32[tsp.dimension];
        pathLenghts = new i64[algorithmSettings.antCount];
    } catch (std::bad_alloc e) {
        std::cerr << "[C++ Error] Can't allocate memory" << std::endl;
        return ExitStatus::ERROR_MEMORY_LIMIT;
    }

    // Fill not visited with city indices, clear pathlengths
    i32* visitedStart = visited;
    i32* visitedEnd = visitedStart + tsp.dimension;
    for (i32 i = 0; i < algorithmSettings.antCount; i++) {
        std::iota(visitedStart, visitedEnd, 0);
        visitedStart = visitedEnd;
        visitedEnd += tsp.dimension;

        pathLenghts[i] = 0;
    }

    // Set ants starting positions
    i32 antsPerNode = algorithmSettings.antCount / tsp.dimension;
    i32 antIndex = 0;
    for (i32 i = 0; i < antsPerNode; i++) {
        for (i32 n = 0; n < tsp.dimension; n++)  {
            i32 index = antIndex++ * tsp.dimension;

            // Swap visited[0] of ant i with visited[n] which equals n 
            std::swap(visited[index], visited[index + n]);
        }
    }

    // Assign remaining ants to distinct, random cities
    std::vector<i32> nodeIndices(tsp.dimension);
    std::iota(nodeIndices.begin(), nodeIndices.end(), 0);
    std::shuffle(nodeIndices.begin(), nodeIndices.end(), gen);

    i32 remaining = algorithmSettings.antCount % tsp.dimension;
    i32 distributed = algorithmSettings.antCount - remaining;

    for (i32 i = 0; i < remaining; i++) {
        i32 index = (distributed + i) * tsp.dimension;
        std::swap(visited[index], visited[index + nodeIndices[i]]);
    }
    
    // Determine rough estimate for initial pheromone strength via NearestNeighbour

    i64 length;
    ExitStatus exitStatus = NearestNeighbourSolver::getPathLength(environment, tsp, length);

    if (exitStatus != ExitStatus::SUCCESS)
        return exitStatus;

    initialPheromoneStrength = 1.0f / (tsp.dimension * length);
    for (i32 i = 0; i < tsp.dimension * tsp.dimension; i++)
        pheromones[i] = initialPheromoneStrength;

    return ExitStatus::SUCCESS;
}

ExitStatus AntColonySolver::solveCPU() {
    environment.updateProgress(0);

    // Solve TSP
    for (i32 i = 0; i < algorithmSettings.iterations; i++) {
        if (environment.interrupt)
            return ExitStatus::INTERRUPTED;
        
        if (analyticFlags & Analytics::Progress)
            environment.updateProgress(100 * i / algorithmSettings.iterations);
            
        runAntColonyIteration();
        evaluateBestPathAndResetAnts();
    }

    solutionData_out.pathLength = minPathLength;
    environment.updateProgress(100);

    return ExitStatus::SUCCESS;
}

void AntColonySolver::runAntColonyIteration() {
    // Iterate through all steps
    for (i32 step = 1; step < tsp.dimension; step++) {

        // Iterate through ants
        for (i32 ant = 0; ant < algorithmSettings.antCount; ant++) {
            // Pick next node

            // Index of the start of the row in indices corresponding to the current ant
            i32 visitedRow = ant * tsp.dimension;

            // The current visited node
            i32 current = visited[visitedRow + step - 1];
            
            // Index of the start of the row in edgeWeights corresponding to the currently visited node
            i32 row = current * tsp.dimension;

            i32 nextVisitedIndex = visitedRow + step;
            f32 q = realGen(gen);
            
            if (q < algorithmSettings.exploitationProbability) {
                // Pick Greedy

                f32 maxScore = -1.0f;
                
                for (i32 notVisited = step; notVisited < tsp.dimension; notVisited++) {
                    i32 notVisitedIndex = visitedRow + notVisited;
                    i32 index = row + visited[notVisitedIndex];
                    f32 score = tsp.heuristics[index] * pheromones[index];

                    if (maxScore < score) {
                        maxScore = score;
                        nextVisitedIndex = notVisitedIndex;
                    }
                }
            } else {
                // Pick probabilistically

                f32 sumScores = 0.0f;

                for (i32 notVisited = step; notVisited < tsp.dimension; notVisited++) {
                    i32 notVisitedIndex = visitedRow + notVisited;
                    i32 index = row + visited[notVisitedIndex];
                    f32 score = tsp.heuristics[index] * pheromones[index];
                    
                    sumScores += score;
                    probabilisticWeights[notVisited] = score;
                }

                f32 r = realGen(gen) * sumScores;

                i32 nextIndex = step;
                while (nextIndex < tsp.dimension) {
                    r -= probabilisticWeights[nextIndex];
                    
                    if (r <= 0) 
                        break;
                        
                    nextIndex++;
                }

                nextVisitedIndex = visitedRow + nextIndex;
            }

            // Get next node to visit up front
            // Visited looks like
            // 10 5 2 8 6 1 4 3 7 9 11 50 21 ...
            // < visited | not visited >
            std::swap(visited[visitedRow + step], visited[nextVisitedIndex]);
            i32 next = visited[visitedRow + step];

            // Update local pheromone

            i32 i1 = current + next * tsp.dimension;
            i32 i2 = next + current * tsp.dimension;
            
            f32 p = (1.0f - algorithmSettings.localEvaporationRate) * pheromones[i1] + algorithmSettings.localEvaporationRate * initialPheromoneStrength;
            pheromones[i1] = p;
            pheromones[i2] = p;

            pathLenghts[ant] += tsp.edgeWeights[i1];
        }
    }

    i32 visitedRow = 0;
    for (i32 ant = 0; ant < algorithmSettings.antCount; ant++) {
        i32 nextVisitedRow = visitedRow + tsp.dimension;

        pathLenghts[ant] += tsp.ew(visited[visitedRow], visited[nextVisitedRow - 1]);
        visitedRow = nextVisitedRow;
    }
}

void AntColonySolver::evaluateBestPathAndResetAnts() {
    i32 minPathLengthAnt = 0;
    for (i32 i = 1; i < algorithmSettings.antCount; i++) {
        i32 worseAnt;

        if (pathLenghts[i] < pathLenghts[minPathLengthAnt]) {
            worseAnt = minPathLengthAnt;
            minPathLengthAnt = i;
        } else {
            worseAnt = i;
        }
    
        pathLenghts[worseAnt] = 0;

        // New start index for the worse ant
        i32 swapIndex = nodeGen(gen);
        i32 visitedRow = worseAnt * tsp.dimension;
        std::swap(visited[visitedRow], visited[visitedRow + swapIndex]);
    }

    f32 pheromoneSecretion = 1.0f / pathLenghts[minPathLengthAnt];
    i32 index = tsp.dimension * minPathLengthAnt;

    auto setPheromones = [&](i32 l, i32 c) {
        i32 i1 = l * tsp.dimension + c;
        i32 i2 = c * tsp.dimension + l;

        f32 p = (1.0f - algorithmSettings.globalEvaporationRate) * pheromones[i1] + algorithmSettings.globalEvaporationRate * pheromoneSecretion;
        
        pheromones[i1] = p;
        pheromones[i2] = p;
    };

    if (pathLenghts[minPathLengthAnt] < minPathLength) {
        minPathLength = pathLenghts[minPathLengthAnt];
        solutionData_out.pathIndices[0] = visited[index++];

        for (i32 i = 1; i < tsp.dimension; i++) {
            solutionData_out.pathIndices[i] = visited[index];

            setPheromones(visited[index - 1], visited[index]);
            index++;
        } 
    } else {
        index++;
        for (i32 i = 1; i < tsp.dimension; i++) {
            setPheromones(visited[index - 1], visited[index]);
            index++;
        } 
    }

    // Apply restart to final ant

    pathLenghts[minPathLengthAnt] = 0;

    i32 swapIndex = nodeGen(gen);
    i32 visitedRow = minPathLengthAnt * tsp.dimension;
    std::swap(visited[visitedRow], visited[visitedRow + swapIndex]);
}

ExitStatus AntColonySolver::prepareGPU() {
    auto exitStatus = prepareCPU();

    if (exitStatus != ExitStatus::SUCCESS) {
        return exitStatus;
    }

    auto& ressourceManager = vulkanContext.getResourceManager();

    auto& memoryManager = ressourceManager.getMemoryManager();
    auto& pipelineManager = ressourceManager.getPipelineManager();
    auto& commandBufferManager = ressourceManager.getCommandBufferManager();

    ////////////////////////////////////
    /// Manage GPU memory transferal ///
    ////////////////////////////////////

    // Calculate the buffer layout
    exitStatus = memoryManager.calculateBufferLayoutAntColony(tsp.dimension, algorithmSettings.antCount);

    if (exitStatus != ExitStatus::SUCCESS) {
        return exitStatus;
    }

    const auto& bufferLayout = memoryManager.getBufferLayout();
    const auto& descriptorSetBundle = pipelineManager.getDescriptorSetBundle();

    // Stage the data and copy it into dedicated VRAM
    StageData edgeWeightsStageData{};
    edgeWeightsStageData.data   = static_cast<void*>(tsp.edgeWeights);
    edgeWeightsStageData.offset = bufferLayout.edgeWeightsOffset;
    edgeWeightsStageData.size   = bufferLayout.edgeWeightsSize;
    
    StageData heuristicsStageData{};
    heuristicsStageData.data   = static_cast<void*>(tsp.heuristics);
    heuristicsStageData.offset = bufferLayout.heuristicsOffset;
    heuristicsStageData.size   = bufferLayout.heuristicsSize;
    
    StageData pheromonesStageData{};
    pheromonesStageData.data   = static_cast<void*>(pheromones);
    pheromonesStageData.offset = bufferLayout.pheromonesOffset;
    pheromonesStageData.size   = bufferLayout.pheromonesSize;
    
    StageData visitedStageData{};
    visitedStageData.data   = static_cast<void*>(visited);
    visitedStageData.offset = bufferLayout.visitedOffset;
    visitedStageData.size   = bufferLayout.visitedSize;
    
    std::vector<u32> initialBestPath((tsp.dimension + 1), 0);
    initialBestPath[0] = UINT32_MAX;
    StageData bestPathStageData{};
    bestPathStageData.data   = static_cast<void*>(initialBestPath.data());
    bestPathStageData.offset = bufferLayout.bestPathOffset;
    bestPathStageData.size   = bufferLayout.bestPathSize;

    std::vector<StageData> stageData ={ 
        edgeWeightsStageData, 
        heuristicsStageData, 
        pheromonesStageData, 
        visitedStageData,
        bestPathStageData
    };
    memoryManager.stageAndCopyData(stageData);

    // Update the descriptor bindings to match the new buffer layout
    TSPDescriptorSetBundle::DescriptorBindingUpdate edgeWeightsUpdate{};
    edgeWeightsUpdate.binding = descriptorSetBundle.getEdgeWeightsBinding();
    edgeWeightsUpdate.offset  = bufferLayout.edgeWeightsOffset;
    edgeWeightsUpdate.range   = bufferLayout.edgeWeightsSize;

    TSPDescriptorSetBundle::DescriptorBindingUpdate heuristicsUpdate{};
    heuristicsUpdate.binding = descriptorSetBundle.getHeuristicsBinding();
    heuristicsUpdate.offset  = bufferLayout.heuristicsOffset;
    heuristicsUpdate.range   = bufferLayout.heuristicsSize;

    TSPDescriptorSetBundle::DescriptorBindingUpdate pheromonesUpdate{};
    pheromonesUpdate.binding = descriptorSetBundle.ac_getPheromonesBinding();
    pheromonesUpdate.offset  = bufferLayout.pheromonesOffset;
    pheromonesUpdate.range   = bufferLayout.pheromonesSize;
    
    TSPDescriptorSetBundle::DescriptorBindingUpdate visitedUpdate{};
    visitedUpdate.binding = descriptorSetBundle.ac_getVisitedBinding();
    visitedUpdate.offset  = bufferLayout.visitedOffset;
    visitedUpdate.range   = bufferLayout.visitedSize;

    TSPDescriptorSetBundle::DescriptorBindingUpdate probabilisticWeightsUpdate{};
    probabilisticWeightsUpdate.binding = descriptorSetBundle.ac_getProbabilisticWeightsBinding();
    probabilisticWeightsUpdate.offset  = bufferLayout.probabilisticWeightsOffset;
    probabilisticWeightsUpdate.range   = bufferLayout.probabilisticWeightsSize;

    TSPDescriptorSetBundle::DescriptorBindingUpdate pathLengthsUpdate{};
    pathLengthsUpdate.binding = descriptorSetBundle.ac_getPathLengthsBinding();
    pathLengthsUpdate.offset  = bufferLayout.pathLengthsOffset;
    pathLengthsUpdate.range   = bufferLayout.pathLengthsSize;
    
    TSPDescriptorSetBundle::DescriptorBindingUpdate bestPathUpdate{};
    bestPathUpdate.binding = descriptorSetBundle.ac_getBestPathBinding();
    bestPathUpdate.offset  = bufferLayout.bestPathOffset;
    bestPathUpdate.range   = bufferLayout.bestPathSize;
    
    std::vector<TSPDescriptorSetBundle::DescriptorBindingUpdate> descriptorBindingUpdates ={
        edgeWeightsUpdate,
        heuristicsUpdate, 
        pheromonesUpdate, 
        visitedUpdate,
        probabilisticWeightsUpdate,
        pathLengthsUpdate,
        bestPathUpdate
    };
    descriptorSetBundle.bindBuffers(memoryManager.getMonolithicBuffer(), descriptorBindingUpdates);

    ///////////////////////////
    /// Bake command buffer ///
    ///////////////////////////

    commandBufferManager.allocateSolveCommand(); 
    VkCommandBuffer& solveCommand = commandBufferManager.getSolveCommand();

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext            = nullptr;
    beginInfo.flags            = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT; 
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(solveCommand, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin command buffer");
    }

    // Calculate group count so that 1 ant : 1 thread
    u32 simGroupCountX = (algorithmSettings.antCount + 31) / 32;

    VkPipelineLayout pipelineLayout = pipelineManager.getPipelineLayout();
    VkDescriptorSet descriptorSet = descriptorSetBundle.getDescriptorSet(); 
    
    vkCmdBindDescriptorSets(solveCommand, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);

    auto now = std::chrono::high_resolution_clock::now();
    auto duration = now.time_since_epoch();
    u32 timeSeed = static_cast<uint32_t>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());

    PushConstants pcs{};
    pcs.dim                      = tsp.dimension;
    pcs.seed                     = timeSeed;  

    pcs.antCount                 = algorithmSettings.antCount;
    pcs.pheromonePower           = algorithmSettings.pheromonePower;
    pcs.exploitationProbability  = algorithmSettings.exploitationProbability;
    pcs.localEvaporation         = algorithmSettings.localEvaporationRate;
    pcs.globalEvaporation        = algorithmSettings.globalEvaporationRate;
    pcs.initialPheromoneStrength = initialPheromoneStrength;

    for (i32 i = 0; i < algorithmSettings.iterations; i++) {
        pcs.iteration = i;

        // Ant simulation stage
        vkCmdBindPipeline(solveCommand, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineManager.getPipeline(VulkanTSPPipelineManager::PipelineType::AntColonySimulateAnts));
        vkCmdPushConstants(solveCommand, pipelineManager.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PushConstants), &pcs);
        vkCmdDispatch(solveCommand, simGroupCountX, 1, 1);
        
        VkMemoryBarrier simulationBarrier{};
        simulationBarrier.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        simulationBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        simulationBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            solveCommand,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, // Src stage
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, // Dst stage
            0, 1, &simulationBarrier, 0, nullptr, 0, nullptr
        );

        // Reward stage
        vkCmdBindPipeline(solveCommand, VK_PIPELINE_BIND_POINT_COMPUTE, pipelineManager.getPipeline(VulkanTSPPipelineManager::PipelineType::AntColonyRewardBestPath));
        vkCmdPushConstants(solveCommand, pipelineManager.getPipelineLayout(), VK_SHADER_STAGE_COMPUTE_BIT, 0, sizeof(PushConstants), &pcs);
        vkCmdDispatch(solveCommand, 1, 1, 1);

        // Bridge the pipeline execution
        VkMemoryBarrier rewardBarrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
        rewardBarrier.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
        rewardBarrier.pNext         = nullptr;
        rewardBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
        rewardBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(
            solveCommand,
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, // Source stage
            VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, // Destination stage
            0,                                    // Dependency flags
            1, &rewardBarrier,                    // Global memory barriers
            0, nullptr,                           // Buffer memory barriers
            0, nullptr                            // Image memory barriers
        );
    }

    VkMemoryBarrier finalBarrier{ VK_STRUCTURE_TYPE_MEMORY_BARRIER };
    finalBarrier.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    finalBarrier.pNext         = nullptr;
    finalBarrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    finalBarrier.dstAccessMask = VK_ACCESS_HOST_READ_BIT;

    vkCmdPipelineBarrier(
        solveCommand, 
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT, 
        VK_PIPELINE_STAGE_HOST_BIT, 
        0, 1, &finalBarrier, 0, nullptr, 0, nullptr
    );

    // Bake the command buffer 
    if (vkEndCommandBuffer(solveCommand) != VK_SUCCESS) {
        throw std::runtime_error("Failed to bake command buffer");
    }

    return ExitStatus::SUCCESS;
}

ExitStatus AntColonySolver::solveGPU() {    
    const auto& device = vulkanContext.getCore().getLogicalDevice();
    auto& resourceManager = vulkanContext.getResourceManager();
    auto& memoryManager = resourceManager.getMemoryManager();
    auto& pipelineManager = resourceManager.getPipelineManager();

    auto& bufferLayout = memoryManager.getBufferLayout();
    auto pipelineLayout = pipelineManager.getPipelineLayout();
    auto computeQueue = vulkanContext.getCore().getComputeQueue();
    VkCommandBuffer solveCommand = resourceManager.getCommandBufferManager().getSolveCommand();

    auto antSimPipeline = pipelineManager.getPipeline(VulkanTSPPipelineManager::PipelineType::AntColonySimulateAnts);
    auto rewardPathsPipeline = pipelineManager.getPipeline(VulkanTSPPipelineManager::PipelineType::AntColonyRewardBestPath);
    
    VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers    = &solveCommand;

    if (vkQueueSubmit(computeQueue, 1, &submitInfo, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit the solve command to the vulkan compute queue");
    }

    if (vkQueueWaitIdle(computeQueue) != VK_SUCCESS) {
        throw std::runtime_error("Failed to wait for command queue");
    }

    struct BestPath {
        u32 length;
        u32 indices[];
    };

    auto* basePointer   = static_cast<u8*>(memoryManager.getMonolithicMapped()); 
    auto* bestPathSlice = reinterpret_cast<BestPath*>(basePointer + bufferLayout.bestPathOffset);
    auto* edgeWeightsSlice = reinterpret_cast<u32*>(basePointer + bufferLayout.edgeWeightsOffset);
    
    environment.updateProgress(100);
    solutionData_out.pathLength = bestPathSlice->length;
    std::memcpy(solutionData_out.pathIndices, bestPathSlice->indices, tsp.dimension * sizeof(u32));

    /*
    std::cout << "PathIndices: ";
    for (i32 i = 0; i < tsp.dimension; i++) {
        std::cout << bestPathSlice->indices[i] << " | ";
    }
    std::cout << std::endl;

    // Test

    auto* heuristicsSlice     = reinterpret_cast<f32*>(basePointer + bufferLayout.heuristicsOffset);
    auto* pheromonesSlice     = reinterpret_cast<f32*>(basePointer + bufferLayout.pheromonesOffset);
    auto* visitedSlice        = reinterpret_cast<u32*>(basePointer + bufferLayout.visitedOffset);
    auto* probabilisticsSlice = reinterpret_cast<f32*>(basePointer + bufferLayout.probabilisticWeightsOffset);

    std::cout << "EdgeWeights   = " << edgeWeightsSlice[0] << " | ";
    std::cout << "Heuristics    = " << heuristicsSlice[0] << " | ";
    std::cout << "Pheromones    = " << pheromonesSlice[0] << " | ";
    std::cout << "Visited       = " << visitedSlice[0] << " | ";
    std::cout << "Probabilistic = " << probabilisticsSlice[0] << " | ";
    std::cout << "PathLength    = " << pathLengthsSlice[0] << " | ";
    std::cout << "BestPath      = " << bestPathSlice[0] << std::endl;
    */
   
    return ExitStatus::SUCCESS;
}

void AntColonySolver::print() {
    std::cout << "[C++] --- AntColonySolver ---" << std::endl;
    
    std::cout << "[C++] Number of ants: " << algorithmSettings.antCount << std::endl;
    std::cout << "[C++] Number of iterations: " << algorithmSettings.iterations << std::endl;
    
    std::cout << std::endl;

    std::cout << "[C++] Beta: " << algorithmSettings.beta << std::endl;
    std::cout << "[C++] Pheromone power: " << algorithmSettings.pheromonePower << std::endl;
    std::cout << "[C++] Exploitation probability: " << algorithmSettings.exploitationProbability << std::endl;
    std::cout << "[C++] Local pheromone evaporation coefficient: " << algorithmSettings.localEvaporationRate << std::endl;
    std::cout << "[C++] Global pheromone evaporation coefficient: " << algorithmSettings.globalEvaporationRate << std::endl;
}

void AntColonySolver::printVisited() {
    std::cout << "[C++] Visited:" << std::endl;

    i32 index = 0;
    for (i32 i = 0; i < algorithmSettings.antCount; i++) {
        std::cout << "[C++] Ant " << std::setw(3) << i << ": ";
        for (i32 j = 0; j < tsp.dimension; j++)
            std::cout << std::setw(3) << visited[index++] << " ";
        std::cout << std::endl;
    }
}

inline void printSquare(i32 colorCode) {
    std::cout << "\033[48;5;" << colorCode << "m  \033[0m";
}

void AntColonySolver::printPheromones() {
    std::cout << "[C++] Pheromones:" << std::endl;

    const i32 min = 234;
    const i32 max = 255;
    const i32 diff = max - min;

    const f32 maxPheromoneStrength = initialPheromoneStrength * 4.0f;
    const f32 pheromoneFactor = 1.0f / (maxPheromoneStrength - initialPheromoneStrength);

    i32 index = 0;
    for (i32 i = 0; i < tsp.dimension; i++) {
        for (i32 j = 0; j < tsp.dimension; j++) {
            f32 normalized = (pheromones[i * tsp.dimension + j] - initialPheromoneStrength) * pheromoneFactor;
            normalized = std::max(0.0f, std::min(1.0f, normalized));

            i32 colorCode = static_cast<i32>(min + normalized * diff);
            printSquare(colorCode);
        }
        std::cout << std::endl;
    }
}
