PROJECT_ROOT="/home/louis/projects/TSP Solver"

cmake -S "$PROJECT_ROOT"/c++ -B "$PROJECT_ROOT"/c++/build -DCMAKE_BUILD_TYPE=Realease
cmake --build "$PROJECT_ROOT/c++/build" -j $(nproc)