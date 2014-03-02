#pragma once

#include "ofMain.h"
#include "ofxFensterManager.h"
#include "ParallaxBarrier.h"

class ParallaxBarrierApp;

class BarrierWindow: public ofxFensterListener {
public:
	BarrierWindow();
	~BarrierWindow();
	void setup();
	void draw();
	void keyReleased(int key, ofxFenster* window);

	ofImage* barrierImage;
	ParallaxBarrierApp* parallaxBarrierApp;
};

class ParallaxBarrierApp : public ofBaseApp {

public:
	// Eye positions need to be 
	// updated in app 'update' method
	ofVec3f leftEyePosition;
	ofVec3f rightEyePosition;

	ParallaxBarrierApp();
	~ParallaxBarrierApp();

	// ParallaxBarrier apps only need to implement setupApp
	void setup();
	virtual void setupApp() {};
	// 'initializeParallaxBarrier' method must be called from 'setupApp' method
	void initializeParallaxBarrier(float width, float height, int screenResolutionWidth, int screenResolutionHeight, int barrierResolutionWidth, int barrierResolutionHeight, float spacing, ofVec3f const &position, ofVec3f const &viewDirection, ofVec3f const &upDirection, int screenOffsetX = 0, int screenOffsetY = 0);

	// ParallaxBarrier apps only need to implement drawLeft and drawRight
	void draw();
	virtual void drawLeft() {};
	virtual void drawRight() {};

	void keyReleased(int key);

	int getScreenOffsetX() const {return screenOffsetX;};
	int getScreenOffsetY() const {return screenOffsetY;};

protected:
	int screenOffsetX, screenOffsetY;

	bool invertBarrier;
	int invertCounter;
	int invertLimit;

	ParallaxBarrier* parallaxBarrier;

	virtual string getScreenMessage();

private:
	ofxFenster* barrierWindow;
	
	GLuint frameBufferObject;
	GLuint frameBufferDepthTexture;

};