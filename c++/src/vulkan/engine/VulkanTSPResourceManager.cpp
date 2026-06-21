
#include "VulkanTSPResourceManager.hpp"

VulkanTSPResourceManager::VulkanTSPResourceManager(const VulkanCore& vulkanCore) :
    pipelineManager     (std::make_unique<VulkanTSPPipelineManager>     (vulkanCore)),
    commandBufferManager(std::make_unique<VulkanTSPCommandBufferManager>(vulkanCore)),
    memoryManager       (std::make_unique<VulkanTSPMemoryManager>       (vulkanCore, *commandBufferManager))
{ }

VulkanTSPResourceManager::~VulkanTSPResourceManager() {
    std::cout << "[C++] Destructing Vulkan TSP manager" << std::endl;
}