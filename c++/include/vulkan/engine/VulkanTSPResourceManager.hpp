
#pragma once

#include "VulkanCore.hpp"

#include "VulkanTSPMemoryManager.hpp"
#include "VulkanTSPPipelineManager.hpp"
#include "TSPDescriptorSetBundle.hpp"
#include "ExitStatus.hpp"

#include <memory>
#include <iostream>

class VulkanTSPResourceManager {
public:
    VulkanTSPResourceManager(const VulkanCore& vulkanCore);
    ~VulkanTSPResourceManager();
    
    ExitStatus bindIterativeBuffers(i32 dim);
    ExitStatus bindAntColonyBuffers(i32 dim, i32 antCount);
private:
    std::unique_ptr<VulkanTSPMemoryManager> memoryManager;
    std::unique_ptr<VulkanTSPPipelineManager> pipelineManager;
};