
#pragma once

#include "types.hpp"

#include <atomic>

struct Environment {
    std::atomic<bool> interrupt{ false };
    std::atomic<i32> progress{ 0 };
};