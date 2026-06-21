
#include "TSPDescriptorSetBundle.hpp"

TSPDescriptorSetBundle::TSPDescriptorSetBundle(const VulkanCore& vulkanCore) : vulkanCore(vulkanCore) {
    createDescriptorSetLayout();
    allocateDescriptorSet();
}

TSPDescriptorSetBundle::~TSPDescriptorSetBundle() {
    std::cout << "[C++] Destructing vulkan descriptor set bundle" << std::endl;

    const auto device = vulkanCore.getLogicalDevice();
    
    if (descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(device, descriptorPool, nullptr);
    }
    
    if (descriptorSetLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(vulkanCore.getLogicalDevice(), descriptorSetLayout, nullptr);
    }
}

void TSPDescriptorSetBundle::createDescriptorSetLayout() {
    const u32 bindingCount = bindings.size();
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
        throw std::runtime_error("[C++] Failed to create descriptor set layout.");
    }
}

void TSPDescriptorSetBundle::allocateDescriptorSet() {
    const auto device = vulkanCore.getLogicalDevice();
    const u32 bindingCount = bindings.size();

    VkDescriptorPoolSize poolSize{};
    poolSize.type            = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = bindingCount;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.pNext         = nullptr;
    poolInfo.flags         = 0;
    poolInfo.maxSets       = 1;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes    = &poolSize;

    if (vkCreateDescriptorPool(device, &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("[C++] Failed to create descriptor pool");
    }

    VkDescriptorSetAllocateInfo descriptorSetInfo{};
    descriptorSetInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetInfo.pNext              = nullptr;
    descriptorSetInfo.descriptorPool     = descriptorPool;
    descriptorSetInfo.descriptorSetCount = 1;
    descriptorSetInfo.pSetLayouts        = &descriptorSetLayout;

    if (vkAllocateDescriptorSets(device, &descriptorSetInfo, &descriptorSet) != VK_SUCCESS) {
        throw std::runtime_error("[C++] Failed to create descriptor pool");
    }
}   

void TSPDescriptorSetBundle::bindBuffers(VkBuffer buffer, const std::vector<DescriptorBindingUpdate>& descriptorBindingUpdates) const {
    const auto device = vulkanCore.getLogicalDevice();
    const size_t bindingCount = descriptorBindingUpdates.size();
    
    if (bindingCount == 0) return;

    std::vector<VkDescriptorBufferInfo> descriptorBufferInfos;
    descriptorBufferInfos.reserve(bindingCount); 

    std::vector<VkWriteDescriptorSet> descriptorWrites;
    descriptorWrites.reserve(bindingCount);

    for (const auto& dbu : descriptorBindingUpdates) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = buffer;
        bufferInfo.offset = dbu.offset;
        bufferInfo.range  = dbu.range;
        descriptorBufferInfos.push_back(bufferInfo);
    }

    for (u32 i = 0; i < bindingCount; ++i) {
        VkWriteDescriptorSet writeInfo{};
        writeInfo.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeInfo.pNext           = nullptr;
        writeInfo.dstSet          = descriptorSet;
        writeInfo.dstBinding      = descriptorBindingUpdates[i].binding;
        writeInfo.dstArrayElement = 0;
        writeInfo.descriptorType  = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        writeInfo.descriptorCount = 1;
        writeInfo.pBufferInfo     = &descriptorBufferInfos[i];
        
        descriptorWrites.push_back(writeInfo);
    }

    vkUpdateDescriptorSets(
        device, 
        static_cast<u32>(descriptorWrites.size()), 
        descriptorWrites.data(), 
        0, 
        nullptr
    );
}