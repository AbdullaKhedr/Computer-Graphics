//------------------------------------------
// Homework #2 Main_Q1.cpp file 
//------------------------------------------

#include <iostream> 
#define GLEW_STATIC 
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

using namespace std;

// Global Variables
const char* APP_TITLE = "CG HW2 Q1 Abdulla Khedr";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gmainWindow;
bool gWireframe = false;

// Translation Related Variables
bool transDirection = true;
float offset = 0.0f;
float maxOffset = 0.7f;
float increment = 0.01f;

// Functions Prototypes
bool initOpenGL();
void print_OpenGL_Info();
void glfwOnKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height);

//------------------------------------------
// Main Application Entry Point
//------------------------------------------
int main() {
	if (!initOpenGL()) {
		cout << "Failed to init OpenGL" << endl;
		return EXIT_FAILURE;
	}

	GLfloat vertices_square1[] =
	{
		// Position				// Color
	   -0.5f,  0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	// top left
		0.5f,  0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	// top right 
		0.5f, -0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	// buttom right
	   -0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	// buttom left		
	};
	
	GLfloat vertices_square2[] =
	{
		// Position				// Color
	   -0.5f,  0.5f, 0.0f,		1.0f, 0.0f, 0.0f,	// top left
		0.5f,  0.5f, 0.0f,		0.0f, 1.0f, 0.0f,	// top right 
		0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f,	// buttom right
	   -0.5f, -0.5f, 0.0f,		1.0f, 1.0f, 1.0f,	// buttom left		
	};

	GLuint indices[] =
	{
		0, 1, 2,	// first square
		0, 2, 3,	// second square
	};

	// Setup buffers on the GPU
	GLuint vao[2], vbo[2], ibo;

	/// <summary>
	/// Shape 1
	/// </summary>
	glGenBuffers(1, &vbo[0]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // Activate it
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_square1), vertices_square1, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao[0]);
	glBindVertexArray(vao[0]); // make as active one

	// Define positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Define color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	/// <summary>
	/// Shape 2
	/// </summary>
	glGenBuffers(1, &vbo[1]);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]); // Activate it
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_square2), vertices_square2, GL_STATIC_DRAW);

	glGenVertexArrays(1, &vao[1]);
	glBindVertexArray(vao[1]); // make as active one

	// Define positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// Define color
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("res/shaders/basic.vert", "res/shaders/basic.frag");

	// ########### Rendering loop (loop until window is closed) Game Loop ########### //
	while (!glfwWindowShouldClose(gmainWindow)) {
		// Get + Handel user input events
		glfwPollEvents();

		//=====================Drawing area (Bind ==> Draw ==> Unbind) =====================//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Activate the program
		shaderProgram.use();

		// Translation Logic
		if (transDirection)
			offset += increment;
		else
			offset -= increment;
		if (abs(offset) >= maxOffset)
			transDirection = !transDirection;

		// Apply any translation on X
		glUniform2f(glGetUniformLocation(shaderProgram.getProgram(), "transform"), offset, 0);
		glBindVertexArray(vao[0]); // ==> Bind vao
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // ==> Draw
		glBindVertexArray(0); // ==> Unbind vao

		// Apply any translation on Y
		glUniform2f(glGetUniformLocation(shaderProgram.getProgram(), "transform"), 0, offset);
		glBindVertexArray(vao[1]); // ==> Bind vao
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // ==> Draw
		glBindVertexArray(0); // ==> Unbind vao

		// Add some delay (to have smooth animation)
		glfwSwapInterval(1);

		//===================================Drawing area===================================//

		// Swap the screen buffers (DOUBLE BUFFER CONCEPT)
		glfwSwapBuffers(gmainWindow);
	}

	// Clean up
	glDeleteVertexArrays(1, &vao[0]);
	glDeleteVertexArrays(1, &vao[1]);
	glDeleteBuffers(1, &vbo[0]);
	glDeleteBuffers(1, &vbo[1]);
	glfwTerminate();

	return EXIT_SUCCESS;
}

// OpenGL Initiolization
bool initOpenGL() {
	// init glfw
	if (!glfwInit())
	{
		cout << "GLFW initialization failed!" << endl;
		return EXIT_FAILURE;
	}

	// Steup glfw propereties and set the opengl version (3.3)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No backwards compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Creat the main window
	gmainWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	if (gmainWindow == NULL)
	{
		cout << "Failed to create GLFW window!" << endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Set context for GLEW to use
	glfwMakeContextCurrent(gmainWindow);

	// Initialize GLEW
	//glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW!" << endl;
		return EXIT_FAILURE;
	}

	// Set callbacks functions
	glfwSetKeyCallback(gmainWindow, glfwOnKey);
	glfwSetFramebufferSizeCallback(gmainWindow, glfw_onFrameBufferSize);

	// Clear the colorbuffer
	glClearColor(0.2f, 0.4f, 0.6f, 1.0f);

	// Print OpenGL info
	print_OpenGL_Info();

	// enable alpha support
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// 09_Transformation
	// enable depth test
	glEnable(GL_DEPTH_TEST);

	return true;
}

// ##### Callback Functions ##### //

// Handel keyboard
void glfwOnKey(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS) {
		gWireframe = !gWireframe; //toggele it

		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

// Handel resize window
void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

// Print OpenGL version information
void print_OpenGL_Info() {
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	GLint major, minor;

	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("GL Renderer	:%s\n", renderer);
	printf("GL Vendor	:%s\n", vendor);
	printf("GL Version	:%s\n", version);
	printf("GL Vendor	:%d.%d\n", major, minor);
	printf("GLSL Version	:%s\n", glslVersion);
}