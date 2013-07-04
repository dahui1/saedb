cmake_minimum_required (VERSION 2.8)

project (indexing)

file(GLOB INDEXING_SOURCES
    "*.hpp"
    "*.cpp"
    )

add_executable(indexing_test ${INDEXING_SOURCES})
target_link_libraries(indexing_test ArnetAnalyzer)

add_subdirectory(analyzer)
