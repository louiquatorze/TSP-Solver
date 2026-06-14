
#include "VulkanContext.hpp"

#include <iostream>

VulkanContext::VulkanContext() {
    core    = std::make_unique<VulkanCore>();
    engine  = std::make_unique<VulkanEngine>(*core);
    manager = std::make_unique<VulkanTSPManager>(*core);

    std::cout << "[C++] Vulkan Context successfully created.\n";  
}

VulkanContext::~VulkanContext() {
    std::cout << "[C++] Destructing Vulkan Context\n";      
}
