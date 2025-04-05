cmake_minimum_required(VERSION 3.10)
project(destiny_place_cpp)
set(CMAKE_CXX_STANDARD 11)

# 递归查找所有 .cpp 文件
file(GLOB_RECURSE ALL_CPP_FILES *.cpp)

# 包含头文件目录
include_directories(
    ${CMAKE_SOURCE_DIR}/algorithm
    ${CMAKE_SOURCE_DIR}/cache
    ${CMAKE_SOURCE_DIR}/design_princples
    ${CMAKE_SOURCE_DIR}/leetcode
    ${CMAKE_SOURCE_DIR}/some_knowledge
)

set(MAIN_CONTAINING_FILES "")
foreach(CPP_FILE ${ALL_CPP_FILES})
    file(READ ${CPP_FILE} FILE_CONTENTS)
    string(FIND "${FILE_CONTENTS}" "int main(" MAIN_FOUND)
    if(NOT ${MAIN_FOUND} EQUAL -1)
        list(APPEND MAIN_CONTAINING_FILES ${CPP_FILE})
    endif()
endforeach()

foreach(CPP_FILE ${MAIN_CONTAINING_FILES})
    get_filename_component(EXECUTABLE_NAME ${CPP_FILE} NAME_WE)
    add_executable(${EXECUTABLE_NAME} ${CPP_FILE})
endforeach()