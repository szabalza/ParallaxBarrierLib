#pragma once
#include "AppModule.h"

class VideoModule :
	public AppModule
{
public:
	VideoModule(testParallaxBarrierApp* app);
	virtual ~VideoModule();

	void setup();

	void update();

	void drawLeft();
	void drawRight();

	void keyReleased(int key);

private:
	ofVideoPlayer movie;
};

