#pragma once

#include "ofVec3f.h"
#include "ofImage.h"

#include "ParallaxBarrierModel.h"

class ParallaxBarrier
{
public:
	ParallaxBarrier(float width, float height, int screenResolutionWidth, int screenResolutionHeight, int barrierResolutionWidth, int barrierResolutionHeight, float spacing, ofVec3f const &position, ofVec3f const &viewDirection, ofVec3f const &upDirection);
	virtual ~ParallaxBarrier();

	float getWidth() const;
	float getHeight() const;
	int getScreenResolutionWidth() const;
	int getScreenResolutionHeight() const;
	int getBarrierResolutionWidth() const;
	int getBarrierResolutionHeight() const;
	float getSpacing();
	const ofVec3f& getPosition() const;
	const ofVec3f& getViewDirection() const;
	const ofVec3f& getUpDirection() const;
	void setWidth(float width);
	void setHeight(float height);
	void setScreenResolutionWidth(int screenResolutionWidth);
	void setScreenResolutionHeight(int screenResolutionHeight);
	void setBarrierResolutionWidth(int barrierResolutionWidth);
	void setBarrierResolutionHeight(int barrierResolutionHeight);
	void setSpacing(float spacing);
	void setPosition(ofVec3f position);
	void setViewDirection(ofVec3f viewDirection);
	void setUpDirection(ofVec3f upDirection);

	void update(ofVec3f const &leftEyePosition, ofVec3f const &rightEyePosition, bool invertedBarrier = false);

	ofImage& getScreenImage();
	ofImage& getBarrierImage();

	ofImage& getScreenLeftImage();
	ofImage& getScreenRightImage();

private:
	float _width;
	float _height;
	int _barrierResolutionWidth;
	int _barrierResolutionHeight;
	int _screenResolutionWidth;
	int _screenResolutionHeight;
	float _spacing;
	ofVec3f _position;
	ofVec3f _viewDirection;
	ofVec3f _upDirection;
	float _barrierInversePixelWidth;
	float _screenInversePixelWidth;

	// model transformation
	ofMatrix4x4 _modelTransformation;
	float _modelScale;
	ofVec2f _modelLeftEyePosition;
	ofVec2f _modelRightEyePosition;

	ParallaxBarrierModel _model;

	ofImage _barrierImage;
	ofImage _screenImage;
	ofImage _screenLeftImage;
	ofImage _screenRightImage;

	void updateModelTransformation();
};

