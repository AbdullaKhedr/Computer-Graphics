/// <summary>
/// GLSL Shader Program
/// </summary>

#define GLEW_STATIC 
#include "GL/glew.h"
#include "string"
#include "glm/glm.hpp"
#include <map>

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

	void setUniform(const GLchar* name, const glm::vec2& v);
	void setUniform(const GLchar* name, const glm::vec3& v);
	void setUniform(const GLchar* name, const glm::vec4& v);

private:
	GLuint mHandle; // The Program itself
	string fileToString(const string& fileName);
	void checkCompileErrors(GLuint shader, ShaderType type);

	// Got the name and returns the location
	GLint getUniformLocation(const GLchar* name);
	std::map<string, GLint> mUniformLocation;
};


#endif SHADER_H