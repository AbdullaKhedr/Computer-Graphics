//==================//
// Vertex Shader
//==================//
#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;

out vec3 vertColor;

uniform mat4 transform;

void main() 
{
	gl_Position = transform * vec4(pos.x, pos.y, pos.z, 1.0);
	vertColor = color;
}