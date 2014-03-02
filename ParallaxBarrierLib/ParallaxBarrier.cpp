#include "ParallaxBarrier.h"

ParallaxBarrier::ParallaxBarrier(float width, float height, int screenResolutionWidth, int screenResolutionHeight, int barrierResolutionWidth, int barrierResolutionHeight, float spacing, ofVec3f const &position, ofVec3f const &viewDirection, ofVec3f const &upDirection)
{
	_width = width;
	_height = height;
	_screenResolutionWidth = screenResolutionWidth;
	_screenResolutionHeight = screenResolutionHeight;
	_barrierResolutionWidth = barrierResolutionWidth;
	_barrierResolutionHeight = barrierResolutionHeight;
	_spacing = spacing;
	_position = position;
	_viewDirection = viewDirection;
	_upDirection = upDirection;
	_screenInversePixelWidth = _screenResolutionWidth/_width;
	_barrierInversePixelWidth = _barrierResolutionWidth/_width;
	_modelScale = 1.f/spacing;

	// images initialization
	_barrierImage.allocate(barrierResolutionWidth, barrierResolutionHeight, OF_IMAGE_COLOR_ALPHA);
	_screenImage.allocate(screenResolutionWidth, screenResolutionHeight, OF_IMAGE_COLOR_ALPHA);
	_screenLeftImage.allocate(screenResolutionWidth, screenResolutionHeight, OF_IMAGE_COLOR_ALPHA);
	_screenRightImage.allocate(screenResolutionWidth, screenResolutionHeight, OF_IMAGE_COLOR_ALPHA);
	
	// initialize model transormation
	updateModelTransformation();
	_model.setWidth(_width*_modelScale);

	_model.initialize(_screenLeftImage, _screenRightImage, _screenImage, _barrierImage, _width*_modelScale/_screenImage.width, _width*_modelScale/_barrierImage.width);
}

ParallaxBarrier::~ParallaxBarrier()
{
}

void ParallaxBarrier::update(ofVec3f const &leftEyePosition, ofVec3f const &rightEyePosition, bool invertedBarrier)
{

	ofVec3f modelLeftEyePosition3d = leftEyePosition * _modelTransformation;
	ofVec3f modelRightEyePosition3d = rightEyePosition * _modelTransformation;

	_modelLeftEyePosition = ofVec2f(modelLeftEyePosition3d.x, modelLeftEyePosition3d.z);
	_modelRightEyePosition = ofVec2f(modelRightEyePosition3d.x, modelRightEyePosition3d.z);

	//modify model for new eye positions
	_model.update(_modelLeftEyePosition, _modelRightEyePosition, invertedBarrier);
}

void ParallaxBarrier::updateModelTransformation()
{
	ofMatrix4x4 modelScale, modelRotation, modelUpRotation, modelTranslation, modelCenterTranslation;
	modelScale.makeScaleMatrix(_modelScale, _modelScale, _modelScale);

	modelRotation.makeRotationMatrix(_viewDirection, ofVec3f(0,0,1));
	modelUpRotation.makeRotationMatrix(modelRotation*_upDirection, ofVec3f(0,1,0));
	modelTranslation.makeTranslationMatrix(ofVec3f(_width * 0.5,0,0));
	modelCenterTranslation.makeTranslationMatrix(-_position);

	_modelTransformation = modelCenterTranslation * modelRotation * modelUpRotation * modelTranslation * modelScale;
}

float ParallaxBarrier::getWidth() const
{
	return _width;
}

float ParallaxBarrier::getHeight() const
{
	return _height;
}

int ParallaxBarrier::getBarrierResolutionWidth() const
{
	return _barrierResolutionWidth;
}

int ParallaxBarrier::getBarrierResolutionHeight() const
{
	return _barrierResolutionHeight;
}

int ParallaxBarrier::getScreenResolutionWidth() const
{
	return _screenResolutionWidth;
}

int ParallaxBarrier::getScreenResolutionHeight() const
{
	return _screenResolutionHeight;
}

float ParallaxBarrier::getSpacing()
{
	return _spacing;
}

void ParallaxBarrier::setWidth(float width)
{
	this->_width = width;
	_model.setWidth(_width*_modelScale);
	updateModelTransformation();
}

void ParallaxBarrier::setHeight(float height)
{
	this->_height = height;
}

void ParallaxBarrier::setBarrierResolutionWidth(int barrierResolutionWidth)
{
	this->_barrierResolutionWidth = barrierResolutionWidth;
}

void ParallaxBarrier::setBarrierResolutionHeight(int barrierResolutionHeight)
{
	this->_barrierResolutionHeight = barrierResolutionHeight;
}

void ParallaxBarrier::setScreenResolutionWidth(int screenResolutionWidth)
{
	this->_screenResolutionWidth = screenResolutionWidth;
}

void ParallaxBarrier::setScreenResolutionHeight(int screenResolutionHeight)
{
	this->_screenResolutionHeight = screenResolutionHeight;
}

void ParallaxBarrier::setSpacing(float spacing)
{
	this->_spacing = spacing;
	_modelScale = 1.f / _spacing;
	_model.setWidth(_width*_modelScale);
	updateModelTransformation();
}

const ofVec3f& ParallaxBarrier::getPosition()  const
{
	return _position;
}

const ofVec3f& ParallaxBarrier::getViewDirection() const
{
	return _viewDirection;
}

const ofVec3f& ParallaxBarrier::getUpDirection() const
{
	return _upDirection;
}

void ParallaxBarrier::setPosition(ofVec3f position)
{
	this->_position = position;
	updateModelTransformation();
}

void ParallaxBarrier::setViewDirection(ofVec3f viewDirection)
{
	this->_viewDirection = viewDirection;
	updateModelTransformation();
}

void ParallaxBarrier::setUpDirection(ofVec3f upDirection)
{
	this->_upDirection = upDirection;
	updateModelTransformation();
}

ofImage& ParallaxBarrier::getScreenImage()
{
	return _screenImage;
}

ofImage& ParallaxBarrier::getBarrierImage()
{
	return _barrierImage;
}

ofImage& ParallaxBarrier::getScreenLeftImage()
{
	return _screenLeftImage;
}

ofImage& ParallaxBarrier::getScreenRightImage()
{
	return _screenRightImage;
}
