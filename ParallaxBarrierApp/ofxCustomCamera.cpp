#include "ofxCustomCamera.h"


ofxCustomCamera::ofxCustomCamera(void): aspectRatio(0)
{
}

void ofxCustomCamera::setAspectRatio(float aspectRatio)
{
	this->aspectRatio = aspectRatio;
}

//----------------------------------------
void ofxCustomCamera::begin(ofRectangle viewport) {

	if(!isActive) ofPushView();
	isActive = true;

	ofSetCoordHandedness(OF_RIGHT_HANDED);

	// autocalculate near/far clip planes if not set by user
	calcClipPlanes(viewport);

	glMatrixMode(GL_PROJECTION);
	ofLoadIdentityMatrix();
	
	if(isOrtho) {
		//			if(vFlip) glOrtho(0, width, height, 0, nearDist, farDist);
		//			else
#ifndef TARGET_OPENGLES
		glOrtho(0, viewport.width, 0, viewport.height, nearClip, farClip);
#else
		ofMatrix4x4 ortho;
		ortho.makeOrthoMatrix(0, viewport.width, 0, viewport.height, nearClip, farClip);
		ofLoadMatrix( ortho );
#endif
	} else {
		
		ofMatrix4x4 persp;
		float aspectRatioPerspective;

		if(aspectRatio == 0)
		{
			aspectRatioPerspective = viewport.width/viewport.height;
		} else
		{
			aspectRatioPerspective = aspectRatio;
		}

		persp.makePerspectiveMatrix( fov, aspectRatioPerspective, nearClip, farClip );
		ofLoadMatrix( persp );
		//gluPerspective(fov, viewport.width/viewport.height, nearClip, farClip);
	}

	glMatrixMode(GL_MODELVIEW);
	ofLoadMatrix( ofMatrix4x4::getInverseOf(getGlobalTransformMatrix()) );
	ofViewport(viewport);
}
