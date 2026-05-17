
from src.solver.structs.tsp import TSP

class TSPCache:
    tsp: TSP = None

    _id: str = ""
    _beta: float = 0.0

    _has_edge_weights: bool = False
    _has_heuristics: bool = False 
    _requires_beta: bool = False 

    def set(self, tsp, id, beta, algorithm):
        self.tsp = tsp

        self._id = id
        self._beta = beta

        self._has_edge_weights = algorithm in TSP.has_edge_weights
        self._has_heuristics = algorithm in TSP.has_heuristics
        self._requires_beta = algorithm in TSP.requires_beta

    def is_cached(self, id, beta, algorithm):
        has_edge_weights = algorithm in TSP.has_edge_weights
        has_heuristics = algorithm in TSP.has_heuristics
        requires_beta = algorithm in TSP.requires_beta
        
        if self.tsp is None:
            return False
        
        if id != self._id:
            return False
        
        if has_edge_weights != self._has_edge_weights:
            return False
        if has_heuristics != self._has_heuristics:
            return False
        if requires_beta != self._requires_beta:
            return False
        
        if requires_beta and beta != self._beta:
            return False
            
        return True