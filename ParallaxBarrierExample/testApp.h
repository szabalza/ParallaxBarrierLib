#pragma once

#include "ofMain.h"
#include "ParallaxBarrier.h"

class testApp : public ofBaseApp 
{
	public:
	
		void setup();
		void update();
		void draw();
		void exit();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		

private:

		bool bShowHelp;
		ParallaxBarrier* parallaxBarrier;
		ofVec3f leftEye, rightEye;
		ofImage *leftView, *rightView;
		float scale;
		ofVec3f traslation;
		float spacing;
		bool updateSpacing;
		bool showScreenImage;
		unsigned int screenFrameBufferId;
		unsigned int barrierFrameBufferId;
		unsigned int screenTextureId;
		unsigned int barrierTextureId;
		GLubyte *screenFrameBufferPixels;
		GLubyte *barrierFrameBufferPixels;
		bool invertBarrier;

		void drawParallaxBarrier();
};

