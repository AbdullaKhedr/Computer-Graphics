//==================//
// Fragment Shader
//==================//
#version 330 core
in vec2 TexCoord;
out vec4 fragColor;

// Samplers to read textures
uniform sampler2D texSampler1;
uniform sampler2D texSampler2;

void main() 
{
	vec4 backgroungTexColor = texture(texSampler1, TexCoord); // Background picture
	vec4 marioTexColor = texture(texSampler2, TexCoord); // Front picture
	fragColor = mix(backgroungTexColor, marioTexColor, marioTexColor.a);
}