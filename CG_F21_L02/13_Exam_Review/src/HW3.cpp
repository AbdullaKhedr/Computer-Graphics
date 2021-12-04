//-----------------------------------------------------------------------------
// Homework 3 
// Draw 5*4 cubes
// Apply Transformation (translate, rotate, scale) by transformation matrix
//-----------------------------------------------------------------------------

#include <iostream>
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "ShaderProgram.h"
#include"Texture.h"
// 09_Transformation
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;

// Global Variables //
const char* APP_TITLE = "CG HW3 Abdulla Khedr";
int gWindowWidth = 800;
int gWindowHeight = 600;
GLFWwindow* gmainWindow;
bool gWireframe = false;

/*
//==================//
// Fragment Shader
//==================//
#version 330 core

in vec3 vertColor;
out vec4 fragColor;

void main() 
{
	fragColor = vec4(vertColor, 1.0);
}
*/

/*
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
*/


// Functions Prototypes
bool initOpenGL();
void print_OpenGL_Info();
void glfwOnKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height);

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

		// position					// tex coords

		// front face
		-1.0f,  1.0f,  1.0f,		1.0f, 0.0f, 0.0f,


	};

	// setup buffers on the GPU
	GLuint vbo, vao;

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // make it as working buffer (Active it)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // We used (GL_STATIC_DRAW) cuz our data is fixed

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao); // make as active one

	// Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0); // Position
	glEnableVertexAttribArray(0);

	// Colors
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(3 * sizeof(GLfloat))); // texture
	glEnableVertexAttribArray(1);


	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("res/shaders/transform.vert", "res/shaders/transform.frag");

	float curAngle = 0.0f;

	// Number of cubes we draw
	const int rows = 5, cols = 4;

	// ########### Rendering loop (loop until window is closed) Game Loop ########### //
	while (!glfwWindowShouldClose(gmainWindow)) {
		// Get + Handel user input events
		glfwPollEvents();

		//=====================Drawing area (Bind ==> Draw ==> Unbind) =====================//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.use();

		// Do some delay (to have smooth animation)
		glfwSwapInterval(1);

		float translateYOffset = 0.6f;
		float colorProp = 100.0f; // To simulate color propagation
		for (int i = 0; i < rows; i++)
		{
			float scaleFactor = 0.035f;
			float translateXOffset = -0.55f;

			for (int i = 0; i < cols; i++)
			{
				// Transform matrix
				glm::mat4 transform = glm::mat4(1.0);

				// Rotation
				curAngle += 0.1f;

				// apply any transformation
				transform = glm::translate(transform, glm::vec3(translateXOffset, translateYOffset, 0.0f));
				transform = glm::rotate(transform, glm::radians(curAngle + colorProp), glm::vec3(1.0f, 1.0f, 0.0f));
				transform = glm::scale(transform, glm::vec3(scaleFactor, scaleFactor, scaleFactor));

				shaderProgram.setUniform("transform", transform);

				// Drawing
				glBindVertexArray(vao); // Bind
				glDrawArrays(GL_TRIANGLES, 0, 36); // Draw
				glBindVertexArray(0); // Unbind

				// Scal 30% each time
				scaleFactor += scaleFactor * 0.3f;
				// translate to the right
				translateXOffset += (7.0f / 20.0f);
				// To simulate color propagation
				colorProp -= 10;
			}
			translateYOffset -= (7.0f / 25.0f);
		}

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
