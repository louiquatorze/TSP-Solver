
#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../config.sh"

mkdir -p "$SHADER_OUT_DIR"
mkdir -p "$SHADER_OUT_DIR/iterative"
mkdir -p "$SHADER_OUT_DIR/antColony"

INCLUDE_FLAG="-I$SHADER_SRC_DIR"

################## Iterative ##################

$SHADER_COMPILER $SHADER_FLAGS $INCLUDE_FLAG \
    "$SHADER_SRC_DIR/iterative/calculateCS.comp" \
    -o "$SHADER_OUT_DIR/iterative/calculateCS.comp.spv"

################## AntColony ##################

$SHADER_COMPILER $SHADER_FLAGS $INCLUDE_FLAG \
    "$SHADER_SRC_DIR/antColony/simulateAntsCS.comp" \
    -o "$SHADER_OUT_DIR/antColony/simulateAntsCS.comp.spv"

$SHADER_COMPILER $SHADER_FLAGS $INCLUDE_FLAG \
    "$SHADER_SRC_DIR/antColony/rewardBestPathCS.comp" \
    -o "$SHADER_OUT_DIR/antColony/rewardBestPathCS.comp.spv"

    