
#pragma once    

#include "ComputePipeline.h"

#include <vulkan/vulkan.h>

class TSPPipelines {
public:
    TSPPipelines() = default;
    ~TSPPipelines() = default;

    void create(VkDevice device);
    void destroyAll(VkDevice device);

    ComputePipelineFamily& getIterativePipelineFamily();
    ComputePipelineFamily& getAntColonyPipelineFamily();
private:
    void createIterativePipelineFamily(VkDevice device);
    void createAntColonyPipelineFamily(VkDevice device);

    ComputePipelineFamily iterative;
    ComputePipelineFamily antColony;
};