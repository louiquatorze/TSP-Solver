
struct PCGState {
    uint state;
};

// Advance the state
uint pcg_next(inout PCGState rng) {
    uint oldstate = rng.state;
    // Advance internal state
    rng.state = oldstate * 747796405u + 2891336453u;
    // Calculate output function (XSH RR), part of PCG
    uint xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31u));
}

// Get a float between 0.0 and 1.0
float pcg_nextf(inout PCGState rng) {
    return float(pcg_next(rng)) / 4294967296.0;
}