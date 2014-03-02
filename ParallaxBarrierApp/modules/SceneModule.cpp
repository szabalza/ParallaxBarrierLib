#include "SceneModule.h"


SceneModule::SceneModule(testParallaxBarrierApp* app): AppModule(app)
{
}


SceneModule::~SceneModule()
{
}

void SceneModule::setup()
{
	lastUpdate = 0;

	//ofSetSmoothLighting(true);
	//light.setAmbientColor(ofColor(20) );
	light.setDiffuseColor( ofColor(150));
	light.setSpecularColor( ofColor(255));
	light.setDirectional();
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
	leftCamera.setFarClip(5.00);
	
	rightCamera.setNearClip(0.10);
	rightCamera.setFarClip(5.00);

	floorOffset = 0.027;

	//setup models
	astroboyModel.loadModel("astroBoy_walk.dae",true);
	astroboyModel.setAnimation(0);
	astroboyModel.setNormalizationFactor(0.15);
	astroboyModel.setScaleNomalization(true);
	astroboyModel.setRotation(0, 180, 1, 0, 0);
	astroboyModel.calculateDimensions();
	astroboyModel.setPosition(0, -8.1535, -0.3);

	animationTime = 0.0;

	vector<ofPoint> pts;
	pts.push_back( ofVec3f(0, -app->getParallaxBarrier()->getHeight() * 0.5f + floorOffset, 0.28));
	pts.push_back( ofVec3f(-0.1, -app->getParallaxBarrier()->getHeight() * 0.5f + floorOffset, 0.25));
	pts.push_back( ofVec3f(-0.1, -app->getParallaxBarrier()->getHeight() * 0.5f + floorOffset, -0.13));
	pts.push_back( ofVec3f(0.1 , -app->getParallaxBarrier()->getHeight() * 0.5f + floorOffset, -0.13));
	pts.push_back( ofVec3f(0.1, -app->getParallaxBarrier()->getHeight() * 0.5f + floorOffset, 0.25));
	pts.push_back( ofVec3f(0, -app->getParallaxBarrier()->getHeight() * 0.5f + floorOffset, 0.28));

	spline.setup(&pts, 0.005, 0.5); // setup with pts, stride, alpha
	spline.update(1);

	dragonModel.loadModel("dragon.dae",true);
	dragonModel.setNormalizationFactor(0.15);
	dragonModel.setScaleNomalization(true);
	dragonModel.setRotation(0, 45, 0, 1, 0);
	dragonModel.setRotation(0, 90, 1, 0, 0);
	dragonModel.calculateDimensions();
	dragonModel.setPosition(0, -5.04, 0);
}

void SceneModule::moduleOn()
{
	//light
    ofEnableSeparateSpecularLight();
}

void SceneModule::moduleOff()
{
	//light
	ofDisableSeparateSpecularLight();
}

void SceneModule::update()
{
	float now = ofGetElapsedTimef();
	float deltaTime = now - lastUpdate;

	updateCameraPositions();

	splineLastPosition = spline.loc;
	// update receives a multiplier for the splines stride
	spline.update();

	animationTime += deltaTime*(ofMap(splineLastPosition.distance(spline.loc), 0.0001, 0.002, 0.6, 0.85));
	if( animationTime >= 1.0 ){
		animationTime = 0.0;
	}
	astroboyModel.setNormalizedTime(animationTime);

	lastUpdate = now;
}

void SceneModule::drawLeft()
{
	leftCamera.begin();
		draw();
	leftCamera.end();
}

void SceneModule::drawRight()
{
	rightCamera.begin();
		draw();
	rightCamera.end();
}

void SceneModule::draw()
{
	ofSetColor(ofColor::white);
	light.enable();
	ofEnableSeparateSpecularLight();

	ofPushMatrix();
	ofTranslate(spline.loc.x, 0, spline.loc.z);
	ofVec3f astroboyDirection = spline.loc - splineLastPosition;
	ofQuaternion astroboyRotation;
	float angle, x, y ,z;
	astroboyRotation.makeRotate(ofVec3f(0,0,-1), astroboyDirection);
	astroboyRotation.getRotate(angle, x, y ,z);
	ofRotate(angle, x, y ,z);
	astroboyModel.drawFaces();
	ofPopMatrix();

	ofPushMatrix();
	ofRotate(30, 0, 1, 0);
	ofSetColor(31, 214, 91);
	dragonModel.drawFaces();
	ofPopMatrix();

	ofSetColor(ofColor::white);

	ofDisableLighting();
	ofPushMatrix();
	ofTranslate(0, -app->getParallaxBarrier()->getHeight() * 0.5f + floorOffset, 0);
	ofDrawAxis(0.01);
	ofRotateZ(90);
	ofSetColor(ofColor::green);
	ofDrawGridPlane(0.5, 16.0f);
	ofPopMatrix();
}

void SceneModule::keyReleased(int key)
{
}

void SceneModule::updateCameraPositions() 
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

