#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetVerticalSync(false);
	showScreenImage = false;

	traslation.x = 300;
	traslation.y = 400;
	
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
	glEnable(GL_DEPTH_TEST);

	spacing = 2;
	parallaxBarrier = new ParallaxBarrier(30, 16.875, 1366, 768, 1366, 768, spacing, ofVec3f(0, 0, 0), ofVec3f(0,0,-1), ofVec3f(0,1,0));

	leftView = &parallaxBarrier->getScreenLeftImage();
	rightView = &parallaxBarrier->getScreenRightImage();

	for (int i = 0; i < parallaxBarrier->getScreenImage().width; i++) {
		for (int j = 0; j < parallaxBarrier->getScreenImage().height; j++) {
			leftView->setColor(i,j,ofColor::green);
			rightView->setColor(i,j,ofColor::red);
		}
	}
	leftView->update();
	rightView->update();

	leftEye.x = 30;
	leftEye.y = 0;
	leftEye.z = -80;
	rightEye.x = -30;
	rightEye.y = 0;
	rightEye.z = -80;

	scale = 6.0f;

	glGenFramebuffers(1, &screenFrameBufferId);
	glGenFramebuffers(1, &barrierFrameBufferId);
	screenFrameBufferPixels = new GLubyte [parallaxBarrier->getScreenResolutionWidth()*4];
	barrierFrameBufferPixels = new GLubyte [parallaxBarrier->getBarrierResolutionWidth()*4];
	screenTextureId = parallaxBarrier->getScreenImage().getTextureReference().texData.textureID;
	barrierTextureId = parallaxBarrier->getBarrierImage().getTextureReference().texData.textureID;

	invertBarrier = false;
}

//--------------------------------------------------------------
void testApp::exit()
{
	delete parallaxBarrier;
	delete[] screenFrameBufferPixels;
	delete[] barrierFrameBufferPixels;
}

//--------------------------------------------------------------
void testApp::update(){
	if (updateSpacing)
	{
		updateSpacing = false;
		parallaxBarrier->setSpacing(spacing);
	}
	parallaxBarrier->update(leftEye, rightEye, invertBarrier);
}

//--------------------------------------------------------------
void testApp::draw(){
	
	ofBackground(0);
	
	ofPushMatrix();

	ofImage &backImage = parallaxBarrier->getScreenImage();
	ofImage &frontImage = parallaxBarrier->getBarrierImage();

	if (showScreenImage) 
	{
		backImage.draw(0.f,0.f);
	}
	else
	{
		//transformation for parallax barrier coordinates
		ofTranslate(traslation);
		ofScale(scale,scale);

		drawParallaxBarrier();
	}
	
	ofPopMatrix();

	ofSetColor(255);

	std::stringstream sstm;
	sstm << ofGetWindowWidth() << endl;
	sstm << "spacing: " << spacing << endl;
	sstm << "left eye(" << leftEye.x << "," << leftEye.y << "," << leftEye.z << ")" <<  "  ";
	sstm << "right eye(" << rightEye.x << "," << rightEye.y << "," << rightEye.z << ")" << endl;
	sstm << "fps: " << ofToString(ofGetFrameRate(), 2);
	string msg  = sstm.str();
	ofDrawBitmapStringHighlight(msg, 10, 20);
}

void testApp::drawParallaxBarrier(){
	float pixelWidthScreen = parallaxBarrier->getWidth()/parallaxBarrier->getScreenResolutionWidth();
	float pixelWidthBarrier = parallaxBarrier->getWidth()/parallaxBarrier->getBarrierResolutionWidth();
	ofVec2f center(parallaxBarrier->getPosition().x, parallaxBarrier->getPosition().z);
	float spacing = parallaxBarrier->getSpacing();
	ofVec2f viewDirection(parallaxBarrier->getViewDirection().x, parallaxBarrier->getViewDirection().z);
	ofVec3f pixelDirectionScreen3d = parallaxBarrier->getViewDirection().getRotated(90.f, ofVec3f(0,1,0)).getScaled(pixelWidthScreen);
	ofVec2f pixelDirectionScreen(pixelDirectionScreen3d.x, pixelDirectionScreen3d.z);
	ofVec3f pixelDirectionBarrier3d = parallaxBarrier->getViewDirection().getRotated(90.f, ofVec3f(0,1,0)).getScaled(pixelWidthBarrier);
	ofVec2f pixelDirectionBarrier(pixelDirectionBarrier3d.x, pixelDirectionBarrier3d.z);
	
	ofVec2f startPixelScreen = center - pixelDirectionScreen*(((float)parallaxBarrier->getScreenResolutionWidth())/2.f);
	ofVec2f endPixelScreen = center + pixelDirectionScreen*(((float)parallaxBarrier->getScreenResolutionWidth())/2.f);

	//draw screen line
	ofSetColor(0,0,255);
	ofLine(startPixelScreen.x, startPixelScreen.y, -0.1, endPixelScreen.x, endPixelScreen.y, -0.1);

	//draw eye positions
	ofSetColor(0,255,0);
	ofCircle(leftEye.x, leftEye.z, 2.f/scale);
	ofSetColor(255,0,0);
	ofCircle(rightEye.x, rightEye.z, 2.f/scale);

	//draw screen images
	const ofImage& backImage = parallaxBarrier->getScreenImage();

	//Attach 2D texture to this FBO
	glBindFramebuffer(GL_FRAMEBUFFER, screenFrameBufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTextureId, 0);
	// read back the pixels
	glReadPixels(0, 0, backImage.width, 1, GL_RGBA, GL_UNSIGNED_BYTE, screenFrameBufferPixels);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ofVec2f actualPixelStartPosition = startPixelScreen;
	ofVec2f actualPixelEndPosition = startPixelScreen + pixelDirectionScreen;
	ofVec2f actualPixelMidPosition = (actualPixelEndPosition - actualPixelStartPosition) * 0.5 + actualPixelStartPosition;
	for (int i = 0; i < backImage.width; i++)
	{
		//draw screen line
		ofSetColor(screenFrameBufferPixels[i*4], screenFrameBufferPixels[i*4+1], screenFrameBufferPixels[i*4+2], screenFrameBufferPixels[i*4+3]);
		ofLine(actualPixelStartPosition.x, actualPixelStartPosition.y, 0, actualPixelEndPosition.x, actualPixelEndPosition.y, 0);

		if (screenFrameBufferPixels[i*4+1] == 255)
		{
			ofLine(actualPixelMidPosition.x, actualPixelMidPosition.y, 0, leftEye.x, leftEye.z, 0);
		} else 
		{
			ofLine(actualPixelMidPosition.x, actualPixelMidPosition.y, 0, rightEye.x, rightEye.z, 0);
		}

		actualPixelStartPosition = actualPixelEndPosition;
		actualPixelEndPosition = actualPixelEndPosition + pixelDirectionScreen;
		actualPixelMidPosition = (actualPixelEndPosition - actualPixelStartPosition) * 0.5 + actualPixelStartPosition;
	}

	ofVec2f shutterStartPixel = startPixelScreen + viewDirection.getScaled(spacing);
	ofVec2f shutterEndPixel = endPixelScreen + viewDirection.getScaled(spacing);

	glDisable(GL_DEPTH_TEST);
	
	//draw shutter images
	const ofImage& frontImage = parallaxBarrier->getBarrierImage();

	//Attach 2D texture to this FBO
	glBindFramebuffer(GL_FRAMEBUFFER, barrierFrameBufferId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, barrierTextureId, 0);
	// read back the pixels
	glReadPixels(0, 0, frontImage.width, 1, GL_RGBA, GL_UNSIGNED_BYTE, barrierFrameBufferPixels);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	actualPixelStartPosition = shutterStartPixel;
	actualPixelEndPosition = shutterStartPixel + pixelDirectionBarrier;
	for (int i = 0; i < frontImage.width; i++)
	{
		//draw screen line
		ofSetColor(barrierFrameBufferPixels[i*4], barrierFrameBufferPixels[i*4+1], barrierFrameBufferPixels[i*4+2], barrierFrameBufferPixels[i*4+3]);
		if (barrierFrameBufferPixels[i*4] != 0 )
		{
			ofLine(actualPixelStartPosition.x, actualPixelStartPosition.y, 0, actualPixelEndPosition.x, actualPixelEndPosition.y, 0);
		}

		actualPixelStartPosition = actualPixelEndPosition;
		actualPixelEndPosition = actualPixelEndPosition + pixelDirectionBarrier;
	}

	glEnable(GL_DEPTH_TEST);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	float newX, newY;
	switch(key) {
		case 'w':
			leftEye.z = leftEye.z - 0.1; 
			break;
		case 'a':
			leftEye.x = leftEye.x - 0.1;
			break;
		case 's':
			leftEye.z = leftEye.z + 0.1;
			break;
		case 'd':
			leftEye.x = leftEye.x + 0.1;
			break;

		case 'u':
			rightEye.z = rightEye.z - 0.1; 
			break;
		case 'h':
			rightEye.x = rightEye.x - 0.1;
			break;
		case 'j':
			rightEye.z = rightEye.z + 0.1;
			break;
		case 'k':
			rightEye.x = rightEye.x + 0.1;
			break;

		case 't':
			newX = ( (((float)ofGetWindowWidth())*0.5f - traslation.x)/scale ) - ( (((float)ofGetWindowWidth())*0.5f - traslation.x)/(scale+0.1) );
			newY = ( (((float)ofGetWindowHeight())*0.5f - traslation.y)/scale ) - ( (((float)ofGetWindowHeight())*0.5f - traslation.y)/(scale+0.1) );
			traslation.x = traslation.x - newX*scale;
			traslation.y = traslation.y - newY*scale;

			scale = scale + 0.1;
			break;
		case 'g':
			newX = ( (((float)ofGetWindowWidth())*0.5f - traslation.x)/scale ) - ( (((float)ofGetWindowWidth())*0.5f - traslation.x)/(scale-0.1) );
			newY = ( (((float)ofGetWindowHeight())*0.5f - traslation.y)/scale ) - ( (((float)ofGetWindowHeight())*0.5f - traslation.y)/(scale-0.1) );
			traslation.x = traslation.x - newX*scale;
			traslation.y = traslation.y - newY*scale;

			scale = scale - 0.1;
			break;
		case 'z':
			spacing += 0.1;
			updateSpacing = true;
			break;
		case 'x':
			spacing -= 0.1;
			updateSpacing = true;
			break;
			
		case 'F':
		case 'f':
			ofToggleFullscreen();
			break;
		case 'C':
		case 'c':
			showScreenImage = !showScreenImage;
			break;

		case 356: // arrow left
			traslation.x = traslation.x + 1*scale;
			break;
			
		case 358: // arrow right
			traslation.x = traslation.x - 1*scale;
			break;
			
		case 357: // arrow up
			traslation.y = traslation.y + 1*scale;
			break;
			
		case 359: // arrow down
			traslation.y = traslation.y - 1*scale;
			break;

		case 'i':
			invertBarrier = !invertBarrier;
			break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 
	
}
