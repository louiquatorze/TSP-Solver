
#pragma once

#include "VulkanCore.hpp"
#include "VulkanEngine.hpp"
#include "VulkanTSPManager.hpp"

class VulkanContext {
public:
    VulkanContext();
    ~VulkanContext();

    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;
private:
    // Engine and Manager rely on Core, so declare first
    std::unique_ptr<VulkanCore>       core;
    std::unique_ptr<VulkanEngine>     engine;
    std::unique_ptr<VulkanTSPManager> manager;
};