cmake_minimum_required(VERSION 3.16)
project(diff_compare LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(CTest)
enable_testing()
option(DIFF_COMPARE_ENABLE_TESTS "Build diff_compare tests" ON)
option(DIFF_COMPARE_ENABLE_PERF "Build diff_compare performance benchmarks" ON)

find_package(fmt CONFIG REQUIRED)
if (DIFF_COMPARE_ENABLE_PERF)
    find_package(benchmark CONFIG QUIET)
endif()

add_library(diff_compare_lib
    src/app/ColumnDiffApp.cpp
    src/app/ColumnProcessingPipeline.cpp
    src/core/SeriesComparator.cpp
    src/core/SeriesComparatorFactory.cpp
    src/core/SeriesData.cpp
    src/core/SeriesDescriptor.cpp
    src/core/SeriesDiff.cpp
    src/core/ValueType.cpp
    src/io/ColumnInputProvider.cpp
    src/io/ColumnParser.cpp
    src/io/DiffOutputWriter.cpp
    src/io/OutputFormatter.cpp
)

target_include_directories(diff_compare_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(diff_compare_lib PUBLIC cxx_std_11)
target_link_libraries(diff_compare_lib PUBLIC fmt::fmt)

add_executable(diff_compare src/app/main.cpp)

target_link_libraries(diff_compare PRIVATE diff_compare_lib)

if (benchmark_FOUND)
    add_executable(diff_compare_perf
        perf/PerfBenchmarks.cpp
    )

    target_link_libraries(diff_compare_perf PRIVATE diff_compare_lib benchmark::benchmark fmt::fmt)
    if (APPLE)
        target_compile_definitions(diff_compare_perf PRIVATE BENCHMARK_FORCE_DISABLE_CPU_AFFINITY)
    endif()
else()
    message(STATUS "Google Benchmark not found; performance benchmarks are disabled. Install via vcpkg (vcpkg install benchmark) and reconfigure to enable.")
endif()

if (MSVC)
    target_compile_options(diff_compare_lib PRIVATE /W4 /permissive-)
    target_compile_options(diff_compare PRIVATE /W4 /permissive-)
    if (benchmark_FOUND)
        target_compile_options(diff_compare_perf PRIVATE /W4 /permissive-)
    endif()
else()
    target_compile_options(diff_compare_lib PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(diff_compare PRIVATE -Wall -Wextra -Wpedantic)
    if (benchmark_FOUND)
        target_compile_options(diff_compare_perf PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endif()

if (BUILD_TESTING AND DIFF_COMPARE_ENABLE_TESTS)
    find_package(GTest CONFIG REQUIRED)

    add_executable(diff_compare_tests
        test/test_diff_compare.cpp
    )
    target_link_libraries(diff_compare_tests PRIVATE diff_compare_lib GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(diff_compare_tests)
endif()
