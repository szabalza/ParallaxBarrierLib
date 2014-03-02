#pragma once
#include "ofCamera.h"

class ofxCustomCamera :
	public ofCamera
{
public:
	ofxCustomCamera();

	// set the matrices
	virtual void begin(ofRectangle viewport = ofGetCurrentViewport());

	void setAspectRatio(float aspectRatio);
private:
	float aspectRatio;
};

