
#pragma once    

#include "ComputePipelineFamily.hpp"

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

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout pipelineLayout           = VK_NULL_HANDLE;

    ComputePipelineFamily* iterative;
    ComputePipelineFamily* antColony;
};