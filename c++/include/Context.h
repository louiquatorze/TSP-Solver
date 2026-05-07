
#pragma once

#include "types.h"
#include "Environment.h"
#include "TSPSolver.h"

typedef void (*ProgressCallback)(i32);

class Context {
public:
    Context();
    ~Context();

    std::unique_ptr<TSPSolver> solver = nullptr;
    Environment environment{};
};