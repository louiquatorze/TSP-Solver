
#pragma once

#include <vulkan/vulkan.h>
#include <vector>

struct ComputePipelineFamily {
    std::vector<VkPipeline> pipelines;

    void destroy(VkDevice device) {
        for (VkPipeline p : pipelines) {
            vkDestroyPipeline(device, p, nullptr);
        }
    }
};