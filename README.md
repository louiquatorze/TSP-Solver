# TSP-Solver

A graphical user interface for visualizing and running various Traveling Salesperson Problem (TSP) optimization algorithms.

---

## 🔧 Creating a New TSP Solver

Adding a new algorithm requires registering it within both the core C++ calculation engine and the Python PySide6 GUI layer. Follow these step-by-step instructions:

### 1. C++ Backend Architecture

1. **Declare the Interface:** Create a new header file in `c++/include/solvers/<your_solver>.h` and declare your solver as a subclass of `TSPSolver`.
2. **Register the Algorithm:** Add your new algorithm variant to the `Algorithm` enum located inside `c++/include/structs/AlgorithmSettings.h`.
3. **Configure Parameters:** Update the `AlgorithmSettings` struct fields if your algorithm requires specific hyper-parameters (such as custom iteration limits or thresholds).
4. **Update the Factory:** Add a new conditional `switch-case` block inside `c++/src/TSPSolverFactory::create` to instantiate your solver class when selected.
5. **Implement the Logic:** Write your core optimization algorithm inside `c++/src/solvers/<your_solver>.cpp`. 
6. **Update Build Files:** Add your new source file (`c++/src/solvers/<your_solver>.cpp`) to the `add_library` target block inside `CMakeLists.txt` so it compiles successfully.

### 2. Python UI & Boundary Layer

1. **Sync Algorithm Settings:** Update the configuration mapping inside `python/src/solver/structs/algorithm_settings.py` to match the C++ enum indexes and parameter payload structures precisely.
2. **Configure Graph Dependencies:** In `python/src/solver/structs/tsp.py` (`TSP` class), add your algorithm keys to the tracking dictionaries:
   * `has_edge_weights`
   * `has_heuristics`
   * `requires_beta`
   
   Set these explicitly to `True` or `False` based on your algorithm's mathematical input dependencies.
3. **Extend the GUI (`SolveTab`):** Modify `python/src/gui/controltabs/solve_tab.py` (`SolveTab` class) to expose the solver to the user:
   * **3.1 Dropdown:** Add your algorithm's display name as a new item to the `algo_combo` box layout.
   * **3.2 Custom Parameters Page:** Implement a `_create_<your_solver>_params()` helper method to build the specific parameter layout widgets. Call this during initialization to push the dynamic configuration page onto the `param_stack`.
   * **3.3 Event Binding:** Inside the `_on_solve_clicked()` method, add a corresponding `match-case` block to capture user configurations from the UI and pack them into the final `algorithm_settings` dictionary before triggering the background worker thread.