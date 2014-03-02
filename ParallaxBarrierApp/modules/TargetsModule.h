#pragma once

#include "AppModule.h"

class TargetsModule :
	public AppModule
{
public:
	TargetsModule(testParallaxBarrierApp* app);
	virtual ~TargetsModule();

	void setup();
	void moduleOn();
	void moduleOff();
	void update();
	void drawLeft();
	void drawRight();

	void keyReleased(int key);

	static bool targetPositionAfter( const ofVec3f& v1, const ofVec3f& v2 ) { return v1.z > v2.z; };

private:
	ofImage targetImage;
	ofVec3f screenLeft, screenRight, screenUp, screenDown;
	ofVec3f screenLeftToRight, screenDownToUp;
	ofxCustomCamera leftCamera, rightCamera;

	float minDepth, maxDepth;
	float targetWidth;
	float targetVelocity;
	bool targetsInPlace;
	vector<float> targetFinalDepths;
	vector<ofVec3f> targetPositions;

	float lastUpdate;

	void updateCameraPositions();
	void draw();
};

