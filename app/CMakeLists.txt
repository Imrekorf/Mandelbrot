cmake_minimum_required(VERSION 3.27)

add_subdirectory(generated/)

# Define mandelbrot output
add_executable(${PROJECT_NAME}
	main.cpp
	src/util.cpp
)

target_link_libraries(${PROJECT_NAME} PUBLIC 	
	-Wl,-Bstatic # hack to make sure that libraries asre linked statically
	stdc++
	pthread
	glfw3
	gdi32
	opengl32
	LibGlad
	LibBigNum
	Shaders
)

target_include_directories(${PROJECT_NAME} PUBLIC 	
	${CMAKE_SOURCE_DIR}/lib/
	${CMAKE_SOURCE_DIR}/lib/ttmath/ttmath/
	${CMAKE_SOURCE_DIR}/app/inc/ 
	${CMAKE_SOURCE_DIR}/app/generated/
)

target_link_options(${PROJECT_NAME} PUBLIC

)

target_compile_definitions(${PROJECT_NAME} PUBLIC
	# Debug definitions
	$<$<CONFIG:DEBUG>:DEBUG>
	# Release definitions
	$<$<CONFIG:RELEASE>:NDEBUG>
	# C defintions
	# $<$<COMPILE_LANG_AND_ID:C,GNU>:__LANGUAGE=C>
	# C++ definitions
	# $<$<COMPILE_LANG_AND_ID:CXX,GNU>:__LANGUAGE=CXX>
)