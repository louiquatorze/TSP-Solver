
#include "VulkanTSPCommandBufferManager.hpp"

#include <iostream>

VulkanTSPCommandBufferManager::VulkanTSPCommandBufferManager(const VulkanCore& vulkanCore) :
    vulkanCore(vulkanCore) 
{
    initializeCommandPools();
}

VulkanTSPCommandBufferManager::~VulkanTSPCommandBufferManager() {
    std::cout << "[C++] Destructing vulkan command buffer manager" << std::endl;
    
    const auto device = vulkanCore.getLogicalDevice();    

    if (solveCommandSlot != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(device, persistentCommandPool, 1, &solveCommandSlot);
    }

    if (persistentCommandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, persistentCommandPool, nullptr);
    }
    
    if (transientCommandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(device, transientCommandPool, nullptr);
    }
}

VkCommandBuffer VulkanTSPCommandBufferManager::beginSingleTimeCommand() const {
    const auto device = vulkanCore.getLogicalDevice();    
    
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext              = nullptr;
    allocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool        = transientCommandPool;
    allocateInfo.commandBufferCount = 1;

    VkCommandBuffer command;
    if (vkAllocateCommandBuffers(device, &allocateInfo, &command) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate single time command buffer");
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.pNext            = nullptr;
    beginInfo.flags            = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    if (vkBeginCommandBuffer(command, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin single time command");
    }
    
    return command;
}

void VulkanTSPCommandBufferManager::endSingleTimeCommand(VkCommandBuffer command) const {
    if (command == VK_NULL_HANDLE) return;

    const auto device = vulkanCore.getLogicalDevice();
    const auto queue  = vulkanCore.getComputeQueue();

    // End the command buffer
    if (vkEndCommandBuffer(command) != VK_SUCCESS) {
        throw std::runtime_error("Failed to end single time command buffer");
    }

    // Submit the command to queue
    VkSubmitInfo submitInfo{};
    submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext                = nullptr;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = &command;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores    = nullptr;
    submitInfo.waitSemaphoreCount   = 0;
    submitInfo.pWaitSemaphores      = nullptr;
    submitInfo.pWaitDstStageMask    = nullptr;

    if (vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit single time command to queue");
    }

    // Wait for the command to be processed
    if (vkQueueWaitIdle(queue) != VK_SUCCESS) {
        throw std::runtime_error("Failed to wait for queue idle during single time command");
    }
    
    // Free the command buffer
    vkFreeCommandBuffers(device, transientCommandPool, 1, &command);
}

void VulkanTSPCommandBufferManager::initializeCommandPools() {
    // TODO implement transferQueue/computeQueue ownership transfer
    // For now transferQueue = computeQueue

    const auto device = vulkanCore.getLogicalDevice();
    const auto queueFamilyIndex = vulkanCore.getComputeQueueFamilyIndex(); // TODO see above
    
    // Create persistent command pool
    VkCommandPoolCreateInfo persistentCommandPoolInfo{};
    persistentCommandPoolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    persistentCommandPoolInfo.pNext            = nullptr;
    persistentCommandPoolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    persistentCommandPoolInfo.queueFamilyIndex = queueFamilyIndex; // TODO see above

    if (vkCreateCommandPool(device, &persistentCommandPoolInfo, nullptr, &persistentCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }
    
    // Create transient command pool
    VkCommandPoolCreateInfo transientCommandPoolInfo{};
    transientCommandPoolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    transientCommandPoolInfo.pNext            = nullptr;
    transientCommandPoolInfo.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    transientCommandPoolInfo.queueFamilyIndex = queueFamilyIndex; // TODO see above

    if (vkCreateCommandPool(device, &transientCommandPoolInfo, nullptr, &transientCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool");
    }
}

void VulkanTSPCommandBufferManager::allocateSolveCommand() {
    const auto device = vulkanCore.getLogicalDevice();

    // Clear out old solver command buffer
    if (solveCommandSlot != VK_NULL_HANDLE) {
        vkFreeCommandBuffers(device, persistentCommandPool, 1, &solveCommandSlot);
        solveCommandSlot = VK_NULL_HANDLE;
    }

    // Allocate new solver command buffer
    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.pNext              = nullptr;
    allocateInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool        = persistentCommandPool;
    allocateInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(device, &allocateInfo, &solveCommandSlot) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate solve command buffer slot");
    }
}