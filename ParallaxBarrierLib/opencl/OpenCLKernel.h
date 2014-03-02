#pragma once

#include <string>
#include <list>
#include "OpenCLTexture.h"
#include "OpenCLBuffer.h"

#ifdef __APPLE__ 
	#import <OpenGL/glew.h>
	#include <OpenCL/opencl.h>
	#include <OpenCL/cl_gl.h>
	static const char *CL_GL_SHARING_EXT = "cl_APPLE_gl_sharing";
#else
	#include <GL/glew.h>
	#include <CL/cl.h>
	#include <CL/cl_gl.h>
	static const char *CL_GL_SHARING_EXT = "cl_khr_gl_sharing";
#endif

#define MEM_SIZE (128)

using namespace std;

class OpenCLKernel
{
public:
	OpenCLKernel(const string &fileName, const string &kernelName);
	virtual ~OpenCLKernel(void);

	string getFileName();
	bool defineArguments(const list<OpenCLBuffer*> * readWriteBuffers, const list<OpenCLBuffer*> * readBuffers, const list<OpenCLBuffer*> * writeBuffers, const list<OpenCLTexture*> * readTextures, const list<OpenCLTexture*> * writeTextures);
	bool execute(const int &workDimension, const size_t* globalSize, const size_t* localSize);
	cl_int getStatus();


private:
	const string fileName;
	const string kernelName;
	cl_command_queue command_queue;
	cl_kernel kernel;
	cl_context context;
	cl_program program;
	cl_int status;
	list<OpenCLBuffer*> readWriteBufferList;
	list<OpenCLBuffer*> readBufferList;
	list<OpenCLBuffer*> writeBufferList;
	cl_mem* readTextureList;
	int readTextureListSize;
	cl_mem* writeTextureList;
	int writeTextureListSize;
	bool refreshArguments;

	void initialize();
	void destroy();
};

