add_library(cache_lib INTERFACE)
target_include_directories(cache_lib INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE test_sources LIST_DIRECTORIES false test/*.cpp)

foreach(test_source ${test_sources})
    get_filename_component(executable_name ${test_source} NAME_WE)
    add_executable(${executable_name} ${test_source})
    target_link_libraries(${executable_name} PRIVATE cache_lib)
endforeach()
