#include "testAppModel.h"

//--------------------------------------------------------------
void testAppModel::setup(){
	ofSetVerticalSync(true);
	
	// this uses depth information for occlusion
	// rather than always drawing things on top of each other
	glEnable(GL_DEPTH_TEST);

	parallaxModel = new ParallaxBarrierModel();
	parallaxModel->setWidth(30);

	leftEye.x = 11.1;
	leftEye.y = 60;
	rightEye.x = 19;
	rightEye.y = 60;

	scale = 10.0f;
	traslation.x = (ofGetWindowWidth() - parallaxModel->getWidth()*scale)*0.5;
	traslation.y = 40;
}

//--------------------------------------------------------------
void testAppModel::update(){
	//std::cout << "1" << std::endl;
	bool success = parallaxModel->update(leftEye,rightEye, false);
	//std::cout << "2:" << parallaxModel->getShutterPoints().size() << std::endl;
	//std::cout << "3:" << parallaxModel->getScreenPoints().size() << std::endl;
	//std::cout << "4:" << success << std::endl;
}

//--------------------------------------------------------------
void testAppModel::draw(){
	
	ofBackground(0);
	
	ofPushMatrix();

	//transformation for parallax barrier model coordinates
	ofTranslate(traslation);
	ofScale(scale,scale);

	drawParallaxBarrierModel();
	
	ofPopMatrix();

	ofSetColor(255);
	string msg = string("");
	msg += "\n\nfps: " + ofToString(ofGetFrameRate(), 2);
	ofDrawBitmapStringHighlight(msg, 10, 20);

	std::stringstream sstm;
	sstm << ofGetWindowWidth();
	string msg2  = sstm.str();
	ofDrawBitmapStringHighlight(msg2, 10, 20);
}

void testAppModel::drawParallaxBarrierModel(){

	//draw screen line
	ofSetColor(0,0,255);
	ofLine(0,0,-0.1,parallaxModel->getWidth(),0,-0.1);

	//draw eye positions
	ofSetColor(0,255,0);
	ofCircle(leftEye.x, leftEye.y, 2.f/scale);
	ofSetColor(255,0,0);
	ofCircle(rightEye.x, rightEye.y, 2.f/scale);

	//draw screen images
	//const vector<float>& screenPoints = parallaxModel->getScreenPoints();
	//bool pair = true;
	//float previousPoint = -1;
	//float point = -1;
	//for (vector<float>::const_iterator it = screenPoints.begin(), end = screenPoints.end(); it != end; ++it)
	//{
	//	previousPoint = point;
	//	point = (*it);
	//	
	//	if (previousPoint != -1)
	//	{
	//		ofSetColor(pair?255:0,pair?0:255,0);
	//		ofLine(previousPoint,0,point,0);
	//		ofLine(previousPoint,0,pair?rightEye.x:leftEye.x,pair?rightEye.y:leftEye.y);
	//		ofLine(point,0,pair?rightEye.x:leftEye.x,pair?rightEye.y:leftEye.y);
	//	}

	//	pair = !pair;
	//}

	////draw shutter images
	//const vector<float>& shutterPoints = parallaxModel->getBarrierPoints();
	//pair = true;
	//previousPoint = -1;
	//point = -1;
	//for (vector<float>::const_iterator it = shutterPoints.begin(), end = shutterPoints.end(); it != end; ++it)
	//{
	//	previousPoint = point;
	//	point = (*it);
	//	
	//	if (!pair)
	//	{
	//		ofSetColor(255,255,0);
	//		ofLine(previousPoint,1,point,1);
	//	}

	//	pair = !pair;
	//}

}

//--------------------------------------------------------------
void testAppModel::keyPressed(int key){
	switch(key) {
		case 'w':
			leftEye.y = leftEye.y - 0.1; 
			break;
		case 'a':
			leftEye.x = leftEye.x - 0.1;
			break;
		case 's':
			leftEye.y = leftEye.y + 0.1;
			break;
		case 'd':
			leftEye.x = leftEye.x + 0.1;
			break;

		case 'u':
			rightEye.y = rightEye.y - 0.1; 
			break;
		case 'h':
			rightEye.x = rightEye.x - 0.1;
			break;
		case 'j':
			rightEye.y = rightEye.y + 0.1;
			break;
		case 'k':
			rightEye.x = rightEye.x + 0.1;
			break;

		case 't':
			scale = scale + 0.1;
			break;
		case 'g':
			scale = scale - 0.1;
			break;
			
		case 'F':
		case 'f':
			ofToggleFullscreen();
			break;

		case 356: // arrow left
			traslation.x = traslation.x + 0.1*scale;
			break;
			
		case 358: // arrow right
			traslation.x = traslation.x - 0.1*scale;
			break;
			
		case 357: // arrow up
			traslation.y = traslation.y + 0.1*scale;
			break;
			
		case 359: // arrow down
			traslation.y = traslation.y - 0.1*scale;
			break;
	}
}

//--------------------------------------------------------------
void testAppModel::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testAppModel::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testAppModel::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testAppModel::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testAppModel::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testAppModel::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testAppModel::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testAppModel::dragEvent(ofDragInfo dragInfo){ 
	
}
