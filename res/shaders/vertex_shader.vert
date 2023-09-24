#version 330 core
layout (location = 0) in vec3 aPos; // position has attribute position 0

uniform float u_time;

void main()
{
	gl_Position = vec4(aPos.xyz, 1.); // we give a vec3 to vec4's constructor
}