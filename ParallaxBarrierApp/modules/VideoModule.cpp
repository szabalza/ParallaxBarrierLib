#include "VideoModule.h"


VideoModule::VideoModule(testParallaxBarrierApp* app): AppModule(app)
{
}


VideoModule::~VideoModule()
{
}

void VideoModule::setup()
{
	movie.loadMovie("Panasonic Forest tweet 3D SBS.mp4");
	movie.play();
}

void VideoModule::update()
{
	movie.update();
}

void VideoModule::drawLeft()
{
	movie.draw(0,0, app->getScreenWidth()*2, app->getScreenHeight());
}

void VideoModule::drawRight()
{
	movie.draw(-app->getScreenWidth(),0, app->getScreenWidth()*2, app->getScreenHeight());
}

void VideoModule::keyReleased(int key)
{
	if(key == ' ')
	{
		movie.setPaused(!movie.isPaused());
	}
}

