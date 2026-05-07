
#pragma once

#include "types.h"

#include <atomic>

struct Environment {
    std::atomic<bool> interrupt{ false };
    std::atomic<i32> progress{ 0 };
};