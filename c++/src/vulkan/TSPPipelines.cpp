
#include "TSPPipelines.h"
#include "VulkanService.h"

#include <string>

void TSPPipelines::create(VkDevice device) {
    createIterativePipelineFamily(device);
    createAntColonyPipelineFamily(device);
}

void TSPPipelines::destroyAll(VkDevice device) {
    VulkanService::destroyComputePipelineFamily(device, iterative);
    VulkanService::destroyComputePipelineFamily(device, antColony);
}

void TSPPipelines::createIterativePipelineFamily(VkDevice device) {
    iterative = ComputePipelineFamily{};
    iterative.pipelines = std::vector<ComputePipeline>(1);

    const std::string fileName = "TODO";
    const std::string entryName = "TODO";

    i32 pushConstantRangeSize = 0;

    VulkanService::createDescriptorSetLayout(device, { 0 }, iterative.descriptorSetLayout);
    VulkanService::createComputePipeline(device, fileName, entryName, pushConstantRangeSize, iterative.descriptorSetLayout, iterative.pipelines[0]);
}

void TSPPipelines::createAntColonyPipelineFamily(VkDevice device) {
    antColony = ComputePipelineFamily{};
    antColony.pipelines = std::vector<ComputePipeline>(1);

    const std::string fileName = "TODO";
    const std::string entryName = "TODO";

    i32 pushConstantRangeSize = 0;

    VulkanService::createDescriptorSetLayout(device, { 0 }, antColony.descriptorSetLayout);
    VulkanService::createComputePipeline(device, fileName, entryName, pushConstantRangeSize, antColony.descriptorSetLayout, antColony.pipelines[0]);
}