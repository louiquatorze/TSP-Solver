
#include "ComputePipelineFamily.h"
#include "VulkanService.h"

void ComputePipelineFamily::destroy(VkDevice device) {
    for (VkPipeline p : pipelines) {
        vkDestroyPipeline(device, p, nullptr);
    }
}