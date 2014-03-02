#include "ParallaxBarrierApp.h"


BarrierWindow::BarrierWindow(): barrierImage(NULL)
{
}

//--------------------------------------------------------------
BarrierWindow::~BarrierWindow()
{
}

//--------------------------------------------------------------
void BarrierWindow::setup()
{
}

//--------------------------------------------------------------
void BarrierWindow::draw()
{
	//draw barrier texture
	if (barrierImage != NULL)
	{
		barrierImage->draw(0,0);
	}
}

//--------------------------------------------------------------
void BarrierWindow::keyReleased(int key, ofxFenster* window)
{
	if(key=='f')
		window->toggleFullscreen();
}

ParallaxBarrierApp::ParallaxBarrierApp(): parallaxBarrier(NULL)
{
}

ParallaxBarrierApp::~ParallaxBarrierApp()
{
	delete parallaxBarrier;
}

//--------------------------------------------------------------
void ParallaxBarrierApp::setup()
{	
	//ParallaxBarrier app setup
	setupApp();

	if (parallaxBarrier != NULL)
	{
		glGenTextures(1, &frameBufferDepthTexture);
		glBindTexture(GL_TEXTURE_2D, frameBufferDepthTexture);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, parallaxBarrier->getScreenResolutionWidth(), parallaxBarrier->getScreenResolutionHeight(), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
		glBindTexture(GL_TEXTURE_2D, 0);

		// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
		glGenFramebuffers(1, &frameBufferObject);
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

		glFramebufferTexture2D(GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, frameBufferDepthTexture, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, parallaxBarrier->getScreenLeftImage().getTextureReference().getTextureData().textureID, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, parallaxBarrier->getScreenRightImage().getTextureReference().getTextureData().textureID, 0);
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//ofBackground(0,0,0);

	//Barrier window is displayed in second monitor if there is one available
	ofxDisplayList displays = ofxDisplayManager::get()->getDisplays();
	ofxDisplay* disp = displays[0];
	if(displays.size() > 1)
		disp = displays[1];
	ofxFensterManager::get()->setActiveDisplay(disp);

	//Create barrier window
	BarrierWindow *barrierWindowListener = new BarrierWindow();
	barrierWindow = ofxFensterManager::get()->createFenster(400, 0, 400, 400, OF_WINDOW);
	barrierWindow->addListener(barrierWindowListener);
	barrierWindow->setWindowTitle("Barrier");
	barrierWindow->setBackgroundColor(0, 0, 0);

	barrierWindowListener->parallaxBarrierApp = this;

	if (parallaxBarrier != NULL)
	{
		barrierWindowListener->barrierImage = &(parallaxBarrier->getBarrierImage());
	}		

	invertBarrier = false;
	invertCounter = 0;
	invertLimit = 0;
}

//--------------------------------------------------------------
void ParallaxBarrierApp::initializeParallaxBarrier(float width, float height, int screenResolutionWidth, int screenResolutionHeight, int barrierResolutionWidth, int barrierResolutionHeight, float spacing, ofVec3f const &position, ofVec3f const &viewDirection, ofVec3f const &upDirection, int screenOffsetX, int screenOffsetY)
{
	this->screenOffsetX = screenOffsetX;
	this->screenOffsetY = screenOffsetY;
	parallaxBarrier = new ParallaxBarrier(width, height, screenResolutionWidth, screenResolutionHeight, barrierResolutionWidth, barrierResolutionHeight, spacing, position, viewDirection, upDirection);
}

//--------------------------------------------------------------
void ParallaxBarrierApp::draw()
{
	if (parallaxBarrier != NULL)
	{
		//draw left image and load into left texture
		glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);
		glDrawBuffer(GL_COLOR_ATTACHMENT0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ofPushMatrix();
		if (ofGetWindowHeight() > parallaxBarrier->getScreenResolutionHeight())
		{
			ofTranslate(0, ofGetWindowHeight() - parallaxBarrier->getScreenResolutionHeight());
		}
		
		ofPushView();
		drawLeft();
		ofPopView();

		//draw right image and load into right texture
		glDrawBuffer(GL_COLOR_ATTACHMENT1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ofPushView();
		drawRight();
		ofPopView();

		ofPopMatrix();

		//disable fbo and use screen
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		ofSetColor(ofColor::white);

		//update parallax barrier
		parallaxBarrier->update(leftEyePosition, rightEyePosition, invertBarrier);

		if (invertLimit > 0 && invertCounter > invertLimit)
		{
			invertCounter = 0;
			invertBarrier = !invertBarrier;
		}
		invertCounter++;

		//draw screen texture
		ofPushMatrix();
		ofScale(1,-1);
		if (ofGetWindowHeight() > parallaxBarrier->getScreenResolutionHeight())
		{
			ofTranslate(0, -(ofGetWindowHeight() - (ofGetWindowHeight() - parallaxBarrier->getScreenResolutionHeight())));
		} 
		else 
		{
			ofTranslate(0, -ofGetWindowHeight());
		}

		ofDisableLighting();

		parallaxBarrier->getScreenImage().draw(screenOffsetX, -screenOffsetY);
		ofPopMatrix();

		
	}

	ofSetColor(255);
	ofDrawBitmapStringHighlight(getScreenMessage(), 10, 20);
}

string ParallaxBarrierApp::getScreenMessage()
{
	string msg = string("");
	msg += "\nfps: " + ofToString(ofGetFrameRate(), 2);
	//msg += "\ns: " + ofToString(parallaxBarrier->getSpacing(), 3);
	//msg += "\nox: " + ofToString(screenOffsetX, 3);
	return msg;
}

//--------------------------------------------------------------
void ParallaxBarrierApp::keyReleased(int key)
{
	if(key=='f')
		ofxFensterManager::get()->getPrimaryWindow()->toggleFullscreen();
}
