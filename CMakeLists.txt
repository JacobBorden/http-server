cmake_minimum_required(VERSION 3.10)
project(http_server)

#Download and include server library
include(FetchContent)
FetchContent_Declare(
	SERVER
	GIT_REPOSITORY https://github.com/JacobBorden/NetworkingLibrary.git
	SOURCE_DIR ../dependencies/my_networking_lib
	)

	FetchContent_MakeAvailable(SERVER)
	set (CMAKE_THREAD_PREFER_PTHREAD ON)
	set (THREADS_PREFER_PTHREAD_FLAG ON)
	find_package(Threads)
#Add the library to the include directories
include_directories(dependencies/my_networking_lib/src/server)

add_executable(my_web_server main.cpp http.h http.cpp)
target_include_directories(my_web_server PRIVATE dependencies/src/server)
target_link_libraries(my_web_server SERVER Threads::Threads)
