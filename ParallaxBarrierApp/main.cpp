#include "ofMain.h"
#include "testParallaxBarrierApp.h"
#include "ofxFensterManager.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){
	ofSetupOpenGL(ofxFensterManager::get(), 400, 400, OF_WINDOW);
	ofRunFensterApp(new testParallaxBarrierApp());
}