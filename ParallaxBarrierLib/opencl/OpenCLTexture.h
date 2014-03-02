#pragma once

#ifdef __APPLE__ 
	#import <OpenGL/glew.h>
#else
	#include <GL/glew.h>
#endif

class OpenCLTexture
{
public:
	OpenCLTexture(const GLuint &textureId, const GLenum &textureTarget);
	virtual ~OpenCLTexture();

	GLuint getTextureId();
	GLenum getTextureTarget();

private:
	GLuint textureId;
	GLenum textureTarget;

};

