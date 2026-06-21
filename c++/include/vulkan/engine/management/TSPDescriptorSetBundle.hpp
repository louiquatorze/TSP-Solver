
#pragma once

#include "types.hpp"
#include "VulkanCore.hpp"

#include <vulkan/vulkan.h>

class TSPDescriptorSetBundle {
public:
    struct DescriptorBindingUpdate {
        i32 binding;
        VkDeviceSize offset;
        VkDeviceSize range;
    };

    TSPDescriptorSetBundle(const VulkanCore& vulkanCore);
    ~TSPDescriptorSetBundle();

    VkDescriptorSet       getDescriptorSet()       const { return descriptorSet; }
    VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }
    void bindBuffers(VkBuffer buffer, const std::vector<DescriptorBindingUpdate>& descriptorBindingUpdates) const;

    u32 getEdgeWeightsBinding()   const { return edgeWeightsBinding; }
    u32 getHeuristicsBinding()    const { return heuristicsBinding; }
    
    u32 ac_getPheromonesBinding() const { return ac_pheromonesBinding; }
    u32 ac_getVisitedBinding()    const { return ac_visitedBinding; }

private:
    void createDescriptorSetLayout();
    void allocateDescriptorSet();

    const VulkanCore& vulkanCore;

    VkDescriptorSetLayout descriptorSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool      descriptorPool      = VK_NULL_HANDLE;
    VkDescriptorSet       descriptorSet       = VK_NULL_HANDLE;
    
    // =================================
    //      Descriptor set bindings
    // =================================

    const u32 edgeWeightsBinding   = 0;
    const u32 heuristicsBinding    = 1;

    const u32 ac_pheromonesBinding = 2;
    const u32 ac_visitedBinding    = 3;

    const std::vector<u32> bindings ={
        edgeWeightsBinding,
        heuristicsBinding,

        ac_pheromonesBinding,
        ac_visitedBinding
    };
};