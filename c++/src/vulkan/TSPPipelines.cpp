
#include "TSPPipelines.h"
#include "VulkanService.h"
#include "PathService.h"
#include "PushConstants.h"

#include <string>

void TSPPipelines::create(VkDevice device) {
    VulkanService::createBindlessDescriptorSetLayout(device, descriptorSetLayout);

    i32 pushConstantsRangeSize = sizeof(PushConstants);
    VulkanService::createPipelineLayout(device, descriptorSetLayout, pushConstantsRangeSize, pipelineLayout);

    declareBuffers();

    createIterativePipelineFamily(device);
    createAntColonyPipelineFamily(device);
}

void TSPPipelines::declareBuffers() {
    // Universal buffers
    
}

void TSPPipelines::destroyAll(VkDevice device) {
    iterative.destroy(device);
    antColony.destroy(device);

    vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
    vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
}

void TSPPipelines::createIterativePipelineFamily(VkDevice device) {
    iterative = ComputePipelineFamily{};
    iterative.pipelines = std::vector<VkPipeline>(1);

    const auto filePath = PathService::getShaderCompiledDir() / "iterativeCS.spv";
    const std::string entryName = "main";

    VulkanService::createPipeline(device, filePath.string(), entryName, descriptorSetLayout, pipelineLayout, iterative.pipelines[0]);
}

void TSPPipelines::createAntColonyPipelineFamily(VkDevice device) {
    antColony = ComputePipelineFamily{};
    antColony.pipelines = std::vector<VkPipeline>(1);

    const auto filePath = PathService::getShaderCompiledDir() / "antColonyCS.spv";
    const std::string entryName = "main";

    VulkanService::createPipeline(device, filePath.string(), entryName, descriptorSetLayout, pipelineLayout, antColony.pipelines[0]);
}