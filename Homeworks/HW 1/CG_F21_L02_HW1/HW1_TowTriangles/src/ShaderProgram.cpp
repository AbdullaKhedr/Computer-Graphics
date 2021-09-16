#include "ShaderProgram.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

ShaderProgram::ShaderProgram():mHandle(0)
{

}
ShaderProgram::~ShaderProgram()
{
	// Delete the program once finish
	glDeleteProgram(mHandle);
}

bool ShaderProgram::loadShaders(const char* vsFileName, const char* fsFileName)
{
	// Read shaders (fragment and vertix) from files
	string vsString = fileToString(vsFileName);
	string fsString = fileToString(fsFileName);
	// convert string to char pointer to our shaders
	const GLchar* vsSourcePtr = vsString.c_str();
	const GLchar* fsSourcePtr = fsString.c_str();

	// create vertix shader
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vs, 1, &vsSourcePtr, NULL);
	glShaderSource(fs, 1, &fsSourcePtr, NULL);

	// Compile then check for compile errors in vertex and fragment shaders code
	glCompileShader(vs);
	checkCompileErrors(vs, VERTEX);
	glCompileShader(fs);
	checkCompileErrors(fs, FRAGMENT);

	// Create shader program and link our vertex and fragment shaders, then check for error
	mHandle = glCreateProgram();
	if (mHandle == 0)
		cerr << "Error: unable to create shader program!";
	glAttachShader(mHandle, vs);
	glAttachShader(mHandle, fs);
	glLinkProgram(mHandle);
	checkCompileErrors(mHandle, PROGRAM);

	// Clean up shaders (we do not need them any more)
	glDeleteShader(vs);
	glDeleteShader(fs);

	return true;
}
void ShaderProgram::use()
{
	if (mHandle > 0)
		glUseProgram(mHandle);
}
GLuint ShaderProgram::getProgram() const
{
	return mHandle;
}

string ShaderProgram::fileToString(const string& fileName)
{
	stringstream ss;
	ifstream file;

	try {
		file.open(fileName, ios::in);
		if (!file.fail())
		{
			ss << file.rdbuf();
		}
		file.close();
	}
	catch (exception e) {
		cerr << "Error in reading shader filename!\n";
		// cerr << "Error in reading shader filename!\n" << e.what();
	}

	return ss.str();
}

void ShaderProgram::checkCompileErrors(GLuint shader, ShaderType type)
{
	GLint status;

	if (type == PROGRAM) // Errors in the program
	{
		glGetShaderiv(mHandle, GL_LINK_STATUS, &status);
		if (status == GL_FALSE)
		{
			// we will get the length of the error first, instead of using fixed lenght
			GLint length = 0;
			glGetProgramiv(mHandle, GL_INFO_LOG_LENGTH, &length);
			string errorLog(length, ' ');

			glGetShaderInfoLog(mHandle, sizeof(length), &length, &errorLog[0]);
			cerr << "Error: Shader program failed to link." << errorLog << endl;
		}
	}
	else // Errors in vertix or fagment shaders
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
		{
			// we will get the length of the error first, instead of using fixed lenght
			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
			string errorLog(length, ' ');

			glGetShaderInfoLog(shader, sizeof(length), &length, &errorLog[0]);
			cerr << "Error: Vertex shader failed to compile." << errorLog << endl;
		}
	}
}

GLint ShaderProgram::getUniformLocation(const GLchar* name)
{
	std::map<string, GLint>::iterator it = mUniformLocation.find(name);

	// if no location for that name
	if (it == mUniformLocation.end())
	{
		mUniformLocation[name] = glGetUniformLocation(mHandle, name);
	}

	// if we found the name ==> return the location
	return mUniformLocation[name];
}

void ShaderProgram::setUniform(const GLchar* name, const glm::vec2& v)
{
	GLint loc = getUniformLocation(name);
	glUniform2f(loc, v.x, v.y);
}
void ShaderProgram::setUniform(const GLchar* name, const glm::vec3& v)
{
	GLint loc = getUniformLocation(name);
	glUniform3f(loc, v.x, v.y, v.z);
}
void ShaderProgram::setUniform(const GLchar* name, const glm::vec4& v)
{
	GLint loc = getUniformLocation(name);
	glUniform4f(loc, v.x, v.y, v.z, v.w);
}