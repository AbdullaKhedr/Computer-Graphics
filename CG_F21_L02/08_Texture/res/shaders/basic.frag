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


	// getting the color from one texture
	//fragColor = texture(texSampler1, TexCoord);


	// Two Options: getting the color from (mix OR overlaying) 2 texture
	vec4 backgroungTexColor = texture(texSampler1, TexCoord); // the background picture
	vec4 marioTexColor = texture(texSampler2, TexCoord); //the mario picture
	// Option 1- Mixing buy using a percentage 
	//fragColor = mix(backgroungTexColor, marioTexColor, 0.7);
	// Option 2- Overlaying by using alpha transparency
	fragColor = mix(backgroungTexColor, marioTexColor, marioTexColor.a);

}