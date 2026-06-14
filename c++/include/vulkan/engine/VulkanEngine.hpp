
#pragma once

#include "VulkanCore.hpp"

#include <vulkan/vulkan.h>

class VulkanEngine {
public:
    VulkanEngine(const VulkanCore& vulkanCore);
    ~VulkanEngine();

    VulkanEngine(const VulkanEngine&) = delete;
    VulkanEngine& operator=(const VulkanEngine&) = delete;

private:
    const VulkanCore& vulkanCore;

    void createTransferCommandPool();
    void createTransferFence();

    void createComputeCommandPool();
    void createComputeFence();

    VkFence         m_transferFence         = VK_NULL_HANDLE;
    VkCommandPool   m_transferCommandPool   = VK_NULL_HANDLE;
    VkCommandBuffer m_transferCommandBuffer = VK_NULL_HANDLE;

    VkFence         m_computeFence         = VK_NULL_HANDLE;
    VkCommandPool   m_computeCommandPool   = VK_NULL_HANDLE;
    VkCommandBuffer m_computeCommandBuffer = VK_NULL_HANDLE;
    
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout      m_pipelineLayout      = VK_NULL_HANDLE;
};