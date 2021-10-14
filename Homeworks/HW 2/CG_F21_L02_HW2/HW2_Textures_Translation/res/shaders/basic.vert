//==================//
// Vertex Shader
//==================//
#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texCoord;

out vec2 TexCoord;

uniform mat4 transform;

void main() 
{
	gl_Position = transform * vec4(pos.x, pos.y, pos.z, 1.0);

	// Out textures with flipping
	TexCoord = vec2(texCoord.x, 1.0 -texCoord.y);
}