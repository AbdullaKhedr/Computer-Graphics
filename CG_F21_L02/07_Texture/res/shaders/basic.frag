#version 330 core
out vec4 fragColor;
in vec2 TexCoord;

uniform vec4 vertColor;
uniform sampler2D texSampler1;

void main() 
{
	//fragColor = vertColor;
	fragColor = texture(texSampler1, TexCoord);
}