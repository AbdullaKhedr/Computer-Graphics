//==================//
// Vertex Shader:
// (Programmable, not optional, Handels vertices individually, 
//  take vertex data from GPU memory as input)
//==================//
#version 330 core
// Attribute Pointers: define where and how shaders can access vertex data.
layout (location = 0) in vec3 pos; // So here,  in location 0 in GPU memory, you will find data in vec3, all them pos (cuz they are position data).
layout (location = 1) in vec2 texCoord; // this in vec2 cuz its only (u,v) or (s,t) (remember texture coordinates!)

out vec2 TexCoord; // output (color/texture) to be used in the fragment shader

// Uniform Variables: solution to access cpu variable by the gpu
// are constant for an entire primitive
// Can be changed in application and sent to shaders but 
// Cannot be changed in shader
// Used to pass information to shader
uniform mat4 transform;

void main() 
{
	gl_Position = transform * vec4(pos.x, pos.y, pos.z, 1.0); // Swizzling ==> pos.x OR pos[1] 
	TexCoord = texCoord;

	// method 1 to flip texture
	TexCoord = vec2(texCoord.x, 1.0 -texCoord.y);
}

// Rasterization: 
// converts primitives to Fragments (groub of pixels).
// pixels between will Interpolated.
// the input is primitive, and the output is pixel.

// Per-Sample Operations: which is infront and which is back (z-buffering: Depth test).
