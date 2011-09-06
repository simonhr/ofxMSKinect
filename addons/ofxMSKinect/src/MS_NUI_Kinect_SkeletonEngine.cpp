
#include "MS_NUI_Kinect_SkeletonEngine.h"
#include "ofxMSKinectSkeleton.h"


MS_NUI_Kinect_SkeletonEngine::MS_NUI_Kinect_SkeletonEngine(void) 
{
	 m_bHasSkels = false; 
}


MS_NUI_Kinect_SkeletonEngine::~MS_NUI_Kinect_SkeletonEngine(void)
{

}


bool MS_NUI_Kinect_SkeletonEngine::Initialize(void)
{
  return true;
}


void  MS_NUI_Kinect_SkeletonEngine::Close(void) 
{

}

void MS_NUI_Kinect_SkeletonEngine::LostSkeleton(void) 
{
	m_bHasSkels = false ;
}

bool MS_NUI_Kinect_SkeletonEngine::HasSeletions()
{
	return m_bHasSkels ;
}

void  MS_NUI_Kinect_SkeletonEngine::ClearTrackingFlags() 
{
	for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
		  m_skls.SkeletonDataBack[i].eTrackingState = NUI_SKELETON_NOT_TRACKED ;
}


void  MS_NUI_Kinect_SkeletonEngine::Update( long flags, long frame,LARGE_INTEGER time, 
	Vector4  vFloorClipPlane, Vector4 vNormalToGravity, NUI_SKELETON_DATA *pdata, int index) 
{
	if( index < NUI_SKELETON_COUNT && index >= 0 )
	{
		m_skls.vFloorClipPlane = vFloorClipPlane;
		m_skls.vNormalToGravity = vNormalToGravity;
		m_skls.flags = flags;
		m_skls.time  = time;
		m_skls.frame = frame;
		m_bHasSkels = true ;
 		m_skls.bIsChanged[index] = true ;
		memcpy( &m_skls.SkeletonDataBack[index] , pdata, sizeof(NUI_SKELETON_DATA) );
	}
}

int MS_NUI_Kinect_SkeletonEngine::NumberSkeletonsTracked()
{
	int iret =0;
	if(  m_bHasSkels == true  )
	{
		for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
		{
			if( m_skls.SkeletonDataBack[i].eTrackingState == NUI_SKELETON_TRACKED )
				iret++;
		}
	}
	return iret;
}


void MS_NUI_Kinect_SkeletonEngine::GetAllPositionOnlySkeletons(vector<ofxMSKinectSkeleton> *skeletons)
{
	if( skeletons )
		GetSkeletons(skeletons, NUI_SKELETON_POSITION_ONLY);
}

void MS_NUI_Kinect_SkeletonEngine::GetAllTrackedSkeletons(vector<ofxMSKinectSkeleton> *skeletons)
{
	if( skeletons )
		GetSkeletons(skeletons, NUI_SKELETON_TRACKED);
}
 
void MS_NUI_Kinect_SkeletonEngine::GetSkeletons(vector<ofxMSKinectSkeleton> *skeletons, 
	NUI_SKELETON_TRACKING_STATE track_state)
{
	skeletons->clear();
	int icount =0;
	
	if(m_bHasSkels == true)
	{
	    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
		{
			if( m_skls.SkeletonDataBack[i].eTrackingState == track_state )
			{
				ofxMSKinectSkeleton data;
				CopyData(&data,  &m_skls.SkeletonDataBack[i]) ;
				skeletons->push_back(data);
			}
			icount++;
		}
	}
}


bool MS_NUI_Kinect_SkeletonEngine::GetSkeleton( int index, ofxMSKinectSkeleton *ptr_in)
{
	int icount =0;
	if( ptr_in != NULL && m_bHasSkels == true)
	{
	    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
		{
			if( m_skls.SkeletonDataBack[i].eTrackingState == NUI_SKELETON_TRACKED )
			{
				if( icount == index )
				{
					CopyData(ptr_in,  &m_skls.SkeletonDataBack[i]) ;
					return true;
				}
				icount++;
			}
		}
	}
	return false;
}

bool MS_NUI_Kinect_SkeletonEngine::GetSkeletonByTrackId(long id, ofxMSKinectSkeleton *ptr_in)
{
	if( ptr_in != NULL && m_bHasSkels == true)
	{
	    for( int i = 0 ; i < NUI_SKELETON_COUNT ; i++ )
		{
			if( m_skls.SkeletonDataBack[i].dwTrackingID == id )
			{
				CopyData(ptr_in,  &m_skls.SkeletonDataBack[i]) ;
				return true;
			}
		}
	}
	return false;
}



void  MS_NUI_Kinect_SkeletonEngine::CopyData( ofxMSKinectSkeleton *ptr_in, NUI_SKELETON_DATA *pdata )
{
	CONVERT_V4TOV3(ptr_in->vNormalToGravity, m_skls.vNormalToGravity ) 
	CONVERT_V4TOV3(ptr_in->vFloorClipPlane,  m_skls.vFloorClipPlane  ) 
	CONVERT_V4TOV3(ptr_in->Position       ,  pdata->Position );

	ptr_in->frame_number = m_skls.frame;
	ptr_in->time_stamp   = m_skls.time;
	ptr_in->flags        = m_skls.flags ;

	ptr_in->dwTrackingID     = pdata->dwTrackingID ;
	ptr_in->dwUserIndex      = pdata->dwUserIndex ;
	ptr_in->dwQualityFlags   = pdata->dwQualityFlags ;
	ptr_in->eTrackingState   = pdata->eTrackingState ;

	for(int i=0; i < NUI_SKELETON_POSITION_COUNT; i++)
	{
		CONVERT_V4TOV3(	ptr_in->SkeletonPositions[i], pdata->SkeletonPositions[i]) 
		ptr_in->eSkeletonPositionTrackingState[i] = (NUI_SKELETON_POSITION_TRACKING_STATE) pdata->eSkeletonPositionTrackingState[i];
	}
}
