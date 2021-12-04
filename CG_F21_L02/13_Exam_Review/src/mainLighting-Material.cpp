//-----------------------------------------------------------------------------
// mainOBJModelLoading.cpp
//
// - Demo loading OBJ files
//-----------------------------------------------------------------------------
#include <iostream>
#include <sstream>
#define GLEW_STATIC
#include "GL/glew.h"	// Important - this header must come before glfw3 header
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp" //added for transform

#include "ShaderProgram.h"
#include "Texture.h"
#include "Camera.h"
#include "Mesh.h"

// Global Variables
const char* APP_TITLE = " Computer Graphics - Lighting Material Demo";
int gWindowWidth = 1200;
int gWindowHeight = 900;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;
const std::string texture1Filename = "res/images/box.jpg";
const std::string texture2Filename = "res/images/mario.png";
const std::string floorImage = "res/images/floor1.jpg";

// experiment with translation
bool transDirection = true;
float offset = 0.0f;
float maxOffest = 0.7f;
float increment = 0.01f;

// experiment with rotation
float curAngle = 0.0f;

// experiment with scaling??
bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;

//FPSCamera fpsCamera(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(1.0, 1.0, 1.0));
FPSCamera fpsCamera(glm::vec3(0.0f, 3.0f, 10.0f));

const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 5.0; // units per second
const float MOUSE_SENSITIVITY = 0.1f;

// Function prototypes
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
	//-------------------------- Viewing: moving to the 3D where we gonna have our camera--------------------------//
	/*
	* <<< The Story of CG from drawing the model until see it on screen >>>
	* 
	* Those are all a different state at which our vertices will be transformed in before finally ending up as fragments.
	* 
	* 1- LOCAL SPACE: Local coordinates are the coordinates of your object relative to its local origin.
	* 2- WORLD SPACE (model matrix): transform the local coordinates to world-space coordinates which are coordinates in respect of a larger world using the (model) matrix.
	* 3- VIEW/EYE/CAMERA SPACE (view matrix): transform the world coordinates to view-space coordinates in such a way that each coordinate is as seen from the camera or viewer’s point of view.
	* 4- HOMOGENEOUS/CLIP SPACE (projection matrix): project the coordinates to clip coordinates (what is inside the frustrm) (Ortho / Prespective) -far = smaller, closer = bigger- (using w = -z).
	* 5- NORMALIZED DEVICE SPACE (NDC) (normalize): transform the clip coordinates to screen coordinates in a process we call viewport.
	* 6- VIEWPORT SPACE (SCREEN): transform that transforms the coordinates from - 1.0 and 1.0 to the coordinate range defined by glViewport.
	* 
	* * Note: going back in any step: useing the invers matrix.
	* 
	* * Why This system has so many transformations?
	* * Cuz some operations/calculations are easier in certain coordinate systems!
	* 
	* *[Model]:
	*	where we do the transforamtions (rotating, scaling, translating).
	* 
	* *[View (lookAt)]:
	*	glm::lookAt(mPosition (eye) , mTargetPos (center) , mUp (up) )
	* 
	*	Camera is at origin (0,0,0) and looking at (0,0,-1) 
	*	the camera is pointing to -z direction, hence all calculations on z will be (-z)
	*	so the forward/front of the camera is back (-z)
	* 
	*	each point in the world we have to multiply it by view matrix to be in camera space
	* 
	*	The “Model-View” matrix (MV = V* M) transform to camera space in ONE STEP.
	*	Pc = MV * Pm
	* 
	* *[Projection (Perspective)]:
	*	Defines viewing volume and clipping planes. Projects points into NDC [-1,1] in x,y plane.
	* 
	*   1- Orthographics is parallel, so the W is always = 1.
	*	to achieve ortho we need to do scael and translate as follows
	*	M = Scale * Translate
	*	Scale = 		   Translate =		   M = glm::Ortho =
	*	2/(r-l) 0 0 0;	   1 0 0 -(l+r)/2;	   2/(r-l) 0 0 -(l+r)/(r-l);
	*	0 2/(t-b) 0 0;	   0 1 0 -(t+b)/2;	   0 2/(t-b) 0 -(t+b)/(t-b);
	*	0 0 2/(f-n) 0;	   0 0 1 -(f+n)/2;	   0 0 -2/(f-n) -(f+n)/(f-n);
	*	0 0    0    1;	   0 0 0     1   ;	   0 0 0             1      ;
	* 
	*   2- Perspective will have devide the W (less W for far, and more W for near).
	*	P = 
	*	1 0	 0	 0;
	*	0 1  0	 0;
	*	0 0	 1	 0;
	*	0 0 -1/d 0;
	*	Note: (Only) last row affected (no longer 0 0 0 1), W coord will no longer = 1.
	* 
	*	Verification:
	*	
	*	1 0	 0	 0 	  x		 x			-(d*x)/z
	*	0 1  0	 0	* y	 = 	 y		=	-(d*y)/z
	*	0 0	 1	 0 	  z		 z				-d
	*	0 0 -1/d 0 	  1		-z/d			1
	* 
	*	** Derivation of Perspective Matrix **
	*	glm::perspective(fov, aspect, near, far);
	* 
	*	P = 			Where: 
	*	A 0 0  0			d = 1/tan(0) , 0 = fov/2
	*	0 d 0  0			A = d/aspectRatio
	*	0 0 B  C			B = (Znear + Zfar) / (Znear - Zfar)
	*	0 0 -1 0			C = 2 * [(Znear * Zfar) / (Znear - Zfar)]
	* 
	* * Finally:
	*	gl_Position = projection * view * model * vec4(pos.x, pos.y, pos.z, 1.0);
	* 
	* 
	* * Mapping of Z is nonlinear :
	*	Many mappings proposed: all have nonlinearities 
	*	Advantage: handles range of depths (10cm – 100m) 
	*	Disadvantage: depth resolution not uniform 
	*	More close to near plane, less further away 
	*	Common mistake: set near = 0, far = infinity. 
	*	Don’t do this. 
	*	Can’t set near = 0; lose depth resolution. 
	* 
	*	V_clip=M_projection . M_view . M_model . V_local
	* 
	*	The resulting vertex should then be assigned to gl_Position in the vertex shader and 
	*	OpenGL will then automatically perform perspective division and clipping.
	* 
	*/


	//-------------------------- Lighting/Shading --------------------------//
	/*
	* * Each point to have a different color or shade based on:
	*	- Light sources 
	*	- Material properties
	*	- Location of the viewer
	*	- Surface orientation
	* 
	*	Rays Reflections on A then B then A back ......
	*	<<Rendering equation>> describes this recursive process!
	*	So, The infinite reflection and absorption of light can be described by the rendering equation!...
	* 
	*	[outgoing]-[incoming] = [emitted]-[absorbed]
	*	[outgoing] = [emitted] + [reflected] (+[transmitted])
	* 
	* * Local vs Global Illumination (إضاءة)
	* 
	* - Direct or Local Illumination: (eg. phong illum. model)2
	*	Simple, one ray will reflect one time (Single interaction) between the light and viewr.
	*		Single interaction of light & objects.
	*		Real-time supported by OpenGL.
	*		Real-time supported by OpenGL.
	* 
	* - Indirect or Global Illumination:
	*		Multiple interaction of light & objects: interobjects reflections, shadows, refractions.
	*		Not real-time (almost there!).
	*		Examples: raytracing, radiosity, photon mapping.
	* 
	* * Correct shading requires a global calculation,
	*	Incompatible with a pipeline model which shades each polygon independently (local rendering).
	* * many techniques for approximating global effects exist.
	* 
	* 
	* * Light Sources:
	*		- we consider light source is one ray! it has a direction D (normalaized), and go to point P with wavelrnght (lampda).
	*		- we consider light is linear (linearity), so we can add , and scale it.
	*		- when we go far the intensity will be less and vice versa,
	*			this is simulated bt equation 1/r^2, however we cant simulate very close light (it will be 1/infinty)
	*			so we come up with the equation 1/(ar^2 + br + c).
	* 
	* * Light types:
	*		1- Point Light: (It has position only not a direction!)
	*			- The amount of light energy received by a surface depends on incoming angle.
	*			- Theta between light direction (l) , and surface normal (n):
	*				Theta = max (0) , than cos(theat = 0) = 1.
	*				Theta = min (90) , than cos(theat = 90) = 0.
	* 
	*		2- Directional Lights: (It has direction only not a position!)
	*			if you put a point light very far (at infinty) the rays will come in parallel like the sun,
	*			so alsmost all the erth will have the same intensity.
	* 
	*			no matter the position is, all the sefus will get the same light (for all objects have the same theta)
	* 
	*		3- Spot Light: (non-uniform directional emission)
	*			maximum intensity at the center of the cone.
	* 
	* 
	* * Reflectance Model (Based on phong model): (Mathematical description of how incoming light is reflected from a surface)
	*		I_Diffuse + I_Specular + I_Emission + I_Ambient
	*		we add them here cuz we considering the linearity.
	* 
	*	1- Diffuse Reflection (ruff serfus سطح خشن ): so ( الضوء الساقط ينعكسس ويتشتت في جميع الاتجاهات)
	*		- one assumes that surface reflects equally in all directions.
	*		- How Much Light is Reflected? Depends on angle θ of incident light. The greater θ is, the less light is reflected.
	*		- The amount of reflected light is dependent on the position of the 
	*		  light source and the object, but independent of the observer's position.
	*		- Lambert’s cosine law (dot product):
	*			The intensity of light diffuse I_D reflected from a surface point P(x,y,z) is 
	*			proportional to the cosine of the angle between the vectorv to the 
	*			light source and the normal vector perpendicular to the surface.
	*			I_D = K_D (N.L)I , K_D is the  Diffuse factor, 0 = 0% , 1 = 100%
	* 
	*	 * Problem with non-uniform scales:
	*		Wrongly skewing normals. Can be countered by creating a “normal matrix” from model matrix.
	*		Transform normals with: mat3(transpose(inverse(model))).
	* 
	* 
	*	2- Specular Reflection (smooth serfuses اسطح ناعمة): (angle of Light == angle of Reflect) ( مثل المرآة N زاوية السقوط = زاوية الانعكاس حول العمود المقام)
	*		- I_s = K_s * L * (V.R)^n 
	* 
	*		the (^n) here is to controll how specular reflection exponent (perfect reflector n=∝).
	* 
	*		View vector is just the difference between the Fragment Position and the Viewer (Camera) position.
	*		Reflection vector can be obtained with a built-in GLSL function: reflect(incident, normal)
	*			Incident: Vector to reflect.
	*			Normal: Normal vector to reflect around.
	*		
	*		Just as with diffuse, use dot product between normalized forms of view and refection vector, to get specular factor.
	* 
	*	3- Ambient Reflection: (Represents Reflection of All <<Indirect Illumination>> )
	*		Ambient light is the illumination of an object caused by reflected light from other surfaces.
	*		very complicated to be calculated exactly.
	*		A simple model assumes ambient light is uniform in the environment.
	*		Ambient light illuminate an object equally from all directions with the same intensity. 
	*
	*		Avoids the complexity of indirect (“global”) illumination.
	*		
	*	Ambient & Diffuse are two types of reflectance  define the color of the material.
	* 
	*	Gouraud Shading:
	*		Find average normal at each vertex.
	*		Apply Phong reflection model at each vertex.
	*		Linearly interpolate vertex shades (colors) across each polygon.
	* 
	*	Phong Shading:
	*		Linear interpolation of the surface normals.
	*		Illumination model applied at every point.
	*		Interpolate normals rather than colors.
	*		Significantly more expensive.
	*		Mostly done off-line (not supported in OpenGL).
	* 
	* 
	* 
	* 
	*	* In flat shading, each elementary surface drawn in the same color where the color computed for central point.
	* 
	*	* Phong over Gouraud: when higher realism is required, and when specular reflections are significant.
	*	* Gouraud shading is more realistic than phong.
	*	 
	* 
	* 
	* 
	* 
	* 
	* 
	*/

	if (!initOpenGL())
	{
		// An error occured
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	//setting shaders
	ShaderProgram shaderProgram, shaderProgramOneTex;
	shaderProgram.loadShaders("res/shaders/lighting-blinn-phong-mat.vert", "res/shaders/lighting-blinn-phong-mat.frag");

	// Shader fro light (lamp)
	ShaderProgram lightShader;
	lightShader.loadShaders("res/shaders/lamp.vert", "res/shaders/lamp.frag");

	// Load meshes and textures
	const int numModels = 2;
	Mesh mesh[numModels];
	Texture texture[numModels];

	mesh[0].loadOBJ("res/models/cylinder.obj");
	mesh[1].loadOBJ("res/models/floor.obj");

	// Load the Light
	Mesh lightMesh;
	lightMesh.loadOBJ("res/models/light.obj");

	texture[0].loadTexture("res/models/cylinder.jpg", true);
	texture[1].loadTexture("res/models/Floor_ConcreteHerringbone.jpg", true);

	// Model positions
	glm::vec3 modelPos[] = {
		glm::vec3(-2.5f,1.4f, 0.0f),	// cylinder
		glm::vec3(0.0f, 0.0f, 0.0f),		// floor
	};

	// Model scale
	glm::vec3 modelScale[] = {
		glm::vec3(0.7f, 0.7f, 0.7f),	// cylinder
		glm::vec3(10.0f, 1.0f, 10.0f),	// floor
	};

	double lastTime = glfwGetTime();
	float angle = 0.0f;

	//print card info
	Print_OpenGL_Version_Information();

	// Rendering loop 
	while (!glfwWindowShouldClose(gWindow))
	{
		showFPS(gWindow);

		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

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

		glm::vec3 viewPos;
		viewPos.x = fpsCamera.getPosition().x;
		viewPos.y = fpsCamera.getPosition().y;
		viewPos.z = fpsCamera.getPosition().z;

		// Set light properties (pos,color)
		glm::vec3 lightPos(0.0f, 1.0f, 10.0f);
		glm::vec3 lightColor(1.0f, 1.0f, 1.0f);

		// Move the light
		angle += (float) deltaTime * 50.0f;
		lightPos.x = 10.0f * sinf(glm::radians(angle));

		shaderProgram.use();

		// Pass the matrices to the shader
		shaderProgram.setUniform("view", view);
		shaderProgram.setUniform("projection", projection);
		shaderProgram.setUniform("viewPos", viewPos);
		
		// New: for the light materia
		shaderProgram.setUniform("light.position", lightPos);
		shaderProgram.setUniform("light.ambient", glm::vec3(0.2f, 0.2f, 0.2f));
		shaderProgram.setUniform("light.diffuse", lightColor);
		shaderProgram.setUniform("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		// Render the scene
		for (int i = 0; i < numModels; i++)
		{
			model = glm::translate(glm::mat4(1.0), modelPos[i]) * glm::scale(glm::mat4(1.0), modelScale[i]);
			if (i == 2) // rotaion only for dolfen
				model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			
			shaderProgram.setUniform("model", model);

			// Set the material for models
			shaderProgram.setUniform("material.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
			shaderProgram.setUniformSampler("material.diffuseMap", 0);
			shaderProgram.setUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
			shaderProgram.setUniform("material.shininess", 45.0f);

			texture[i].bind(0);		// set the texture before drawing.  Our simple OBJ mesh loader does not do materials yet.
			mesh[i].draw();			// Render the OBJ mesh
			texture[i].unbind(0);
		}

		// Draw the light
		model = glm::translate(glm::mat4(1.0), lightPos);
		lightShader.use();
		lightShader.setUniform("model", model);
		lightShader.setUniform("view", view);
		lightShader.setUniform("projection", projection);
		lightShader.setUniform("lightColor", lightColor);
		lightMesh.draw();

		glBindVertexArray(0);

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

	glClearColor(0.3f, 0.4f, 0.6f, 1.0f);

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
