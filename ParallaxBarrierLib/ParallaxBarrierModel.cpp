#include "ParallaxBarrierModel.h"
#include "ofGraphics.h"

ParallaxBarrierModel::ParallaxBarrierModel(): _width(1.f)
{
}

ParallaxBarrierModel::~ParallaxBarrierModel()
{
}

bool ParallaxBarrierModel::initialize(ofImage& leftImage, ofImage& rightImage, ofImage& screenImage, ofImage& barrierImage, float screenPixelWidth, float barrierPixelWidth)
{
	bool s1 = _screenShader.load("", "shaders/screenShader.sl");
	bool s2 = _barrierShader.load("", "shaders/barrierShader.sl");

	_leftImage = &leftImage;
	_rightImage = &rightImage;
	_screenImage = &screenImage;
	_barrierImage = &barrierImage;
	_screenPixelWidth = screenPixelWidth;
	_barrierPixelWidth = barrierPixelWidth;
	
	//screen
	glGenTextures(1, &_screenFrameBufferDepthTexture);
	glBindTexture(GL_TEXTURE_2D, _screenFrameBufferDepthTexture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _screenImage->width, _screenImage->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(_screenImage->getTextureReference().texData.textureTarget, _screenImage->getTextureReference().texData.textureID);
		// Poor filtering. Needed
		glTexParameteri(_screenImage->getTextureReference().texData.textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(_screenImage->getTextureReference().texData.textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(_screenImage->getTextureReference().texData.textureTarget, 0);

	glGenFramebuffers(1, &_screenFrameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, _screenFrameBufferObject);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _screenImage->getTextureReference().texData.textureID, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _screenFrameBufferDepthTexture, 0);

	// check framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;

	//barrier
	glGenTextures(1, &_barrierFrameBufferDepthTexture);
	glBindTexture(GL_TEXTURE_2D, _barrierFrameBufferDepthTexture);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, _barrierImage->width, _barrierImage->height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glBindTexture(_barrierImage->getTextureReference().texData.textureTarget, _barrierImage->getTextureReference().texData.textureID);
		// Poor filtering. Needed
		glTexParameteri(_barrierImage->getTextureReference().texData.textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(_barrierImage->getTextureReference().texData.textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glBindTexture(_barrierImage->getTextureReference().texData.textureTarget, 0);

	glGenFramebuffers(1, &_barrierFrameBufferObject);
	glBindFramebuffer(GL_FRAMEBUFFER, _barrierFrameBufferObject);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _barrierImage->getTextureReference().texData.textureID, 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _barrierFrameBufferDepthTexture, 0);

	// check framebuffer is ok
	if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		return false;
	
	// Render to the screen
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

bool ParallaxBarrierModel::update(ofVec2f leftEyePosition, ofVec2f rightEyePosition, bool invertedBarrier)
{
	float minPoint = getMinVisiblePoint(leftEyePosition, rightEyePosition);
	float maxPoint = getMaxVisiblePoint(leftEyePosition, rightEyePosition);

	if (minPoint == -1 || maxPoint == -1)
	{
		return false;
	}

	float rightShutterDistanceFraction = 1/rightEyePosition.y;
	float leftShutterDistanceFraction = 1/leftEyePosition.y;
	float BCoef = (rightEyePosition.x * rightShutterDistanceFraction - leftEyePosition.x * leftShutterDistanceFraction) / (1 - leftShutterDistanceFraction);
	float ACoef = (1.f - rightShutterDistanceFraction) / (1.f - leftShutterDistanceFraction);

	// START: Update screen texture
	//render to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _screenFrameBufferObject);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// begin shader
	_screenShader.begin();

	_screenShader.setUniformTexture("leftTexture", *_leftImage, 0);
	_screenShader.setUniformTexture("rightTexture", *_rightImage, 1);
	_screenShader.setUniform1f("A", ACoef);
	_screenShader.setUniform1f("B", BCoef);
	_screenShader.setUniform1f("Xo", minPoint);
	_screenShader.setUniform1f("pixelWidth", _screenPixelWidth);
	_screenShader.setUniform1f("pixelCoveragePercentageEpsilon", SCREEN_PIXEL_EPSILON_PERCENTAGE);
	_screenShader.setUniform1f("inverted", invertedBarrier? 1.f : 0.f);
	

	// render rectangle to apply shader
	ofRect(0,0,ofGetCurrentViewport().getWidth(),ofGetCurrentViewport().getHeight());

	// end shader
	_screenShader.end();
	// END: Update screen texture

	// START: Update barrier texture
	//render to framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, _barrierFrameBufferObject);
	glDrawBuffer(GL_COLOR_ATTACHMENT0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// begin shader
	_barrierShader.begin();

	_barrierShader.setUniform1f("A", ACoef);
	_barrierShader.setUniform1f("B", BCoef);
	_barrierShader.setUniform1f("Xo", minPoint);
	_barrierShader.setUniform1f("leftEyePositionX", leftEyePosition.x);
	_barrierShader.setUniform1f("leftEyePositionY", leftEyePosition.y);
	_barrierShader.setUniform1f("pixelWidth", _barrierPixelWidth);
	_barrierShader.setUniform1f("pixelCoveragePercentageEpsilon", BARRIER_PIXEL_EPSILON_PERCENTAGE);
	_barrierShader.setUniform1f("inverted", invertedBarrier? 1.f : 0.f);
	

	// render rectangle to apply shader
	ofRect(0,0,ofGetCurrentViewport().getWidth(),ofGetCurrentViewport().getHeight());

	// end shader
	_barrierShader.end();
	// END: Update barrier texture

	//render to regular output
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return true;
}

float ParallaxBarrierModel::getMaxVisiblePoint(ofVec2f leftEyePosition, ofVec2f rightEyePosition)
{
	ofVec2f maxLineOfSight = (rightEyePosition - leftEyePosition).rotate(-NON_VISIBLE_ANGLE);
	
	float angle = maxLineOfSight.angle(ofVec2f(-1,0));

	if (angle >= 0 && angle <= NON_VISIBLE_ANGLE)
	{
		return -1;
	}

	if (maxLineOfSight.align(ofVec2f(-1,0), DEGREES_EPSILON))
	{
		return -1;
	}

	if (maxLineOfSight.align(ofVec2f(1,0), DEGREES_EPSILON))
	{
		return _width;
	}

	if (maxLineOfSight.y > 0)
	{
		return _width;
	}

	float intersection = intersectionXAxis(rightEyePosition, maxLineOfSight);

	if (intersection <= 0)
	{
		return -1;
	}
	else if (intersection > _width)
	{
		return _width;
	}
	else 
	{
		return intersection;
	}
}

float ParallaxBarrierModel::getMinVisiblePoint(ofVec2f leftEyePosition, ofVec2f rightEyePosition)
{
	ofVec2f minLineOfSight = (leftEyePosition - rightEyePosition).rotate(NON_VISIBLE_ANGLE);

	float angle = minLineOfSight.angle(ofVec2f(1,0));

	if (angle <= 0 && angle >= -NON_VISIBLE_ANGLE)
	{
		return -1;
	}
	
	if (minLineOfSight.align(ofVec2f(1,0), DEGREES_EPSILON))
	{
		return -1;
	}

	if (minLineOfSight.align(ofVec2f(-1,0), DEGREES_EPSILON))
	{
		return 0;
	}

	if (minLineOfSight.y > 0)
	{
		return 0;
	}

	float intersection = intersectionXAxis(leftEyePosition, minLineOfSight);

	if (intersection > _width)
	{
		return -1;
	}
	else if (intersection <= 0)
	{
		return 0;
	}
	else 
	{
		return intersection;
	}
}

float ParallaxBarrierModel::intersectionXAxis(ofVec2f point, ofVec2f dir)
{
	return point.x - point.y * dir.x / dir.y;
}

void ParallaxBarrierModel::setWidth(float width)
{
	_width = width;
}

float ParallaxBarrierModel::getWidth()
{
	return _width;
}

