
#include "Context.h"
#include "interface.h"

#include <iostream>

Context::Context() {
    std::cout << "[C++] Constructing context" << std::endl;

    vulkanContext = std::make_unique<VulkanContext>();
}

Context::~Context() {
    std::cout << "[C++] Destructing context" << std::endl;
}

void initVulkan() {

}

void destroyVulkan() {

}