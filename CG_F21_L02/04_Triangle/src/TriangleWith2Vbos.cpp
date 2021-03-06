//-----------------------------------------------------------------------------
// - Tow VBOs one for position and one for color
// - Modify shaders to read colors from the data 
//-----------------------------------------------------------------------------

#include <iostream> // Deals with input/output
#define GLEW_STATIC // Important to be static
#include "GL/glew.h" // include glew before glfw
#include "GLFW/glfw3.h"

using namespace std;

// Global Variables
const char* APP_TITLE = "Computer Graphics - Hello Triangle using 2 Vbos";
const int gWindowWidth = 800;
const int gWindowHeight = 600;
GLFWwindow* gMainWindow;

// Shaders
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

	// 1. setup array of vertices for triangle
	GLfloat vert_position[] = {
		// Positions	
		0.0f, 0.5f, 0.0f,	// top
		0.5f, -0.5f, 0.0f,	// right
		-0.5f, -0.5f, 0.0f	// left
	};

	GLfloat vert_color[] = {
		// Colors
		1.0f, 0.0f, 0.0f,	// red
		0.0f, 1.0f, 0.0f,	// green
		0.0f, 0.0f, 1.0f	// blue
	};


	// 2. setup buffers on the GPU
	GLuint vbo, vbo2, vao;

	// first we generate the buffers (GPU memory for the data)
	// then we bind this buffer (make it active to recive the data and store it)
	// then we give him the data (vertecies) by using glBufferData()
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // make it as working buffer (Active it)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert_position), vert_position, GL_STATIC_DRAW); // We used (GL_STATIC_DRAW) cuz our data is fixed

	glGenBuffers(1, &vbo2);
	glBindBuffer(GL_ARRAY_BUFFER, vbo2); // make it as working buffer (Active it)
	glBufferData(GL_ARRAY_BUFFER, sizeof(vert_color), vert_color, GL_STATIC_DRAW); // We used (GL_STATIC_DRAW) cuz our data is fixed

	// after we done eith setting the buffers (VBOs) 
	// we will do the VAO (that discripes the data we had in the VBOs).
	// firts we generate VAO ( one VAO can deal -discribe- mant VBOs ).
	// then bind it (make it active -ready- to take the discription of the data)
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao); // make as active one
	
	// after that we will give bind (active) each VBO and give it to the VAO
	// and finally we will enable it to the vertex shaders.
	glBindBuffer(GL_ARRAY_BUFFER, vbo); // (Active it) to be used in the next line (for position)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0); // Position
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo2);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 3, (GLvoid*)0); // Color
	glEnableVertexAttribArray(1);

	// IMPORTANT NOTE: for STRIDE: 
	// if the vbo has only pos or only color,
	// so we can make it 0 because its tightly packed
	// or 3*sizeof(GL_float) . Both are correct!
	// 
	// glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	//-------------------------------------------------^---this could be also as (sizeof(GLfloat) * 3)
	//-----------------------------------------------------cuz there is one data only in the array (tightly binded)

	// 3.0 create vertix shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vs, 1, &vertexShaderSrc, NULL);
	glCompileShader(vs);

	// 3.2 check for compile errors in vertex shaders code
	GLint vsResult;
	GLchar vsInfoLog[512];
	glGetShaderiv(vs, GL_COMPILE_STATUS, &vsResult);
	if (!vsResult) {
		glGetShaderInfoLog(vs, sizeof(vsInfoLog), NULL, vsInfoLog);
		cerr << "Error: Vertex shader failed to compile." << vsInfoLog << endl;
	}

	// 4.0 Creat fragment shader
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fs, 1, &fragmentShaderSrc, NULL);
	glCompileShader(fs);

	// 4.2 check for compile errors in fragment shaders code
	GLint fsResult;
	GLchar fsInfoLog[512];
	glGetShaderiv(fs, GL_COMPILE_STATUS, &fsResult);
	if (!fsResult) {
		glGetShaderInfoLog(fs, sizeof(fsInfoLog), NULL, fsInfoLog);
		cerr << "Error: Fragment shader failed to compile." << fsInfoLog << endl;
	}

	// 5. Create shader program and link our vertex and fragment shaders
	GLint shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vs);
	glAttachShader(shaderProgram, fs);
	glLinkProgram(shaderProgram);

	// 5.1 Check for error in shader program
	GLint progResult;
	GLchar progInfoLog[512];
	glGetShaderiv(shaderProgram, GL_LINK_STATUS, &progResult);
	if (!progResult) {
		glGetShaderInfoLog(shaderProgram, sizeof(progInfoLog), NULL, progInfoLog);
		cerr << "Error: Shader program failed to link." << progInfoLog << endl;
	}

	// 6. Clean up shaders (we do not need them any more)
	glDeleteShader(vs);
	glDeleteShader(fs);

	// ########### Rendering loop (loop until window is closed) Game Loop ########### //
	while (!glfwWindowShouldClose(gMainWindow)) {
		// Get + Handel user input events
		glfwPollEvents();

		//=====================Drawing area=====================//
		glClear(GL_COLOR_BUFFER_BIT);
		// 7. Draw (Bind ==> Draw ==> Unbind)
		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glBindVertexArray(0);
		//=====================Drawing area=====================//

		// Swap the screen buffers (DOUBLE BUFFER CONCEPT)
		glfwSwapBuffers(gMainWindow);
	}

	// Clean up
	glDeleteProgram(shaderProgram);
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