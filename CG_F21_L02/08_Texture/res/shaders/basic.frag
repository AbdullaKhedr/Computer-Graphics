//==================//
// Fragment Shader
//==================//
#version 330 core
out vec4 fragColor;
in vec2 TexCoord;

uniform vec4 vertColor;
uniform sampler2D texSampler1;
uniform sampler2D texSampler2;

void main() 
{
	//fragColor = vertColor;

	// having the color from one texture
	//fragColor = texture(texSampler1, TexCoord);

	// having the color from mix of 2 texture
	fragColor = mix(texture(texSampler1, TexCoord),texture(texSampler2, TexCoord),0.7);
}