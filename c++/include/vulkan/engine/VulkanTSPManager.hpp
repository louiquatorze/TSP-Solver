
#pragma once

#include "VulkanCore.hpp"

#include "VulkanTSPMemoryManager.hpp"
#include "VulkanTSPPipelineManager.hpp"
#include "TSPDescriptorSet.hpp"
#include "ExitStatus.hpp"

#include <memory>
#include <iostream>

class VulkanTSPManager {
public:
    VulkanTSPManager(const VulkanCore& vulkanCore) :
        pipelineManager(std::make_unique<VulkanTSPPipelineManager>(vulkanCore)),
        memoryManager(std::make_unique<VulkanTSPMemoryManager>(vulkanCore))
    { }

    ~VulkanTSPManager() {
        std::cout << "[C++] Destructing Vulkan TSP manager" << std::endl;
    }
private:
    std::unique_ptr<VulkanTSPMemoryManager> memoryManager;
    std::unique_ptr<VulkanTSPPipelineManager> pipelineManager;
};