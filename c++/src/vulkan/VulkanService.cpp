
#include "VulkanService.hpp"

#include <stdexcept>
#include <cstring>
#include <fstream>

void VulkanService::createPipelineLayout(VkDevice device, VkDescriptorSetLayout& descriptorSetLayout, 
                                         i32 pushConstantRangeSize, VkPipelineLayout& pipelineLayout_out) {
    VkPushConstantRange pushConstantRange;
    bool hasPushConstant = pushConstantRangeSize != 0;

    if (hasPushConstant) {
        pushConstantRange = VkPushConstantRange{};
        pushConstantRange.size = pushConstantRangeSize;
        pushConstantRange.offset = 0;
        pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    }

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.pNext = nullptr;
    pipelineLayoutInfo.pPushConstantRanges = hasPushConstant ? &pushConstantRange : nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = hasPushConstant ? 1 : 0;
    pipelineLayoutInfo.pSetLayouts = &descriptorSetLayout;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.flags = 0;

    if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout_out) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout.");
    }
}

void VulkanService::createPipeline(VkDevice device, const std::string& spirvFileName, const std::string& entryName,
                                   VkDescriptorSetLayout& descriptorSetLayout,  VkPipelineLayout& pipelineLayout,
                                   VkPipeline& pipeline_out) {
    // Read spir-v file
    std::vector<u32> spirv = readSPIRVFile(spirvFileName);

    // Create shader module
    VkShaderModuleCreateInfo shaderModelInfo{};
    shaderModelInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModelInfo.pNext = nullptr;    
    shaderModelInfo.codeSize = spirv.size() * sizeof(u32);
    shaderModelInfo.pCode = spirv.data();

    VkShaderModule computeShaderModule;
    if (vkCreateShaderModule(device, &shaderModelInfo, nullptr, &computeShaderModule) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create shader module.");
    }

    // Configure shader stage
    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.pName = entryName.data();
    shaderStageInfo.module = computeShaderModule;
    shaderStageInfo.pNext = nullptr;
    shaderStageInfo.pSpecializationInfo = nullptr;
    shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageInfo.flags = 0;

    // Create pipeline
    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.stage = shaderStageInfo;
    pipelineInfo.layout = pipelineLayout;
    pipelineInfo.basePipelineIndex = 0;
    pipelineInfo.flags = 0;

    if (vkCreateComputePipelines(device, nullptr, 1, &pipelineInfo, nullptr, &pipeline_out) != VK_SUCCESS) {
        vkDestroyShaderModule(device, computeShaderModule, nullptr);
        throw std::runtime_error("Failed to bake compute pipeline.");
    }

    vkDestroyShaderModule(device, computeShaderModule, nullptr);
}

std::vector<u32> VulkanService::readSPIRVFile(const std::string& fileName) {
    std::ifstream file(fileName, std::ios::binary | std::ios::ate);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open SPIR-V file " + fileName);
    }

    std::streamsize fileSize = file.tellg();

    if (fileSize % sizeof(u32) != 0) {
        throw std::runtime_error("SPIR-V file size invalid (not multiple of 4 bytes) " + fileName);
    }
    
    i32 elementCount = fileSize / sizeof(u32);
    std::vector<u32> spirv(elementCount);   

    file.seekg(0, std::ios::beg);

    file.read(reinterpret_cast<char*>(spirv.data()), fileSize);
    file.close();

    return spirv;
}