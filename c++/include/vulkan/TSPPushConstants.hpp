
#pragma once

#include "types.hpp"

struct TSPPushConstants {
    u32 dim;

    // Ant colony solver
    u32 antCount;
    f32 pheromonePower;
    f32 exploitationProbability;
    f32 localEvaporation;
    f32 globalEvaporation;
};