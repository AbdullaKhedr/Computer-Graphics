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
// 09_Transformation
// experiment with translation
bool transDirection = true;
float offset = 0.0f;
float maxOffset = 0.7f;
float increment = 0.01f;
// experiment with rotation
float curAngle = 0.0f;
// experiment with scaling
bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

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

		1.0f, -1.0f,  1.0f,			1.0f, 0.0f, 0.0f,

		1.0f,  1.0f,  1.0f,			1.0f, 0.0f, 0.0f,

		-1.0f,  1.0f,  1.0f,		1.0f, 0.0f, 0.0f,

		-1.0f, -1.0f,  1.0f,		1.0f, 0.0f, 0.0f,

		1.0f, -1.0f,  1.0f,			1.0f, 0.0f, 0.0f,

		// back face
		-1.0f,  1.0f, -1.0f,		0.0f, 1.0f, 0.0f,

		 1.0f, -1.0f, -1.0f,		0.0f, 1.0f, 0.0f,

		 1.0f,  1.0f, -1.0f,		0.0f, 1.0f, 0.0f,

		-1.0f,  1.0f, -1.0f,		0.0f, 1.0f, 0.0f,

		-1.0f, -1.0f, -1.0f,		0.0f, 1.0f, 0.0f,

		 1.0f, -1.0f, -1.0f,		0.0f, 1.0f, 0.0f,

		 // left face
		-1.0f,  1.0f, -1.0f,		1.0f, 1.0f, 0.0f,

		-1.0f, -1.0f,  1.0f,		1.0f, 1.0f, 0.0f,

		-1.0f,  1.0f,  1.0f,		1.0f, 1.0f, 0.0f,

		-1.0f,  1.0f, -1.0f,		1.0f, 1.0f, 0.0f,

		-1.0f, -1.0f, -1.0f,		1.0f, 1.0f, 0.0f,

		-1.0f, -1.0f,  1.0f,		1.0f, 1.0f, 0.0f,

		// right face
		 1.0f,  1.0f,  1.0f,		1.0f, 0.0f, 1.0f,

		 1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 1.0f,

		 1.0f,  1.0f, -1.0f,		1.0f, 0.0f, 1.0f,

		 1.0f,  1.0f,  1.0f,		1.0f, 0.0f, 1.0f,

		 1.0f, -1.0f,  1.0f,		1.0f, 0.0f, 1.0f,

		 1.0f, -1.0f, -1.0f,		1.0f, 0.0f, 1.0f,

		 // top face
		-1.0f,  1.0f, -1.0f,		0.0f, 1.0f, 1.0f,

		 1.0f,  1.0f,  1.0f,		0.0f, 1.0f, 1.0f,

		 1.0f,  1.0f, -1.0f,		0.0f, 1.0f, 1.0f,

		-1.0f,  1.0f, -1.0f,		0.0f, 1.0f, 1.0f,

		-1.0f,  1.0f,  1.0f,		0.0f, 1.0f, 1.0f,

		 1.0f,  1.0f,  1.0f,		0.0f, 1.0f, 1.0f,

		 // bottom face
		-1.0f, -1.0f,  1.0f,		0.0f, 0.0f, 1.0f,

		 1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 1.0f,

		 1.0f, -1.0f,  1.0f,		0.0f, 0.0f, 1.0f,

		-1.0f, -1.0f,  1.0f,		0.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 1.0f,

		 1.0f, -1.0f, -1.0f,		0.0f, 0.0f, 1.0f,

	};

	glm::vec3 cubesPositions[] = {
		// Line 1
		glm::vec3(-3.5f, 5.0f, -20.0f),
		glm::vec3(-1.167f, 5.0f, -20.0f),
		glm::vec3(1.167f, 5.0f, -20.0f),
		glm::vec3(3.5f, 5.0f, -20.0f),

		// Line 2
		glm::vec3(-3.5f, 2.5f, -20.0f),
		glm::vec3(-1.167f, 2.5f, -20.0f),
		glm::vec3(1.167f, 2.5f, -20.0f),
		glm::vec3(3.5f, 2.5f, -20.0f),

		// Line 3
		glm::vec3(-3.5f,   0.0f, -20.0f),
		glm::vec3(-1.167f, 0.0f, -20.0f),
		glm::vec3(1.167f, 0.0f, -20.0f),
		glm::vec3(3.5f,   0.0f, -20.0f),

		// Line 4
		glm::vec3(-3.5f,   -2.5f, -20.0f),
		glm::vec3(-1.167f, -2.5f, -20.0f),
		glm::vec3(1.167f, -2.5f, -20.0f),
		glm::vec3(3.5f,   -2.5f, -20.0f),

		// Line 5
		glm::vec3(-3.5f,   -5.0f, -20.0f),
		glm::vec3(-1.167f, -5.0f, -20.0f),
		glm::vec3(1.167f, -5.0f, -20.0f),
		glm::vec3(3.5f,   -5.0f, -20.0f),
	};

	// setup buffers on the GPU
	GLuint vbo, vao[20];

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // make it as working buffer (Active it)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // We used (GL_STATIC_DRAW) cuz our data is fixed

	for (int i = 0; i < 20; i++)
	{
		glGenVertexArrays(1, &vao[i]);
		glBindVertexArray(vao[i]); // make as active one

		// Positions
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)0); // Position
		glEnableVertexAttribArray(0);

		// Colors
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*)(3 * sizeof(GLfloat))); // texture
		glEnableVertexAttribArray(1);
	}

	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("res/shaders/transform.vert", "res/shaders/transform.frag");

	double lastTime = glfwGetTime();
	float cubeAngle = 0.0f;

	// ########### Rendering loop (loop until window is closed) Game Loop ########### //
	while (!glfwWindowShouldClose(gmainWindow)) {

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		// Get + Handel user input events
		glfwPollEvents();

		//=====================Drawing area (Bind ==> Draw ==> Unbind) =====================//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderProgram.use();

		// Do some delay (to have smooth animation)
		glfwSwapInterval(1);

		for (int i = 0; i < 20; i++)
		{
			// Model, View, and Projection
			glm::mat4 model(1.0);
			glm::mat4 view(1.0);
			glm::mat4 projection(1.0);

			// Create the Model matrix
			cubeAngle += (float)(deltaTime * 50.0f);
			if (cubeAngle >= 360.0f) cubeAngle = 0.0f;

			model = glm::translate(model, cubesPositions[i]) * glm::rotate(model, glm::radians(cubeAngle), glm::vec3(1.0f, 1.0f, 0.0f));

			// Create the View matrix
			glm::vec3 camPos(0.0f, 0.0f, 0.0f);
			glm::vec3 targetPos(0.0f, 0.0f, -1.0f);
			glm::vec3 up(0.0f, 1.0f, 0.0f);

			view = glm::lookAt(camPos, camPos + targetPos, up);

			// Create the Projection matrix
			projection = glm::perspective(glm::radians(45.0f), (float)gWindowHeight / (float)gWindowWidth, 0.1f, 100.0f);

			shaderProgram.setUniform("model", model);
			shaderProgram.setUniform("view", view);
			shaderProgram.setUniform("projection", projection);

			// Drawing
			glBindVertexArray(vao[i]); // Bind
			glDrawArrays(GL_TRIANGLES, 0, 36); // Draw
			glBindVertexArray(0); // Unbind
		}

		lastTime = currentTime;

		//=====================Drawing area=====================//

		// Swap the screen buffers (DOUBLE BUFFER CONCEPT)
		glfwSwapBuffers(gmainWindow);
	}

	// Clean up
	glDeleteVertexArrays(20, vao);
	glDeleteBuffers(1, &vbo);
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