
#include "VulkanTSPResourceManager.hpp"

VulkanTSPResourceManager::VulkanTSPResourceManager(const VulkanCore& vulkanCore) :
    pipelineManager(std::make_unique<VulkanTSPPipelineManager>(vulkanCore)),
    memoryManager(std::make_unique<VulkanTSPMemoryManager>(vulkanCore))
{ }

VulkanTSPResourceManager::~VulkanTSPResourceManager() {
    std::cout << "[C++] Destructing Vulkan TSP manager" << std::endl;
}

ExitStatus VulkanTSPResourceManager::bindIterativeBuffers(i32 dim) {
    auto exitStatus = memoryManager->calculateBufferLayoutIterative(dim);

    if (exitStatus != ExitStatus::SUCCESS) {
        return exitStatus;
    }

    const auto& bufferLayout = memoryManager->getBufferLayout();
    const auto& descriptorSetBundle = pipelineManager->getDescriptorSetBundle();

    std::vector<DescriptorBindingUpdate> descriptorBindingUpdates(1);
    descriptorBindingUpdates[0].binding = descriptorSetBundle.getEdgeWeightsBinding();
    descriptorBindingUpdates[0].offset  = bufferLayout.edgeWeightsOffset;
    descriptorBindingUpdates[0].range   = bufferLayout.edgeWeightsSize;

    pipelineManager->bindBuffers(memoryManager->getBuffer(), descriptorBindingUpdates);

    return ExitStatus::SUCCESS;
}

ExitStatus VulkanTSPResourceManager::bindAntColonyBuffers(i32 dim, i32 antCount) {
    auto exitStatus = memoryManager->calculateBufferLayoutAntColony(dim, antCount);

    if (exitStatus != ExitStatus::SUCCESS) {
        return exitStatus;
    }

    const auto& bufferLayout = memoryManager->getBufferLayout();
    const auto& descriptorSetBundle = pipelineManager->getDescriptorSetBundle();

    std::vector<DescriptorBindingUpdate> descriptorBindingUpdates(3);
    
    descriptorBindingUpdates[0].binding = descriptorSetBundle.getHeuristicsBinding();
    descriptorBindingUpdates[0].offset  = bufferLayout.heuristicsOffset;
    descriptorBindingUpdates[0].range   = bufferLayout.heuristicsSize;
    
    descriptorBindingUpdates[1].binding = descriptorSetBundle.ac_getPheromonesBinding();
    descriptorBindingUpdates[1].offset  = bufferLayout.pheromonesOffset;
    descriptorBindingUpdates[1].range   = bufferLayout.pheromonesSize;
    
    descriptorBindingUpdates[2].binding = descriptorSetBundle.ac_getVisitedBinding();
    descriptorBindingUpdates[2].offset  = bufferLayout.visitedOffset;
    descriptorBindingUpdates[2].range   = bufferLayout.visitedSize;

    pipelineManager->bindBuffers(memoryManager->getBuffer(), descriptorBindingUpdates);

    return ExitStatus::SUCCESS;
}