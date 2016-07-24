#pragma once

#include "ParallaxBarrierApp.h"
#include "ofxCustomCamera.h"
#include "modules/AppModule.h"
#include "tracker/SkeletonTracker.h"
#include "node/Cube.h"

class AppModule;

class testParallaxBarrierApp :
	public ParallaxBarrierApp
{
public:
	void setupApp();
	void drawLeft();
	void drawRight();

	void update();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	void mouseMovedEvent(ofMouseEventArgs &args);

	int getScreenWidth();
	int getScreenHeight();
	const ParallaxBarrier* getParallaxBarrier();

	bool isTracking;
	float deltaTime, lastUpdate;
	SkeletonTracker skeletonTracker;
	ofVec3f leftHandPosition, rightHandPosition;
	bool lastTrackSucceeded;

private:

	void updateEyeAndHandsPositions();
	string getScreenMessage();

	AppModule* activeModule;
	vector<AppModule*> modules;
	int activeModuleIndex;
};

