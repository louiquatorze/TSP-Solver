
#pragma once

#include "VulkanCore.hpp"
#include "VulkanEngine.hpp"
#include "VulkanTSPResourceManager.hpp"

class VulkanContext {
public:
    VulkanContext();
    ~VulkanContext();

    VulkanContext(const VulkanContext&) = delete;
    VulkanContext& operator=(const VulkanContext&) = delete;

    VulkanCore&               getCore()            { return *core; }
    VulkanEngine&             getEngine()          { return *engine; }
    VulkanTSPResourceManager& getResourceManager() { return *manager; }

private:
    // Engine and Manager rely on Core, so declare first
    std::unique_ptr<VulkanCore>               core;
    std::unique_ptr<VulkanEngine>             engine;
    std::unique_ptr<VulkanTSPResourceManager> manager;
};