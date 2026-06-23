
#pragma once

#include "types.hpp"

struct PushConstants {   
    alignas(4) u32 dim;
    
    alignas(4) u32 iteration;
    alignas(4) u32 seed;

    // Ant colony solver
    alignas(4) u32 antCount;
    alignas(4) f32 pheromonePower;
    alignas(4) f32 exploitationProbability;
    alignas(4) f32 localEvaporation;
    alignas(4) f32 globalEvaporation;
    alignas(4) f32 initialPheromoneStrength;
};