
#pragma once    

#include "ComputePipelineFamily.h"

#include <vulkan/vulkan.h>

class TSPPipelines {
public:
    TSPPipelines() = default;
    ~TSPPipelines() = default;

    void create(VkDevice device);
    void destroyAll(VkDevice device);

    VkPipeline& getIterativePipeline();
    VkPipeline& getAntColonyPipeline();
private:
    void declareBuffers();
    
    void createIterativePipelineFamily(VkDevice device);
    void createAntColonyPipelineFamily(VkDevice device);

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout           = VK_NULL_HANDLE;

    ComputePipelineFamily iterative;
    ComputePipelineFamily antColony;
};