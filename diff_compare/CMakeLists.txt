cmake_minimum_required(VERSION 3.16)
project(diff_compare LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(diff_compare_lib
    src/ColumnComparator.cpp
    src/ColumnComparatorFactory.cpp
    src/ColumnData.cpp
    src/ColumnDescriptor.cpp
    src/ColumnDiff.cpp
    src/ColumnDiffApp.cpp
    src/ColumnInputProvider.cpp
    src/ColumnParser.cpp
    src/ColumnProcessingPipeline.cpp
    src/ColumnType.cpp
    src/DiffOutputWriter.cpp
    src/OutputFormatter.cpp
)

target_include_directories(diff_compare_lib
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_features(diff_compare_lib PUBLIC cxx_std_11)

add_executable(diff_compare
    src/main.cpp
)

target_link_libraries(diff_compare PRIVATE diff_compare_lib)

if (MSVC)
    target_compile_options(diff_compare_lib PRIVATE /W4 /permissive-)
    target_compile_options(diff_compare PRIVATE /W4 /permissive-)
else()
    target_compile_options(diff_compare_lib PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(diff_compare PRIVATE -Wall -Wextra -Wpedantic)
endif()
