cmake_minimum_required(VERSION 3.10)

# Create a test executable target
add_executable(my_tests)

# Add source files to the test executable
target_sources(my_tests
    PRIVATE
        test_main.cpp
	http_server_tests.cpp
        # Add other test source files here
)

# Link your test executable to the libraries you need (your project library, Google Test, and Google Mock if necessary)
target_link_libraries(my_tests
    PRIVATE
    	http_server
	SERVER
        gtest_main
        gmock_main
)

# Include your project's include directories
target_include_directories(my_tests
    PRIVATE
        ${PROJECT_SOURCE_DIR}/dependencies/my_networking_lib/src/server
	${PROJECT_SOURCE_DIR}/src
)

# Register the test executable with CTest
include(CTest)
add_test(NAME my_tests COMMAND my_tests)

