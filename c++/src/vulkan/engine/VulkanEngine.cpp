
#include "VulkanEngine.hpp"

#include <iostream>

VulkanEngine::VulkanEngine(const VulkanCore& vulkanCore) : vulkanCore(vulkanCore) {
    createTransferCommandPool();
    createTransferFence();

    createComputeCommandPool();
    createComputeFence();

    std::cout << "[C++] Vulkan Engine successfully created.\n";   
}

VulkanEngine::~VulkanEngine() {
    const auto device = vulkanCore.getLogicalDevice();

    if (m_transferFence != VK_NULL_HANDLE) 
        vkDestroyFence(device, m_transferFence, nullptr);

    if (m_transferCommandPool != VK_NULL_HANDLE) 
        vkDestroyCommandPool(device, m_transferCommandPool, nullptr);


    if (m_computeFence != VK_NULL_HANDLE) 
        vkDestroyFence(device, m_computeFence, nullptr);

    if (m_computeCommandPool != VK_NULL_HANDLE) 
        vkDestroyCommandPool(device, m_computeCommandPool, nullptr);

    std::cout << "[C++] Vulkan Engine successfully destroyed.\n"; 
}

void VulkanEngine::createTransferCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.pNext = nullptr;
    poolInfo.queueFamilyIndex = vulkanCore.getTransferQueueFamilyIndex();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

    if (vkCreateCommandPool(vulkanCore.getLogicalDevice(), &poolInfo, nullptr, &m_transferCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create transfer command pool");
    }
}

void VulkanEngine::createTransferFence() {
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(vulkanCore.getLogicalDevice(), &fenceInfo, nullptr, &m_transferFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create transfer fence");
    }
}

void VulkanEngine::createComputeCommandPool() {
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.pNext = nullptr;
    poolInfo.queueFamilyIndex = vulkanCore.getComputeQueueFamilyIndex();
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(vulkanCore.getLogicalDevice(), &poolInfo, nullptr, &m_computeCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create compute command pool");
    }
}

void VulkanEngine::createComputeFence() {
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.pNext = nullptr;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateFence(vulkanCore.getLogicalDevice(), &fenceInfo, nullptr, &m_computeFence) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create compute fence");
    }
}