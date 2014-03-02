#pragma once

#include "ofVec3f.h"
#include "ofQuaternion.h"
#include "kinect/SkeletonTrackerHelper.h"

class SkeletonTracker
{
public:
	SkeletonTracker();
	~SkeletonTracker();

	bool initialize();
	void calibrateToWorldCoordinates(const ofVec3f& leftEyeWorldPosition, const ofVec3f& rightEyeWorldPosition);

	const ofVec3f& getLeftEyePosition() {return leftEyePosition;};
	const ofVec3f& getRightEyePosition() {return rightEyePosition;};
	const ofVec3f& getLeftHandPosition() {return leftHandPosition;};
	const ofVec3f& getRightHandPosition() {return rightHandPosition;};
	const bool& getLastTrackSucceeded() {return lastTrackSucceeded;};

private:
	/*
	* The "Face Tracker" helper class is generic. It will call back this function
	* after a face has been successfully tracked.
	*/
	static void SkeletonTrackerHelperCallBack(void *pVoid);
	void updateEyesAndHandsPosition();
	ofVec3f transformToWorldCoords(const ofVec3f& coord );

	ofVec3f leftEyePosition, rightEyePosition;
	ofVec3f leftEyePositionTracker, rightEyePositionTracker;
	ofVec3f leftHandPosition, rightHandPosition;
	ofVec3f leftHandPositionTracker, rightHandPositionTracker;
	bool lastTrackSucceeded;
	SkeletonTrackerHelper skHelper;

	// world coordinates transformation parameters
	ofQuaternion toWorldCoordsRotation;
	ofVec3f toWorldCoordsTraslation;
};

