//-----------------------------------------------------------------------------
// - Render 2 triangles to create quad
// - add wireframe key toggling
// - add index buffer
// + New: clean up shaders and MORE!
//-----------------------------------------------------------------------------

#include <iostream> // Deals with input/output
#define GLEW_STATIC // Important to be static
#include "GL/glew.h" // include glew before glfw
#include "GLFW/glfw3.h"
#include "ShaderProgram.h"

using namespace std;

// Global Variables
const char* APP_TITLE = "Computer Graphics - Quad (2 Triangles)";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gMainWindow;
bool gWireframe = false;

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

	// 1. setup array of vertices for Quad (2 Triangles), but with no repetion in the vertices
	GLfloat vertices[] = {
		// Positions
		-0.5, 0.5f, 0.0f,	// top left
		0.5f, 0.5f, 0.0f,	// top right 
		0.5f, -0.5f, 0.0f,	// buttom right
		-0.5f, -0.5f, 0.0f,	// buttom left
	};

	// 2. creat the indices buffer, to reuse each point
	GLuint indices[] = {
		0,1,2,	// first triangle
		0,2,3,	// second triangle
	};

	// setup buffers on the GPU
	GLuint vbo, vao, ibo; // ibo -> index / element buffer object

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // make it as working buffer (Active it)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW); // We used (GL_STATIC_DRAW) cuz our data is fixed

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao); // make as active one

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); // Position
	glEnableVertexAttribArray(0);

	// 3. setup index buffer
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("shaders/basic.vert", "shaders/basic.frag");

	// ########### Rendering loop (loop until window is closed) Game Loop ########### //
	while (!glfwWindowShouldClose(gMainWindow)) {
		// Get + Handel user input events
		glfwPollEvents();

		//=====================Drawing area (Bind ==> Draw ==> Unbind) =====================//
		glClear(GL_COLOR_BUFFER_BIT);

		shaderProgram.use();

		// create uniform variables
		GLfloat time = (GLfloat)glfwGetTime();
		GLfloat greenColor = (sin(time) / 2) + 0.5;
		//GLint vertexColor = glGetUniformLocation(shaderProgram.getProgram(), "vertColor"); // XXX old way
		//glUniform4f(vertexColor, 0.0f, greenColor, 0.0f, 1.0f); // XXX old way
		shaderProgram.setUniform("vertColor", glm::vec4(0.0f, greenColor, 0.0f, 1.0f));


		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0); // ==> unbind vao
		//=====================Drawing area=====================//

		// Swap the screen buffers (DOUBLE BUFFER CONCEPT)
		glfwSwapBuffers(gMainWindow);
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
	gMainWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);
	if (gMainWindow == NULL)
	{
		cout << "Failed to create GLFW window!" << endl;
		glfwTerminate();
		return EXIT_FAILURE;
	}

	// Set context for GLEW to use
	glfwMakeContextCurrent(gMainWindow);

	// Initialize GLEW
	//glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK)
	{
		cout << "Failed to initialize GLEW!" << endl;
		return EXIT_FAILURE;
	}

	// Set callbacks functions
	glfwSetKeyCallback(gMainWindow, glfwOnKey);
	glfwSetFramebufferSizeCallback(gMainWindow, glfw_onFrameBufferSize);

	// Clear the colorbuffer
	glClearColor(0.2f, 0.4f, 0.6f, 1.0f);

	// Print OpenGL info
	print_OpenGL_Info();


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