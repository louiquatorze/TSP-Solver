
#pragma once

#include "types.hpp"

#include <vulkan/vulkan.h>
#include <vector>

class ComputePipelineFamily {
public:
    ComputePipelineFamily(i32 pipelineCount);
    void destroy(VkDevice device);

    std::vector<VkPipeline> pipelines;
};