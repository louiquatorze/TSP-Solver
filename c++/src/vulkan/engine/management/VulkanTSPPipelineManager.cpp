
#include "VulkanTSPPipelineManager.hpp"
#include "PushConstants.hpp"

#include <fstream>

VulkanTSPPipelineManager::VulkanTSPPipelineManager(const VulkanCore& vulkanCore) : 
    vulkanCore(vulkanCore),
    descriptorSetBundle(std::make_unique<TSPDescriptorSetBundle>(vulkanCore)),
    pipelineData({
        PipelineInitializationData{ &iterative.calculate     , "hello", "world" },

        PipelineInitializationData{ &antColony.simulateAnts  , "hello", "world" },
        PipelineInitializationData{ &antColony.rewardBestPath, "hello", "world" },
    }) 
{
    createPipelineLayout();
}

VulkanTSPPipelineManager::~VulkanTSPPipelineManager() {
    const auto device = vulkanCore.getLogicalDevice();

    for (auto& pipelineInit : pipelineData) {
        const auto pipeline = pipelineInit.pipeline;

        if (pipeline != nullptr && *pipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(device, *pipeline, nullptr);
            *pipeline = VK_NULL_HANDLE;
        }
    }

    if (pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    }
}

void VulkanTSPPipelineManager::bindBuffers(VkBuffer buffer, const std::vector<DescriptorBindingUpdate>& descriptorBindingUpdates) {
    const auto device = vulkanCore.getLogicalDevice();

    i32 bindingCount = descriptorBindingUpdates.size();

    if (bindingCount == 0) return;

    std::vector<VkDescriptorBufferInfo> descriptorBufferInfos(bindingCount);
    std::vector<VkWriteDescriptorSet> descriptorWrites(bindingCount);

    for (i32 i = 0; i < bindingCount; i++) {
        const DescriptorBindingUpdate& dbu = descriptorBindingUpdates[i];

        descriptorBufferInfos[i].buffer = buffer;
        descriptorBufferInfos[i].offset = dbu.offset;
        descriptorBufferInfos[i].range  = dbu.range;

        descriptorWrites[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[i].dstSet = descriptorSetBundle->getDescriptorSet();
        descriptorWrites[i].dstBinding = dbu.binding;
        descriptorWrites[i].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        descriptorWrites[i].descriptorCount = 1;
        descriptorWrites[i].pBufferInfo = &descriptorBufferInfos[i];
    }

    vkUpdateDescriptorSets(device, bindingCount, descriptorWrites.data(), 0, nullptr);
}

void VulkanTSPPipelineManager::createPipelineLayout() {
    const auto device = vulkanCore.getLogicalDevice();
    const auto descriptorSetLayout = descriptorSetBundle->getDescriptorSetLayout();

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
    pushConstantRange.offset     = 0;
    pushConstantRange.size       = sizeof(PushConstants);

    VkPipelineLayoutCreateInfo layoutInfo{};
    layoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    layoutInfo.pNext                  = nullptr;
    layoutInfo.flags                  = 0;
    layoutInfo.pPushConstantRanges    = &pushConstantRange;
    layoutInfo.pushConstantRangeCount = 1;
    layoutInfo.pSetLayouts            = &descriptorSetLayout;
    layoutInfo.setLayoutCount         = 1;

    if (vkCreatePipelineLayout(device, &layoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create pipeline layout");
    }
}

void VulkanTSPPipelineManager::createPipelines() {
    const auto device = vulkanCore.getLogicalDevice();

    for (auto& pipelineInit : pipelineData) {
        const auto spirv = VulkanTSPPipelineManager::readSPIRVFile(pipelineInit.fileName);

        VkShaderModuleCreateInfo shaderModuleInfo{};
        shaderModuleInfo.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shaderModuleInfo.pNext    = nullptr;
        shaderModuleInfo.flags    = 0;
        shaderModuleInfo.pCode    = spirv.data();
        shaderModuleInfo.codeSize = spirv.size() * sizeof(u32);

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &shaderModuleInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create shader module");
        }

        VkPipelineShaderStageCreateInfo shaderStageInfo{};
        shaderStageInfo.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStageInfo.pNext               = nullptr;
        shaderStageInfo.flags               = 0;
        shaderStageInfo.module              = shaderModule;
        shaderStageInfo.pName               = pipelineInit.entryName.c_str();
        shaderStageInfo.pSpecializationInfo = nullptr;
        shaderStageInfo.stage               = VK_SHADER_STAGE_COMPUTE_BIT;

        VkComputePipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType              = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
        pipelineInfo.pNext              = nullptr;
        pipelineInfo.flags              = 0;
        pipelineInfo.stage              = shaderStageInfo;
        pipelineInfo.layout             = pipelineLayout;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.basePipelineIndex  = -1;

        auto result = vkCreateComputePipelines(device, nullptr, 1, &pipelineInfo, nullptr, pipelineInit.pipeline);

        vkDestroyShaderModule(device, shaderModule, nullptr);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create compute pipeline");
        }
    }
}

std::vector<u32> VulkanTSPPipelineManager::readSPIRVFile(const std::string& fileName) {
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