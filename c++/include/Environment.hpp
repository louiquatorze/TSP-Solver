
#pragma once

#include "types.hpp"

#include <atomic>

// cbl: Current best length
// cbp: Current best path (indices)
// lib: Last iteration best (length)

// Align to a fresh cache line to avoid clogging all atomic variables every time one is changed
struct alignas(64) Environment {
    // Heavily polled and written
    std::atomic<bool> interrupt{ false };
    std::atomic<i32> progress{ 0 };
    std::atomic<bool> cbl_updated{ false }; 
    std::atomic<bool> cbp_updated{ false };
    std::atomic<bool> lib_updated{ false };

    alignas(64)

    // TODO: implement analytic framework
    i32 cbl_data{ 0 };
    i32 cbp_data[1]{ 0 };   
    i32 lib_data{ 0 };

    inline void updateProgress(i32 progress) {
        this->progress.store(progress, std::memory_order_release);
    }

    inline void updateCBL(i32 cbl_data) {
        this->cbl_data = cbl_data;
        cbl_updated.store(true, std::memory_order_release);
    }

    inline void updateCBP(i32* indices, i32 size) {
        for (i32 i = 0; i < size; i++)
            cbp_data[i] = indices[i];
        
        cbp_updated.store(true, std::memory_order_release);
    }

    inline void updateLIB(i32 lib_data) {
        this->lib_data = lib_data;
        lib_updated.store(true, std::memory_order_release);
    }
};