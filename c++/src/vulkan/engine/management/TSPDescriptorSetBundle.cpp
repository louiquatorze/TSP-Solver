
#include "TSPDescriptorSetBundle.hpp"

TSPDescriptorSetBundle::TSPDescriptorSetBundle(const VulkanCore& vulkanCore) : vulkanCore(vulkanCore) {
    createDescriptorSetLayout();
    createDescriptorSet();
}

TSPDescriptorSetBundle::~TSPDescriptorSetBundle() {
    vkDestroyDescriptorSetLayout(vulkanCore.getLogicalDevice(), descriptorSetLayout, nullptr);
}

void TSPDescriptorSetBundle::createDescriptorSetLayout() {
    u32 bindingCount = bindings.size();
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings(bindingCount);

    for (i32 i = 0; i < bindingCount; i++) {
        descriptorSetLayoutBindings[i].binding            = bindings[i];
        descriptorSetLayoutBindings[i].descriptorType     = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorSetLayoutBindings[i].descriptorCount    = 1;
        descriptorSetLayoutBindings[i].stageFlags         = VK_SHADER_STAGE_COMPUTE_BIT;
        descriptorSetLayoutBindings[i].pImmutableSamplers = nullptr;
    }

    // Create layout with bindings 
    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
    descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutInfo.pNext = nullptr;
    descriptorLayoutInfo.pBindings = descriptorSetLayoutBindings.data();
    descriptorLayoutInfo.bindingCount = bindingCount;
    descriptorLayoutInfo.flags = 0;

    if (vkCreateDescriptorSetLayout(vulkanCore.getLogicalDevice(), &descriptorLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout.");
    }
}

void TSPDescriptorSetBundle::createDescriptorSet() {
    
}