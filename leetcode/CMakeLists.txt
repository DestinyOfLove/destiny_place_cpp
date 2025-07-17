file(GLOB_RECURSE sources LIST_DIRECTORIES false *.cpp)

foreach(source_file ${sources})
    get_filename_component(executable_name ${source_file} NAME_WE)
    add_executable(${executable_name} ${source_file})
endforeach()
