
import numpy as np

from dataclasses import dataclass
from enum import IntEnum

@dataclass
class TSPParsed:
    class EdgeWeightType(IntEnum):
        EXPLICIT = 0
        EUC_2D   = 1
        EUC_3D   = 2
        MAX_2D   = 3
        MAX_3D   = 4
        MAN_2D   = 5
        MAN_3D   = 6
        CEIL_2D  = 7
        GEO      = 8
        ATT      = 9
        XRAY1    = 10
        XRAY2    = 11
        SPECIAL  = 12

    class EdgeWeightFormat(IntEnum):
        FUNCTION        = 0
        FULL_MATRIX     = 1
        LOWER_ROW       = 2
        UPPER_COL       = 3
        LOWER_COL       = 4
        UPPER_ROW       = 5
        LOWER_DIAG_ROW  = 6
        UPPER_DIAG_COL  = 7
        LOWER_DIAG_COL  = 8
        UPPER_DIAG_ROW  = 9

    class NodeCoordType(IntEnum):
        NO_COORDS     = 0
        TWOD_COORDS   = 1
        THREED_COORDS = 2

    class DisplayDataType(IntEnum):
        COORD_DISPLAY   = 0
        TWOD_DISPLAY    = 1
        NO_DISPLAY      = 2

    name: str = ""
    comment: str = ""
    dimension: int = 0

    edge_weight_type: EdgeWeightType = None
    edge_weight_format: EdgeWeightFormat = None
    edge_weights: list = None

    node_coord_type: NodeCoordType = None
    node_coords: list = None

    display_data_type: DisplayDataType = None
    display_data: list = None
    
    opt_path_indices: list = None
    opt_path_length: int = 0
    
    def get_display_range(self, display_data=None):
        if display_data is None:
            display_data = self.get_display_data()

        if display_data is None:
            return None
        
        x = display_data[:, 0]
        y = display_data[:, 1]
        
        return [[x.min(), x.max()], [y.min(), y.max()]]
    
    def get_opt_path(self, display_data=None):
        if self.opt_path_indices is None:
            return None
        
        if display_data is None:
            display_data = self.get_display_data()

        if display_data is None:
            return None

        return display_data[self.opt_path_indices]

    def get_display_data(self):
        if self.display_data_type == self.DisplayDataType.NO_DISPLAY:
            return None
        if self.display_data_type == self.DisplayDataType.TWOD_DISPLAY:
            return np.array(self.display_data)
        if self.node_coords:
            display_data = np.array(self.node_coords)
            
            if self.node_coord_type == "THREED_COORDS":
                display_data = display_data.reshape(-1, 3)
            else:
                display_data = display_data.reshape(-1, 2)

            return display_data

        return None
