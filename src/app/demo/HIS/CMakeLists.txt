project(HIS)

file(GLOB HIS_SOURCES
    "HIS.hpp"
    "runHIS.cpp")

add_library(HIS ${HIS_SOURCES})
target_link_libraries(HIS saedb sae_test)

