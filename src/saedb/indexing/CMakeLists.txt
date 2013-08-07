cmake_minimum_required (VERSION 2.8)

project (indexing)

file (GLOB INDEXING_SOURCES
    "analyzer.hpp"
    "ICTCLAS50.h"
    "indexing.hpp"
    "indexing.cpp"
    "query.hpp"
    "query.cpp"
    "search.hpp"
    )

add_library(indexing ${INDEXING_SOURCES})
target_link_libraries(indexing ArnetAnalyzer)

add_executable(indexing_test indexing_test.cpp)
target_link_libraries(indexing_test indexing sae_test)

file (GLOB stream_test_SOURCES "test.cpp")
add_executable(stream_test ${stream_test_SOURCES})
target_link_libraries(stream_test ArnetAnalyzer)

add_test(NAME indexing_test COMMAND indexing_test)

add_subdirectory(analyzer)
