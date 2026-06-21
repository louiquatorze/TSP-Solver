
    layout(std430, set = 0, binding = 0) buffer EdgeWeights {
        uint matrix[];
    } edgeWeights;

    layout(std430, set = 0, binding = 1) buffer Heuristics {
        float matrix[];
    } heuristics;

    layout(std430, set = 0, binding = 2) buffer Pheromones {
        float matrix[];
    } pheromones;

    layout(std430, set = 0, binding = 3) buffer Visited {
        uint indices[];
    } visited;