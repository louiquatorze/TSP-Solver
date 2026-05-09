
#pragma once

#include "types.h"

extern "C" {
    struct TSPRaw {
        enum EdgeWeightType : i32 {
            EXPLICIT = 0,
            EUC_2D,
            EUC_3D,
            MAX_2D,
            MAX_3D,
            MAN_2D,
            MAN_3D,
            CEIL_2D,
            GEO,
            ATT,
            XRAY1,
            XRAY2,
            SPECIAL
        };

        enum EdgeWeightFormat : i32 {
            FUNCTION = 0,
            FULL_MATRIX,
            LOWER_ROW,
            UPPER_COL,
            LOWER_COL,
            UPPER_ROW,
            LOWER_DIAG_ROW,
            UPPER_DIAG_COL,
            LOWER_DIAG_COL,
            UPPER_DIAG_ROW
        };

        enum NodeCoordType : i32 {
            NO_COORDS = 0,
            TWOD_COORDS,
            THREED_COORDS
        };

        i32 dimension;
        
        EdgeWeightType edgeWeightType;
        EdgeWeightFormat edgeWeightFormat;
        NodeCoordType nodeCoordType;
        
        u32* edgeWeights;
        f32* nodeCoords;
    };
}