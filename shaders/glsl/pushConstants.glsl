
// Must match c++/include/vulkan/engine/PushConstants.h !

layout(push_constant, std430) uniform Block {
    uint dim;
        
    // Ant colony solver
    uint antCount;
    float pheromonePower;
    float exploitationProbability;
    float localEvaporation;
    float globalEvaporation;
} pushConstants;