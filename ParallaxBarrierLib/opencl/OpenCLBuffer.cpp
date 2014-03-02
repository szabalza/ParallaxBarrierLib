#include "OpenCLBuffer.h"


OpenCLBuffer::OpenCLBuffer(void * buffer, const int &size): buffer(buffer), size(size)
{
}


OpenCLBuffer::~OpenCLBuffer(void)
{
}

void * &OpenCLBuffer::getBuffer()
{
	return this->buffer;
}

const int &OpenCLBuffer::getSize()
{
	return this->size;
}

cl_mem &OpenCLBuffer::getMemObj()
{
	return memObj;
}

void OpenCLBuffer::setMemObj(cl_mem memObj)
{
	this->memObj = memObj;
}
