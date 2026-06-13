
#include "ComputePipelineFamily.hpp"

ComputePipelineFamily::ComputePipelineFamily(i32 pipelineCount) {
    pipelines = std::vector<VkPipeline>(pipelineCount);
}
    
void ComputePipelineFamily::destroy(VkDevice device) {
    for (VkPipeline p : pipelines) {
        vkDestroyPipeline(device, p, nullptr);
    }
}