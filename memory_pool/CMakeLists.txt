cmake_minimum_required(VERSION 3.16)

project(memory_pool)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- Find Dependencies via vcpkg ---
find_package(nlohmann_json REQUIRED)

# --- Library Definitions ---

add_library(fixed_memory_pool fixed_memory_pool/src/fixed_memory_pool.cpp)
target_include_directories(fixed_memory_pool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/fixed_memory_pool/include)

add_library(ts_fixed_memory_pool_mutex thread_safe_fixed_memory_pool_mutex/src/fixed_memory_pool.cpp)
target_include_directories(ts_fixed_memory_pool_mutex PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thread_safe_fixed_memory_pool_mutex/include)

add_library(ts_fixed_memory_pool_lock_free thread_safe_fixed_memory_pool_lock_free/src/fixed_memory_pool.cpp)
target_include_directories(ts_fixed_memory_pool_lock_free PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thread_safe_fixed_memory_pool_lock_free/include)

add_library(ts_thread_local_memory_pool thread_safe_fixed_memory_pool_thread_local/src/thread_local_memory_pool.cpp)
target_include_directories(ts_thread_local_memory_pool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/thread_safe_fixed_memory_pool_thread_local/include)
target_link_libraries(ts_thread_local_memory_pool PUBLIC ts_fixed_memory_pool_lock_free)

add_library(general_memory_pool general_memory_pool/src/general_memory_pool.cpp)
target_include_directories(general_memory_pool PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/general_memory_pool/include)
target_link_libraries(general_memory_pool PUBLIC ts_thread_local_memory_pool)


# --- Test Executables ---
enable_testing()

add_executable(test_memory_pool test/test_memory_pool.cpp)
target_link_libraries(test_memory_pool PRIVATE
    fixed_memory_pool
    ts_fixed_memory_pool_mutex
    ts_fixed_memory_pool_lock_free
    ts_thread_local_memory_pool
    general_memory_pool
)
add_test(NAME CorrectnessTests COMMAND test_memory_pool)

add_executable(benchmark_memory_pool test/benchmark_memory_pool.cpp)
target_link_libraries(benchmark_memory_pool PRIVATE
    fixed_memory_pool
    ts_fixed_memory_pool_mutex
    ts_fixed_memory_pool_lock_free
    ts_thread_local_memory_pool
    general_memory_pool
    nlohmann_json::nlohmann_json
)
add_test(NAME BenchmarkTest COMMAND benchmark_memory_pool)