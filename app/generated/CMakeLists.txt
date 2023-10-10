cmake_minimum_required(VERSION 3.27)

set(SHADER_DEPENDENCIES
	${CMAKE_SOURCE_DIR}/res/shaders/fragment_shader.frag
	${CMAKE_SOURCE_DIR}/res/shaders/bignum.frag
	${CMAKE_SOURCE_DIR}/res/shaders/vertex_shader.vert
	${CMAKE_SOURCE_DIR}/res/gen_shaders.sh)

add_custom_command(
	OUTPUT ${CMAKE_SOURCE_DIR}/app/generated/gen_shaders.cpp
	COMMAND bash -c "res/gen_shaders.sh"
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
	DEPENDS ${SHADER_DEPENDENCIES}
	BYPRODUCTS ${CMAKE_SOURCE_DIR}/app/generated/gen_shaders.h
)

add_library(Shaders STATIC gen_shaders.cpp)

# Add dependency to ShaderGenTool
add_dependencies(Shaders ShaderGenTool)