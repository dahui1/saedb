cmake_minimum_required (VERSION 2.8)

project (indexing)

file(GLOB INDEXING_SOURCES
    "anaylyzer.hpp"
    "indexing.hpp"
    "indexing.cpp"
    "query.hpp"
    "search.hpp"
    )

add_library(indexing ${INDEXING_SOURCES})
target_link_libraries(indexing ArnetAnalyzer)

add_executable(indexing_test search_test.cpp)
target_link_libraries(indexing_test indexing)

add_subdirectory(analyzer)
