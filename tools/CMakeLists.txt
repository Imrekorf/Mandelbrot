cmake_minimum_required(VERSION 3.27)

project(ShaderGenTool
		VERSION 		1.0
		DESCRIPTION 	"Generates shader source files"
		LANGUAGES		C CXX)

add_executable(ShaderGenTool 
	ShaderGenTool.cpp)

add_dependencies(ShaderGenTool GIT_HASH)

target_link_libraries(ShaderGenTool PUBLIC 	

)

target_compile_options(ShaderGenTool PUBLIC

)

target_include_directories(ShaderGenTool PUBLIC 	
	${CMAKE_SOURCE_DIR}/res/
)

target_link_options(ShaderGenTool PUBLIC

)

target_compile_definitions(ShaderGenTool PUBLIC
	# Debug definitions
	$<$<CONFIG:DEBUG>:DEBUG>
	# Release definitions
	$<$<CONFIG:RELEASE>:NDEBUG>
	# C defintions
	# $<$<COMPILE_LANG_AND_ID:C,GNU>:__LANGUAGE=C>
	# C++ definitions
	# $<$<COMPILE_LANG_AND_ID:CXX,GNU>:__LANGUAGE=CXX>
)