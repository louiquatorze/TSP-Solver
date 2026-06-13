
#pragma once

#include "DataBuffer.h"
#include "TSPPipelines.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <optional>
#include <types.h>
#include <memory>

class VulkanContext {
public:
    VulkanContext();
    ~VulkanContext();

    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;

    VkDevice getLogicalDevice() const { return m_device; }
    VkPhysicalDevice getPhysicalDevice() const { return m_physicalDevice; }
    VkQueue getComputeQueue() const { return m_computeQueue; }
    u32 getComputeQueueFamilyIndex() const { return m_computeFamilyIndex; }
private:
    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();

    void createTransferCommandPool();
    void createTransferFence();

    void createComputeCommandPool();
    void createComputeFence();
    
    void transferData(const std::vector<DataBuffer>& dataBuffers, const std::vector<const void*>& dataPtrs);
    void retrieveData(const DataBuffer& dataBuffer, void* data_out);
    
    bool checkValidationLayerSupport();
    
    std::optional<std::pair<u32, u32>> findComputeQueueFamily(VkPhysicalDevice device);
    std::optional<u32> findTransferQueueFamily(VkPhysicalDevice device);
    
    i32 ratePhysicalDevice(VkPhysicalDevice device);
    i32 rateComputeQueueFamily(VkQueueFamilyProperties& properties);

    std::vector<VkDeviceQueueCreateInfo> getQueueCreateInfos();
    
    void createPipelineLayouts();
    void compileComputePipelines(); 
    std::vector<char> readSpirvFile(const std::string& filename);

    // Core Configuration
    const std::vector<const char*> m_validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    TSPPipelines pipelines;

#ifdef NDEBUG
    const bool m_enableValidationLayers = false;
#else
    const bool m_enableValidationLayers = true;
#endif

    // Stripped down handles (No surfaces or extra queues)
    VkInstance       m_instance             = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice       = VK_NULL_HANDLE;
    VkDevice         m_device               = VK_NULL_HANDLE;
    VkQueue          m_computeQueue         = VK_NULL_HANDLE;
    VkQueue          m_transferQueue        = VK_NULL_HANDLE;

    u32              m_computeFamilyIndex   = 0;
    u32              m_transferFamilyIndex  = 0;

    VkFence         m_transferFence         = VK_NULL_HANDLE;
    VkCommandPool   m_transferCommandPool   = VK_NULL_HANDLE;
    VkCommandBuffer m_transferCommandBuffer = VK_NULL_HANDLE;

    VkFence         m_computeFence         = VK_NULL_HANDLE;
    VkCommandPool   m_computeCommandPool   = VK_NULL_HANDLE;
    VkCommandBuffer m_computeCommandBuffer = VK_NULL_HANDLE;
    
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    VkPipelineLayout      m_pipelineLayout      = VK_NULL_HANDLE;
};