#pragma once

#ifdef __APPLE__ 
	#include <OpenCL/opencl.h>
#else
	#include <CL/cl.h>
#endif

class OpenCLBuffer
{
public:
	OpenCLBuffer(void * buffer, const int &size);
	virtual ~OpenCLBuffer(void);

	void *&getBuffer();
	const int &getSize();
	cl_mem &getMemObj();

	void setMemObj(cl_mem memObj);

private:
	void *buffer;
	int size;
	cl_mem memObj;
};

