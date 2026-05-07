
#pragma once

#include "types.h"

class Util {
public:
    template <typename T>
    static inline T factorial(u32 value) {
        T ret = 1;

        while (value)
            ret *= value--;

        return ret;
    }
};