
cmake_minimum_required(VERSION 3.15)
project(TSPSolver LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Vulkan REQUIRED)

add_library(TSPSolver SHARED 
    src/Context.cpp
    src/interface.cpp
    
    src/util/GraphUtil.cpp
    src/util/PathService.cpp
    src/util/Timer.cpp
    
    src/preprocessing/ComputeTSP.cpp

    src/solvers/TSPSolver.cpp 
    src/solvers/TSPSolverFactory.cpp
    src/solvers/IterativeSolver.cpp 
    src/solvers/NearestNeighbourSolver.cpp 
    src/solvers/AntColonySolver.cpp 
    src/solvers/ChristofidesSolver.cpp

    src/vulkan/VulkanContext.cpp
    src/vulkan/VulkanService.cpp

    src/vulkan/engine/VulkanCore.cpp
    src/vulkan/engine/VulkanEngine.cpp
    src/vulkan/engine/VulkanTSPResourceManager.cpp

    src/vulkan/engine/management/TSPDescriptorSetBundle.cpp
    src/vulkan/engine/management/VulkanTSPPipelineManager.cpp
    src/vulkan/engine/management/VulkanTSPMemoryManager.cpp
)

include(FetchContent)
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG        v0.9.5 # Or latest stable
)
FetchContent_MakeAvailable(magic_enum)

target_include_directories(TSPSolver PUBLIC 
    include
    include/preprocessing
    include/util
    include/solvers
    include/structs

    include/vulkan
    include/vulkan/engine
    include/vulkan/engine/management
)
set_target_properties(TSPSolver PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/../lib")

target_link_libraries(TSPSolver 
    PRIVATE Vulkan::Vulkan
    PRIVATE magic_enum::magic_enum
)

target_compile_definitions(TSPSolver PRIVATE BUILDING_DLL)