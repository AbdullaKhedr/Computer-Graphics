#version 330 core
out vec4 fragColor;

uniform vec4 vertColor;

void main() 
{
	fragColor = vertColor;
}