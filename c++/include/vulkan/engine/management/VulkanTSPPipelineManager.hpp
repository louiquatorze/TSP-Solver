
#pragma once

#include "VulkanCore.hpp"
#include "TSPDescriptorSetBundle.hpp"
#include "ExitStatus.hpp"

struct DescriptorBindingUpdate {
    i32 binding;
    VkDeviceSize offset;
    VkDeviceSize range;
};

class VulkanTSPPipelineManager {
public:
    VulkanTSPPipelineManager(const VulkanCore& vulkanCore);
    ~VulkanTSPPipelineManager();

    void bindBuffers(VkBuffer buffer, const std::vector<DescriptorBindingUpdate>& descriptorBindingUpdates);

    const TSPDescriptorSetBundle& getDescriptorSetBundle() const { return *descriptorSetBundle; }
private:
    const VulkanCore& vulkanCore;   
    
    std::unique_ptr<TSPDescriptorSetBundle> descriptorSetBundle = nullptr;
};