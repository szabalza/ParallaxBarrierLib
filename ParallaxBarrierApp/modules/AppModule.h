#pragma once

#include "../testParallaxBarrierApp.h"

class testParallaxBarrierApp;

class AppModule
{
public:
	AppModule(testParallaxBarrierApp* app);
	virtual ~AppModule();

	virtual void setup() {};

	virtual void moduleOn() {};
	virtual void moduleOff() {};

	virtual void update() {};

	virtual void drawLeft() {};
	virtual void drawRight() {};

	virtual void keyPressed  (int key) {};
	virtual void keyReleased(int key) {};
	virtual void mouseMoved(int x, int y ) {};
	virtual void mouseDragged(int x, int y, int button) {};
	virtual void mousePressed(int x, int y, int button) {};
	virtual void mouseReleased(int x, int y, int button) {};

protected:
	testParallaxBarrierApp* app;
};

