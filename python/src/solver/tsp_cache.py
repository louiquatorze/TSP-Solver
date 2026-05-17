
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

        self._has_edge_weights = TSP.has_edge_weights[algorithm] 
        self._has_heuristics = TSP.has_heuristics[algorithm]
        self._requires_beta = TSP.requires_beta[algorithm]

    def is_cached(self, id, beta, algorithm):
        print("c")
        
        has_edge_weights = TSP.has_edge_weights[algorithm] 
        has_heuristics = TSP.has_heuristics[algorithm]
        requires_beta = TSP.requires_beta[algorithm]
        
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