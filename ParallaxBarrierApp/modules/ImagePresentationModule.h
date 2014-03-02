#pragma once
#include "AppModule.h"
class ImagePresentationModule :
	public AppModule
{
public:
	ImagePresentationModule(testParallaxBarrierApp* app);
	virtual ~ImagePresentationModule();

	void setup();

	void drawLeft();
	void drawRight();

	void keyReleased(int key);

private:
	ofImage* activeImageLeft;
	ofImage* activeImageRight;
	vector<ofImage*> imagesLeft;
	vector<ofImage*> imagesRight;
	int selectedImage;
};

