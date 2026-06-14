
#pragma once

#include "types.hpp"

#include <vulkan/vulkan.h>

class TSPDescriptorSet {
public:
    TSPDescriptorSet();
    ~TSPDescriptorSet();

    VkDescriptorSet getDescriptorSet() { return descriptorSet; }

    u32 getEdgeWeightsBinding() { return edgeWeightsBinding; }
    u32 getHeuristicsBinding()  { return heuristicsBinding; }
    
    u32 ac_getPheromonesBinding() { return ac_pheromones; }
    u32 ac_getVisitedBinding() { return ac_visited; }

private:
    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorSet descriptorSet             = VK_NULL_HANDLE;
    
    // =================================
    //      Descriptor set bindings
    // =================================

    const u32 edgeWeightsBinding = 0;
    const u32 heuristicsBinding  = 1;

    const u32 ac_pheromones      = 2;
    const u32 ac_visited         = 3;
};