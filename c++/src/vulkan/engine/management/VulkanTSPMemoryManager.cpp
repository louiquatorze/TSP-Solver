
#include "VulkanTSPMemoryManager.hpp"

#include "VulkanService.hpp"
#include "PathService.hpp"
#include "ExitStatus.hpp"

#include <string>
#include <cstring>
#include <iostream>

VulkanTSPMemoryManager::VulkanTSPMemoryManager(const VulkanCore& vulkanCore, const VulkanTSPCommandBufferManager& vulkanCommandBufferManager) : 
    vulkanCore(vulkanCore),
    vulkanCommandBufferManager(vulkanCommandBufferManager) 
{
    const auto device = vulkanCore.getLogicalDevice();

    // Fetch storage buffer alignment from physical device properties
    VkPhysicalDeviceProperties props;
    vkGetPhysicalDeviceProperties(vulkanCore.getPhysicalDevice(), &props);
    storageBufferAlignment = props.limits.minStorageBufferOffsetAlignment;
    
    reserveMonolithicMemory();
}

VulkanTSPMemoryManager::~VulkanTSPMemoryManager() {
    std::cout << "[C++] Destructing vulkan memory manager" << std::endl;
    
    const auto device = vulkanCore.getLogicalDevice();

    vkDestroyBuffer(device, monolithic.buffer, nullptr);
    vkFreeMemory(device, monolithic.allocated, nullptr);
}

void VulkanTSPMemoryManager::reserveMonolithicMemory() {
    const auto device = vulkanCore.getLogicalDevice();
    
    // Create global monolithic buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.pNext                 = nullptr;
    bufferInfo.flags                 = 0;
    bufferInfo.size                  = RESERVED_MEMORY_SIZE;
    bufferInfo.usage                 = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.pQueueFamilyIndices   = nullptr;
    bufferInfo.queueFamilyIndexCount = 0;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &monolithic.buffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create monolithic buffer");
    }
    
    // Query hardware memory type requirements
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(device, monolithic.buffer, &requirements);

    VkMemoryPropertyFlags idealProperties = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | 
                                            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    // Allocate VRAM block
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.pNext           = nullptr;
    allocateInfo.allocationSize  = requirements.size;
    allocateInfo.memoryTypeIndex = findOptimalMemoryType(requirements.memoryTypeBits, idealProperties);

    if (vkAllocateMemory(device, &allocateInfo, nullptr, &monolithic.allocated) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate global VRAM block");
    }

    // Bind monolithic buffer to reserved memory
    if (vkBindBufferMemory(device, monolithic.buffer, monolithic.allocated, 0) != VK_SUCCESS) {
        throw std::runtime_error("Failed to bind global monolithic buffer to allocated VRAM");
    }

    // Map monolithic memory
    if (vkMapMemory(device, monolithic.allocated, 0, RESERVED_MEMORY_SIZE, 0, &monolithic.mapped) != VK_SUCCESS) {
        throw std::runtime_error("Failed to map monolithic memory");
    }
}

void VulkanTSPMemoryManager::stageAndCopyData(const std::vector<StageData>& stageData) {
    // TODO implement transferQueue/computeQueue ownership transfer
    // For now transferQueue = computeQueue

    for (auto data : stageData) {
        if (data.offset + data.size > bufferLayout.totalSize) {
            throw std::runtime_error("Trying to write outside of reserved memory bounds");
        }

        if (data.data == nullptr) {
            throw std::runtime_error("Data to be staged is invalid (nullptr)");
        }

        u8* mapped = static_cast<u8*>(monolithic.mapped) + data.offset;
        std::memcpy(static_cast<void*>(mapped), data.data, data.size);
    }
}

void VulkanTSPMemoryManager::pushPushConstants(PushConstants pushConstants) {
    
}

ExitStatus VulkanTSPMemoryManager::calculateBufferLayoutIterative(i32 dim) {
    const auto device = vulkanCore.getLogicalDevice();
    
    bufferLayout.edgeWeightsSize   = alignUp(dim * dim * sizeof(u32));
    bufferLayout.edgeWeightsOffset = 0;

    bufferLayout.totalSize = bufferLayout.edgeWeightsOffset + bufferLayout.edgeWeightsSize;
    
    if (bufferLayout.totalSize > RESERVED_MEMORY_SIZE) {
        return ExitStatus::ERROR_MEMORY_LIMIT;
    }

    return ExitStatus::SUCCESS;
}

ExitStatus VulkanTSPMemoryManager::calculateBufferLayoutAntColony(i32 dim, i32 antCount) {
    const auto device = vulkanCore.getLogicalDevice();
    
    bufferLayout.edgeWeightsSize = alignUp(dim * dim * sizeof(f32));
    bufferLayout.heuristicsSize  = alignUp(dim * dim * sizeof(f32));
    bufferLayout.pheromonesSize  = alignUp(dim * dim * sizeof(f32));
    bufferLayout.visitedSize     = alignUp(dim * antCount * sizeof(i32));

    bufferLayout.edgeWeightsOffset = 0;
    bufferLayout.heuristicsOffset  = bufferLayout.edgeWeightsOffset + bufferLayout.edgeWeightsSize;
    bufferLayout.pheromonesOffset  = bufferLayout.heuristicsOffset + bufferLayout.heuristicsSize;
    bufferLayout.visitedOffset     = bufferLayout.pheromonesOffset + bufferLayout.pheromonesSize;
    
    bufferLayout.totalSize = bufferLayout.visitedOffset + bufferLayout.visitedSize;

    if (bufferLayout.totalSize > RESERVED_MEMORY_SIZE) {
        return ExitStatus::ERROR_MEMORY_LIMIT;
    }

    return ExitStatus::SUCCESS;
}

VkDeviceSize VulkanTSPMemoryManager::alignUp(VkDeviceSize size) {
    return (size + storageBufferAlignment - 1) & ~(storageBufferAlignment - 1);
}

u32 VulkanTSPMemoryManager::findOptimalMemoryType(u32 memoryTypeBits, VkMemoryPropertyFlags idealProperties) {
    // Get physical memory layout properties from the graphics card
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(vulkanCore.getPhysicalDevice(), &memProperties);

    // Loop through all available memory types supported by the hardware
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        bool isSupportedType = (memoryTypeBits & (1 << i));
        bool hasMatchingProperties = (memProperties.memoryTypes[i].propertyFlags & idealProperties) == idealProperties;

        if (isSupportedType && hasMatchingProperties) {
            return i;
        }
    }
    
    throw std::runtime_error("[C++] Failed to find a suitable hardware memory type index");
}