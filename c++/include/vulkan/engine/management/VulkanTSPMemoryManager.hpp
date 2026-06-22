
#pragma once

#include "VulkanCore.hpp"
#include "VulkanTSPCommandBufferManager.hpp"
#include "ExitStatus.hpp"
#include "PushConstants.hpp"

#include <vulkan/vulkan.h>

struct StageData {
    void*        data   = nullptr;
    VkDeviceSize offset = 0;
    VkDeviceSize size   = 0;
};

struct BufferLayout {
    VkDeviceSize totalSize                  = 0;

    VkDeviceSize edgeWeightsOffset          = 0;
    VkDeviceSize edgeWeightsSize            = 0;

    VkDeviceSize heuristicsOffset           = 0;
    VkDeviceSize heuristicsSize             = 0;

    VkDeviceSize pheromonesOffset           = 0;
    VkDeviceSize pheromonesSize             = 0;
    
    VkDeviceSize visitedOffset              = 0;
    VkDeviceSize visitedSize                = 0;

    VkDeviceSize probabilisticWeightsOffset = 0;
    VkDeviceSize probabilisticWeightsSize   = 0;
    
    VkDeviceSize pathLengthsOffset          = 0;
    VkDeviceSize pathLengthsSize            = 0;

    VkDeviceSize bestPathOffset             = 0;
    VkDeviceSize bestPathSize               = 0;
    
};

class VulkanTSPMemoryManager {
public:
    VulkanTSPMemoryManager(const VulkanCore& vulkanCore, const VulkanTSPCommandBufferManager& vulkanCommandBufferManager);
    ~VulkanTSPMemoryManager();

    ExitStatus calculateBufferLayoutIterative(i32 dim);
    ExitStatus calculateBufferLayoutAntColony(i32 dim, i32 antCount);

    void stageAndCopyData(const std::vector<StageData>& stageData);

    const BufferLayout& getBufferLayout() const     { return bufferLayout; }
    VkBuffer            getMonolithicBuffer()       { return monolithic.buffer; }
    VkDeviceMemory      getMonolithicMemoryHandle() { return monolithic.allocated; }
    void*               getMonolithicMapped()       { return monolithic.mapped; }
    
private:
    void reserveMonolithicMemory();
    
    VkDeviceSize alignUp(VkDeviceSize size);
    u32 findOptimalMemoryType(u32 memoryTypeBits, VkMemoryPropertyFlags properties);

    const VkDeviceSize RESERVED_MEMORY_SIZE = 512 * 1024 * 1024; // 512 MiB

    const VulkanCore& vulkanCore;
    const VulkanTSPCommandBufferManager& vulkanCommandBufferManager;

    VkDeviceSize storageBufferAlignment;

    struct {
        VkDeviceMemory allocated;
        VkBuffer       buffer;
        void*          mapped;
    } monolithic;

    BufferLayout bufferLayout;
};