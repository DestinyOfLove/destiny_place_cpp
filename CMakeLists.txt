cmake_minimum_required(VERSION 3.25)
project(destiny_place_cpp)

set(CMAKE_CXX_STANDARD 17)

# 递归查找所有 .cpp 文件
file(GLOB_RECURSE ALL_CPP_FILES *.cpp)

foreach (cpp_file ${ALL_CPP_FILES})
    # 获取文件名（不带扩展名）和相对路径
    get_filename_component(cpp_file_name ${cpp_file} NAME_WE)
    get_filename_component(cpp_file_path ${cpp_file} PATH)

    # 将路径中的斜杠替换为下划线，以避免目标名称中的非法字符
    string(REPLACE "/" "_" target_name ${cpp_file_path}_${cpp_file_name})

    # 创建可执行文件
    add_executable(${target_name} ${cpp_file})
endforeach ()
