
#pragma once

#include "VulkanCore.hpp"
#include "ExitStatus.hpp"

#include <vulkan/vulkan.h>

struct BufferLayout {
    VkDeviceSize edgeWeightsOffset = 0;
    VkDeviceSize edgeWeightsSize   = 0;

    VkDeviceSize heuristicsOffset  = 0;
    VkDeviceSize heuristicsSize    = 0;

    VkDeviceSize pheromonesOffset  = 0;
    VkDeviceSize pheromonesSize    = 0;
    
    VkDeviceSize visitedOffset     = 0;
    VkDeviceSize visitedSize       = 0;
};


class VulkanTSPMemoryManager {
public:
    VulkanTSPMemoryManager(const VulkanCore& vulkanCore);
    ~VulkanTSPMemoryManager();

    ExitStatus calculateBufferLayoutIterative(i32 dim);
    ExitStatus calculateBufferLayoutAntColony(i32 dim, i32 antCount);

    const BufferLayout& getBufferLayout() const { return bufferLayout; }
    VkBuffer getBuffer() { return monolithicMemory.buffer; }
    
private:
    void reserveMonolithicMemory();
    
    VkDeviceSize alignUp(VkDeviceSize size);
    u32 findOptimalMemoryType(u32 memoryTypeBits, VkMemoryPropertyFlags properties);

    const VkDeviceSize RESERVED_MEMORY_SIZE = 512 * 1024 * 1024; // 512 MiB
    const VulkanCore& vulkanCore;   

    VkDeviceSize   storageBufferAlignment;
    
    struct {
        VkDeviceMemory allocated;
        VkBuffer       buffer;
    } monolithicMemory;

    BufferLayout bufferLayout;
};