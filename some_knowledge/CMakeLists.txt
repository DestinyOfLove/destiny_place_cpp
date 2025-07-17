file(GLOB_RECURSE sources LIST_DIRECTORIES false *.cpp)

add_library(some_knowledge_lib STATIC ${sources})
target_include_directories(some_knowledge_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})