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
file(GLOB MySources "src/*.cpp")

add_executable(server.exe ${MySources})
target_link_libraries(server.exe SERVER Threads::Threads)
set_target_properties(server.exe PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
