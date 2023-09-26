cmake_minimum_required(VERSION 3.27)

# save flags here
set(${PROJECT_NAME}_NON_LIB_C_FLAGS 	"${CMAKE_C_FLAGS}")
set(${PROJECT_NAME}_NON_LIB_CXX_FLAGS 	"${CMAKE_CXX_FLAGS}")

# disable here for Library builds
set(CMAKE_C_FLAGS "")
set(CMAKE_CXX_FLAGS "")

add_library(LibGlad STATIC glad.c)

target_include_directories(LibGlad PUBLIC 	
	../
)