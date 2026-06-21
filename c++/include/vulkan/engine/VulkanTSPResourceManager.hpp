
#pragma once

#include "VulkanCore.hpp"

#include "VulkanTSPMemoryManager.hpp"
#include "VulkanTSPPipelineManager.hpp"
#include "VulkanTSPCommandBufferManager.hpp"

#include "TSPDescriptorSetBundle.hpp"
#include "ExitStatus.hpp"

#include <memory>
#include <iostream>

class VulkanTSPResourceManager {
public:
    VulkanTSPResourceManager(const VulkanCore& vulkanCore);
    ~VulkanTSPResourceManager();

    VulkanTSPMemoryManager&        getMemoryManager()        const { return *memoryManager; }
    VulkanTSPPipelineManager&      getPipelineManager()      const { return *pipelineManager; }
    VulkanTSPCommandBufferManager& getCommandBufferManager() const { return *commandBufferManager; }

private:
    std::unique_ptr<VulkanTSPPipelineManager>      pipelineManager;
    std::unique_ptr<VulkanTSPCommandBufferManager> commandBufferManager;
    std::unique_ptr<VulkanTSPMemoryManager>        memoryManager;
};