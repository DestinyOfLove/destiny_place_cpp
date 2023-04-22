cmake_minimum_required(VERSION 3.25)
project(destiny_place_cpp)

set(CMAKE_CXX_STANDARD 11)

# make all cpp files executable
file(GLOB_RECURSE ALL_CPP_FILES *.cpp)
foreach (cpp_file ${ALL_CPP_FILES})
    get_filename_component(cpp_file_name ${cpp_file} NAME_WE)
    add_executable(${cpp_file_name} ${cpp_file})
endforeach ()
