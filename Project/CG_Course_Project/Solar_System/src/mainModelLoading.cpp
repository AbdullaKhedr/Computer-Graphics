//-----------------------------------------------------------------------------
// - Computer Graphics Course Project
// - 3D Model Simulates The Solar System 
// - Using Assimp Library For Loading 3D Models from different file formats 
//-----------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#define GLEW_STATIC
#include "GL/glew.h" // Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" // added for transform
#include "ShaderProgram.h"
#include "Camera.h"
#include "Mesh.h"
#include "Model.h"

// Global Variables
const char* APP_TITLE = "CG HW4 Abdulla Khedr";
int gWindowWidth = 1200;
int gWindowHeight = 900;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;

// Camera Settings
FPSCamera fpsCamera(glm::vec3(-20.0f, 40.0f, 90.0f), glm::vec3(20.0f, 0.0f, -10.0f));

// Control Settings
const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 10.0; // units per second
const float MOUSE_SENSITIVITY = 0.1f;

// Functions prototypes
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
void update(double elapsedTime);
void showFPS(GLFWwindow* window);
bool initOpenGL();
void Print_OpenGL_Version_Information();

//-----------------------------------------------------------------------------
// Main Application Entry Point
//-----------------------------------------------------------------------------
int main()
{
	if (!initOpenGL())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	// Print Graphics Card Info
	Print_OpenGL_Version_Information();

	// Setting shaders
	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("res/shaders/lighting-phong-mat-point.vert", "res/shaders/lighting-phong-mat-point.frag");

	// Setting light shaders
	ShaderProgram lightShader;
	lightShader.loadShaders("res/shaders/lamp.vert", "res/shaders/lamp.frag");

	// Load Models 
	const int numModels = 10;
	Model ourModels[numModels];

	ourModels[0].loadModel("res/models/Sun/Sun.obj");
	ourModels[1].loadModel("res/models/Mercury/Mercury.obj");
	ourModels[2].loadModel("res/models/Venus/Venus.obj");
	ourModels[3].loadModel("res/models/Earth/Earth.obj");
	ourModels[4].loadModel("res/models/Earth/Moon/Moon.obj");
	ourModels[5].loadModel("res/models/Mars/Mars.obj");
	ourModels[6].loadModel("res/models/Ceres/Ceres.obj");
	ourModels[7].loadModel("res/models/Saturn/Saturn.obj");
	ourModels[8].loadModel("res/models/Uranus/Uranus.obj");
	ourModels[9].loadModel("res/models/Neptune/Neptune.obj");

	// Simulating the stars in the outer space
	Model star;
	star.loadModel("res/models/Stars/Star.obj");
	const int starsNum = 400;
	glm::vec3 p[starsNum];
	glm::vec3 s[starsNum];
	int min = -100, max = 100;
	int redArea = 30; // Range from -50 to 50 on x, y, z should not have any stars 
	for (int i = 0; i < starsNum; i++)
	{
		float pX = rand() % (max - min + 1) + min;
		float pY = rand() % (max - min + 1) + min;
		float pZ = rand() % (max - min + 1) + min;

		if (pX >= 0)
			pX += redArea;
		if (pY >= 0)
			pY += redArea;
		if (pZ >= 0)
			pZ += redArea;
		
		if (pX < 0)
			pX -= redArea;
		if (pY < 0)
			pY -= redArea;
		if (pZ < 0)
			pZ -= redArea;

		cout << "(" << pX << ", " << pY << ", " << pZ << ")" << endl; // position of a star

		float sc = rand() % (1 - 0 + 1) + 0;

		p[i] = glm::vec3(pX, pY, pZ);
		s[i] = glm::vec3(sc + 0.1, sc + 0.1, sc + 0.1);
	}

	// Model Positions
	glm::vec3 modelPos[] = {
		glm::vec3(0.0f, 0.0f, 0.0f), // The Sun (Light)
		glm::vec3(10.0f, 0.0f, 0.0f), // Mercury
		glm::vec3(18.0f, 0.0f, 0.0f), // Venus
		glm::vec3(24.0f, 0.0f, 0.0f), // Earth - Moon
		glm::vec3(29.0f, 0.0f, 0.0f), // Earth
		glm::vec3(32.0f, 0.0f, 0.0f), // Mars
		glm::vec3(44.0f, 0.0f, 0.0f), // Ceres
		glm::vec3(60.0f, 0.0f, 0.0f), // Saturn
		glm::vec3(76.0f, 0.0f, 0.0f), // Uranus
		glm::vec3(86.0f, 0.0f, 0.0f)  // Neptune
	};

	// Model Scale
	glm::vec3 modelScale[] = {
		glm::vec3(2.0f, 2.0f, 2.0f), // The Sun (Light)
		glm::vec3(0.6f, 0.6f, 0.6f), // Mercury
		glm::vec3(0.8f, 0.8f, 0.8f), // Venus
		glm::vec3(1.2f, 1.2f, 1.2f), // Earth
		glm::vec3(1.4f, 1.4f, 1.4f), // Earth - Moon
		glm::vec3(0.6f, 0.6f, 0.6f), // Mars
		glm::vec3(2.0f, 2.0f, 2.0f), // Ceres
		glm::vec3(1.8f, 1.8f, 1.8f), // Saturn
		glm::vec3(1.6f, 1.6f, 1.6f), // Uranus
		glm::vec3(0.6f, 0.6f, 0.6f), // Neptune
	};

	double lastTime = glfwGetTime();
	float curAngle = 0.0f;

	// Rendering loop 
	while (!glfwWindowShouldClose(gWindow))
	{
		showFPS(gWindow);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		curAngle += 0.2f;

		// Poll for and process events
		glfwPollEvents();
		update(deltaTime);

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model(1.0), view(1.0), projection(1.0);

		// Create the View matrix
		view = fpsCamera.getViewMatrix();

		// Create the projection matrix
		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 200.0f);

		// View position to be passed to fragment shader
		glm::vec3 viewPos;
		viewPos.x = fpsCamera.getPosition().x;
		viewPos.y = fpsCamera.getPosition().y;
		viewPos.z = fpsCamera.getPosition().z;

		// Set light properties (pos, color)
		glm::vec3 lightPos(0.0f, 0.0f, 0.0f);
		glm::vec3 lightColor(1.0f, 1.0f, 0.8f);

		shaderProgram.use();

		// Pass the matrices to the shader
		shaderProgram.setUniform("view", view);
		shaderProgram.setUniform("projection", projection);
		shaderProgram.setUniform("viewPos", viewPos);
		// Set Point light
		shaderProgram.setUniform("pointLight.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
		shaderProgram.setUniform("pointLight.diffuse", lightColor);
		shaderProgram.setUniform("pointLight.specular", glm::vec3(0.0f, 0.0f, 0.0f));
		shaderProgram.setUniform("pointLight.position", lightPos);
		shaderProgram.setUniform("pointLight.constant", 0.8f); // More = Less light
		shaderProgram.setUniform("pointLight.linear", 0.0f); // ??
		shaderProgram.setUniform("pointLight.exponent", 0.0000001f); // Less = Far reaching for the light

		// Render the scene (The planets after the Sun)
		for (int i = 1; i < numModels; i++)
		{
			glm::mat4 mat = glm::mat4(1.0);
			// Set the Positions, and Scale for each Model
			model = glm::translate(mat, modelPos[i]) * glm::scale(mat, modelScale[i]);

			// Set the material properties for models
			shaderProgram.setUniform("material.ambient", glm::vec3(0.1f, 0.1f, 0.1f));
			shaderProgram.setUniformSampler("material.diffuseMap", 0.6f);
			shaderProgram.setUniform("material.specular", glm::vec3(0.0f, 0.0f, 0.0f));
			shaderProgram.setUniform("material.shininess", 45.0f);

			// Set the Rotations for each planet
			switch (i)
			{
				//case 0: // Sun
				//	model = model * glm::rotate(mat, glm::radians(curAngle * 2), glm::vec3(0.0f, 1.0f, 0.0f));
				//	break;
			case 1: // Mercury
				model = glm::rotate(mat, glm::radians(-curAngle * 1.5f), glm::vec3(0.0f, 1.0f, 0.0f)) * model
					* glm::rotate(mat, glm::radians(curAngle * 6), glm::vec3(1.0f, 1.0f, 0.0f));
				break;
			case 2: // Venus
				model = glm::rotate(mat, glm::radians(-curAngle * 1.2f), glm::vec3(0.0f, 1.0f, 0.0f)) * model
					* glm::rotate(mat, glm::radians(curAngle * 1.5f), glm::vec3(1.0f, 1.0f, 0.0f));
				break;
			case 3: // Earth
				model = glm::rotate(mat, glm::radians(-curAngle * 2.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * model
					* glm::rotate(mat, glm::radians(curAngle * 6), glm::vec3(1.0f, 1.0f, 0.0f));
				break;
			case 4: // Earth - Moon
				model = glm::rotate(mat, glm::radians(-curAngle * 2.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * model
					* glm::rotate(mat, glm::radians(-curAngle * 6), glm::vec3(1.0f, 1.0f, 0.0f));
				break;
			case 5: // Mars
				model = glm::rotate(mat, glm::radians(-curAngle * 2.5f), glm::vec3(0.0f, 1.0f, 0.0f)) * model
					* glm::rotate(mat, glm::radians(-curAngle * 4), glm::vec3(1.0f, 1.0f, 0.0f));
				break;
			case 6: // Ceres
				model = glm::rotate(mat, glm::radians(-curAngle * 3.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * model
					* glm::rotate(mat, glm::radians(curAngle * 2), glm::vec3(1.0f, 1.0f, 0.0f));
				break;
			case 7: // Saturn
				model = glm::rotate(mat, glm::radians(-curAngle * 4.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * model
					* glm::rotate(mat, glm::radians(curAngle * 2), glm::vec3(1.0f, 1.0f, 0.0f));
				break;
			case 8: // Uranus
				model = glm::rotate(mat, glm::radians(-curAngle * 3.7f), glm::vec3(0.0f, 1.0f, 0.0f)) * model
					* glm::rotate(mat, glm::radians(curAngle * 2), glm::vec3(1.0f, 1.0f, 0.0f));
				break;
			case 9: // Neptune
				model = glm::rotate(mat, glm::radians(-curAngle * 2.0f), glm::vec3(0.0f, 1.0f, 0.0f)) * model
					* glm::rotate(mat, glm::radians(curAngle * 2), glm::vec3(1.0f, 1.0f, 0.0f));
				break;
			}
			shaderProgram.setUniform("model", model);

			ourModels[i].Draw(shaderProgram); // Draw
		}

		// Render the Light (Sun)
		model = glm::translate(glm::mat4(1.0), modelPos[0]) * glm::scale(glm::mat4(1.0), modelScale[0])
			* glm::rotate(glm::mat4(1.0), glm::radians(curAngle * 2), glm::vec3(1.0f, 1.0f, 0.0f));
		lightShader.use();
		lightShader.setUniform("model", model);
		lightShader.setUniform("view", view);
		lightShader.setUniform("projection", projection);
		lightShader.setUniform("lightColor", lightColor);
		ourModels[0].Draw(lightShader);

		for (int i = 0; i < starsNum; i++)
		{
			model = glm::translate(glm::mat4(1.0), p[i]) * glm::scale(glm::mat4(1.0), s[i]);
			lightShader.setUniform("model", model);
			star.Draw(lightShader);
		}

		// Swap front and back buffers
		glfwSwapBuffers(gWindow);

		lastTime = currentTime;
	}

	glfwTerminate();

	return 0;
}

//-----------------------------------------------------------------------------
// Initialize GLFW and OpenGL
//-----------------------------------------------------------------------------
bool initOpenGL()
{
	// Intialize GLFW 
	// GLFW is configured.  Must be called before calling any GLFW functions
	if (!glfwInit())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);	// forward compatible with newer versions of OpenGL as they become available but not backward compatible (it will not run on devices that do not support OpenGL 3.3

	// Create an OpenGL 3.3 core, forward compatible context window
	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	if (gWindow == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	// Make the window's context the current one
	glfwMakeContextCurrent(gWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	// Set the required callback functions
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);
	glfwSetScrollCallback(gWindow, glfw_onMouseScroll);

	// Hides and grabs cursor, unlimited movement
	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Define the viewport dimensions
	glViewport(0, 0, gWindowWidth, gWindowHeight);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);

	return true;
}

//-----------------------------------------------------------------------------
// Is called whenever a key is pressed/released via GLFW
//-----------------------------------------------------------------------------
void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}

//-----------------------------------------------------------------------------
// Is called when the window is resized
//-----------------------------------------------------------------------------
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height)
{
	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, width, height);
}

//-----------------------------------------------------------------------------
// Called by GLFW when the mouse wheel is rotated
//-----------------------------------------------------------------------------
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY)
{
	double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;

	fov = glm::clamp(fov, 1.0, 120.0);

	fpsCamera.setFOV((float)fov);
}

//-----------------------------------------------------------------------------
// Update stuff every frame
//-----------------------------------------------------------------------------
void update(double elapsedTime)
{
	// Camera orientation
	double mouseX, mouseY;

	// Get the current mouse cursor position delta
	glfwGetCursorPos(gWindow, &mouseX, &mouseY);

	// Rotate the camera the difference in mouse distance from the center screen.  Multiply this delta by a speed scaler
	fpsCamera.rotate((float)(gWindowWidth / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(gWindowHeight / 2.0 - mouseY) * MOUSE_SENSITIVITY);

	// Clamp mouse cursor to center of screen
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	// Camera FPS movement

	// Forward/backward
	if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook());
	else if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook());

	// Strafe left/right
	if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getRight());
	else if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getRight());

	// Up/down
	if (glfwGetKey(gWindow, GLFW_KEY_Z) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * fpsCamera.getUp());
	else if (glfwGetKey(gWindow, GLFW_KEY_X) == GLFW_PRESS)
		fpsCamera.move(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getUp());
}

//-----------------------------------------------------------------------------
// Code computes the average frames per second, and also the average time it takes
// to render one frame.  These stats are appended to the window caption bar.
//-----------------------------------------------------------------------------
void showFPS(GLFWwindow* window)
{
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime(); // returns number of seconds since GLFW started, as double float

	elapsedSeconds = currentSeconds - previousSeconds;

	// Limit text updates to 4 times per second
	if (elapsedSeconds > 0.25)
	{
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		// The C++ way of setting the window title
		std::ostringstream outs;
		outs.precision(3);	// decimal places
		outs << std::fixed
			<< APP_TITLE << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << msPerFrame << " (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		// Reset for next average.
		frameCount = 0;
	}

	frameCount++;
}

// Print OpenGL version information
void Print_OpenGL_Version_Information()
{
	const GLubyte* renderer = glGetString(GL_RENDERER);
	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion =
		glGetString(GL_SHADING_LANGUAGE_VERSION);

	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);

	printf("GL Vendor            : %s\n", vendor);
	printf("GL Renderer          : %s\n", renderer);
	printf("GL Version (string)  : %s\n", version);
	printf("GL Version (integer) : %d.%d\n", major, minor);
	printf("GLSL Version         : %s\n", glslVersion);
}
