#include "OpenCLTexture.h"

OpenCLTexture::OpenCLTexture(const GLuint &textureId, const GLenum &textureTarget): textureId(textureId), textureTarget(textureTarget)
{
}

OpenCLTexture::~OpenCLTexture()
{
}

GLuint OpenCLTexture::getTextureId()
{
	return textureId;
}

GLenum OpenCLTexture::getTextureTarget()
{
	return textureTarget;
}