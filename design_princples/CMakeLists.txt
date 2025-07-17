file(GLOB_RECURSE sources LIST_DIRECTORIES false *.cpp *.h)

add_library(design_principles_lib STATIC ${sources})
target_include_directories(design_principles_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})