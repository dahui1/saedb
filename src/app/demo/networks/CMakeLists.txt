project(networks)

file(GLOB PAGERANK_TEST_SOURCES "*.hpp" "*.cpp")

add_executable(networks_test ${PAGERANK_TEST_SOURCES})
target_link_libraries(networks_test saedb sae_test)

add_test(NAME networks COMMAND networks_test)
