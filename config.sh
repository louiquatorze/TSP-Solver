
#!/bin/bash
set -e

# --- SHARED DIRECTORY PATHS ---
export PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)" 

# --- SHADER DEFINITIONS ---
export SHADER_DIR="$PROJECT_ROOT/shaders"
export SHADER_SRC_DIR="$SHADER_DIR/glsl"
export SHADER_OUT_DIR="$SHADER_DIR/compiled"

# --- C++ DEFINITIONS ---
export CXX_DIR="$PROJECT_ROOT/c++"
export LIB_DIR="$PROJECT_ROOT/lib"

# --- PYTHON DEFINITIONS ---
export PYTHON_DIR="$PROJECT_ROOT/python"
export PYTHON_VENV="$PYTHON_DIR/.venv"

# --- TSP-Data DEFINITIONS ---
export DATA_DIR="$PROJECT_ROOT/data"

# --- SHADER COMPILATION DEFINITIONS ---
export SHADER_COMPILER="glslc"
export SHADER_FLAGS="-fshader-stage=compute -O --target-env=vulkan1.2"

# --- VULKAN ENGINE ENVIRONMENT OVERRIDES ---
export VK_ICD_FILENAMES="/usr/share/vulkan/icd.d/dzn_icd.json"
export DZN_DEBUG="adapter"
export APP_ENV="development"

# --- REUSABLE HELPER FUNCTIONS ---
log_info() {
    echo -e "\033[1;34m[INFO]\033[0m $1"
}

log_error() {
    echo -e "\033[1;31m[ERROR]\033[0m $1" >&2
}

# Verifies a tool is installed before trying to run it
check_dependency() {
    if ! command -v "$1" &> /dev/null; then
        log_error "Dependency tracking failed: '$1' could not be found."
        exit 1
    fi
}