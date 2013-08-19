project(xgytest)

file(GLOB PAGERANK_TEST_SOURCES "*.hpp" "*.cpp")

add_executable(xgytest_test ${PAGERANK_TEST_SOURCES})
target_link_libraries(xgytest_test saedb sae_test)

add_test(NAME xgytest COMMAND xgytest_test)
