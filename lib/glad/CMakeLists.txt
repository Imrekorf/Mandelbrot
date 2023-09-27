cmake_minimum_required(VERSION 3.27)

# disable warning flags here for Library builds
string(REPLACE $ENV{WARNING_FLAGS} "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
string(REPLACE $ENV{WARNING_FLAGS} "" CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

add_library(LibGlad STATIC glad.c)

target_include_directories(LibGlad PUBLIC 	
	../
)