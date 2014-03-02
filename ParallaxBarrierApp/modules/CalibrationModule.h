#pragma once

#include "AppModule.h"

class CalibrationModule :  public AppModule
{
public:
	CalibrationModule(testParallaxBarrierApp* app);
	virtual ~CalibrationModule();

	void setup();

	void moduleOn();
	void moduleOff();

	void update();

	void drawLeft();
	void drawRight();

	void keyReleased(int key);

private:
	ofImage aImage;
	ofImage bImage;
};

