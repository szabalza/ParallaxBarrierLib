#pragma once
#include "AppModule.h"

class RibbonModule : public AppModule
{
public:
	RibbonModule(testParallaxBarrierApp* app);

	void setup();

	void update();
	void moduleOn();
	void moduleOff();

	void drawLeft();
	void drawRight();

	void keyReleased(int key);
	void mouseMoved(int x, int y );

	virtual ~RibbonModule();

private:
	ofVec3f screenLeft, screenRight, screenUp, screenDown;
	ofVec3f screenLeftToRight, screenDownToUp;
	ofxCustomCamera leftCamera, rightCamera;
	float lastUpdate, lastInsert;

	//this holds all of our points
    vector<ofVec3f> pointsLeftHand, pointsRightHand, pointsMouse;
	vector<ofColor> pointsLeftHandColors, pointsRightHandColors, pointsMouseColors;
    //this keeps track of the center of all the points
    ofVec3f center;
	//our camera objects for looking at the scene from multiple perspectives
	ofxCustomCamera leftFreeCamera, rightFreeCamera;
	//if usecamera is true, we'll turn on the camera view
    bool useFreeCamera;

	ofImage rightHandImage;
	ofImage leftHandImage;

	void updateCameraPositions();
	void updateLeftHandPoints(float deltaTime);
	void updateRightHandPoints(float deltaTime);
	void updateMousePoints(float deltaTime);
	void draw();
	void drawLeftHandPoints();
	void drawRightHandPoints();
	void drawMousePoints();
	void setPointerLeftHandPosition(ofVec3f position);
	void setPointerRightHandPosition(ofVec3f position);
	void setPointerMousePosition(ofVec3f position);
	ofColor getPointerLeftHandColor();
	ofColor getPointerRightHandColor();
	ofColor getPointerMouseColor();
};

