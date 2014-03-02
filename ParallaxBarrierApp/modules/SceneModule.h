#pragma once

#include "AppModule.h"
#include "../ofxCustomCamera.h"
#include "ofxAssimpModelLoader.h"
#include "ofVboMesh.h"
#include "../ofxHSpline.h"

class SceneModule : public AppModule
{
public:
	SceneModule(testParallaxBarrierApp* app);
	virtual ~SceneModule();

	void setup();
	void moduleOn();
	void moduleOff();

	void update();

	void drawLeft();
	void drawRight();

	void keyReleased(int key);

private:
	ofVec3f screenLeft, screenRight, screenUp, screenDown;
	ofVec3f screenLeftToRight, screenDownToUp;
	ofxCustomCamera leftCamera, rightCamera;

	ofLight light;
	float lastUpdate;
	float floorOffset;
	ofVec3f splineLastPosition;

	bool bAnimate;
	bool bAnimateMouse;
	float animationTime;
	ofxAssimpModelLoader astroboyModel, dragonModel;
	ofxHSpline spline;

	void updateCameraPositions();
	void draw();
};

