//-----------------------------------------------------------------------------
// Transformations Tutorial
//-----------------------------------------------------------------------------

#include <iostream> // Deals with input/output
#define GLEW_STATIC // Important to be static
#include "GL/glew.h" // include glew before glfw
#include "GLFW/glfw3.h"
#include "ShaderProgram.h"
#include"Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;

// Global Variables
const char* APP_TITLE = "Computer Graphics - Transformation 3 (Basic)";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gmainWindow;
bool gWireframe = false;

// Old Shaders
// 3.1 Get the shader (usully from file), but for now it's just an array of chars
const GLchar* vertexShaderSrc =
"#version 330 core\n"
"layout (location = 0) in vec3 pos;"
"layout (location = 1) in vec3 color;"
"out vec3 vertColor;"
"void main() {"
"	vertColor = color;"
"	gl_Position = vec4(pos.x, pos.y, pos.z, 1.0);"
"}";
// 4.1 Get the shader (usully from file), but for now it's just an array of chars
const GLchar* fragmentShaderSrc =
"#version 330 core\n"
"in vec3 vertColor;"
"out vec4 fragColor;"
"void main() {"
"	fragColor = vec4(vertColor, 1.0);"
"}";


//-----------------------------------------------------------------------------
// Main Application Entry Point
//-----------------------------------------------------------------------------
int main() {
	if (!initOpenGL()) {
		cout << "Failed to init OpenGL" << endl;
		return EXIT_FAILURE;
	}

	// set the array of vertex data for a cube with index buffer data
	GLfloat vertices[] = {

		// position				// texture
		1.0f, 1.0f, 1.0f,		-1.0f, 1.0f
	}

	// setup buffers on the GPU
	GLuint vbo, vao, ibo; // ibo -> index / element buffer object

	// first we generate the buffers (GPU memory for the data)
	// then we bind this buffer (make it active to recive the data and store it)
	// then we give him the data (vertecies) by using glBufferData()
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // make it as working buffer (Active it)
	// Function that copies the defined vertex data into the buffer’s memory 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // We used (GL_STATIC_DRAW) cuz our data is fixed

	// after we done with setting the buffers (VBOs) 
	// we will do the VAO (that discripes the data we had in the VBOs).
	// firts we generate VAO ( one VAO can deal -discribe- mant VBOs ).
	// then bind it (make it active -ready- to take the discription of the data)
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao); // make as active one

	// Most Important function: ** glVertexAttribPointer(layout index, size, type, normalize, stride, offset) **
	// LAYOUT:
	// the layout must match the one you spicefed in the vertex shaders
	// SIZE:
	// the size of the data (x,y,z) or (R,G,B) ==> 3
	// TYPE:
	// data type is float
	// NORMALIZE:
	// false == the data is normalized (no need for normalization)
	// true == the data need to be normalized
	// STRIDE:
	// Each vertix from the array of vertices noe has a pos(x,y,z) and color (R,G,B)
	// and the type is float, so each component has 4 BYTES , so the pos is 3*4 = 12 , or 3 * sizeof(GL_float)
	// allso the same for color (R,G,B) 3 components * 4 BYTES = 12 BYTES
	// So in total the ONE vertex will have vertex and color fo it 
	// which result on (3 components for pos + 3 components for color) * sizeof(GL_float) ===> 6*sizeof(GL_float)
	// OFFSET:
	// each vertex has a pos and color.
	// we puted the pos data first, so the offset for it will start from zero
	// then color data comes, so it will be after 3 the components of the pos.
	// NOTE: if the data contains only pos or only color, then no need for the offset (we can make it NULL or pointer to void).

	// positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)0); // Position
	glEnableVertexAttribArray(0);

	// texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)(3 * sizeof(GLfloat))); // texture
	glEnableVertexAttribArray(1);

	// IMPORTANT NOTE: for STRIDE: 
	// if the vbo has only pos or only color,
	// so we can make it 0 because its tightly packed
	// or 3*sizeof(GL_float) . Both are correct!
	// 
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	//-------------------------------------------------^---this could be also as (sizeof(GLfloat) * 3)
	//-----------------------------------------------------cuz there is one data only in the array (tightly binded)

	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("res/shaders/transform.vert", "res/shaders/transform.frag");

	Texture texture1;
	texture1.loadTexture(texture1FileName, true);

	Texture texture2;
	texture2.loadTexture(texture2FileName, true);

	// ########### Rendering loop (loop until window is closed) Game Loop ########### //
	while (!glfwWindowShouldClose(gmainWindow)) {
		// Get + Handel user input events
		glfwPollEvents();

		//=====================Drawing area (Bind ==> Draw ==> Unbind) =====================//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate the shader program
		shaderProgram.use();

		// create uniform variables to change color dynamicly (prev project) 
		//GLfloat time = (GLfloat)glfwGetTime();
		//GLfloat greenColor = (sin(time) / 2) + 0.5;
		//shaderProgram.setUniform("vertColor", glm::vec4(0.0f, greenColor, 0.0f, 1.0f));

		//==== 09_Transformation new code start ====//
		texture1.bind(0);
		texture2.bind(1);
		// Texture Unit is an identifire in the gpu for each texture in the gpu memory (up to 32).
		// and the sampler will read the texture units from gpu.
		// if more than 1 texture we need this (like this case)
		// each sampler has only one texture.
		glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "texSampler1"), 0);
		glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "texSampler2"), 1);

		glm::mat4 transform = glm::mat4(1.0);

		// experiment with translation
		if (transDirection)
			offset += increment;
		else
			offset -= increment;

		if (abs(offset) >= maxOffset)
			transDirection = !transDirection;

		// experiment with rotation
		curAngle += 0.5f;

		// experiment with scaling
		if (sizeDirection)
			curSize += 0.01f;
		else
			curSize -= 0.01f;

		if (curSize >= maxSize || curSize <= minSize)
			sizeDirection = !sizeDirection;

		// apply any transformation
		transform = glm::translate(transform, glm::vec3(offset, 0.0f, 0.0f));
		transform = glm::rotate(transform, glm::radians(curAngle), glm::vec3(1.0f, 1.0f, 1.0f));
		transform = glm::scale(transform, glm::vec3(curSize, curSize, curSize));

		shaderProgram.setUniform("transform", transform);

		// Do some delay (to have sommth animation)
		glfwSwapInterval(1);

		// Notes:
		// * Scalar: italic, Vector: bold, Matrix: uppercase bold.
		// * Vectors has no position (only direction and magnitude).
		// * Zero vector the only one with magnitude = 0, but it has direction.
		// * Negating a vector results on same magnitude and opposite direction (-v).
		// * Multiplying a vector by a scalar k has the effect of scaling the length by a factor of | k |.
		// * To normalize a vector, we divide the vector by its magnitude.
		// * To compute the distance between two points |(Vf-Vi)|
		//		Ex. dist([5 0], [-1 8]) = sqrt((-1-5)^2+(8-0)^2).
		// * The dot product is related to the angle between two vectors.
		// * The cross product produces a third vector that is orthogonal (perpendicular) to both the input vectors.
		// * If a set of vectors is linearly independent, we cannot represent one in terms of the others.
		// * In an n-dimensional space, any set of n linearly independent vectors form a basis for the space.
		// * In OpenGL we start by representing vectors using the object basis but 
		//		later the system needs a representation in terms of the camera or eye basis.
		// * Affine space we can add a single point the origin.
		// * The 4th dimention (w) could be 0 ==> to represent a vector, or 1 ==> to represent a point,
		//		which makes the (homogeneous coordinate representation).
		// * The parallel lines should meet at infinity in projective space, 
		//		but cannot do in Euclidean space. the solution: Homogeneous Coordinates.
		// * When (w) is 1 ==> very close to you, 0 ==> far (infinity).
		// To convert a vector from one base to another just multiply by M matrix.
		//

		//==== 09_Transformation new code end ====//

		glBindVertexArray(vao);
		// This method will initiate the rest of the pipeline (will trigger the shaders)
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0); // ==> unbind vao
		//=====================Drawing area=====================//

		// Swap the screen buffers (DOUBLE BUFFER CONCEPT)
		glfwSwapBuffers(gmainWindow);
	}

	// Clean up
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	glfwTerminate();

	return EXIT_SUCCESS;
}

// Notes for Open_GL
/*
	- OpenGL is a state machine.
	- OpenGL is platform-independent API.
	- OpenGL is not object oriented.
	- Control GPU through programs called shaders.
	- GPU does all rendering.	
	- Each application must provide both a vertex and a fragment shader.
	- GLFW Handles window creation and control.
*/

// Rendering Pipeline
/*
	- The Rendering Pipeline is a series of stages that take place in order to render an image to the screen.
	- Four (4) stages are programmable via “Shaders”.
	- Shaders are pieces of code written in GLSL and they are C-like.
	- A primitive is a simple shape defined using one or more vertices.

	- The Rendering Pipeline Stages:
		1.Vertex Specification.
		2.Vertex Shader (programmable).
		3.Tessellation (programmable). ==> Allows you to divide up data in to smaller primitives.
		4.Geometry Shader (programmable).
		5.Vertex Post-Processing.
		6.Primitive Assembly.
		7.Rasterization. ==> Converts primitives (geometric) into “Fragments”. the Input: primitive, the Output: pixels.
		8.Fragment Shader (programmable). ==>  Computer color of pixel (Phone shading, Texture mapping, Bump mapping).
		9.Per-Sample Operations.

	- Vertex Shader: NOT OPTIONAL, Handles vertices individually (per-vertex operations).
	- to use a shader progeam: glUseProgram(shaderID)
	
	- Texture wrapping describes the behavior of the sampler when the texture coordinates fall outside the range of 0-1.
	- Fragments in the rasterizer have Location in the frame buffer, Color, Depth

	- Pipe-line:
	  [Vertices] ==> Vertex Processor -> Clipper and primitive assembler -> Rasterizer -> Fragment Processor ==> [Pixel]
*/

// Textures
/*
	1- Three Types of Mapping:
		- Texture Mapping: Uses images to fill inside of polygons.
		- Environment(reflection mapping): 
			• Uses a picture of the environment for texture maps.
			• Allows simulation of highly specular surfaces.
		- Bump mapping: Emulates altering normal vectors during the rendering process.

	2- Mapping techniques are implemented at the end of the rendering pipeline.
		- There are two pipelines: geometric pipeline and pixel pipeline.
		- For texture mapping the pixel pipeline merges with fragment processing after rasterization.
			- Texture mapping is done as part of fragment processing.
			- Each fragment is then tested for visibility with the Z-buffer.

	3- This is called parametric texture mapping .
	4- A single point in the texture is called a texel.
	5- Texels are defined between 0 and 1.


	Mipmap: Create multiple versions of image at different resolutions and switch between them based on distance.

	Fillters:
	Nearest: Use the texel with most overlap (creates a pixelated effect).
	Linear: Use a weighted average of surrounding texels (blends pixel boundaries).

	Wraping: Describes the behavior of the sampler when the texture coordinates fall outside the range of 0-1.
		

	Wrapping and Filtering are defined using the glTexParameteri() function.

*/

// Transformations
/*
	
 1- Translation Matrix:
	1 0 0 d_x
	0 1 0 d_y
	0 0 1 d_z
	0 0 0 1

 2- Rotation Matrix:

	about Z:
	cos() -sin() 0 0
	sin() cos()  0 0
	0		0	 1 0
	0		0	 0 1

	about X:
	1	0		0  0
	0 cos() -sin() 0
	0 sin() cos()  0
	0	0	 0	   1

	about Y:
	cos() 0 -sin()  0
	0   1		0	0
	sin() 0  cos()  0
	0	0	   0    1

 3- Scaling:
	S_x 0 0 0
	0 S_y 0 0
	0 0 S_z 0
	0 0 0	1


 4- Reflect:
	
	on x : y = -y
	on y : x = -x

*/