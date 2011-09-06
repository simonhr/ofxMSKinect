#pragma once

#include <Windows.h>
#include "MS_NUI_Def.h"
#include "ofxMSKinectSkeleton.h"
#include "ofMain.h"

#define CONVERT_V4TOV3(v3,v4) v3.x = v4.x; \
			v3.y = v4.y; \
			v3.z = v4.z;


typedef struct
{
    Vector4              vFloorClipPlane;
    Vector4              vNormalToGravity;
	long frame;
	LARGE_INTEGER time;
	long flags;
	NUI_SKELETON_DATA SkeletonDataBack[NUI_SKELETON_COUNT];
	bool bIsChanged[NUI_SKELETON_COUNT];
} SkeletonCollection;



class MS_NUI_Kinect_SkeletonEngine
{
public:

	MS_NUI_Kinect_SkeletonEngine(void);
	~MS_NUI_Kinect_SkeletonEngine(void);

	bool Initialize(void);
   	void Close(void) ;
	void LostSkeleton(void);
	int  NumberSkeletonsTracked(void);
	bool HasSeletions();
	bool GetSkeletonByTrackId(long id, ofxMSKinectSkeleton *ptr_in);
	bool GetSkeleton( int index, ofxMSKinectSkeleton *ptr_in);
	
	void Update( long flags, long frame, LARGE_INTEGER time, Vector4  vFloorClipPlane, Vector4 vNormalToGravity, NUI_SKELETON_DATA *pdata, int index) ;
	void GetAllTrackedSkeletons(vector<ofxMSKinectSkeleton> *skeletons);
	void GetAllPositionOnlySkeletons(vector<ofxMSKinectSkeleton> *skeletons);
	void ClearTrackingFlags(); 

private:

	void CopyData( ofxMSKinectSkeleton *ptr_in, NUI_SKELETON_DATA *pdata );
	void GetSkeletons(vector<ofxMSKinectSkeleton> *skeletons, NUI_SKELETON_TRACKING_STATE track_state);
	bool m_bHasSkels ;
    SkeletonCollection m_skls ;
};
