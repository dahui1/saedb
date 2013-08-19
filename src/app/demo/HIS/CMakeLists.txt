project(HIS)

file(GLOB HIS_TEST_SOURCES "*.hpp" "*.cpp")

add_executable(HIS_test ${HIS_TEST_SOURCES})
target_link_libraries(HIS_test saedb sae_test)

add_test(NAME HIS COMMAND HIS_test)
