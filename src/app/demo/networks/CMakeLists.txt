project(networks)

file(GLOB networks_SOURCES
    "networks.hpp"
    "runNetworks.cpp"
)

add_library(networks ${networks_SOURCES})
target_link_libraries(networks saedb sae_test)

