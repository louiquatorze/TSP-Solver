
#pragma once

#include "types.h"

#include <memory>
#include <vector>

class GraphUtil {
public:
    struct TreeNode {
        i32 value;
        TreeNode* parent = nullptr;
    };
    
    static std::vector<i32> minimumSpanningTree(u32* edgeWeights, i32 dimension);
};