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

add_definitions(-DOS_LINUX)
#add_library(ICTCLAS50 SHARED IMPORTED )
add_library(indexing ${INDEXING_SOURCES})
target_link_libraries(indexing ArnetAnalyzer ${SAE_SOURCE_DIR}/src/saedb/indexing/libICTCLAS50.a)

add_executable(indexing_test indexing_test.cpp)
target_link_libraries(indexing_test indexing sae_test)

add_test(NAME indexing_test COMMAND indexing_test)

add_subdirectory(analyzer)
