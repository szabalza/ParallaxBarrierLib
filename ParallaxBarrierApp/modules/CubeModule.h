#pragma once

#include "AppModule.h"
#include "../ofxCustomCamera.h"
#include "../node/Cube.h"

class CubeModule :	public AppModule
{
public:
	CubeModule(testParallaxBarrierApp* app);
	virtual ~CubeModule();

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
	Cube cube;
	ofLight light;
	float lastUpdate;
	bool rotate;

	void updateCameraPositions();
};

