
#include "TSPPipelines.hpp"
#include "VulkanService.hpp"
#include "PathService.hpp"

#include <string>

void TSPPipelines::create(VkDevice device) {
    // createIterativePipelineFamily(device);
    // createAntColonyPipelineFamily(device);
}

void TSPPipelines::destroyAll(VkDevice device) {
    // VulkanService::destroyComputePipelineFamily(device, iterative);
    // VulkanService::destroyComputePipelineFamily(device, antColony);
}

void TSPPipelines::createIterativePipelineFamily(VkDevice device) {
    iterative = new ComputePipelineFamily(1);

    const auto filePath = PathService::getShaderCompiledDir() / "iterativeCS.spv";
    const std::string entryName = "main";

    VulkanService::createPipeline(device, filePath.string(), entryName, descriptorSetLayout, pipelineLayout, iterative->pipelines[0]);
}

void TSPPipelines::createAntColonyPipelineFamily(VkDevice device) {
    antColony = new ComputePipelineFamily(1);

    const auto filePath = PathService::getShaderCompiledDir() / "antColonyCS.spv";
    const std::string entryName = "main";

    VulkanService::createPipeline(device, filePath.string(), entryName, descriptorSetLayout, pipelineLayout, antColony->pipelines[0]);
}