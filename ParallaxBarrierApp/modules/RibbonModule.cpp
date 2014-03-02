#include "RibbonModule.h"



RibbonModule::RibbonModule(testParallaxBarrierApp* app): AppModule(app)
{
}


RibbonModule::~RibbonModule()
{
}

void RibbonModule::moduleOn()
{
	ofEnableAlphaBlending();
	ofSetBackgroundColor(255);
}

void RibbonModule::moduleOff()
{
	ofDisableAlphaBlending();
}

void RibbonModule::setup()
{

	//initialize the variable so it's off at the beginning
    useFreeCamera = false;

	lastUpdate = 0;
	lastInsert = 0;

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

	leftFreeCamera.setNearClip(0.10);
	leftFreeCamera.setFarClip(2.00);
	
	rightFreeCamera.setNearClip(0.10);
	rightFreeCamera.setFarClip(2.00);

	// load image
	leftHandImage.loadImage("data/hand-icon.png");
	leftHandImage.mirror(true, false);
	rightHandImage.loadImage("data/hand-icon.png");
	rightHandImage.mirror(true, true);
}

void RibbonModule::update()
{
	float now = ofGetElapsedTimef();
	float deltaTime = now - lastUpdate;

	if (app->isTracking && now - lastInsert > 0.033)
	{
		if ((app->leftHandPosition.x != 0 && app->leftHandPosition.y != 0)
			&& app->leftHandPosition.x > -app->getScreenWidth()*0.5
			&& app->leftHandPosition.x < app->getScreenWidth()*0.5
			&& app->leftHandPosition.y > -app->getScreenHeight()*0.5
			&& app->leftHandPosition.y < app->getScreenHeight()*0.5)
		{
			setPointerLeftHandPosition(ofVec3f(app->leftHandPosition.x, app->leftHandPosition.y, 0));
		}

		if ((app->rightHandPosition.x != 0 && app->rightHandPosition.y != 0)
			&& app->rightHandPosition.x > -app->getScreenWidth()*0.5
			&& app->rightHandPosition.x < app->getScreenWidth()*0.5
			&& app->rightHandPosition.y > -app->getScreenHeight()*0.5
			&& app->rightHandPosition.y < app->getScreenHeight()*0.5)
		{
			setPointerRightHandPosition(ofVec3f(app->rightHandPosition.x, app->rightHandPosition.y, 0));
		}

		lastInsert = now;
	}

	//don't move the points if we are using the free camera
    if(!useFreeCamera){
		updateLeftHandPoints(deltaTime);
		updateRightHandPoints(deltaTime);
		updateMousePoints(deltaTime);
    }

	updateCameraPositions();

	lastUpdate = now;
}

void RibbonModule::updateLeftHandPoints(float deltaTime)
{
	for(int i = 0; i < pointsLeftHand.size(); i++){
        pointsLeftHand[i].z += 0.18 * deltaTime;
    }
		
	ofVec3f sumOfAllPoints(0,0,0);
	vector<ofVec3f>::iterator iter = pointsLeftHand.begin();
	vector<ofColor>::iterator colorIter = pointsLeftHandColors.begin();
	while (iter != pointsLeftHand.end())
	{
		if(iter->z > leftCamera.getFarClip() && iter->z > rightCamera.getFarClip())
		{
			iter = pointsLeftHand.erase(iter);
			colorIter = pointsLeftHandColors.erase(colorIter);
		}
		else
		{
			sumOfAllPoints += *iter;
			++iter;
			++colorIter;
		}
	}

    center = sumOfAllPoints / pointsLeftHand.size();
}

void RibbonModule::updateRightHandPoints(float deltaTime)
{
	for(int i = 0; i < pointsRightHand.size(); i++){
        pointsRightHand[i].z += 0.18 * deltaTime;
    }
		
	ofVec3f sumOfAllPoints(0,0,0);
	vector<ofVec3f>::iterator iter = pointsRightHand.begin();
	vector<ofColor>::iterator colorIter = pointsRightHandColors.begin();
	while (iter != pointsRightHand.end())
	{
		if(iter->z > leftCamera.getFarClip() && iter->z > rightCamera.getFarClip())
		{
			iter = pointsRightHand.erase(iter);
			colorIter = pointsRightHandColors.erase(colorIter);
		}
		else
		{
			sumOfAllPoints += *iter;
			++iter;
			++colorIter;
		}
	}

    center = sumOfAllPoints / pointsRightHand.size();
}

void RibbonModule::updateMousePoints(float deltaTime)
{
	for(int i = 0; i < pointsMouse.size(); i++){
        pointsMouse[i].z += 0.18 * deltaTime;
    }
		
	ofVec3f sumOfAllPoints(0,0,0);
	vector<ofVec3f>::iterator iter = pointsMouse.begin();
	vector<ofColor>::iterator colorIter = pointsMouseColors.begin();
	while (iter != pointsMouse.end())
	{
		if(iter->z > leftCamera.getFarClip() && iter->z > rightCamera.getFarClip())
		{
			iter = pointsMouse.erase(iter);
			colorIter = pointsMouseColors.erase(colorIter);
		}
		else
		{
			sumOfAllPoints += *iter;
			++iter;
			++colorIter;
		}
	}

    center = sumOfAllPoints / pointsMouse.size();
}

void RibbonModule::updateCameraPositions() 
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

void RibbonModule::drawLeft()
{
	if(useFreeCamera){
        leftFreeCamera.begin();
    }
	else
	{
		leftCamera.begin();
	}
		draw();

	if(useFreeCamera){
        leftFreeCamera.end();
    }
	else
	{
		leftCamera.end();
	}
}

void RibbonModule::drawRight()
{
	if(useFreeCamera){
        rightFreeCamera.begin();
    }
	else
	{
		rightCamera.begin();
	}

		draw();

	if(useFreeCamera){
        rightFreeCamera.end();
    }
	else
	{
		rightCamera.end();
	}
}

void RibbonModule::draw()
{
	ofSetColor(255);

	//draw mouse
	drawMousePoints();

	//draw left hand
	drawLeftHandPoints();

	//draw right hand
	drawRightHandPoints();

	ofSetColor(ofColor::white);
}

void RibbonModule::drawMousePoints()
{
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	for(int i = 1; i < pointsMouse.size(); i++){

		//find this point and the next point
		ofVec3f thisPoint = pointsMouse[i-1];
		ofVec3f nextPoint = pointsMouse[i];

		//get the direction from one to the next.
		//the ribbon should fan out from this direction
		ofVec3f direction = (nextPoint - thisPoint);

		//get the distance from one point to the next
		float distance = direction.length();

		//get the normalized direction. normalized vectors always have a length of one
		//and are really useful for representing directions as opposed to something with length
		ofVec3f unitDirection = direction.normalized();

		//find both directions to the left and to the right
		ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,0,1));
		ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));

		//use the map function to determine the distance.
		//the longer the distance, the narrower the line.
		//this makes it look a bit like brush strokes
		float thickness = ofMap(distance, 0, 0.03, 0.01, 0.001, true);

		//calculate the points to the left and to the right
		//by extending the current point in the direction of left/right by the length
		ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
		ofVec3f rightPoint = thisPoint+toTheRight*thickness;

		//add these points to the triangle strip
		mesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
		mesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));

		mesh.addColor(pointsMouseColors[i]);
		mesh.addColor(pointsMouseColors[i]);
	}

	//end the shape
	mesh.draw();
}

void RibbonModule::drawLeftHandPoints()
{
	//draw left hand
	//ofSetColor(getPointerLeftHandColor());
	//ofCircle(app->leftHandPosition.x, app->leftHandPosition.y, 0, 0.007);

	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	for(int i = 1; i < pointsLeftHand.size(); i++){

		//find this point and the next point
		ofVec3f thisPoint = pointsLeftHand[i-1];
		ofVec3f nextPoint = pointsLeftHand[i];

		//get the direction from one to the next.
		//the ribbon should fan out from this direction
		ofVec3f direction = (nextPoint - thisPoint);

		//get the distance from one point to the next
		float distance = direction.length();

		//get the normalized direction. normalized vectors always have a length of one
		//and are really useful for representing directions as opposed to something with length
		ofVec3f unitDirection = direction.normalized();

		//find both directions to the left and to the right
		ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,0,1));
		ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));

		//use the map function to determine the distance.
		//the longer the distance, the narrower the line.
		//this makes it look a bit like brush strokes
		float thickness = ofMap(distance, 0, 0.03, 0.01, 0.001, true);

		//calculate the points to the left and to the right
		//by extending the current point in the direction of left/right by the length
		ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
		ofVec3f rightPoint = thisPoint+toTheRight*thickness;

		//add these points to the triangle strip
		mesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
		mesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));

		mesh.addColor(pointsLeftHandColors[i]);
		mesh.addColor(pointsLeftHandColors[i]);
	}

	//end the shape
	mesh.draw();

	//draw left hand
	ofSetColor(getPointerLeftHandColor());
	leftHandImage.draw(app->leftHandPosition.x - 0.007, app->leftHandPosition.y - 0.007, 0, 0.014, 0.014);
}

void RibbonModule::drawRightHandPoints()
{
	//ofSetColor(getPointerRightHandColor());
	//ofCircle(app->rightHandPosition.x, app->rightHandPosition.y, 0, 0.007);

	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	for(int i = 1; i < pointsRightHand.size(); i++){

		//find this point and the next point
		ofVec3f thisPoint = pointsRightHand[i-1];
		ofVec3f nextPoint = pointsRightHand[i];

		//get the direction from one to the next.
		//the ribbon should fan out from this direction
		ofVec3f direction = (nextPoint - thisPoint);

		//get the distance from one point to the next
		float distance = direction.length();

		//get the normalized direction. normalized vectors always have a length of one
		//and are really useful for representing directions as opposed to something with length
		ofVec3f unitDirection = direction.normalized();

		//find both directions to the left and to the right
		ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,0,1));
		ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));

		//use the map function to determine the distance.
		//the longer the distance, the narrower the line.
		//this makes it look a bit like brush strokes
		float thickness = ofMap(distance, 0, 0.03, 0.01, 0.001, true);

		//calculate the points to the left and to the right
		//by extending the current point in the direction of left/right by the length
		ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
		ofVec3f rightPoint = thisPoint+toTheRight*thickness;

		//add these points to the triangle strip
		mesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
		mesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));

		mesh.addColor(pointsRightHandColors[i]);
		mesh.addColor(pointsRightHandColors[i]);
	}

	//end the shape
	mesh.draw();

	//draw right hand
	ofSetColor(getPointerRightHandColor());
	rightHandImage.draw(app->rightHandPosition.x - 0.007, app->rightHandPosition.y - 0.007, 0, 0.014, 0.014);
}

void RibbonModule::keyReleased(int key)
{
	if(key==' ')
	{
		//swaps the camera view
		useFreeCamera = !useFreeCamera;
	}
}

void RibbonModule::mouseMoved(int x, int y )
{
		if (useFreeCamera)
	{
		float rotateAmountX = ofMap(x, 0, ofGetWidth(), 0, 360);
		float rotateAmountY = ofMap(y, 0, ofGetHeight(), 0, 360);
        ofVec3f furthestPoint;
        if (pointsMouse.size() > 0) {
            furthestPoint = pointsMouse[0];
        }
        else
        {
            furthestPoint = ofVec3f(x, y, 0);
        }

		float eyeDistance = app->leftEyePosition.distance(app->rightEyePosition);
        ofVec3f directionToFurthestPoint = (furthestPoint - center);
        ofVec3f directionToFurthestPointRotated = directionToFurthestPoint.rotated(rotateAmountX, ofVec3f(0,1,0));
		directionToFurthestPointRotated.rotate(rotateAmountY, directionToFurthestPointRotated.rotated(-90, ofVec3f(0,1,0)));
		ofVec3f upRotated = ofVec3f(0,1,0).rotate(rotateAmountY, directionToFurthestPointRotated.rotated(-90, ofVec3f(0,1,0)));
		ofVec3f leftDirection = directionToFurthestPointRotated.rotated(90, upRotated).scale(eyeDistance*0.5);
		ofVec3f rightDirection = directionToFurthestPointRotated.rotated(-90, upRotated).scale(eyeDistance*0.5);

		leftFreeCamera.setPosition(center + directionToFurthestPointRotated + leftDirection);
        leftFreeCamera.lookAt(center, ofVec3f(0,1,0));
		rightFreeCamera.setPosition(center + directionToFurthestPointRotated + rightDirection);
        rightFreeCamera.lookAt(center, ofVec3f(0,1,0));
	}
	else
	{
		ofVec3f mousePoint(
				(-x+app->getScreenOffsetX()+(app->getParallaxBarrier()->getScreenResolutionWidth()/2))*(app->getParallaxBarrier()->getWidth()/app->getParallaxBarrier()->getScreenResolutionWidth()),
				(-y+app->getScreenOffsetY()+(app->getParallaxBarrier()->getScreenResolutionHeight()/2))*(app->getParallaxBarrier()->getHeight()/app->getParallaxBarrier()->getScreenResolutionHeight()),
				0);

		setPointerMousePosition(mousePoint);
	}
}

void RibbonModule::setPointerLeftHandPosition(ofVec3f position)
{
	pointsLeftHand.push_back(position);

	ofColor color = getPointerLeftHandColor();

	pointsLeftHandColors.push_back(color);
}

void RibbonModule::setPointerRightHandPosition(ofVec3f position)
{
	pointsRightHand.push_back(position);

	ofColor color = getPointerRightHandColor();

	pointsRightHandColors.push_back(color);
}

void RibbonModule::setPointerMousePosition(ofVec3f position)
{
	pointsMouse.push_back(position);

	ofColor color = getPointerMouseColor();

	pointsMouseColors.push_back(color);
}

ofColor RibbonModule::getPointerLeftHandColor()
{
	float now = ofGetElapsedTimef();
	float colorValue = fmod(now*80, 765);

	return ofColor (colorValue < 256? (255 - colorValue) : max(0.f, colorValue - 510),
				colorValue < 256 ? colorValue : max(0.f, 510 - colorValue), 
				colorValue < 511 ? max(0.f, colorValue - 255) : 765 - colorValue);
}

ofColor RibbonModule::getPointerRightHandColor()
{
	float now = ofGetElapsedTimef();
	float colorValue = fmod((now*80) + 255, 765);

	return ofColor (colorValue < 256? (255 - colorValue) : max(0.f, colorValue - 510),
				colorValue < 256 ? colorValue : max(0.f, 510 - colorValue), 
				colorValue < 511 ? max(0.f, colorValue - 255) : 765 - colorValue);
}

ofColor RibbonModule::getPointerMouseColor()
{
	float now = ofGetElapsedTimef();
	float colorValue = fmod((now*80) + 512, 765);

	return ofColor (colorValue < 256? (255 - colorValue) : max(0.f, colorValue - 510),
				colorValue < 256 ? colorValue : max(0.f, 510 - colorValue), 
				colorValue < 511 ? max(0.f, colorValue - 255) : 765 - colorValue);
}