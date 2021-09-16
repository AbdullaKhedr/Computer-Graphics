//-----------------------------------------------------------------------------
// - Render 2 triangles beside each other
// - One VBO with position and color for each triangle
// - Wireframe key toggling (Press W Key)
//-----------------------------------------------------------------------------

#include <iostream> // Deals with input/output
#define GLEW_STATIC // Important to be static
#include "GL/glew.h" // include glew before glfw
#include "GLFW/glfw3.h"
#include "ShaderProgram.h"

using namespace std;

// Global Variables
const char* APP_TITLE = "Computer Graphics - HW1 - Abdulla Khedr";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gmainWindow;
bool gWireframe = false;

// Functions Prototypes
bool initOpenGL();
void print_OpenGL_Info();
void glfwOnKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFrameBufferSize(GLFWwindow* window, int width, int height);

//-----------------------------------------------------------------------------
// Main Application Entry Point
//-----------------------------------------------------------------------------
int main() 
{
	if (!initOpenGL()) 
	{
		cout << "Failed to init OpenGL" << endl;
		return EXIT_FAILURE;
	}

	// 1. setup array of vertices for the two triangles
	GLfloat vertices1[] = {
		// Positions			// Colors
		0.45f, 0.5f, 0.0f,		0.0f, 1.0f,1.0f,	//top
		0.9f, -0.5f, 0.0f,		0.0f, 1.0f,1.0f,	//right
		0.0f, -0.5f, 0.0f,		0.0f, 1.0f,1.0f,	// left
	};

	GLfloat vertices2[] = {
		// Positions			// Colors
		-0.45f, 0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	//top
		0.0f, -0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	//right
		-0.9f, -0.5f, 0.0f,		1.0f, 1.0f, 0.0f,	// left
	};

	// 2. setup buffers on the GPU
	GLuint vbo1, vbo2, vao1, vao2;

	glGenBuffers(1, &vbo1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo1); // make it as working buffer 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW); // We used (GL_STATIC_DRAW) cuz our data is fixed

	glGenVertexArrays(1, &vao1);
	glBindVertexArray(vao1); // make as active one
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); // Position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3)); // Color

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);


	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2); // make it as working buffer 
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices2), vertices2, GL_STATIC_DRAW); // We used (GL_STATIC_DRAW) cuz our data is fixed
	
	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2); // make as active one
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0); // Position
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(sizeof(GLfloat) * 3)); // Color

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// 3. Get vertix & fragment shaders
	ShaderProgram shaderProgram;
	shaderProgram.loadShaders("shaders/basic.vert", "shaders/basic.frag");

	// ########### Rendering loop (loop until window is closed) Game Loop ########### //
	while (!glfwWindowShouldClose(gmainWindow)) 
	{
		// Get + Handel user input events
		glfwPollEvents();

		//=====================Drawing area=====================//
		glClear(GL_COLOR_BUFFER_BIT);

		// 4. Use the program
		shaderProgram.use();

		// 5. Draw (Bind ==> Draw ==> Unbind)
		glBindVertexArray(vao1); // ==> Bind
		glDrawArrays(GL_TRIANGLES, 0, 3); // ==> Draw
		glBindVertexArray(0); // ==> Unbind
		
		glBindVertexArray(vao2); // ==> Bind
		glDrawArrays(GL_TRIANGLES, 0, 3); // ==> Draw
		glBindVertexArray(0); // ==> Unbind
		//=====================Drawing area=====================//

		// Swap the screen buffers (DOUBLE BUFFER CONCEPT)
		glfwSwapBuffers(gmainWindow);
	}

	// Clean up
	glDeleteVertexArrays(1, &vao1);
	glDeleteVertexArrays(1, &vao2);
	glDeleteBuffers(1, &vbo1);
	glDeleteBuffers(1, &vbo2);
	glfwTerminate();

	return EXIT_SUCCESS;
}

// OpenGL Initiolization
bool initOpenGL() 
{
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
	glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

	// Print OpenGL info
	print_OpenGL_Info();

	return true;
}

// ##### Callback Functions ##### //

// Handel keyboard
void glfwOnKey(GLFWwindow* window, int key, int scancode, int action, int mode) 
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) 
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key == GLFW_KEY_W && action == GLFW_PRESS) 
	{
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
	glViewport(0, 0, width, height);
}


// Print OpenGL version information
void print_OpenGL_Info() 
{
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