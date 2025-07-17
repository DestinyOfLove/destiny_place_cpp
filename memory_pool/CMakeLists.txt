file(GLOB_RECURSE sources LIST_DIRECTORIES false *.cpp *.h)

add_library(memory_pool_lib STATIC ${sources})
target_include_directories(memory_pool_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
