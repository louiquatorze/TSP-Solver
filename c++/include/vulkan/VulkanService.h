
#pragma once

#include "DataBuffer.h"
#include "ComputePipeline.h"
#include "types.h"

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class VulkanService {
public:
    static void createDataBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, DataBuffer& dataBuffer);

    static void destroyDataBuffer(VkDevice device, DataBuffer& dataBuffer);

    static void createBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size,
                             VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, 
                             VkBuffer& buffer_out, VkDeviceMemory& bufferMemory_out);

    static void createDescriptorSetLayout(VkDevice device, const std::vector<i32> bindings, VkDescriptorSetLayout& descriptorSetLayout_out);
    
    static void createComputePipeline(VkDevice device, const std::string& spirvFileName, const std::string& entryName, 
                                      i32 pushConstantRangeSize, VkDescriptorSetLayout& descriptorSetLayout, 
                                      ComputePipeline& computePipeline_out);
    
    static void destroyComputePipelineFamily(VkDevice device, ComputePipelineFamily& pipelineFamily);

    static std::vector<u32> readSPIRVFile(const std::string& fileName);
};