/// <summary>
/// GLSL Shader Program
/// </summary>

#define GLEW_STATIC 
#include "GL/glew.h"
#include "string"

using namespace std;

#ifndef SHADER_H
#define SHADER_H

class ShaderProgram
{
public:
	ShaderProgram();
	~ShaderProgram();

	enum ShaderType {
		VERTEX,
		FRAGMENT,
		PROGRAM
	};

	bool loadShaders(const char* vsFileName, const char* fsFileName); // Read shaders (fragment and vertix) from files
	void use();
	GLuint getProgram() const;

private:
	GLuint mHandle; // The Program itself
	string fileToString(const string& fileName);
	void checkCompileErrors(GLuint shader, ShaderType type);
};


#endif SHADER_H