cmake_minimum_required (VERSION 2.8)

project (indexing)

find_package(Protobuf REQUIRED)

include_directories ("${SAE_SOURCE_DIR}/src/saedb")
include_directories("${PROTOBUF_INCLUDE_DIRS}")
include_directories("${CMAKE_CURRENT_BINARY_DIR}")

file (GLOB INDEXING_SOURCES
	"analyzer.hpp"
	"indexing.hpp"
	"indexing.cpp"
	"query.hpp"
	"query.cpp"
	)
add_library(indexing ${INDEXING_SOURCES})
target_link_libraries(indexing ArnetAnalyzer saedb)

file (GLOB aminerimporter_SOURCES "aminerimporter.cpp")
add_executable(aminerimporter ${aminerimporter_SOURCES})
target_link_libraries(aminerimporter mgraph)

add_subdirectory(analyzer)
