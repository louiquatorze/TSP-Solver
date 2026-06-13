
#pragma once

#include "types.hpp"
#include "Environment.hpp"
#include "TSPSolver.hpp"
#include "VulkanContext.hpp"

typedef void (*ProgressCallback)(i32);

class Context {
public:
    Context();
    ~Context();

    std::unique_ptr<TSPSolver> solver = nullptr;

    Environment environment{};
    std::unique_ptr<VulkanContext> vulkanContext;
};