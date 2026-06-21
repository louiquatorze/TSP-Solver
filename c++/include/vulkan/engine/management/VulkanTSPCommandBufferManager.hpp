
#pragma once

#include "VulkanCore.hpp"

#include <vulkan/vulkan.h>

class VulkanTSPCommandBufferManager {
public:
    VulkanTSPCommandBufferManager(const VulkanCore& vulkanCore);
    ~VulkanTSPCommandBufferManager();
    
    VulkanTSPCommandBufferManager(const VulkanTSPCommandBufferManager&) = delete;
    VulkanTSPCommandBufferManager& operator=(const VulkanTSPCommandBufferManager&) = delete;
    
    VkCommandBuffer beginSingleTimeCommand() const;
    void endSingleTimeCommand(VkCommandBuffer command) const;

    void allocateSolveCommand();
    VkCommandBuffer& getSolveCommand() { return solveCommandSlot; };

private:
    void initializeCommandPools();

    const VulkanCore& vulkanCore;

    VkCommandPool persistentCommandPool = VK_NULL_HANDLE;
    VkCommandPool transientCommandPool  = VK_NULL_HANDLE;
    
    // Individual solvers write into this slot in preperation and call it in execution
    VkCommandBuffer solveCommandSlot    = VK_NULL_HANDLE;
};