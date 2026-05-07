
cmake_minimum_required(VERSION 3.15)
project(TSPSolver)

add_library(TSPSolver SHARED 
    src/interface.cpp
    src/Context.cpp
    src/ComputeTSP.cpp
    src/Timer.cpp
    src/TSPSolverFactory.cpp
    
    src/solvers/TSPSolver.cpp 
    src/solvers/IterativeSolver.cpp 
    src/solvers/NearestNeighbourSolver.cpp 
    src/solvers/AntColonySolver.cpp 
)

target_include_directories(TSPSolver PUBLIC 
    include
    include/solvers
    include/structs
)
set_target_properties(TSPSolver PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

target_compile_definitions(TSPSolver PRIVATE BUILDING_DLL)