//-----------------------------------------------------------------------------
// Camera
//-----------------------------------------------------------------------------

#include <iostream> // Deals with input/output
#define GLEW_STATIC // Important to be static
#include "GL/glew.h" // include glew before glfw
#include "GLFW/glfw3.h"
#include "ShaderProgram.h"
#include"Texture.h"
// 09_Transformation
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "Camera.h"

using namespace std;

// Global Variables
const char* APP_TITLE = "Computer Graphics - Projection";
int gWindowWidth = 800;
int gWindowHeight = 600;
GLFWwindow* gmainWindow;
bool gWireframe = false;

const string texture1FileName = "res/images/image1.jpg";
const string texture2FileName = "res/images/mario.png";
const string texture3FileName = "res/images/TexturesCom_WoodPanelOrnate02_header.jpg";

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

// setting fbs camera
FBSCamera fpsCamera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(1.0f, 1.0f, 1.0f));
const double ZOOM_SENSITIVITY = -3.0f;
const float MOVE_SPEED = 5.0f;
const float MOUSE_SENSITIVITY = 0.1f;

// Functions Prototypes
bool initOpenGL();
void print_OpenGL_Info();
void glfwOnKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
void update(double elapsedTime);

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

		// position         // tex coords

		// front face
	   -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,

		1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

		1.0f,  1.0f,  1.0f, 1.0f, 1.0f,

	   -1.0f,  1.0f,  1.0f, 0.0f, 1.0f,

	   -1.0f, -1.0f,  1.0f, 0.0f, 0.0f,

		1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

		// back face
		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,

		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

		 1.0f,  1.0f, -1.0f, 1.0f, 1.0f,

		-1.0f,  1.0f, -1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, -1.0f, 0.0f, 0.0f,

		 1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

		 // left face
		  -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,

		  -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

		  -1.0f,  1.0f,  1.0f, 1.0f, 1.0f,

		  -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,

		  -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,

		  -1.0f, -1.0f,  1.0f, 1.0f, 0.0f,

		  // right face
		   1.0f,  1.0f,  1.0f, 0.0f, 1.0f,

		   1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

		   1.0f,  1.0f, -1.0f, 1.0f, 1.0f,

		   1.0f,  1.0f,  1.0f, 0.0f, 1.0f,

		   1.0f, -1.0f,  1.0f, 0.0f, 0.0f,

		   1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

		   // top face
		  -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,

		   1.0f,  1.0f,  1.0f, 1.0f, 0.0f,

		   1.0f,  1.0f, -1.0f, 1.0f, 1.0f,

		  -1.0f,  1.0f, -1.0f, 0.0f, 1.0f,

		  -1.0f,  1.0f,  1.0f, 0.0f, 0.0f,

		   1.0f,  1.0f,  1.0f, 1.0f, 0.0f,

		   // bottom face
		  -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,

		   1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

		   1.0f, -1.0f,  1.0f, 1.0f, 1.0f,

		  -1.0f, -1.0f,  1.0f, 0.0f, 1.0f,

		  -1.0f, -1.0f, -1.0f, 0.0f, 0.0f,

		   1.0f, -1.0f, -1.0f, 1.0f, 0.0f,

	};

	glm::vec3 cubePos(0.0f, 0.0f, 0.0f);
	glm::vec3 floorPos(0.0f, -1.0f, 0.0f);

	// setup buffers on the GPU
	GLuint vbo, vao; // ibo -> index / element buffer object

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // make it as working buffer (Active it)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // We used (GL_STATIC_DRAW) cuz our data is fixed

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao); // make as active one

	// positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)0); // Position
	glEnableVertexAttribArray(0);

	// texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (GLvoid*)(3 * sizeof(GLfloat))); // texture
	glEnableVertexAttribArray(1);

	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("res/shaders/texture.vert", "res/shaders/texture.frag");

	Texture texture1;
	texture1.loadTexture(texture1FileName, true);

	Texture texture2;
	texture2.loadTexture(texture2FileName, true);
	
	Texture floorTexture;
	floorTexture.loadTexture(texture3FileName, true);

	double lastTime = glfwGetTime();
	float cubeAngle = 0.0f;

	// ########### Rendering loop (loop until window is closed) Game Loop ########### //
	while (!glfwWindowShouldClose(gmainWindow))
	{
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;
		// Get + Handel user input events
		glfwPollEvents();
		update(deltaTime);

		//=====================Drawing area (Bind ==> Draw ==> Unbind) =====================//
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		texture1.bind(0);
		texture2.bind(1);
		// if more than 1 texture we need this (like this case)
		glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "texSampler1"), 0);
		glUniform1i(glGetUniformLocation(shaderProgram.getProgram(), "texSampler2"), 1);

		shaderProgram.use();

		// Do some delay (to have sommth animation)
		glfwSwapInterval(1);

		glm::mat4 transform = glm::mat4(1.0);

		// model, view, and projection
		glm::mat4 model = glm::mat4(1.0);
		glm::mat4 view(1.0), projection(1.0);


		view = fpsCamera.getViewMatrix();

		// create the projection matrix
		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowHeight / (float)gWindowWidth, 0.1f, 100.0f);

		shaderProgram.setUniform("model", model);
		shaderProgram.setUniform("view", view);
		shaderProgram.setUniform("projection", projection);

		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0); // ==> unbind vao
		//=====================Drawing area=====================//

		floorTexture.bind(0);
		model = glm::translate(model, floorPos) * glm::scale(model, glm::vec3(10.0f, 0.01f, 10.0f));
		shaderProgram.setUniform("model", model);

		lastTime = currentTime;

		// Swap the screen buffers (DOUBLE BUFFER CONCEPT)
		glfwSwapBuffers(gmainWindow);
	}

	// Clean up

	glDeleteVertexArrays(1, &vao);
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
	glfwSetScrollCallback(gmainWindow, glfw_onMouseScroll);

	// set the mouse to center and hide the cursor
	glfwSetInputMode(gmainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(gmainWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);


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
void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height)
{
	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, width, height);
}

// Handel mouse scrolling
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;
	fov = glm::clamp(fov, 1.0, 120.0);
	fpsCamera.setFOV((float)fov);
}

// Update the camera every frame
void update(double elapsedTime)
{
	double mouseX, mouseY;
	glfwGetCursorPos(gmainWindow, &mouseX, &mouseY);

	// Rotate the camer
	fpsCamera.rotate((float)(gWindowWidth / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(gWindowHeight / 2.0 - mouseY) * MOUSE_SENSITIVITY);

	glfwSetCursorPos(gmainWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	// Handel camera movement based on WASD keys
	// forward & backward
	if (glfwGetKey(gmainWindow, GLFW_KEY_W) == GLFW_PRESS)
		fpsCamera, move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
	if (glfwGetKey(gmainWindow, GLFW_KEY_S) == GLFW_PRESS)
		fpsCamera, move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook());

	// left & right
	if (glfwGetKey(gmainWindow, GLFW_KEY_A) == GLFW_PRESS)
		fpsCamera, move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getRight());
	if (glfwGetKey(gmainWindow, GLFW_KEY_D) == GLFW_PRESS)
		fpsCamera, move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getRight());

	// up & down
	if (glfwGetKey(gmainWindow, GLFW_KEY_Z) == GLFW_PRESS)
		fpsCamera, move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getUp());
	if (glfwGetKey(gmainWindow, GLFW_KEY_X) == GLFW_PRESS)
		fpsCamera, move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getUp());

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