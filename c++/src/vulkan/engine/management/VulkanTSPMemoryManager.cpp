
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
    
    reserveMonolithicMemory();
}

VulkanTSPMemoryManager::~VulkanTSPMemoryManager() {
    const auto device = vulkanCore.getLogicalDevice();

    vkDestroyBuffer(device, monolithicMemory.buffer, nullptr);
    vkFreeMemory(device, monolithicMemory.allocated, nullptr);
}

void VulkanTSPMemoryManager::reserveMonolithicMemory() {
    const auto device = vulkanCore.getLogicalDevice();
    
    // Create global monolithic buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext = nullptr;
    bufferInfo.flags = 0;
    bufferInfo.size = RESERVED_MEMORY_SIZE;
    bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &monolithicMemory.buffer) != VK_SUCCESS) {
        throw std::runtime_error("[C++] Failed to create monolithic buffer");
    }
    
    // Query hardware memory type requirements
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(device, monolithicMemory.buffer, &requirements);

    // Allocate VRAM block
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext = nullptr;
    allocateInfo.allocationSize = requirements.size;
    allocateInfo.memoryTypeIndex = findOptimalMemoryType(requirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(device, &allocateInfo, nullptr, &monolithicMemory.allocated) != VK_SUCCESS) {
        throw std::runtime_error("[C++] Failed to allocate global VRAM block");
    }

    // Bind monolithic buffer to reserved memory
    if (vkBindBufferMemory(device, monolithicMemory.buffer, monolithicMemory.allocated, 0) != VK_SUCCESS) {
        throw std::runtime_error("[C++] Failed to abind global monolithic buffer to allocated VRAM");
    }
}

ExitStatus VulkanTSPMemoryManager::calculateBufferLayoutIterative(i32 dim) {
    const auto device = vulkanCore.getLogicalDevice();
    
    bufferLayout.edgeWeightsSize   = alignUp(dim * dim * sizeof(u32));
    bufferLayout.edgeWeightsOffset = 0;

    VkDeviceSize totalSize = bufferLayout.edgeWeightsOffset + bufferLayout.edgeWeightsSize;
    
    if (totalSize > RESERVED_MEMORY_SIZE) {
        return ExitStatus::ERROR_MEMORY_LIMIT;
    }

    return ExitStatus::SUCCESS;
}

ExitStatus VulkanTSPMemoryManager::calculateBufferLayoutAntColony(i32 dim, i32 antCount) {
    const auto device = vulkanCore.getLogicalDevice();
    
    bufferLayout.heuristicsSize = alignUp(dim * dim * sizeof(f32));
    bufferLayout.pheromonesSize = alignUp(dim * dim * sizeof(f32));
    bufferLayout.visitedSize    = alignUp(dim * antCount * sizeof(i32));

    bufferLayout.heuristicsOffset = 0;
    bufferLayout.pheromonesOffset = bufferLayout.heuristicsOffset + bufferLayout.heuristicsSize;
    bufferLayout.visitedOffset    = bufferLayout.pheromonesOffset + bufferLayout.heuristicsSize;
    
    VkDeviceSize totalSize = bufferLayout.visitedOffset + bufferLayout.visitedSize;

    if (totalSize > RESERVED_MEMORY_SIZE) {
        return ExitStatus::ERROR_MEMORY_LIMIT;
    }

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