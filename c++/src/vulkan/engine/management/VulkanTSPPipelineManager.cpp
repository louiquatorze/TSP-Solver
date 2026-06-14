
#include "VulkanTSPPipelineManager.hpp"


VulkanTSPPipelineManager::VulkanTSPPipelineManager(const VulkanCore& vulkanCore) : vulkanCore(vulkanCore) {

}

VulkanTSPPipelineManager::~VulkanTSPPipelineManager() {
    
}

void VulkanTSPPipelineManager::bindBuffers(VkBuffer buffer, const std::vector<DescriptorBindingUpdate>& descriptorBindingUpdates) {
    const auto device = vulkanCore.getLogicalDevice();

    i32 bindingCount = descriptorBindingUpdates.size();

    if (bindingCount == 0) return;

    std::vector<VkDescriptorBufferInfo> descriptorBufferInfos(bindingCount);
    std::vector<VkWriteDescriptorSet> descriptorWrites(bindingCount);

    for (i32 i = 0; i < bindingCount; i++) {
        const DescriptorBindingUpdate& dbu = descriptorBindingUpdates[i];

        descriptorBufferInfos[i].buffer = buffer;
        descriptorBufferInfos[i].offset = dbu.offset;
        descriptorBufferInfos[i].range  = dbu.range;

        descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i].dstSet = descriptorSetBundle->getDescriptorSet();
        descriptorWrites[i].dstBinding = dbu.binding;
        descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[i].descriptorCount = 1;
        descriptorWrites[i].pBufferInfo = &descriptorBufferInfos[i];
    }

    vkUpdateDescriptorSets(device, bindingCount, descriptorWrites.data(), 0, nullptr);
}