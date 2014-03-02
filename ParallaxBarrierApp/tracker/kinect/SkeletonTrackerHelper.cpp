//------------------------------------------------------------------------------
// <copyright file="FaceTrackerHelper.cpp" company="Microsoft">
//     Copyright (c) Microsoft Corporation.  All rights reserved.
// </copyright>
//------------------------------------------------------------------------------

#include "SkeletonTrackerHelper.h"
#include <iostream>

#ifdef SAMPLE_OPTIONS
#include "Options.h"
#else
PVOID _opt = NULL;
#endif

using namespace std;

const int LEFT_UP_EYE_INDEX = 71;
const int LEFT_DOWN_EYE_INDEX = 72;
const int RIGHT_UP_EYE_INDEX = 73;
const int RIGHT_DOWN_EYE_INDEX = 74;

SkeletonTrackerHelper::SkeletonTrackerHelper()
{
    m_pFaceTracker = 0;
    m_pFTResult = NULL;
    m_colorImage = NULL;
    m_depthImage = NULL;
    m_ApplicationIsRunning = false;
    m_LastTrackSucceeded = false;
    m_CallBack = NULL;
    m_XCenterFace = 0;
    m_YCenterFace = 0;
    m_hFaceTrackingThread = NULL;
    m_depthType = NUI_IMAGE_TYPE_DEPTH;
    m_depthRes = NUI_IMAGE_RESOLUTION_INVALID;
    m_bNearMode = FALSE;
    m_bFallbackToDefault = FALSE;
    m_colorType = NUI_IMAGE_TYPE_COLOR;
    m_colorRes = NUI_IMAGE_RESOLUTION_INVALID;
	m_leftEyePosition.x = 0;
	m_leftEyePosition.y = 0;
	m_leftEyePosition.z = 0;
	m_rightEyePosition.x = 0;
	m_rightEyePosition.y = 0;
	m_rightEyePosition.z = 0;
}

SkeletonTrackerHelper::~SkeletonTrackerHelper()
{
    Stop();
}

bool SkeletonTrackerHelper::Init(SkeletonTrackerHelperCallBack callBack, PVOID callBackParam, 
                       NUI_IMAGE_TYPE depthType, NUI_IMAGE_RESOLUTION depthRes, bool bNearMode, bool bFallbackToDefault, NUI_IMAGE_TYPE colorType, NUI_IMAGE_RESOLUTION colorRes, bool bSeatedSkeletonMode)
{
    if (!callBack)
    {
        return false;
    }
    m_CallBack = callBack;
    m_CallBackParam = callBackParam;
    m_ApplicationIsRunning = true;
    m_depthType = depthType;
    m_depthRes = depthRes;
    m_bNearMode = bNearMode;
    m_bFallbackToDefault = bFallbackToDefault;
    m_bSeatedSkeletonMode = bSeatedSkeletonMode;
    m_colorType = colorType;
    m_colorRes = colorRes;
    m_hFaceTrackingThread = CreateThread(NULL, 0, SkeletonTrackingStaticThread, (PVOID)this, 0, 0);
    return true;
}

bool SkeletonTrackerHelper::Stop()
{
    m_ApplicationIsRunning = false;
    if (m_hFaceTrackingThread)
    {
        WaitForSingleObject(m_hFaceTrackingThread, 1000);
    }
    m_hFaceTrackingThread = 0;
    return true;
}

bool SkeletonTrackerHelper::SubmitFaceTrackingResult(IFTResult* pResult)
{
    if (pResult != NULL && SUCCEEDED(pResult->GetStatus()))
    {
        FLOAT* pSU = NULL;
        UINT numSU;
        BOOL suConverged;
        m_pFaceTracker->GetShapeUnits(NULL, &pSU, &numSU, &suConverged);
        POINT viewOffset = {0, 0};
        FT_CAMERA_CONFIG cameraConfig;
        if (m_KinectSensorPresent)
        {
            m_KinectSensor.GetVideoConfiguration(&cameraConfig);
        }
        else
        {
            cameraConfig.Width = 640;
            cameraConfig.Height = 480;
            cameraConfig.FocalLength = 500.0f;
        }
        IFTModel* ftModel;
        HRESULT hr = m_pFaceTracker->GetFaceModel(&ftModel);
        if (SUCCEEDED(hr))
        {
			UINT vertexCount = ftModel->GetVertexCount();
			FT_VECTOR3D* points3D = reinterpret_cast<FT_VECTOR3D*> (_malloca(sizeof(FT_VECTOR3D) * vertexCount));

			FLOAT* pAU = NULL;
			UINT numAU;
			pResult->GetAUCoefficients(&pAU, &numAU);
			//pApp->m_eggavatar.SetCandideAU(pAU, numAU);
			FLOAT scale;
			FLOAT rotationXYZ[3];
			FLOAT translationXYZ[3];
			pResult->Get3DPose(&scale, rotationXYZ, translationXYZ);

			hr = ftModel->Get3DShape(pSU,ftModel->GetSUCount(), pAU, numAU, 1.0f, rotationXYZ, translationXYZ, points3D, vertexCount);

			m_leftEyePosition.x = (points3D[LEFT_UP_EYE_INDEX].x + points3D[LEFT_DOWN_EYE_INDEX].x) * 0.5;
			m_leftEyePosition.y = (points3D[LEFT_UP_EYE_INDEX].y + points3D[LEFT_DOWN_EYE_INDEX].y) * 0.5;
			m_leftEyePosition.z = (points3D[LEFT_UP_EYE_INDEX].z + points3D[LEFT_DOWN_EYE_INDEX].z) * 0.5;

			m_rightEyePosition.x = (points3D[RIGHT_UP_EYE_INDEX].x + points3D[RIGHT_DOWN_EYE_INDEX].x) * 0.5;
			m_rightEyePosition.y = (points3D[RIGHT_UP_EYE_INDEX].y + points3D[RIGHT_DOWN_EYE_INDEX].y) * 0.5;
			m_rightEyePosition.z = (points3D[RIGHT_UP_EYE_INDEX].z + points3D[RIGHT_DOWN_EYE_INDEX].z) * 0.5;

			ftModel->Release();
			_freea(points3D);
        }

        if (m_CallBack && SUCCEEDED(hr))
        {
            (*m_CallBack)(m_CallBackParam);
        }
    }
    return true;
}

bool SkeletonTrackerHelper::SubmitFailedTrackingResult(IFTResult* pResult)
{
	if (m_CallBack)
    {
        (*m_CallBack)(m_CallBackParam);
    }
	return true;
}

// We compute here the nominal "center of attention" that is used when zooming the presented image.
void SkeletonTrackerHelper::SetCenterOfImage(IFTResult* pResult)
{
    float centerX = ((float)m_colorImage->GetWidth())/2.0f;
    float centerY = ((float)m_colorImage->GetHeight())/2.0f;
    if (pResult)
    {
        if (SUCCEEDED(pResult->GetStatus()))
        {
            RECT faceRect;
            pResult->GetFaceRect(&faceRect);
            centerX = (faceRect.left+faceRect.right)/2.0f;
            centerY = (faceRect.top+faceRect.bottom)/2.0f;
        }
        m_XCenterFace += 0.02f*(centerX-m_XCenterFace);
        m_YCenterFace += 0.02f*(centerY-m_YCenterFace);
    }
    else
    {
        m_XCenterFace = centerX;
        m_YCenterFace = centerY;
    }
}

// Get a video image and process it.
void SkeletonTrackerHelper::CheckCameraInput()
{
    HRESULT hrFT = E_FAIL;

    if (m_KinectSensorPresent && m_KinectSensor.GetVideoBuffer())
    {
        HRESULT hrCopy = m_KinectSensor.GetVideoBuffer()->CopyTo(m_colorImage, NULL, 0, 0);
        if (SUCCEEDED(hrCopy) && m_KinectSensor.GetDepthBuffer())
        {
            hrCopy = m_KinectSensor.GetDepthBuffer()->CopyTo(m_depthImage, NULL, 0, 0);
        }
        // Do face tracking
        if (SUCCEEDED(hrCopy))
        {
            FT_SENSOR_DATA sensorData(m_colorImage, m_depthImage, m_KinectSensor.GetZoomFactor(), m_KinectSensor.GetViewOffSet());

            FT_VECTOR3D* hint = NULL;
            if (SUCCEEDED(m_KinectSensor.GetClosestHint(m_hint3D)))
            {
                hint = m_hint3D;
            }
            if (m_LastTrackSucceeded)
            {
                hrFT = m_pFaceTracker->ContinueTracking(&sensorData, hint, m_pFTResult);
            }
            else
            {
                hrFT = m_pFaceTracker->StartTracking(&sensorData, NULL, hint, m_pFTResult);
            }
        }
    }

    m_LastTrackSucceeded = SUCCEEDED(hrFT) && SUCCEEDED(m_pFTResult->GetStatus());
    if (m_LastTrackSucceeded)
    {
		m_leftHandPosition = m_hint3D[2];
		m_rightHandPosition = m_hint3D[3];
        SubmitFaceTrackingResult(m_pFTResult);
    }
    else
    {
		SubmitFailedTrackingResult(m_pFTResult);
        m_pFTResult->Reset();
    }
    SetCenterOfImage(m_pFTResult);
}

DWORD WINAPI SkeletonTrackerHelper::SkeletonTrackingStaticThread(PVOID lpParam)
{
    SkeletonTrackerHelper* context = static_cast<SkeletonTrackerHelper*>(lpParam);
    if (context)
    {
        return context->SkeletonTrackingThread();
    }
    return 0;
}

DWORD WINAPI SkeletonTrackerHelper::SkeletonTrackingThread()
{
    FT_CAMERA_CONFIG videoConfig;
    FT_CAMERA_CONFIG depthConfig;
    FT_CAMERA_CONFIG* pDepthConfig = NULL;

    // Try to get the Kinect camera to work
    HRESULT hr = m_KinectSensor.Init(m_depthType, m_depthRes, m_bNearMode, m_bFallbackToDefault, m_colorType, m_colorRes, m_bSeatedSkeletonMode);
    if (SUCCEEDED(hr))
    {
        m_KinectSensorPresent = TRUE;
        m_KinectSensor.GetVideoConfiguration(&videoConfig);
        m_KinectSensor.GetDepthConfiguration(&depthConfig);
        pDepthConfig = &depthConfig;
        m_hint3D[0] = m_hint3D[1] = m_hint3D[2] = m_hint3D[3] = FT_VECTOR3D(0, 0, 0);
    }
    else
    {
        m_KinectSensorPresent = FALSE;
        WCHAR errorText[MAX_PATH];
        ZeroMemory(errorText, sizeof(WCHAR) * MAX_PATH);
        cout << "Could not initialize the Kinect sensor. hr= " << hr << endl;
        cout << "Face Tracker Initialization Error" << endl;
        return 1;
    }

    // Try to start the face tracker.
    m_pFaceTracker = FTCreateFaceTracker(_opt);
    if (!m_pFaceTracker)
    {
		cout << "Could not create the face tracker.\n" << "Face Tracker Initialization Error"<< endl;
        return 2;
    }

    hr = m_pFaceTracker->Initialize(&videoConfig, pDepthConfig, NULL, NULL); 
    if (FAILED(hr))
    {
        WCHAR path[512], buffer[1024];
        GetCurrentDirectoryW(ARRAYSIZE(path), path);
        cout << "Could not initialize face tracker (" << path << "). hr=" << hr << endl;

       cout << "Face Tracker Initialization Error" << endl;

        return 3;
    }

    hr = m_pFaceTracker->CreateFTResult(&m_pFTResult);
    if (FAILED(hr) || !m_pFTResult)
    {
        cout << "Could not initialize the face tracker result.\n" <<"Face Tracker Initialization Error\n" << endl;
        return 4;
    }

    // Initialize the RGB image.
    m_colorImage = FTCreateImage();
    if (!m_colorImage || FAILED(hr = m_colorImage->Allocate(videoConfig.Width, videoConfig.Height, FTIMAGEFORMAT_UINT8_B8G8R8X8)))
    {
        return 5;
    }

    if (pDepthConfig)
    {
        m_depthImage = FTCreateImage();
        if (!m_depthImage || FAILED(hr = m_depthImage->Allocate(depthConfig.Width, depthConfig.Height, FTIMAGEFORMAT_UINT16_D13P3)))
        {
            return 6;
        }
    }

    SetCenterOfImage(NULL);
    m_LastTrackSucceeded = false;

    while (m_ApplicationIsRunning)
    {
        CheckCameraInput();
        Sleep(10);
    }

    m_pFaceTracker->Release();
    m_pFaceTracker = NULL;

    if(m_colorImage)
    {
        m_colorImage->Release();
        m_colorImage = NULL;
    }

    if(m_depthImage) 
    {
        m_depthImage->Release();
        m_depthImage = NULL;
    }

    if(m_pFTResult)
    {
        m_pFTResult->Release();
        m_pFTResult = NULL;
    }
    m_KinectSensor.Release();
    return 0;
}

bool SkeletonTrackerHelper::GetCameraConfig(FT_CAMERA_CONFIG* cameraConfig)
{
    return m_KinectSensorPresent ? (m_KinectSensor.GetVideoConfiguration(cameraConfig) == S_OK? true : false ): false;
}
