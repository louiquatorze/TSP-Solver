
#include "VulkanTSPMemoryManager.hpp"

#include "VulkanService.hpp"
#include "PathService.hpp"
#include "ExitStatus.hpp"

#include <string>

VulkanTSPMemoryManager::VulkanTSPMemoryManager(const VulkanCore& vulkanCore) : vulkanCore(vulkanCore) {
    const auto device = vulkanCore.getLogicalDevice();

    // Fetch storage buffer alignment from physical device properties
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(vulkanCore.getPhysicalDevice(), &props);
    storageBufferAlignment = props.limits.minStorageBufferOffsetAlignment;

    descriptorSet = std::make_unique<TSPDescriptorSet>();
    reserveMonolithicMemoryBuffer();
}

VulkanTSPMemoryManager::~VulkanTSPMemoryManager() {
    const auto device = vulkanCore.getLogicalDevice();

    vkDestroyBuffer(device, monolithicBuffer, nullptr);
    vkFreeMemory(device, reservedMemory, nullptr);
}

void VulkanTSPMemoryManager::reserveMonolithicMemoryBuffer() {
    const auto device = vulkanCore.getLogicalDevice();
    
    // Create global monolithic buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.flags = 0;
    bufferInfo.size = RESERVED_MEMORY_SIZE;
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &monolithicBuffer) != VK_SUCCESS) {
        throw std::runtime_error("[C++] Failed to create monolithic buffer");
    }
    
    // Query hardware memory type requirements
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(device, monolithicBuffer, &requirements);

    // Allocate VRAM block
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = findOptimalMemoryType(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(device, &allocateInfo, nullptr, &reservedMemory) != VK_SUCCESS) {
        throw std::runtime_error("[C++] Failed to allocate global VRAM block");
    }

    // Bind monolithic buffer to reserved memory
    if (vkBindBufferMemory(device, monolithicBuffer, reservedMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("[C++] Failed to abind global monolithic buffer to allocated VRAM");
    }
}

ExitStatus VulkanTSPMemoryManager::bindIterativeBuffers(i32 dim) {
    const auto device = vulkanCore.getLogicalDevice();
    
    activeProblemLayout.edgeWeightsSize   = alignUp(dim * dim * sizeof(u32));
    activeProblemLayout.edgeWeightsOffset = 0;

    VkDeviceSize totalSize = activeProblemLayout.edgeWeightsOffset + activeProblemLayout.edgeWeightsSize;
    
    if (totalSize > RESERVED_MEMORY_SIZE) {
        return ExitStatus::ERROR_MEMORY_LIMIT;
    }

    // Create descriptor buffer info for edgeWeights
    VkDescriptorBufferInfo edgeWeightsBufferInfo{};
    edgeWeightsBufferInfo.buffer = monolithicBuffer;
    edgeWeightsBufferInfo.offset = activeProblemLayout.edgeWeightsOffset;
    edgeWeightsBufferInfo.range  = activeProblemLayout.edgeWeightsSize;

    // Push descriptor buffer infos into writeDescriptorSet array
    VkWriteDescriptorSet descriptorWrites[1]{};
    
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = this->descriptorSet->getDescriptorSet();
    descriptorWrites[0].dstBinding = this->descriptorSet->getEdgeWeightsBinding();
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &edgeWeightsBufferInfo;

    vkUpdateDescriptorSets(device, 1, descriptorWrites, 0, nullptr);

    return ExitStatus::SUCCESS;
}

ExitStatus VulkanTSPMemoryManager::bindAntColonyBuffers(i32 dim, i32 antCount) {
    const auto device = vulkanCore.getLogicalDevice();
    
    activeProblemLayout.heuristicsSize = alignUp(dim * dim * sizeof(f32));
    activeProblemLayout.pheromonesSize = alignUp(dim * dim * sizeof(f32));
    activeProblemLayout.visitedSize    = alignUp(dim * antCount * sizeof(i32));

    activeProblemLayout.heuristicsOffset = 0;
    activeProblemLayout.pheromonesOffset = activeProblemLayout.heuristicsOffset + activeProblemLayout.heuristicsSize;
    activeProblemLayout.visitedOffset    = activeProblemLayout.pheromonesOffset + activeProblemLayout.heuristicsSize;
    
    VkDeviceSize totalSize = activeProblemLayout.visitedOffset + activeProblemLayout.visitedSize;

    if (totalSize > RESERVED_MEMORY_SIZE) {
        return ExitStatus::ERROR_MEMORY_LIMIT;
    }

    // Create descriptor buffer infos
    VkDescriptorBufferInfo heuristicsBufferInfo{};
    heuristicsBufferInfo.buffer = monolithicBuffer;
    heuristicsBufferInfo.offset = activeProblemLayout.heuristicsOffset;
    heuristicsBufferInfo.range  = activeProblemLayout.heuristicsSize;

    VkDescriptorBufferInfo pheromonesBufferInfo{};
    pheromonesBufferInfo.buffer = monolithicBuffer;
    pheromonesBufferInfo.offset = activeProblemLayout.pheromonesOffset;
    pheromonesBufferInfo.range  = activeProblemLayout.pheromonesSize;
    
    VkDescriptorBufferInfo visitedBufferInfo{};
    visitedBufferInfo.buffer = monolithicBuffer;
    visitedBufferInfo.offset = activeProblemLayout.visitedOffset;
    visitedBufferInfo.range  = activeProblemLayout.visitedSize;

    // Push descriptor buffer infos into writeDescriptorSet array
    VkWriteDescriptorSet descriptorWrites[3]{};
    VkDescriptorSet descriptorSet = this->descriptorSet->getDescriptorSet();
    
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = descriptorSet;
    descriptorWrites[0].dstBinding = this->descriptorSet->getHeuristicsBinding();
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &heuristicsBufferInfo;
    
    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = descriptorSet;
    descriptorWrites[1].dstBinding = this->descriptorSet->ac_getPheromonesBinding();
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pBufferInfo = &pheromonesBufferInfo;
    
    descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[2].dstSet = descriptorSet;
    descriptorWrites[2].dstBinding = this->descriptorSet->ac_getVisitedBinding();
    descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[2].descriptorCount = 1;
    descriptorWrites[2].pBufferInfo = &visitedBufferInfo;

    vkUpdateDescriptorSets(device, 3, descriptorWrites, 0, nullptr);
    
    return ExitStatus::SUCCESS;
}

VkDeviceSize VulkanTSPMemoryManager::alignUp(VkDeviceSize size) {
    return (size + storageBufferAlignment - 1) & ~(storageBufferAlignment - 1);
}

u32 VulkanTSPMemoryManager::findOptimalMemoryType(u32 memoryTypeBits, VkMemoryPropertyFlags properties) {
    // Get physical memory layout properties from the graphics card
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkanCore.getPhysicalDevice(), &memProperties);

    // Loop through all available memory types supported by the hardware
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        bool isSupportedType = (memoryTypeBits & (1 << i));
        bool hasMatchingProperties = (memProperties.memoryTypes[i].propertyFlags & properties) == properties;

        if (isSupportedType && hasMatchingProperties) {
            return i;
        }
    }
    
    throw std::runtime_error("[C++] Failed to find a suitable hardware memory type index");
}