
#include "VulkanService.h"

#include <stdexcept>
#include <cstring>
#include <fstream>

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

    /*
        TODO implement VK_SHARING_MODE_EXCLUSIVE
    */

    bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;

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

void VulkanService::createBindlessDescriptorSetLayout(VkDevice device, VkDescriptorSetLayout& descriptorSetLayout) {
    VkDescriptorSetLayoutBinding binding{};
    binding.binding = 0;
    binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    binding.descriptorCount = 1000; 
    binding.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorBindingFlags bindingFlags = 
        VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT | 
        VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;

    VkDescriptorSetLayoutBindingFlagsCreateInfo extendedInfo{};
    extendedInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
    extendedInfo.bindingCount = 1;
    extendedInfo.pBindingFlags = &bindingFlags;

    // Create layout with bindings 
    VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo{};
    descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorLayoutInfo.pNext = &extendedInfo;
    descriptorLayoutInfo.pBindings = &binding;
    descriptorLayoutInfo.bindingCount = 1;
    descriptorLayoutInfo.flags = 0;

    if (vkCreateDescriptorSetLayout(device, &descriptorLayoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create descriptor set layout.");
    }
}

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