
#pragma once

#include "VulkanCore.hpp"

class VulkanTSPPipelineManager {
public:
    VulkanTSPPipelineManager(const VulkanCore& vulkanCore);
    ~VulkanTSPPipelineManager();

private:
    const VulkanCore& vulkanCore;   
};