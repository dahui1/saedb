cmake_minimum_required (VERSION 2.8)
set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include (SubDirList)

enable_testing()

project (SAE)

# binary output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${SAE_BINARY_DIR}")

# try enable c++11
# g++ and clang++ need flags to enable c++11
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++11" HAS_CPP11)
if (HAS_CPP11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# add external dependencies
add_all_subdirectories ("${SAE_SOURCE_DIR}/external")

# add includes for google-glog
include_directories ("${google-glog_SOURCE_DIR}/src")
# add includes for gflags
include_directories ("${gflags_BINARY_DIR}/include")

# add the core lib
add_subdirectory (src/saedb)

# add applications
add_subdirectory (src/app)
