
from enum import Enum

class Analytics(Enum):
    Progress          = 1
    CurrentBestLength = 2
    CurrentBestPath   = 4
    LastIterationBest = 8