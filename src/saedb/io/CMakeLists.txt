cmake_minimum_required(VERSION 2.8)

project(mgraph)

set (MMAP_FILE_SOURCES "mmap_file.hpp")
if (WIN32)
    LIST (APPEND MMAP_FILE_SOURCES "mmap_file_win.cpp")
else()
    LIST (APPEND MMAP_FILE_SOURCES "mmap_file_posix.cpp")
endif(WIN32)

set(MGRAPH_LIBRARY_SOURCES
    "graph_builder.hpp"
    "graph_builder.cpp"
    "mgraph.hpp"
    "mgraph.cpp"
    "mmap_file.hpp"
    "type_info.cpp"
    "type_info.hpp"
    "../graph.hpp"
    "../serialization/serialization_includes.hpp"
    )

add_library(mmap_file ${MMAP_FILE_SOURCES})
add_executable(mmap_test mmap_file.hpp mmap_test.cpp)
target_link_libraries(mmap_test mmap_file sae_test)
add_test(NAME mmap_test COMMAND mmap_test)

add_library(mgraph ${MGRAPH_LIBRARY_SOURCES})
target_link_libraries(mgraph mmap_file)

add_executable(mgraph_test mgraph_test.cpp)
target_link_libraries(mgraph_test mgraph)
add_executable(type_info_test type_info_test.cpp type_info.cpp)
