cmake_minimum_required(VERSION 3.27)

set(CMAKE_C_COMPILER 		"gcc")
set(CMAKE_CXX_COMPILER 		"g++")

project(Mandlebrot
		VERSION 		1.0
		DESCRIPTION 	"Generates a mandlebrot"
		LANGUAGES		C CXX)

set(CMAKE_SYSTEM_NAME 		Generic)
set(CMAKE_SYSTEM_VERSION 	1)

set(CMAKE_C_STANDARD 11 CACHE STRING "Version of the C standard to use")
set(CMAKE_CXX_STANDARD 20 CACHE STRING "Version of the C++ standard to use")
option(CMAKE_C_STANDARD_REQUIRED "Enable to enforce strict C standard selection" ON)
option(CMAKE_CXX_STANDARD_REQUIRED "Enable to enforce strict C standard selection" ON)
option(CMAKE_C_EXTENSIONS "Enable to allow compiler-specific C extensions" OFF)

set(CMAKE_VERBOSE_MAKEFILE 1)

add_subdirectory(lib/)
add_subdirectory(tools/)
add_subdirectory(app/)

find_package(glfw3 3.3 REQUIRED)