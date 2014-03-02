#include "CubeModule.h"


CubeModule::CubeModule(testParallaxBarrierApp* app): AppModule(app)
{
}


CubeModule::~CubeModule()
{
}

void CubeModule::setup()
{
	rotate = false;
	lastUpdate = 0;

	ofSetSmoothLighting(true);
	light.setDiffuseColor( ofColor::white);
	light.setSpecularColor( ofColor(255.f, 255.f, 255.f));
	light.setPosition(0.30, 0.30, -0.30);

	screenLeft = (app->getParallaxBarrier()->getPosition() + app->getParallaxBarrier()->getUpDirection().getPerpendicular(app->getParallaxBarrier()->getViewDirection()).getScaled(app->getParallaxBarrier()->getWidth() * 0.5));
	screenRight = (app->getParallaxBarrier()->getPosition() - app->getParallaxBarrier()->getUpDirection().getPerpendicular(app->getParallaxBarrier()->getViewDirection()).getScaled(app->getParallaxBarrier()->getWidth() * 0.5));
	screenUp = (app->getParallaxBarrier()->getPosition() + app->getParallaxBarrier()->getUpDirection().getScaled(app->getParallaxBarrier()->getHeight() * 0.5));
	screenDown = (app->getParallaxBarrier()->getPosition() - app->getParallaxBarrier()->getUpDirection().getScaled(app->getParallaxBarrier()->getHeight() * 0.5));

	screenLeftToRight = (screenRight - screenLeft).normalize();
	screenDownToUp = app->getParallaxBarrier()->getUpDirection().normalized();

	//setup cameras
	updateCameraPositions();

	leftCamera.setNearClip(0.10);
	leftCamera.setFarClip(2.00);
	
	rightCamera.setNearClip(0.10);
	rightCamera.setFarClip(2.00);

	//setup nodes
	cube.setPosition(0, -0.03, 0.20);
	cube.rotate(20, 0, 0, 1);
	cube.rotate(35, 0, 1, 0);
}

void CubeModule::moduleOn()
{
	ofBackground(0);
}

void CubeModule::moduleOff()
{
}

void CubeModule::update()
{
	float now = ofGetElapsedTimef();
	float deltaTime = now - lastUpdate;

	updateCameraPositions();
	if (rotate)
	{
		cube.rotate(fmod(10.f*deltaTime, 360.f), 0, 1, 0);
		cube.rotate(fmod(3*deltaTime, 360.f), 1, 0, 0);
	}

	lastUpdate = now;
}

void CubeModule::updateCameraPositions() 
{
	//update cameras position based on eyes position
	leftCamera.setPosition(app->leftEyePosition);
	leftCamera.lookAt(app->getParallaxBarrier()->getPosition(), app->getParallaxBarrier()->getUpDirection());
	leftCamera.setAspectRatio((app->getParallaxBarrier()->getWidth()*(screenDownToUp.getPerpendicular(app->getParallaxBarrier()->getPosition() - app->leftEyePosition).dot((screenLeftToRight))))/(app->getParallaxBarrier()->getHeight()*((app->getParallaxBarrier()->getPosition() - app->leftEyePosition).getPerpendicular(screenLeftToRight).dot((screenDownToUp)))) );
	leftCamera.setFov((screenUp - app->leftEyePosition).angle(screenDown - app->leftEyePosition));

	rightCamera.setPosition(app->rightEyePosition);
	rightCamera.lookAt(app->getParallaxBarrier()->getPosition(), app->getParallaxBarrier()->getUpDirection());
	rightCamera.setAspectRatio((app->getParallaxBarrier()->getWidth()*(screenDownToUp.getPerpendicular(app->getParallaxBarrier()->getPosition() - app->rightEyePosition).dot((screenLeftToRight))))/(app->getParallaxBarrier()->getHeight()*((app->getParallaxBarrier()->getPosition() - app->rightEyePosition).getPerpendicular(screenLeftToRight).dot((screenDownToUp)))) );
	rightCamera.setFov((screenUp - app->rightEyePosition).angle(screenDown - app->rightEyePosition));
}

void CubeModule::drawLeft()
{
	leftCamera.begin();
		light.enable();
		cube.draw();
	leftCamera.end();
}

void CubeModule::drawRight()
{
	rightCamera.begin();
		light.enable();
		cube.draw();
	rightCamera.end();
}

void CubeModule::keyReleased(int key)
{
	if(key==' ')
	{
		rotate = !rotate;
	}
}


