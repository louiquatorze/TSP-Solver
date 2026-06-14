
#pragma once

#include "VulkanCore.hpp"
#include "TSPDescriptorSetBundle.hpp"
#include "ExitStatus.hpp"

struct DescriptorBindingUpdate {
    i32 binding;
    VkDeviceSize offset;
    VkDeviceSize range;
};

struct PipelineInitializationData {
    VkPipeline* pipeline;
    std::string fileName;
    std::string entryName;
};

class VulkanTSPPipelineManager {
public:
    VulkanTSPPipelineManager(const VulkanCore& vulkanCore);
    ~VulkanTSPPipelineManager();

    void bindBuffers(VkBuffer buffer, const std::vector<DescriptorBindingUpdate>& descriptorBindingUpdates);

    const TSPDescriptorSetBundle& getDescriptorSetBundle() const { return *descriptorSetBundle; }
private:
    const VulkanCore& vulkanCore;   

    void createPipelineLayout();
    void createPipelines();
    static std::vector<u32> readSPIRVFile(const std::string& fileName);

    std::unique_ptr<TSPDescriptorSetBundle> descriptorSetBundle = nullptr;

    VkPipelineLayout pipelineLayout = VK_NULL_HANDLE;

    struct {
        VkPipeline calculate      = VK_NULL_HANDLE;
    } iterative;

    struct {
        VkPipeline simulateAnts   = VK_NULL_HANDLE;
        VkPipeline rewardBestPath = VK_NULL_HANDLE;
    } antColony;

    // Automatic initialization and destruction of pipelines
    // Initialization of pipelineData occurs in constructor
    // Fill with {<pointer to pipeline>, <file name>, <entry name>}
    const std::vector<PipelineInitializationData> pipelineData;
};