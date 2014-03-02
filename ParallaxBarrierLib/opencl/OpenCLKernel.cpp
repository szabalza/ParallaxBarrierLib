#include "OpenCLKernel.h"

#include <fstream>
#include <sstream>
#include <GL/glew.h>

#if defined(WIN32) || defined(WIN64)
	#include <Windows.h>
#elif __APPLE__
#elif __unix
	#include <X11/Xlib.h>
#endif

#include "OpenCLBuffer.h"

OpenCLKernel::OpenCLKernel(const string &fileName, const string &kernelName): fileName(fileName), kernelName(kernelName), readTextureListSize(0), writeTextureListSize(0), readTextureList(NULL), writeTextureList(NULL), refreshArguments(false)
{
	initialize();
}

string getFileContents(const string &filename)
{
  std::ifstream in(filename, std::ios::in | std::ios::binary);
  if (in)
  {
    std::ostringstream contents;
    contents << in.rdbuf();
    in.close();
	return(contents.str());
  }
  throw(errno);
}

void OpenCLKernel::initialize()
{
	string sourceString;
	const char *sourceCString;
	cl_platform_id *platform_ids, platform_id = NULL;
	cl_device_id *device_ids, device_id = NULL;
	cl_uint ret_num_devices;
	cl_uint ret_num_platforms;
	cl_uint extensions_size;
	char* extensions;
	bool deviceSelected = false;

	/* Load the source code containing the kernel*/
	sourceString = getFileContents(fileName);
	sourceCString = sourceString.c_str();

	//Wait until all opengl commands are processed
	glFinish();

	/* Get Platform and Device Info */
	status = clGetPlatformIDs( 0, NULL, &ret_num_platforms);
	if (status != CL_SUCCESS)
		return;

	platform_ids = new cl_platform_id[ret_num_platforms];
	status = clGetPlatformIDs(ret_num_platforms, platform_ids, &ret_num_platforms);
	if (status != CL_SUCCESS)
		return;

	for(cl_uint i = 0; i < ret_num_platforms && !deviceSelected; i++)
	{
		platform_id = platform_ids[i];

		/* Get GPU type devices */
		status = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 0, NULL, &ret_num_devices);
		device_ids = new cl_device_id[ret_num_devices];
		status = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_GPU, ret_num_devices, device_ids, NULL);

		for(cl_uint j = 0; j < ret_num_devices && !deviceSelected; j++)
		{
			device_id = device_ids[j];

			/* Look for device with CL_GL_SHARING_EXT extension */
			status = clGetDeviceInfo( device_id, CL_DEVICE_EXTENSIONS, 0, NULL, &extensions_size);
			extensions = new char[extensions_size];
			status = clGetDeviceInfo( device_id, CL_DEVICE_EXTENSIONS, extensions_size, extensions, NULL);

			string ext = extensions;   
			deviceSelected = ext.find(CL_GL_SHARING_EXT) != string::npos;
			delete[] extensions;
		}

		delete[] device_ids;
	}

	delete[] platform_ids;

// Apple use share group
#ifdef __APPLE__
	// Get current CGL Context and CGL Share group
	CGLContextObj kCGLContext = CGLGetCurrentContext();
	CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
#endif

	// Create CL context properties
	cl_context_properties properties[] = 
	{
//Windows properties
#if defined(WIN32) || defined(WIN64)
		CL_GL_CONTEXT_KHR, (cl_context_properties) wglGetCurrentContext(), // WGL Context
		CL_WGL_HDC_KHR, (cl_context_properties) wglGetCurrentDC(), // WGL HDC
		CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id, // OpenCL platform
//Apple properties
#elif __APPLE__
		CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
		(cl_context_properties) kCGLShareGroup,
//Other unix properties 
#elif __unix
		CL_GL_CONTEXT_KHR, (cl_context_properties) glXGetCurrentContext(), // GLX Context
		CL_GLX_DISPLAY_KHR, (cl_context_properties) glXGetCurrentDisplay(), // GLX Display
		CL_CONTEXT_PLATFORM, (cl_context_properties) platform_id, // OpenCL platform
#endif
		0
	};

#ifdef __APPLE__
	// Create a context with device in the CGL share group
	context = clCreateContext(properties, 0, 0, NULL, 0, 0);
#else
	/* Create OpenCL context */
	context = clCreateContext(properties, 1, &device_id, NULL, NULL, &status);
#endif

	/* Create Command Queue */
	command_queue = clCreateCommandQueue(context, device_id, 0, &status);

	/* Create Kernel Program from the source */
	program = clCreateProgramWithSource(context, 1, &sourceCString,
		NULL, &status);

	/* Build Kernel Program */
	status = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

	/* Create OpenCL Kernel */
	kernel = clCreateKernel(program, kernelName.c_str(), &status);
}

bool OpenCLKernel::defineArguments(const list<OpenCLBuffer*> * readWriteBuffers, const list<OpenCLBuffer*> * readBuffers, const list<OpenCLBuffer*> * writeBuffers, const list<OpenCLTexture*> * readTextures, const list<OpenCLTexture*> * writeTextures)
{
	refreshArguments = true;

	//clean objects list
	if(readWriteBuffers != NULL)
		readWriteBufferList = *readWriteBuffers;
	if (readBuffers != NULL)
		readBufferList = *readBuffers;
	if (writeBuffers != NULL)
		writeBufferList = *writeBuffers;
	if (readTextures != NULL)
		readTextureListSize = (*readTextures).size();
	if (writeTextures != NULL)
		writeTextureListSize = (*writeTextures).size();

	list<OpenCLBuffer *>::const_iterator iterator, end;
	cl_mem memobj;

	// create read/write Memory Objects
	if(readWriteBuffers != NULL) 
	{
		for (iterator = readWriteBufferList.begin(), end = readWriteBufferList.end(); iterator != end; ++iterator)
		{
			/* Create Memory Buffer */
			memobj = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR, (*iterator)->getSize(), (*iterator)->getBuffer(), &status);
			if (status != CL_SUCCESS)
				return false;

			(*iterator)->setMemObj(memobj);
		}
	}

	// create read Memory Objects
	if (readBuffers != NULL)
	{
		for (iterator = readBufferList.begin(), end = readBufferList.end(); iterator != end; ++iterator)
		{
			/* Create Memory Buffer */
			memobj = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, (*iterator)->getSize(), (*iterator)->getBuffer(), &status);
			if (status != CL_SUCCESS)
				return false;

			(*iterator)->setMemObj(memobj);
		}
	}

	// create write Memory Objects
	if (writeBuffers != NULL)
	{
		for (iterator = writeBufferList.begin(), end = writeBufferList.end(); iterator != end; ++iterator)
		{
			/* Create Memory Buffer */
			memobj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, (*iterator)->getSize(), NULL, &status);
			if (status != CL_SUCCESS)
				return false;

			(*iterator)->setMemObj(memobj);
		}
	}

	list<OpenCLTexture *>::const_iterator textureIterator, textureEnd;
	int i;
	
	if (readTextures != NULL)
	{
		if(readTextureListSize > 0)
		{
			readTextureList = new cl_mem[readTextureListSize];
			i = 0;
			for (textureIterator = (*readTextures).begin(), textureEnd = (*readTextures).end(); textureIterator != textureEnd; ++textureIterator, ++i)
			{
				memobj = clCreateFromGLTexture( context, CL_MEM_READ_ONLY, (*textureIterator)->getTextureTarget(), 0, (*textureIterator)->getTextureId(), &status);
				if (status != CL_SUCCESS)
					return false;

				readTextureList[i] = memobj;
			}
		}
	}

	if (writeTextures != NULL)
	{
		if(writeTextureListSize > 0)
		{
			writeTextureList = new cl_mem[writeTextureListSize];
			i = 0;
			for (textureIterator = (*writeTextures).begin(), textureEnd = (*writeTextures).end(); textureIterator != textureEnd; ++textureIterator, ++i)
			{
				memobj = clCreateFromGLTexture( context, CL_MEM_WRITE_ONLY, (*textureIterator)->getTextureTarget(), 0, (*textureIterator)->getTextureId(), &status);
				if (status != CL_SUCCESS)
					return false;

				writeTextureList[i] = memobj;
			}
		}
	}

	return true;
}

bool OpenCLKernel::execute(const int &workDimension, const size_t* globalSize, const size_t* localSize)
{
	list<OpenCLBuffer *>::const_iterator iterator, end;

	if (readTextureListSize > 0)
	{
		//Acquire shared objects (read textures)
		clEnqueueAcquireGLObjects ( command_queue, readTextureListSize, readTextureList, 0, NULL, NULL );
	}

	if (writeTextureListSize > 0)
	{
		//Acquire shared objects (write textures)
		clEnqueueAcquireGLObjects ( command_queue, writeTextureListSize, writeTextureList, 0, NULL, NULL );
	}

	//arguments do not need to be set each time
	//only when new aguments are defined
	if (refreshArguments)
	{
		int argumentNumber = 0;

		// read/write Memory Objects arguments
		for (iterator = readWriteBufferList.begin(), end = readWriteBufferList.end(); iterator != end; ++iterator)
		{
			/* Set OpenCL Kernel Parameters */
			status = clSetKernelArg(kernel, argumentNumber++, sizeof(cl_mem), &(*iterator)->getMemObj());
			if (status != CL_SUCCESS)
				return false;
		}

		// read Memory Objects arguments
		for (iterator = readBufferList.begin(), end = readBufferList.end(); iterator != end; ++iterator)
		{
			/* Set OpenCL Kernel Parameters */
			status = clSetKernelArg(kernel, argumentNumber++, sizeof(cl_mem), &(*iterator)->getMemObj());
			if (status != CL_SUCCESS)
				return false;
		}

		// write Memory Objects arguments
		for (iterator = writeBufferList.begin(), end = writeBufferList.end(); iterator != end; ++iterator)
		{
			/* Set OpenCL Kernel Parameters */
			status = clSetKernelArg(kernel, argumentNumber++, sizeof(cl_mem), &(*iterator)->getMemObj());
			if (status != CL_SUCCESS)
				return false;
		}

		//read texture arguments
		for (int i = 0; i < readTextureListSize; i++)
		{
			/* Set OpenCL Kernel Parameters */
			status = clSetKernelArg(kernel, argumentNumber++, sizeof(cl_mem), &readTextureList[i]);
			if (status != CL_SUCCESS)
				return false;
		}

		//write texture arguments
		for (int i = 0; i < writeTextureListSize; i++)
		{
			/* Set OpenCL Kernel Parameters */
			status = clSetKernelArg(kernel, argumentNumber++, sizeof(cl_mem), &writeTextureList[i]);
			if (status != CL_SUCCESS)
				return false;
		}

		refreshArguments = false;
	}

	/* Execute OpenCL Kernel */
	status = clEnqueueNDRangeKernel(command_queue, kernel, workDimension, NULL, globalSize, localSize, 0, NULL, NULL);
	if (status != CL_SUCCESS)
		return false;

	// Copy results from read/write Memory Objects
	for (iterator = readWriteBufferList.begin(), end = readWriteBufferList.end(); iterator != end; ++iterator)
	{
		status = clEnqueueReadBuffer(command_queue, (*iterator)->getMemObj(), CL_TRUE, 0,
			(*iterator)->getSize(), (*iterator)->getBuffer(), 0, NULL, NULL);
		if (status != CL_SUCCESS)
			return false;
	}

	// Copy results from write Memory Objects
	for (iterator = writeBufferList.begin(), end = writeBufferList.end(); iterator != end; ++iterator)
	{
		status = clEnqueueReadBuffer(command_queue, (*iterator)->getMemObj(), CL_TRUE, 0,
			(*iterator)->getSize(), (*iterator)->getBuffer(), 0, NULL, NULL);
		if (status != CL_SUCCESS)
			return false;
	}

	if (readTextureListSize > 0)
	{
		//Release shared Objects (read textures)
		clEnqueueReleaseGLObjects ( command_queue, readTextureListSize, readTextureList, 0, NULL, NULL );
	}

	if (writeTextureListSize > 0)
	{
		//Release shared Objects (write textures)
		clEnqueueReleaseGLObjects ( command_queue, writeTextureListSize, writeTextureList, 0, NULL, NULL );
	}

	/* Finalization */
	status = clFlush(command_queue);
	if (status != CL_SUCCESS)
		return false;

	status = clFinish(command_queue);
	if (status != CL_SUCCESS)
		return false;

	return true;
}

string OpenCLKernel::getFileName()
{
	return this->fileName;
}

cl_int OpenCLKernel::getStatus()
{
	return this->status;
}

OpenCLKernel::~OpenCLKernel()
{
	destroy();
}

void OpenCLKernel::destroy()
{
	status = clReleaseKernel(kernel);

	status = clReleaseProgram(program);

	list<OpenCLBuffer *>::const_iterator iterator, end;

	// release read/write Memory Objects
	for (iterator = readWriteBufferList.begin(), end = readWriteBufferList.end(); iterator != end; ++iterator)
	{
		status = clReleaseMemObject((*iterator)->getMemObj());
	}

	// release read Memory Objects
	for (iterator = readBufferList.begin(), end = readBufferList.end(); iterator != end; ++iterator)
	{
		status = clReleaseMemObject((*iterator)->getMemObj());
	}

	// release write Memory Objects
	for (iterator = writeBufferList.begin(), end = writeBufferList.end(); iterator != end; ++iterator)
	{
		status = clReleaseMemObject((*iterator)->getMemObj());
	}

	// release read textures
	for (int i = 0; i < readTextureListSize; i++)
	{
		status = clReleaseMemObject(readTextureList[i]);
	}

	// release write textures
	for (int i = 0; i < writeTextureListSize; i++)
	{
		status = clReleaseMemObject(writeTextureList[i]);
	}

	status = clReleaseCommandQueue(command_queue);

	status = clReleaseContext(context);

	delete[] readTextureList;
	delete[] writeTextureList;
}


