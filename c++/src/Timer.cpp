
#include "Timer.h"

#include <chrono>

ExitStatus Timer::time(std::function<ExitStatus()> func, i64& ns_out) {
    auto start = std::chrono::high_resolution_clock::now();

    ExitStatus exitStatus = func();
    
    auto finish = std::chrono::high_resolution_clock::now();
    auto deltans = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start);
    ns_out = deltans.count();

    return exitStatus;
}