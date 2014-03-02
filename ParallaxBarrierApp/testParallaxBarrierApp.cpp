#include "testParallaxBarrierApp.h"
#include "modules/CalibrationModule.h"
#include "modules/CubeModule.h"
#include "modules/SceneModule.h"
#include "modules/ImagePresentationModule.h"
#include "modules/RibbonModule.h"
#include "modules/TargetsModule.h"
#include "modules/VideoModule.h"

void testParallaxBarrierApp::setupApp()
{
	glEnable(GL_DEPTH_TEST);
	ofSetVerticalSync(true);

	//setup Parallax Barrier
	initializeParallaxBarrier(0.3442, 0.1935, 1418, 796, 1366, 768, 0.01397, ofVec3f(0,0,0), ofVec3f(0,0,-1), ofVec3f(0,1,0), 133, 62);
	//initializeParallaxBarrier(0.3442, 0.1935, 1366, 768, 1366, 768, 0.01397, ofVec3f(0,0,0), ofVec3f(0,0,-1), ofVec3f(0,1,0), 0, 0);

	//initialize face tracking
	skeletonTracker.initialize();
	isTracking = false;
	lastTrackSucceeded = false;

	updateEyeAndHandsPositions();

	lastUpdate = 0;

	//initialize modules
	modules.push_back(new CalibrationModule(this));
	modules.push_back(new SceneModule(this));
	modules.push_back(new ImagePresentationModule(this));
	modules.push_back(new RibbonModule(this));
	modules.push_back(new TargetsModule(this));
	modules.push_back(new VideoModule(this));

	//setup modules
	for (int i = 0; i < modules.size(); i++)
	{
		modules[i]->setup();
	}

	activeModule = modules[0];
	activeModule->moduleOn();
}

void testParallaxBarrierApp::update()
{
	deltaTime = ofGetElapsedTimef() - lastUpdate;
	lastUpdate = ofGetElapsedTimef();

	updateEyeAndHandsPositions();

	activeModule->update();
}

void testParallaxBarrierApp::updateEyeAndHandsPositions() 
{
	// update eyes and hands position from tracker
	if (isTracking)
	{
		leftEyePosition = skeletonTracker.getLeftEyePosition();
		rightEyePosition = skeletonTracker.getRightEyePosition();
		leftHandPosition = skeletonTracker.getLeftHandPosition();
		rightHandPosition = skeletonTracker.getRightHandPosition();
		lastTrackSucceeded = skeletonTracker.getLastTrackSucceeded();
	}
}

void testParallaxBarrierApp::drawLeft()
{
	activeModule->drawLeft();
}

void testParallaxBarrierApp::drawRight()
{
	activeModule->drawRight();
}

//--------------------------------------------------------------
void testParallaxBarrierApp::keyPressed(int key)
{
	if(key == 'i')
	{
		invertBarrier = !invertBarrier;
	}

	activeModule->keyPressed(key);
}

//--------------------------------------------------------------
void testParallaxBarrierApp::keyReleased(int key)
{
	ParallaxBarrierApp::keyReleased(key);

	if(key == OF_KEY_LEFT)
	{
		leftEyePosition.x -= 0.01;
		rightEyePosition.x -= 0.01;
	}

	if(key == OF_KEY_RIGHT)
	{
		leftEyePosition.x += 0.01;
		rightEyePosition.x += 0.01;
	}

	if(key == OF_KEY_UP)
	{
		leftEyePosition.z += 0.01;
		rightEyePosition.z += 0.01;
	}

	if(key == OF_KEY_DOWN)
	{
		leftEyePosition.z -= 0.01;
		rightEyePosition.z -= 0.01;
	}

	if(key == 'a')
	{
		parallaxBarrier->setSpacing(parallaxBarrier->getSpacing() + 0.00001f);
	}

	if(key == 'z')
	{
		parallaxBarrier->setSpacing(parallaxBarrier->getSpacing() - 0.00001f);
	}

	if(key == 'x')
	{
		screenOffsetX -= 0.01;
	}

	if(key == 'c')
	{
		screenOffsetX += 0.01;
	}

	if(key == '=')
	{
		invertLimit++;
	}
	
	if(key == '-')
	{
		invertLimit--;
	}

	if(key >= '1' && key <= '9' && key - '1' < modules.size())
	{
		activeModule->moduleOff();
		activeModule = modules[key - '1'];
		activeModule->moduleOn();
	}

	activeModule->keyReleased(key);
}

//--------------------------------------------------------------
void testParallaxBarrierApp::mouseMoved(int x, int y)
{
	activeModule->mouseMoved(x, y);
}

//--------------------------------------------------------------
void testParallaxBarrierApp::mouseDragged(int x, int y, int button)
{
	activeModule->mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void testParallaxBarrierApp::mousePressed(int x, int y, int button)
{
	activeModule->mousePressed(x, y, button);
}

//--------------------------------------------------------------
void testParallaxBarrierApp::mouseReleased(int x, int y, int button)
{
	activeModule->mouseReleased(x, y, button);
}

//--------------------------------------------------------------
void testParallaxBarrierApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void testParallaxBarrierApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void testParallaxBarrierApp::dragEvent(ofDragInfo dragInfo)
{
}

void testParallaxBarrierApp::mouseMovedEvent(ofMouseEventArgs &args)
{
}

int testParallaxBarrierApp::getScreenWidth()
{
	if (parallaxBarrier != NULL)
	{
		return parallaxBarrier->getScreenResolutionWidth();
	}

	return 0;
}

int testParallaxBarrierApp::getScreenHeight()
{
	if (parallaxBarrier != NULL)
	{
		return parallaxBarrier->getScreenResolutionHeight();
	}

	return 0;
}

const ParallaxBarrier* testParallaxBarrierApp::getParallaxBarrier()
{
	return parallaxBarrier;
}

string testParallaxBarrierApp::getScreenMessage()
{
	string msg = ParallaxBarrierApp::getScreenMessage();
	msg += "\nUser tracked: ";
	msg += lastTrackSucceeded? "true" : "false";

	return msg;
}
