
#pragma once

#include "types.hpp"

#include <vulkan/vulkan.h>
#include <vector>
#include <string>

class VulkanService {
public:
    static void createDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout& descriptorSetLayout_out);
    
    static void createPipelineLayout(VkDevice device, VkDescriptorSetLayout& descriptorSetLayout, 
                                     i32 pushConstantRangeSize, VkPipelineLayout& pipelineLayout_out);

    static void createPipeline(VkDevice device, const std::string& spirvFileName, const std::string& entryName,
                               VkDescriptorSetLayout& descriptorSetLayout,  VkPipelineLayout& pipelineLayout,
                               VkPipeline& pipeline_out);
    
    static std::vector<u32> readSPIRVFile(const std::string& fileName);
};