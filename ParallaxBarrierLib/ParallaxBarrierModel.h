#pragma once

#include "ofVec2f.h"
#include "ofImage.h"
#include "ofShader.h"
#include <vector>

#define PARALLAX_BARRIER_MAX_ITERATIONS 2000

#define SCREEN_PIXEL_EPSILON_PERCENTAGE 0.01f//0.10f
#define BARRIER_PIXEL_EPSILON_PERCENTAGE 0.01f//0.05f

#define DEGREES_EPSILON 0.01f
#define NON_VISIBLE_ANGLE 20.f

using namespace std;

// ParallaxBarrierModel assumptions:
// - Shutter 'y' coordinate is 1
// - Screen 'y' coordinate is 0
// - Distance from screen to shutter is 1
// - Eyes 'y' coordinate must be bigger than 1
// - left/right denomination corresponds to the viewer perspective
// - First point in 'ScreenPoints' corresponds to the start of a left eye view zone
// - Points in 'ScreenPoints' altarnate between 'Left Eye Start View Zone'/'Right Eye Start View Zone'
// - First point in 'ShutterPoints' corresponds to the start of a non-transparent zone in the shutter 
// - Points in 'ShutterPoints' altarnate between 'Translucid Start Zone'/'Non-Transparent Start Zone'
class ParallaxBarrierModel
{
public:
	ParallaxBarrierModel();
	virtual ~ParallaxBarrierModel();

	bool ParallaxBarrierModel::initialize(ofImage& leftImage, ofImage& rightImage, ofImage& screenImage, ofImage& barrierImage, float screenPixelWidth, float barrierPixelWidth);

	bool update(ofVec2f leftEyePosition, ofVec2f rightEyePosition, bool invertedBarrier);

	float getWidth();
	void setWidth(float width);
private:
	float _width;

	float getMinVisiblePoint(ofVec2f leftEyePosition, ofVec2f rightEyePosition);
	float getMaxVisiblePoint(ofVec2f leftEyePosition, ofVec2f rightEyePosition);
	float intersectionXAxis(ofVec2f point, ofVec2f dir);

	ofImage *_leftImage, *_rightImage, *_screenImage, *_barrierImage;
	ofShader _screenShader;
	ofShader _barrierShader;
	GLuint _screenFrameBufferObject, _barrierFrameBufferObject;
	GLuint _screenFrameBufferDepthTexture, _barrierFrameBufferDepthTexture;
	float _screenPixelWidth;
	float _barrierPixelWidth;

};

