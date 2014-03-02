#include "TargetsModule.h"


TargetsModule::TargetsModule(testParallaxBarrierApp* app): AppModule(app)
{
}


TargetsModule::~TargetsModule()
{
}

void TargetsModule::setup()
{
	ofSeedRandom();

	targetImage.loadImage("target.png");

	minDepth = -0.25f;
	maxDepth = 0.8f;
	targetWidth = 0.05f;
	targetVelocity = 0.06f;
	targetsInPlace = true;

	targetPositions.push_back(ofVec3f(0.f,0.02f,-0.2f));
	targetPositions.push_back(ofVec3f(0.02f,-0.07f,0.f));
	targetPositions.push_back(ofVec3f(-0.12f,-0.09f,0.2f));
	targetPositions.push_back(ofVec3f(0.15f,0.05f,0.5f));
	targetPositions.push_back(ofVec3f(-0.1f,0.08f,0.1f));
	targetPositions.push_back(ofVec3f(0.12f,-0.085f,0.3f));
	
	targetFinalDepths.push_back(-0.2f);
	targetFinalDepths.push_back(0.f);
	targetFinalDepths.push_back(0.2f);
	targetFinalDepths.push_back(0.5f);
	targetFinalDepths.push_back(0.1f);
	targetFinalDepths.push_back(0.3f);	

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
}

void TargetsModule::updateCameraPositions() 
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

void TargetsModule::moduleOn()
{
	ofEnableAlphaBlending();
}

void TargetsModule::moduleOff()
{
	ofDisableAlphaBlending();
}

void TargetsModule::update()
{
	float now = ofGetElapsedTimef();
	float deltaTime = now - lastUpdate;

	updateCameraPositions();

	if (!targetsInPlace)
	{
		targetsInPlace = true;
		float targetMovement = deltaTime * targetVelocity;

		for (int i = 0; i < targetPositions.size(); i++)
		{
			float distance = targetFinalDepths[i] - targetPositions[i].z; 
			
			if (abs(distance) <= targetMovement)
			{
				targetPositions[i].z = targetFinalDepths[i];
			}
			else
			{
				targetsInPlace = false;
				targetPositions[i].z = targetPositions[i].z + (distance > 0? targetMovement: -targetMovement);
			}
		}
	}

	lastUpdate = now;
}

void TargetsModule::drawLeft()
{
	leftCamera.begin();
		draw();
	leftCamera.end();
}

void TargetsModule::drawRight()
{
	rightCamera.begin();
		draw();
	rightCamera.end();
}

void TargetsModule::draw()
{	
	ofSetColor(ofColor::blue);
	ofLine(app->getParallaxBarrier()->getWidth()*0.5, -app->getParallaxBarrier()->getHeight()*0.5, 0.0,
			app->getParallaxBarrier()->getWidth()*0.5, -app->getParallaxBarrier()->getHeight()*0.5, maxDepth);
	ofLine(app->getParallaxBarrier()->getWidth()*0.5, app->getParallaxBarrier()->getHeight()*0.5, 0.0,
			app->getParallaxBarrier()->getWidth()*0.5, app->getParallaxBarrier()->getHeight()*0.5, maxDepth);
	ofLine(-app->getParallaxBarrier()->getWidth()*0.5, -app->getParallaxBarrier()->getHeight()*0.5, 0.0,
			-app->getParallaxBarrier()->getWidth()*0.5, -app->getParallaxBarrier()->getHeight()*0.5, maxDepth);
	ofLine(-app->getParallaxBarrier()->getWidth()*0.5, app->getParallaxBarrier()->getHeight()*0.5, 0.0,
			-app->getParallaxBarrier()->getWidth()*0.5, app->getParallaxBarrier()->getHeight()*0.5, maxDepth);

	float lineDepth = maxDepth;

	while (lineDepth > 0.f)
	{
		ofLine(app->getParallaxBarrier()->getWidth()*0.5, app->getParallaxBarrier()->getHeight()*0.5, lineDepth,
				-app->getParallaxBarrier()->getWidth()*0.5, app->getParallaxBarrier()->getHeight()*0.5, lineDepth);
		ofLine(app->getParallaxBarrier()->getWidth()*0.5, -app->getParallaxBarrier()->getHeight()*0.5, lineDepth,
				-app->getParallaxBarrier()->getWidth()*0.5, -app->getParallaxBarrier()->getHeight()*0.5, lineDepth);
		ofLine(app->getParallaxBarrier()->getWidth()*0.5, app->getParallaxBarrier()->getHeight()*0.5, lineDepth,
				app->getParallaxBarrier()->getWidth()*0.5, -app->getParallaxBarrier()->getHeight()*0.5, lineDepth);
		ofLine(-app->getParallaxBarrier()->getWidth()*0.5, app->getParallaxBarrier()->getHeight()*0.5, lineDepth,
				-app->getParallaxBarrier()->getWidth()*0.5, -app->getParallaxBarrier()->getHeight()*0.5, lineDepth);

		lineDepth -= 0.15f;
	}

	ofSetColor(ofColor::white);

	sort(targetPositions.begin(), targetPositions.end(), TargetsModule::targetPositionAfter);

	for (int i = 0; i < targetPositions.size(); i++)
	{
		ofLine(targetPositions[i].x, targetPositions[i].y, targetPositions[i].z,
			targetPositions[i].x, targetPositions[i].y, maxDepth);
		targetImage.draw(targetPositions[i].x - targetWidth* 0.5f, targetPositions[i].y - targetWidth* 0.5f, targetPositions[i].z, targetWidth, targetWidth);
	}
	
}

void TargetsModule::keyReleased(int key)
{
	if (key == ' ')
	{
		for (int i = 0; i < targetFinalDepths.size(); i++)
		{
			targetFinalDepths[i] = ofRandom(minDepth, maxDepth);
		}
		targetsInPlace = false;
	}
}
