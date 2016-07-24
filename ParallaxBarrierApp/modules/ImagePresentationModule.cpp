#include "ImagePresentationModule.h"


ImagePresentationModule::ImagePresentationModule(testParallaxBarrierApp* app): AppModule(app)
{
}


ImagePresentationModule::~ImagePresentationModule()
{
}

void ImagePresentationModule::setup()
{
	ofImage* imageLeft = new ofImage();
	ofImage* imageRight = new ofImage();
	imageLeft->loadImage("streetLeft.png");
	imageRight->loadImage("streetRight.png");
	imagesLeft.push_back(imageLeft);
	imagesRight.push_back(imageRight);

	imageLeft = new ofImage();
	imageRight = new ofImage();
	imageLeft->loadImage("birdLeft.png");
	imageRight->loadImage("birdRight.png");
	imagesLeft.push_back(imageLeft);
	imagesRight.push_back(imageRight);

	imageLeft = new ofImage();
	imageRight = new ofImage();
	imageLeft->loadImage("chairsLeft.png");
	imageRight->loadImage("chairsRight.png");
	imagesLeft.push_back(imageLeft);
	imagesRight.push_back(imageRight);

	imageLeft = new ofImage();
	imageRight = new ofImage();
	imageLeft->loadImage("cloudsLeft.png");
	imageRight->loadImage("cloudsRight.png");
	imagesLeft.push_back(imageLeft);
	imagesRight.push_back(imageRight);

	imageLeft = new ofImage();
	imageRight = new ofImage();
	imageLeft->loadImage("guitarLeft.png");
	imageRight->loadImage("guitarRight.png");
	imagesLeft.push_back(imageLeft);
	imagesRight.push_back(imageRight);

	imageLeft = new ofImage();
	imageRight = new ofImage();
	imageLeft->loadImage("natureLeft.png");
	imageRight->loadImage("natureRight.png");
	imagesLeft.push_back(imageLeft);
	imagesRight.push_back(imageRight);

	imageLeft = new ofImage();
	imageRight = new ofImage();
	imageLeft->loadImage("soldierLeft.png");
	imageRight->loadImage("soldierRight.png");
	imagesLeft.push_back(imageLeft);
	imagesRight.push_back(imageRight);

	imageLeft = new ofImage();
	imageRight = new ofImage();
	imageLeft->loadImage("squaresLeft.png");
	imageRight->loadImage("squaresRight.png");
	imagesLeft.push_back(imageLeft);
	imagesRight.push_back(imageRight);

	activeImageLeft = imagesLeft[0];
	activeImageRight = imagesRight[0];
	selectedImage = 0;
}

void ImagePresentationModule::drawLeft()
{
	activeImageLeft->draw(0,0, app->getScreenWidth(), app->getScreenHeight());
}

void ImagePresentationModule::drawRight()
{
	activeImageRight->draw(0,0, app->getScreenWidth(), app->getScreenHeight());
}

void ImagePresentationModule::keyReleased(int key)
{
	if(key == ' ')
	{
		selectedImage = (selectedImage + 1) % imagesLeft.size();
		activeImageLeft = imagesLeft[selectedImage];
		activeImageRight = imagesRight[selectedImage];
	}
}

void ImagePresentationModule::mouseReleased(int x, int y, int button)
{
	if(button == 2)
	{
		selectedImage = (selectedImage + 1) % imagesLeft.size();
		activeImageLeft = imagesLeft[selectedImage];
		activeImageRight = imagesRight[selectedImage];
	}
};

