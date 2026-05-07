
#pragma once

#include "ExitStatus.h"
#include "types.h"

#include <functional>

class Timer {
public:
    static ExitStatus time(std::function<ExitStatus()>, i64& ns_out);
};