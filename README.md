# Destiny's C++ Playground

This repository is a collection of C++ code, including solutions to LeetCode problems, algorithm implementations, explorations of design principles, and various C++ language feature tests.

The project is organized into a modular structure using CMake, where each major directory is a self-contained subproject.

## Project Structure

The codebase is divided into several modules:

-   `/algorithm`: Implementations of various algorithms.
-   `/cache`: Different cache policies (LRU, LFU).
-   `/design_princples`: Examples of software design principles.
-   `/leetcode`: Solutions to LeetCode problems.
-   `/memory_pool`: A simple memory pool implementation.
-   `/message_queue`: Implementations of message queues.
-   `/playground`: A space for daily tests and experiments.
-   `/some_knowledge`: Snippets demonstrating specific C++ features.

Each `.cpp` file in the `leetcode`, `algorithm`, and `playground` directories is compiled into a separate executable. The other directories are compiled as static libraries.

## Prerequisites

To build and run this project, you will need:
-   A C++11 compliant compiler (like Clang or GCC)
-   [CMake](https://cmake.org/) (version 3.20 or higher)
-   `make`

## Building the Project

1.  **Clone the repository:**
    ```bash
    git clone <repository-url>
    cd destiny_place_cpp
    ```

2.  **Create a build directory:**
    ```bash
    mkdir build
    cd build
    ```

3.  **Configure the project with CMake:**
    ```bash
    cmake ..
    ```

4.  **Compile all targets:**
    ```bash
    make
    ```
    This will compile all libraries and executables. The build artifacts will be located in the `build/` directory, organized by their respective modules.

## Running Executables

All executables are placed in subdirectories within the `build` folder.

For example, to run a LeetCode solution:
```bash
./build/leetcode/twoSum2
```

To run the cache test:
```bash
./build/cache/test_cache
```

## Adding New Code

Thanks to the modular CMake setup, adding new code is simple:

-   **For executables (e.g., a new LeetCode problem):**
    1.  Add your new `.cpp` file (e.g., `new_problem.cpp`) to the corresponding directory (e.g., `leetcode/`).
    2.  Run `make` from the `build` directory.
    3.  CMake will automatically detect the new file and build it. No need to edit any `CMakeLists.txt` files.

-   **For libraries:**
    1.  Add your new `.h` or `.cpp` files to the appropriate library directory (e.g., `cache/`).
    2.  Run `make` from the `build` directory.

## IDE Integration

For the best development experience, it is recommended to open the project's root directory in an IDE that supports CMake, such as:

-   [CLion](https://www.jetbrains.com/clion/)
-   [Visual Studio Code](https://code.visualstudio.com/) with the [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools) extension.

The IDE will automatically parse the `CMakeLists.txt` files and provide a user-friendly interface for building, running, and debugging individual targets.
