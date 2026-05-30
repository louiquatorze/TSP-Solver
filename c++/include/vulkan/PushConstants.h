
#pragma once

#include "types.h"

struct PushConstants {
    u32 dimension;
    u32 interrupt;

    union {
        struct {
            u32 maxIterations;
        } iterative;

        struct {
            u32 localEvap;
        } antColony;
    };
};