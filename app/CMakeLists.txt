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
	mpfr
	glfw3
	gdi32
	opengl32
	LibGlad
	Shaders
	gmp # needs to be last for some reason
)

target_include_directories(${PROJECT_NAME} PUBLIC 	
	{CMAKE_SOURCE_DIR}/lib/ 
	inc/ 
	generated/
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