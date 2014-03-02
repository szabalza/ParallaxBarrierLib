#include "SkeletonTracker.h"

SkeletonTracker::SkeletonTracker(void)
{
}


SkeletonTracker::~SkeletonTracker(void)
{
}

bool SkeletonTracker::initialize()
{
	NUI_IMAGE_TYPE m_depthType = NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX;
	NUI_IMAGE_RESOLUTION m_depthRes = NUI_IMAGE_RESOLUTION_320x240;
	bool nearMode = false;
	bool fallbackToDefault = true;
	NUI_IMAGE_TYPE m_colorType = NUI_IMAGE_TYPE_COLOR;
	NUI_IMAGE_RESOLUTION m_colorRes = NUI_IMAGE_RESOLUTION_640x480;
	bool m_bSeatedSkeletonMode = true;

	toWorldCoordsTraslation.x = 0;
	toWorldCoordsTraslation.y = 0;
	toWorldCoordsTraslation.z = 0;
	lastTrackSucceeded = false;

	return skHelper.Init(SkeletonTrackerHelperCallBack, this, m_depthType, m_depthRes, nearMode, fallbackToDefault, m_colorType, m_colorRes, m_bSeatedSkeletonMode);
}

void SkeletonTracker::calibrateToWorldCoordinates(const ofVec3f& leftEyeWorldPosition, const ofVec3f& rightEyeWorldPosition)
{
	ofVec3f faceTrackerEyeVector = rightEyePositionTracker - leftEyePositionTracker;
	ofVec3f worldEyeVector = rightEyeWorldPosition - leftEyeWorldPosition;

	toWorldCoordsRotation.makeRotate(faceTrackerEyeVector, worldEyeVector);

	ofVec3f leftEyeTrackerInWorld =  toWorldCoordsRotation * leftEyePositionTracker;
	ofVec3f rightEyeTrackerInWorld =  toWorldCoordsRotation * rightEyePositionTracker;
	ofVec3f worldFaceTrackerOriginLeft = leftEyeWorldPosition - leftEyeTrackerInWorld;
	ofVec3f worldFaceTrackerOriginRight = rightEyeWorldPosition - rightEyeTrackerInWorld;

	toWorldCoordsTraslation = (worldFaceTrackerOriginLeft + worldFaceTrackerOriginRight) * 0.5;

	leftEyePosition = transformToWorldCoords(leftEyePositionTracker);
	rightEyePosition = transformToWorldCoords(rightEyePositionTracker);
}

ofVec3f SkeletonTracker::transformToWorldCoords(const ofVec3f& coord )
{
	return (toWorldCoordsRotation * coord) + toWorldCoordsTraslation;
}

void SkeletonTracker::updateEyesAndHandsPosition()
{
	lastTrackSucceeded = skHelper.GetLastTrackSucceeded();

	if (lastTrackSucceeded)
	{
		FT_VECTOR3D leftEyePosTracker = skHelper.GetLeftEyePosition(); 
		FT_VECTOR3D rightEyePosTracker = skHelper.GetRightEyePosition(); 
		FT_VECTOR3D leftHandPosTracker = skHelper.GetLeftHandPosition(); 
		FT_VECTOR3D rightHandPosTracker = skHelper.GetRightHandPosition();

		leftEyePositionTracker.x = leftEyePosTracker.x;
		leftEyePositionTracker.y = leftEyePosTracker.y;
		leftEyePositionTracker.z = leftEyePosTracker.z;
		rightEyePositionTracker.x = rightEyePosTracker.x;
		rightEyePositionTracker.y = rightEyePosTracker.y;
		rightEyePositionTracker.z = rightEyePosTracker.z;

		leftHandPositionTracker.x = leftHandPosTracker.x;
		leftHandPositionTracker.y = leftHandPosTracker.y;
		leftHandPositionTracker.z = leftHandPosTracker.z;
		rightHandPositionTracker.x = rightHandPosTracker.x;
		rightHandPositionTracker.y = rightHandPosTracker.y;
		rightHandPositionTracker.z = rightHandPosTracker.z;

		leftEyePosition = transformToWorldCoords(leftEyePositionTracker);
		rightEyePosition = transformToWorldCoords(rightEyePositionTracker);

		leftHandPosition = transformToWorldCoords(leftHandPositionTracker);
		rightHandPosition = transformToWorldCoords(rightHandPositionTracker);
	}
}

void SkeletonTracker::SkeletonTrackerHelperCallBack(void *pVoid)
{
	SkeletonTracker *skeletonTracker = reinterpret_cast<SkeletonTracker*>(pVoid);
	skeletonTracker->updateEyesAndHandsPosition();
}