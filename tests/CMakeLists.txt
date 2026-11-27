add_executable(my_tests http_server_tests.cpp)
target_link_libraries(my_tests PRIVATE http_server GTest::gtest_main)
include(GoogleTest)
gtest_discover_tests(my_tests WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}" DISCOVERY_TIMEOUT 30)
