#pragma once

#include <Windows.h>
#include "MS_NUI_Def.h"
#include "MS_NUI_Kinect_SkeletonEngine.h"
#include "ofxMSKinectSkeleton.h"
//#include <winbase.h>
#include "ofThread.h" 

typedef struct
{
	int r;
	int g;
	int b;
	int a;
} MSKinectColor;

const int depthBytesPerPixel = 2;
const int colorBytesPerPixel = 4;


class MS_NUI_Kinect_Interface: public ofThread
{
public:
	MS_NUI_Kinect_Interface(int deviceindex, bool  bMultidevice);
	~MS_NUI_Kinect_Interface(void);
	
	static int GetDeviceCount(); 

	void DepthEvent(void);
	void VideoEvent(void);
	void SkeletonEvent(void);

	bool Initialize();
	bool StartSkeletonTracking(void) ;
	bool StartVideoRecording(int resolution ) ;
	bool StartDepthRecording(int resolution) ;

	void SetCameraElevationAngle(long angle);
	long GetCameraElevationAngle(void);

	bool IsConnected(void) {return m_isConnected ; } ;
	unsigned char * GetVidBuffer() ;
	unsigned char *GetDepthBuffer();
	int GetVidHeight(){return vidHeight ;};
	int GetVidWidth() {return vidWidth  ;};
	int GetVidChannels() {return colorBytesPerPixel ;};
	
	int GetDepthWidth() {return depthWidth ; } ;
	int GetDepthHeight(){return depthHeight ; } ;
	int GetDepthChannels() {return depthBytesPerPixel ;};
	
	void  SetDefaultSmoothing(); 
	void  SetTransformSmoothParamaters(NUI_TRANSFORM_SMOOTH_PARAMETERS *inptr ) ;
	void  GetTransformSmoothParamaters(NUI_TRANSFORM_SMOOTH_PARAMETERS *inptr ) ;

	float GetSmoothing() { return m_smooth.fSmoothing ; };
	void  SetSmoothing(float f);
	float GetCorrection() { return m_smooth.fCorrection; };
	void  SetCorrection(float f) ;
	float GetJitterRadius() { return m_smooth.fJitterRadius ;};
	void  SetJitterRadius(float f) { m_smooth.fJitterRadius = f ; };
	float GetMaxDeviationRadius() { return m_smooth.fMaxDeviationRadius ; };
	void  SetMaxDeviationsRadius(float f) {m_smooth.fMaxDeviationRadius = f ;};
	float GetPrediction() { return m_smooth.fPrediction ; };
	void  SetPredition(float f) {m_smooth.fPrediction = f ;};

	
	int NumberSkeletonsTracked(void);
	bool HasSeletions();
	bool GetSkeletonByTrackId(long id, ofxMSKinectSkeleton *ptr_in );
//	bool GetSkeleton(ofxMSKinectSkeleton *ptr_in);
	bool GetSkeleton(int index, ofxMSKinectSkeleton *ptr_in);
	
	void GetAllTrackedSkeletons(vector<ofxMSKinectSkeleton> *skeletons);
	void GetAllPositionOnlySkeletons(vector<ofxMSKinectSkeleton> *skeletons);
	void GetSkeletons(vector<ofxMSKinectSkeleton> *tracked_skeletons,	vector<ofxMSKinectSkeleton> *positiononly_skeletons);

	void static TransFormToDepth(float x, float y, float z, float *fx, float *fy);
	void static TransFormToDepth(ofVec3f vec3, float *fx, float *fy) ;

	MSKinectColor	getColorAt(int x, int y) ;
   	void Close() ;
	char* deviceName;

	MS_NUI_Kinect_SkeletonEngine m_skelEngine;
	void getColorPixelPositionFromDepthPixel(long depthX, long depthY, long* colorX, long* colorY, short depthVal);

	int VideoResolution;
	int DepthResolution;
private:

	int dev_index;

	void getProps();
	void StartEventThread();
	
	NUI_IMAGE_RESOLUTION GetImageRes(int i );
	void ImageResolutionToSize( NUI_IMAGE_RESOLUTION res ,int *vidWidth, int *vidHeight );
	RGBQUAD ShortToQuad_Depth( USHORT s ) ;
	void SaveSkeleton( NUI_SKELETON_DATA * pSkel, int i );
	
	virtual void threadedFunction();

	HANDLE        m_DepthFrameEvent;
    HANDLE        m_VideoFrameEvent;
    HANDLE        m_SkeletonEvent;
	HANDLE        m_pDepthStreamHandle;
    HANDLE        m_pVideoStreamHandle;

	bool		  m_isConnected ;
	bool          m_IsSkelTrackingOn;
	bool		  m_IsVideoOn;
	bool          m_IsDepthOn;

	unsigned char *vidimage_buff;
	unsigned char *vidimage_backbuff;

	unsigned char *depth_buff_raw;
	unsigned char *depth_backbuff_raw;


	int    vidWidth ;
    int    vidHeight ;
	int    vidChannels ;
	int    depthWidth ;
    int    depthHeight ;
	int    depthChannels;

	bool   m_VidRefresh;
	bool   m_DepthRefresh;
	bool   m_SkelRefresh;

	ofxINuiInstance *ppInstance;
	bool bMultidevice;


	NUI_TRANSFORM_SMOOTH_PARAMETERS m_smooth;
	

};

