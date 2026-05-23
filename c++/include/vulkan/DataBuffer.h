
#pragma once

#include <vulkan/vulkan.h>

struct DataBuffer {
    VkDeviceSize bufferSize; 

    VkBuffer       stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkBuffer       deviceBuffer;
    VkDeviceMemory deviceBufferMemory;
};