
// Must match c++/include/vulkan/engine/PushConstants.h !

layout(push_constant) uniform Block {
    uint dim;

    uint iteration;
    uint seed;
        
    // Ant colony solver
    uint antCount;
    float pheromonePower;
    float exploitationProbability;
    float localEvaporation;
    float globalEvaporation;
    float initialPheromoneStrength;
} pushConstants;

