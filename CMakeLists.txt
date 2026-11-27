cmake_minimum_required(VERSION 3.14)
project(http_server LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
include(FetchContent)
include(CTest)
find_package(Threads REQUIRED)
FetchContent_Declare(SERVER
  GIT_REPOSITORY https://github.com/JacobBorden/NetworkingLibrary.git
  GIT_TAG 928837c83c3a72d8102ee8f3ffd0703a0cd44018)
FetchContent_MakeAvailable(SERVER)
add_library(http_server src/http.cpp)
target_include_directories(http_server PUBLIC src "${server_SOURCE_DIR}/src/server")
target_link_libraries(http_server PUBLIC SERVER Threads::Threads)
add_executable(server.exe src/main.cpp)
target_link_libraries(server.exe PRIVATE http_server)
if(BUILD_TESTING)
  FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG b514bdc898e2951020cbdca1304b75f5950d1f59)
  FetchContent_MakeAvailable(googletest)
  configure_file(tests/test_resources/index.html tests/public/index.html COPYONLY)
  configure_file(tests/test_resources/binary.txt tests/public/binary.txt COPYONLY)
  add_subdirectory(tests)
endif()
