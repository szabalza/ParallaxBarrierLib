#include "CalibrationModule.h"


CalibrationModule::CalibrationModule(testParallaxBarrierApp* app): AppModule(app)
{
}


CalibrationModule::~CalibrationModule()
{
}

void CalibrationModule::setup()
{
	//setup initial eyes position
	app->leftEyePosition.x = 0.0322;
	app->leftEyePosition.y = 0;
	app->leftEyePosition.z = -0.90;
	app->rightEyePosition.x = -0.0322;
	app->rightEyePosition.y = 0;
	app->rightEyePosition.z = -0.90;

	//load images
	aImage.loadImage("data/A.png");
	bImage.loadImage("data/B.png");
}

void CalibrationModule::moduleOn()
{
}

void CalibrationModule::moduleOff()
{
}

void CalibrationModule::update()
{
}

void CalibrationModule::drawLeft()
{
	aImage.draw(0,0, app->getScreenWidth(), app->getScreenHeight());
}

void CalibrationModule::drawRight()
{
	bImage.draw(0,0, app->getScreenWidth(), app->getScreenHeight());
}

void CalibrationModule::keyReleased(int key)
{
	if(key == ' ')
	{
		if(!app->isTracking)
		{
			app->skeletonTracker.calibrateToWorldCoordinates(app->leftEyePosition, app->rightEyePosition);
		} else
		{
			app->leftEyePosition.x = 0.0322;
			app->leftEyePosition.y = 0;
			app->leftEyePosition.z = -0.80;
			app->rightEyePosition.x = -0.0322;
			app->rightEyePosition.y = 0;
			app->rightEyePosition.z = -0.80;
		}

		app->isTracking = !app->isTracking;
	}
}

void CalibrationModule::mouseReleased(int x, int y, int button)
{
	if(button == 2)
	{
		if(!app->isTracking)
		{
			app->skeletonTracker.calibrateToWorldCoordinates(app->leftEyePosition, app->rightEyePosition);
		} else
		{
			app->leftEyePosition.x = 0.0322;
			app->leftEyePosition.y = 0;
			app->leftEyePosition.z = -0.80;
			app->rightEyePosition.x = -0.0322;
			app->rightEyePosition.y = 0;
			app->rightEyePosition.z = -0.80;
		}

		app->isTracking = !app->isTracking;
	}
};