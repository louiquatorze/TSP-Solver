
#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$SCRIPT_DIR/../config.sh"

cmake -S "$CXX_DIR" \
      -B "$CXX_DIR/build" \
      -DCMAKE_BUILD_TYPE=Release

cmake --build "$CXX_DIR/build" -j $(nproc)