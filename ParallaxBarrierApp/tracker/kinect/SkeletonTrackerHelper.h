#pragma once
#include <FaceTrackLib.h>
#include "KinectSensor.h"

typedef void (*SkeletonTrackerHelperCallBack)(PVOID lpParam);

class SkeletonTrackerHelper
{
public:
	SkeletonTrackerHelper();
	~SkeletonTrackerHelper();

	bool Init(SkeletonTrackerHelperCallBack callBack, PVOID callBackParam, 
        NUI_IMAGE_TYPE depthType, NUI_IMAGE_RESOLUTION depthRes, bool bNearMode, bool bFallbackToDefault, NUI_IMAGE_TYPE colorType, NUI_IMAGE_RESOLUTION colorRes, bool bSeatedSkeletonMode);
    bool Stop();
    IFTResult* GetResult()      { return(m_pFTResult);}
    bool IsKinectPresent()      { return(m_KinectSensorPresent);}
    IFTImage* GetColorImage()   { return(m_colorImage);}
    float GetXCenterFace()      { return(m_XCenterFace);}
    float GetYCenterFace()      { return(m_YCenterFace);}
	FT_VECTOR3D GetLeftEyePosition()	{ return m_leftEyePosition;}
	FT_VECTOR3D GetRightEyePosition()	{ return m_rightEyePosition;}
	FT_VECTOR3D GetLeftHandPosition()	{ return m_leftHandPosition;}
	FT_VECTOR3D GetRightHandPosition()	{ return m_rightHandPosition;}
	bool GetLastTrackSucceeded()	{ return m_LastTrackSucceeded;}
    IFTFaceTracker* GetTracker() { return(m_pFaceTracker);}
    bool GetCameraConfig(FT_CAMERA_CONFIG* cameraConfig);

private:
    KinectSensor                m_KinectSensor;
    bool                        m_KinectSensorPresent;
    IFTFaceTracker*             m_pFaceTracker;
    IFTResult*                  m_pFTResult;
    IFTImage*                   m_colorImage;
    IFTImage*                   m_depthImage;
    FT_VECTOR3D                 m_hint3D[4];
    bool                        m_LastTrackSucceeded;
    bool                        m_ApplicationIsRunning;
    SkeletonTrackerHelperCallBack   m_CallBack;
    LPVOID                      m_CallBackParam;
    float                       m_XCenterFace;
    float                       m_YCenterFace;
    HANDLE                      m_hFaceTrackingThread;
    NUI_IMAGE_TYPE              m_depthType;
    NUI_IMAGE_RESOLUTION        m_depthRes;
    bool                        m_bNearMode;
    bool                        m_bFallbackToDefault;
    bool                        m_bSeatedSkeletonMode;
    NUI_IMAGE_TYPE              m_colorType;
    NUI_IMAGE_RESOLUTION        m_colorRes;
	FT_VECTOR3D					m_leftEyePosition;
	FT_VECTOR3D					m_rightEyePosition;
	FT_VECTOR3D					m_leftHandPosition;
	FT_VECTOR3D					m_rightHandPosition;

    bool SubmitFaceTrackingResult(IFTResult* pResult);
	bool SubmitFailedTrackingResult(IFTResult* pResult);
    void SetCenterOfImage(IFTResult* pResult);
    void CheckCameraInput();
    DWORD WINAPI SkeletonTrackingThread();
    static DWORD WINAPI SkeletonTrackingStaticThread(PVOID lpParam);
};

