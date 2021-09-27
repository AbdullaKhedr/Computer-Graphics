#include "Texture.h"

Texture::Texture() 
	:mTexture(0)
{

}
Texture::~Texture()
{
	glDeleteTextures(1, &mTexture);
}

bool Texture::loadTexture(const string& fileName, bool generateMipMaps = true)
{

}
void Texture::bind(GLuint texUnit = 0)
{

}
