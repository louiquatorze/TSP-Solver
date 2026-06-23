
struct PCGState {
    uint state;
};

// Advance the state
uint pcg_next(inout PCGState rng) {
    uint state = rng.state;
    rng.state = state * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

// Get a float between 0.0 and 1.0
float pcg_nextf(inout PCGState rng) {
    return float(pcg_next(rng)) / 4294967296.0;
}