
import numpy as np
import os

from enum import Enum
from pathlib import Path
from src.tsp_parsed import TSPParsed

class TSPParser:
    @staticmethod
    def parse(path):
        class SectionType(Enum):
            EdgeWeight = 0
            NodeCoord = 1
            DisplayData = 2
        
        with open(path, "r") as file:
            lines = file.readlines()
        
        section_type = None
        tsp_parsed = TSPParsed()
        
        for line in lines:
            line = line.strip()

            if not line:
                continue

            if line in ["-1", "EOF"]:
                break
            
            if ":" in line:
                section_type = None
                
                (key, value) = line.split(":", 1)
                key = key.strip().lower()
                value = value.strip()

                if not hasattr(tsp_parsed, key):
                    continue

                if key in ["dimension", "capacity"]:
                    value = int(value.split()[0])
                elif key == "edge_weight_type":
                    value = TSPParsed.EdgeWeightType[value]
                elif key == "edge_weight_format":
                    value = TSPParsed.EdgeWeightFormat[value]
                elif key == "node_coord_type":
                    value = TSPParsed.NodeCoordType[value]
                elif key == "display_data_type":
                    value = TSPParsed.DisplayDataType[value]

                setattr(tsp_parsed, key, value)
            
            elif line == "EDGE_WEIGHT_SECTION":
                section_type = SectionType.EdgeWeight
                
                edge_weights_current_index = 0
                dim = tsp_parsed.dimension

                if tsp_parsed.edge_weight_format == TSPParsed.EdgeWeightFormat.FULL_MATRIX:
                    edge_weight_count = tsp_parsed.dimension * tsp_parsed.dimension
                elif "DIAG" in tsp_parsed.edge_weight_format.name:
                    edge_weight_count = (dim + 1) * dim // 2    
                else:
                    edge_weight_count = dim * (dim - 1) // 2
                
                tsp_parsed.edge_weights = [None] * edge_weight_count
                continue
            
            elif line == "NODE_COORD_SECTION":
                section_type = SectionType.NodeCoord
                coord_dim = 3 if tsp_parsed.node_coord_type == TSPParsed.NodeCoordType.THREED_COORDS else 2
                tsp_parsed.node_coords = [None] * (tsp_parsed.dimension * coord_dim)
                continue
            
            elif line == "DISPLAY_DATA_SECTION":
                section_type = SectionType.DisplayData
                tsp_parsed.display_data = [None] * tsp_parsed.dimension
                continue
            
            if not section_type:
                continue
            
            if section_type == SectionType.EdgeWeight:
                new_edge_weights = line.split()
                
                for new_edge_weight in new_edge_weights:
                    tsp_parsed.edge_weights[edge_weights_current_index] = int(new_edge_weight)
                    edge_weights_current_index += 1

            elif section_type == SectionType.NodeCoord:
                (i, *coords) = line.split()

                index = (int(i) - 1) * coord_dim
                for coord in coords:
                    tsp_parsed.node_coords[index] = float(coord)
                    index += 1

            elif section_type == SectionType.DisplayData:
                (i, *coords) = line.split()
                tsp_parsed.display_data[int(i) - 1] = [ float(coord) for coord in coords ]

        opt_path = f"{ str(Path(path).parent.parent) }/opt.tour/{ tsp_parsed.name }.opt.tour"
        
        if os.path.isfile(opt_path):
            tsp_parsed.opt_path_indices = [None] * (tsp_parsed.dimension + 1)

            with open(opt_path, "r") as file:
                lines = file.readlines()

            tour_section = False
            tour_index = 0

            for line in lines:
                line = line.strip()

                if not line:
                    continue
                
                if line in ["-1", "EOF"]:
                    break

                if not tour_section:
                    if line == "TOUR_SECTION":
                        tour_section = True
                        continue
                else:
                    for coord in line.split():
                        tsp_parsed.opt_path_indices[tour_index] = int(coord) - 1
                        tour_index += 1

            tsp_parsed.opt_path_indices[tsp_parsed.dimension] = tsp_parsed.opt_path_indices[0]
        
        # For simplicity; No actual enforcement of name singularity implemented
        tsp_parsed.id = tsp_parsed.name

        return tsp_parsed