
#include "VulkanService.h"

#include <stdexcept>

void VulkanService::createDataBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, DataBuffer& dataBuffer) {
    createBuffer(
        device,
        physicalDevice,
        size,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        dataBuffer.stagingBuffer,
        dataBuffer.stagingBufferMemory
    );

    createBuffer(
        device,
        physicalDevice,
        size,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        dataBuffer.deviceBuffer,
        dataBuffer.deviceBufferMemory
    );
}

void VulkanService::destroyDataBuffer(VkDevice device, DataBuffer& dataBuffer) {
    vkDestroyBuffer(device, dataBuffer.stagingBuffer, nullptr);
    vkDestroyBuffer(device, dataBuffer.deviceBuffer, nullptr);

    vkFreeMemory(device, dataBuffer.stagingBufferMemory, nullptr);
    vkFreeMemory(device, dataBuffer.deviceBufferMemory, nullptr);
}

void VulkanService::createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size,
                                 VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
                                 VkBuffer& buffer_out, VkDeviceMemory& bufferMemory_out) {
    // Create buffer
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer_out) != VK_SUCCESS) {
        throw std::runtime_error("[C++] Failed to create buffer");
    }

    // Get memory requirements
    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device, buffer_out, &memoryRequirements);

    // Get memory properties
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memoryProperties);
    
    // Find the correct memory type index on the GPU
    uint32_t memoryTypeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
        if ((memoryRequirements.memoryTypeBits & (1 << i)) && 
            (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            memoryTypeIndex = i;
            break;
        }
    }

    if (memoryTypeIndex == UINT32_MAX) {
        throw std::runtime_error("[C++] No suitable memory type found");
    }

    // Allocate the Physical Memory
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memoryRequirements.size;
    allocInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory_out) != VK_SUCCESS) {
        throw std::runtime_error("[C++] Failed to allocate memory");
    }

    vkBindBufferMemory(device, buffer_out, bufferMemory_out, 0);
}
