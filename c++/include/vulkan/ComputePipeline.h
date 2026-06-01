
#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct ComputePipeline {
    VkPipelineLayout      pipelineLayout      = VK_NULL_HANDLE;
    VkPipeline            pipeline            = VK_NULL_HANDLE;
};

struct ComputePipelineFamily {
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    std::vector<ComputePipeline> pipelines;
};