
enum Analytics {
    Progress          = 1,
    CurrentBestLength = 2,
    CurrentBestPath   = 4,
    LastIterationBest = 8,
    
    All = Progress | CurrentBestLength | CurrentBestPath
};