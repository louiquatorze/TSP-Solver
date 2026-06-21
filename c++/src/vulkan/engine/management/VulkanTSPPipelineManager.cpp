
#include "VulkanTSPPipelineManager.hpp"
#include "PushConstants.hpp"
#include "PathService.hpp"

#include <fstream>

VulkanTSPPipelineManager::VulkanTSPPipelineManager(const VulkanCore& vulkanCore) : 
    vulkanCore(vulkanCore),
    descriptorSetBundle(std::make_unique<TSPDescriptorSetBundle>(vulkanCore))
{
    pipelines.resize(static_cast<size_t>(PipelineType::Count));

    // Configure metadata mappings
    pipelines[static_cast<size_t>(PipelineType::IterativeCalculate)] = {
        PathService::getSPIRVFile("iterative/calculateCS.comp.spv").string(), "main"
    };
    pipelines[static_cast<size_t>(PipelineType::AntColonySimulateAnts)] = {
        PathService::getSPIRVFile("antColony/simulateAntsCS.comp.spv").string(), "main"
    };
    pipelines[static_cast<size_t>(PipelineType::AntColonyRewardBestPath)] = {
        PathService::getSPIRVFile("antColony/rewardBestPathCS.comp.spv").string(), "main"
    };

    // Sequential initialization dependency chain
    createPipelineLayout();
    compileAllPipelines();
}

VulkanTSPPipelineManager::~VulkanTSPPipelineManager() {
    std::cout << "[C++] Destructing vulkan pipeline manager" << std::endl;

    const auto device = vulkanCore.getLogicalDevice();

    for (const auto& pipeline : pipelines) {
        if (pipeline.handle != VK_NULL_HANDLE) {
            vkDestroyPipeline(device, pipeline.handle, nullptr);
        }
    }

    if (pipelineLayout != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    }
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
    
void VulkanTSPPipelineManager::compileAllPipelines() {
    const auto device = vulkanCore.getLogicalDevice();

    for (auto& pipeline : pipelines) {
        const auto spirv = VulkanTSPPipelineManager::readSPIRVFile(pipeline.glslPath);

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
        shaderStageInfo.pName               = pipeline.entryPoint.c_str();
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

        auto result = vkCreateComputePipelines(device, nullptr, 1, &pipelineInfo, nullptr, &pipeline.handle);

        vkDestroyShaderModule(device, shaderModule, nullptr);

        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to create compute pipeline");
        }
    }
}

VkPipeline VulkanTSPPipelineManager::getPipeline(PipelineType type) const {
    return pipelines[static_cast<size_t>(type)].handle;
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