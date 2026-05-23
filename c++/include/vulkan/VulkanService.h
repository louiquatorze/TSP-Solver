
#pragma once

#include "DataBuffer.h"

#include <vulkan/vulkan.h>

class VulkanService {
public:
    static void createDataBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, DataBuffer& dataBuffer);

    static void destroyDataBuffer(VkDevice device, DataBuffer& dataBuffer);

    static void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size,
                             VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
                             VkBuffer& buffer_out, VkDeviceMemory& bufferMemory_out);
};