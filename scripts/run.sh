
#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Building DLL..."
$SCRIPT_DIR/build_dll.sh

echo "Compiling Shaders..."
$SCRIPT_DIR/compile_shaders.sh

echo "Executing."
$SCRIPT_DIR/execute.sh