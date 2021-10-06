#include "Texture.h"

// Image loader
#define STB_IMAGE_IMPLEMENTATION // cuz it is header file type library
#include "stb_image/stb_image.h";

Texture::Texture()
	:mTexture(0)
{

}

Texture::~Texture()
{
	glDeleteTextures(1, &mTexture);
}

bool Texture::loadTexture(const string& fileName, bool generateMipMaps)
{
	GLint width, height, nrComponent;

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);
	// set texture parameters warp 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture parameters filter
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Load image and create texture
	unsigned char* texelData = stbi_load(fileName.c_str(), &width, &height, &nrComponent, 0);
	if (!texelData)
	{
		printf("Fail to load image file %s \n", fileName.c_str());
	}

	GLenum format = 0;
	if (nrComponent == 0)
		format = GL_RED;
	if (nrComponent == 3)
		format = GL_RGB;
	if (nrComponent == 4)
		format = GL_RGBA;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, format, GL_UNSIGNED_BYTE, texelData);

	if (generateMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(texelData);
	glBindTexture(GL_TEXTURE_2D, 2); // unbind

	return true;
}
void Texture::bind(GLuint texUnit)
{
	glActiveTexture(GL_TEXTURE0 + texUnit);
	glBindTexture(GL_TEXTURE_2D, mTexture);
}
