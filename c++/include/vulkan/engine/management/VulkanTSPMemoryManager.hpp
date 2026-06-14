
#pragma once

#include "VulkanCore.hpp"

#include "ExitStatus.hpp"
#include "TSPDescriptorSet.hpp"

#include <vulkan/vulkan.h>

class VulkanTSPMemoryManager {
public:
    VulkanTSPMemoryManager(const VulkanCore& vulkanCore);
    ~VulkanTSPMemoryManager();
private:
    void reserveMonolithicMemoryBuffer();
    
    ExitStatus bindIterativeBuffers(i32 dim);
    ExitStatus bindAntColonyBuffers(i32 dim, i32 antCount);
    
    VkDeviceSize alignUp(VkDeviceSize size);
    u32 findOptimalMemoryType(u32 memoryTypeBits, VkMemoryPropertyFlags properties);

    const VkDeviceSize RESERVED_MEMORY_SIZE = 512 * 1024 * 1024; // 512 MiB
    const VulkanCore& vulkanCore;   

    VkDeviceSize   storageBufferAlignment;
    
    VkDeviceMemory reservedMemory;
    VkBuffer       monolithicBuffer;

    std::unique_ptr<TSPDescriptorSet> descriptorSet;

    struct {
        VkDeviceSize edgeWeightsOffset = 0;
        VkDeviceSize edgeWeightsSize   = 0;

        VkDeviceSize heuristicsOffset  = 0;
        VkDeviceSize heuristicsSize    = 0;

        VkDeviceSize pheromonesOffset  = 0;
        VkDeviceSize pheromonesSize    = 0;
        
        VkDeviceSize visitedOffset     = 0;
        VkDeviceSize visitedSize       = 0;
    } activeProblemLayout;
};