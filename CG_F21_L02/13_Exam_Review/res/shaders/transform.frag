//==================//
// Fragment Shader:
// (programmable, Optional but it’s rare to not use it, 
//  Handles data for each fragment (assign the color for each fragment), 
//  the output is the color of the pixel that a fragmet covers).
//==================//
#version 330 core
out vec4 fragColor;
in vec2 TexCoord;


uniform vec4 vertColor;
// Samplers to read textures (they must be same name as the one in glUniform1i() function).
uniform sampler2D texSampler1;
uniform sampler2D texSampler2;

void main() 
{
	// getting only color
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

/*
	Textures in Shaders are accessed via “Samplers”.
	Textures are attached to a “Texture Unit”.
	Samplers access textures attached to their Texture Unit.
	In shader, use “sampler2D” type. 
	To get the value of a texel, use GLSL “texture” function. 
		texture(textureSampler, TexCoord);
	textureSampler: the sampler2D object.
	TexCoord: the interpolated texel coordinate in the Fragment Shader. 
*/