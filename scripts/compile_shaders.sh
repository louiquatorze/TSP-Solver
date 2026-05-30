
#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../config.sh"

mkdir -p "$SHADER_OUT_DIR"

$SHADER_COMPILER $SHADER_FLAGS \
    "$SHADER_SRC_DIR/iterativeCS.comp" \
    -o "$SHADER_OUT_DIR/iterativeCS.spv"

$SHADER_COMPILER $SHADER_FLAGS \
    "$SHADER_SRC_DIR/antColonyCS.comp" \
    -o "$SHADER_OUT_DIR/antColonyCS.spv"