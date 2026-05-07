
#pragma once

#include "types.h"

#include <cmath>

struct AlgoEUC { 
    i32 dim;

    AlgoEUC(i32 dim) : dim(dim) { }

    inline i32 operator()(f32* const p1, f32* const p2) const{
        f32 sum = 0.0f;

        for (i32 i = 0; i < dim; i++) {
            f32 d = p1[i] - p2[i];
            sum += d * d;
        }

        return static_cast<i32>(sqrtf(sum) + 0.5f);
    } 
};

struct AlgoMAX { 
    i32 dim;

    AlgoMAX(i32 dim) : dim(dim) { }
      
    inline i32 operator()(f32* const p1, f32* const p2) const {
        i32 max = 0;

        for (i32 i = 0; i < dim; i++) {
            f32 d = static_cast<i32>(fabsf(p1[i] - p2[i]) + 0.5f);
            if (d > max) max = d;
        }
        
        return max;
    }   
};

struct AlgoMAN {
    i32 dim;

    AlgoMAN(i32 dim) : dim(dim) { }

    inline i32 operator()(f32* const p1, f32* const p2) const {
        f32 sum = 0.0f;

        for (i32 i = 0; i < dim; i++) {
            sum += fabsf(p1[i] - p2[i]);
        }

        return static_cast<i32>(sum + 0.5f);
    } 
};

struct AlgoCEIL {
    i32 dim;

    AlgoCEIL(i32 dim) : dim(dim) { }
      
    inline i32 operator()(f32* const p1, f32* const p2) const {
        f32 sum = 0.0f;

        for (i32 i = 0; i < dim; i++) {
            f32 d = p1[i] - p2[i];
            sum += d * d;
        }

        return static_cast<i32>(std::ceil(sqrtf(sum)));
    }   
};

struct AlgoGEO { 
    i32 dim = 2;
     
    inline i32 operator()(f32* const p1, f32* const p2) const {
        return 0;
    }   
};

struct AlgoATT { 
    i32 dim = 2;
     
    inline i32 operator()(f32* const p1, f32* const p2) const {
        return 0;
    }    
};

struct AlgoXRAY1 {
    i32 dim = 2;
     
    inline i32 operator()(f32* const p1, f32* const p2) const {
        return 0;
    }    
};

struct AlgoXRAY2 { 
    i32 dim = 2;
     
    inline i32 operator()(f32* const p1, f32* const p2) const {
        return 0;
    }    
};

struct AlgoSPECIAL { 
    i32 dim = 2;
     
    inline i32 operator()(f32* const p1, f32* const p2) const {
        return 0;
    }    
};