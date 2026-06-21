#pragma once

#include "VulkanCore.hpp"
#include "TSPDescriptorSetBundle.hpp"
#include <string>
#include <vector>
#include <memory>

class VulkanTSPPipelineManager {
public:
    enum class PipelineType {
        IterativeCalculate = 0,
        AntColonySimulateAnts,
        AntColonyRewardBestPath,
        Count // Helper for array sizing
    };

    VulkanTSPPipelineManager(const VulkanCore& vulkanCore);
    ~VulkanTSPPipelineManager();

    VulkanTSPPipelineManager(const VulkanTSPPipelineManager&) = delete;
    VulkanTSPPipelineManager& operator=(const VulkanTSPPipelineManager&) = delete;

    VkPipeline getPipeline(PipelineType type) const;
    VkPipelineLayout getPipelineLayout() const { return pipelineLayout; }
    const TSPDescriptorSetBundle& getDescriptorSetBundle() const { return *descriptorSetBundle; }

private:
    struct PipelineMetadata {
        std::string glslPath;
        std::string entryPoint;
        VkPipeline handle = VK_NULL_HANDLE;
    };

    void createPipelineLayout();
    void compileAllPipelines();

    std::vector<u32> readSPIRVFile(const std::string& fileName);

    const VulkanCore& vulkanCore;

    std::unique_ptr<TSPDescriptorSetBundle> descriptorSetBundle;
    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    std::vector<PipelineMetadata> pipelines;
};