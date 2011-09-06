#include "MS_NUI_Kinect_Interface.h"
#include "ofxMSKinectSkeleton.h"

int MS_NUI_Kinect_Interface::GetDeviceCount()
{
	int count = 0 ;
	HRESULT hr =  MSR_NUIGetDeviceCount( &count ); 
	return count;
}

MS_NUI_Kinect_Interface::MS_NUI_Kinect_Interface(int deviceindex, bool bUseMultidevice) :ofThread()
{
	  dev_index = deviceindex ;
	  bMultidevice =  bUseMultidevice;

	  ppInstance = NULL ;
	  vidimage_buff = 
	  vidimage_backbuff = 
	  depth_buff_raw =
	  depth_backbuff_raw =  NULL;
	  
	
	  m_VidRefresh = m_DepthRefresh =  m_SkelRefresh = false;
	  vidHeight = vidWidth =   depthWidth =  depthHeight = 0 ;
   
	  m_isConnected = false;
	  m_IsSkelTrackingOn = false;
	  m_IsVideoOn = false;
	  m_IsDepthOn = false;
	  deviceName =NULL;
	  SetDefaultSmoothing();

}

void MS_NUI_Kinect_Interface::SetDefaultSmoothing()
{
	m_smooth.fSmoothing= 0.5;
    m_smooth.fCorrection=0.5;
    m_smooth.fPrediction=0.5;
    m_smooth.fJitterRadius=0.05;
    m_smooth.fMaxDeviationRadius=0.04;
}



void MS_NUI_Kinect_Interface::getProps()
{
	BSTR dev;
	bool bHasData = false ;
	if(  bMultidevice )
	{
		if( ppInstance == NULL)
			return ;
		
		bHasData = ppInstance->MSR_NuiGetPropsBlob(START_ARG MsrNui::INDEX_UNIQUE_DEVICE_NAME, &dev, 0) ;
	}
	else
	{
		bHasData = MSR_NuiGetPropsBlob(MsrNui::INDEX_UNIQUE_DEVICE_NAME, &dev, 0) ;
	}

	if( bHasData ) 
	{
		int len = SysStringLen(dev) ;
		deviceName = (char*) malloc(1+len);
		memset(deviceName,0,len+1);
		WideCharToMultiByte(CP_ACP, 0, dev, -1, deviceName, len, NULL, NULL);
		deviceName[len] = '\0';
	}
}

MS_NUI_Kinect_Interface::~MS_NUI_Kinect_Interface(void)
{
	Close(); // interface to kinetic

	if( vidimage_buff != NULL )
		delete[] vidimage_buff;
	vidimage_buff = NULL ;

	if( vidimage_backbuff != NULL )
		delete[] vidimage_backbuff;
	vidimage_backbuff = NULL ;

	if( depth_backbuff_raw != NULL )
		delete[] depth_backbuff_raw;
	depth_backbuff_raw = NULL ;

	if( depth_buff_raw != NULL )
		delete[] depth_buff_raw;
	depth_buff_raw = NULL ;

	if( deviceName != NULL)
		free(deviceName);
	deviceName = NULL;
}


bool MS_NUI_Kinect_Interface::Initialize()
{
  if(  m_isConnected == true)
	  return true;

    HRESULT hr ;
	
	if(  bMultidevice )
	{
		hr = MSR_NuiCreateInstanceByIndex(  dev_index, (INuiInstance **)&ppInstance );
		if( FAILED( hr ) )
			{ return false ;  }
  		
		
		if(dev_index == 0)
		{
			hr = ppInstance->NuiInitialize( START_ARG NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | 
										NUI_INITIALIZE_FLAG_USES_SKELETON | 
										NUI_INITIALIZE_FLAG_USES_COLOR);
		}
		else
		{
			hr = ppInstance->NuiInitialize( START_ARG NUI_INITIALIZE_FLAG_USES_DEPTH | 
										NUI_INITIALIZE_FLAG_USES_COLOR);
		}
	}
	else
	{
			hr = NuiInitialize( NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | 
										NUI_INITIALIZE_FLAG_USES_SKELETON | 
										NUI_INITIALIZE_FLAG_USES_COLOR);
	}
	
	if( FAILED( hr ) )
    { return false ;  }
  
    m_DepthFrameEvent = CreateEventW( NULL, TRUE, FALSE, NULL );
    m_VideoFrameEvent = CreateEventW( NULL, TRUE, FALSE, NULL );
    m_SkeletonEvent   = CreateEventW( NULL, TRUE, FALSE, NULL );

    m_isConnected = true;
	getProps();
    return m_isConnected;  
}

void MS_NUI_Kinect_Interface::StartEventThread()
{
	startThread( true, false ) ; 
}

void MS_NUI_Kinect_Interface::SetCameraElevationAngle(long angle)
{
	if(angle < NUI_CAMERA_ELEVATION_MINIMUM || angle >  NUI_CAMERA_ELEVATION_MAXIMUM)
		return;

	if(  bMultidevice )
	{
		if( ppInstance != NULL)
			HRESULT res = ppInstance->NuiCameraElevationSetAngle(START_ARG angle);
	}
	else
	{
		HRESULT res = NuiCameraElevationSetAngle(angle);
	}
}

long MS_NUI_Kinect_Interface::GetCameraElevationAngle(void)
{
	long angle = 0;
	
	if(  bMultidevice )
	{
		if( ppInstance != NULL)
			HRESULT res = ppInstance->NuiCameraElevationGetAngle(START_ARG &angle);
	}
	else
	{
		HRESULT res = NuiCameraElevationGetAngle(&angle);
	}
	return angle;
}

NUI_IMAGE_RESOLUTION MS_NUI_Kinect_Interface::GetImageRes(int i )
{
	switch ( i )
	{
	 case 0: return NUI_IMAGE_RESOLUTION_80x60 ; break ;
	 case 1: return NUI_IMAGE_RESOLUTION_320x240; break;
	 case 2: return NUI_IMAGE_RESOLUTION_640x480; break;
	 case 3: return NUI_IMAGE_RESOLUTION_1280x1024; break;    
	}
	return NUI_IMAGE_RESOLUTION_INVALID ;
}

unsigned char * MS_NUI_Kinect_Interface::GetVidBuffer() 
{ 
	bool bfresh = m_VidRefresh ; 

	if( bfresh == true ) 
	{
		int index = 0;
		lock() ;
		for(int yy=0; yy< vidHeight; yy++)
		{
			for(int xx=0; xx< vidWidth; xx++,index += colorBytesPerPixel )
			{
				vidimage_buff[index] = vidimage_backbuff[index+2];//r;
				vidimage_buff[index+1] = vidimage_backbuff[index+1];//g;
				vidimage_buff[index+2] = vidimage_backbuff[index];//b;
			}
		}
		unlock();
		m_VidRefresh = false ;
		return   vidimage_buff;
	}
	else
	{
		if(vidimage_buff != NULL)
			return vidimage_buff;
	}
	return NULL;
}

unsigned char * MS_NUI_Kinect_Interface::GetDepthBuffer() 
{ 
	bool bfresh = m_DepthRefresh; 
	if( bfresh == true ) 
	{
		lock() ;
		memcpy(depth_buff_raw, depth_backbuff_raw, (depthWidth * depthHeight) * depthBytesPerPixel );
		m_DepthRefresh = false ;
		unlock();
		return   depth_buff_raw;
	}
	return NULL;
}

	
bool MS_NUI_Kinect_Interface::StartVideoRecording(int resolution ) 
{
	if( IsConnected() == false )
		return false ;
	
	if( m_IsVideoOn == true )
		return true;
	
	VideoResolution = resolution;
	NUI_IMAGE_RESOLUTION imagesize = GetImageRes(resolution) ;

   HRESULT hr;
   if(  bMultidevice )
   {
	   if(ppInstance == NULL)
		   return false ;
  	   hr = ppInstance->NuiImageStreamOpen( START_ARG NUI_IMAGE_TYPE_COLOR,  imagesize, 0, 2, m_VideoFrameEvent, &m_pVideoStreamHandle );
   }
   else
   {
	   hr = NuiImageStreamOpen( NUI_IMAGE_TYPE_COLOR,  imagesize, 0, 2, m_VideoFrameEvent, &m_pVideoStreamHandle );
   }

	if( FAILED( hr ) )
    {   return false ;    }


	if(  vidimage_buff != NULL )
	   delete[] vidimage_buff ;
	
	if(  vidimage_backbuff != NULL )
	   delete[] vidimage_backbuff ;
	
	ImageResolutionToSize( imagesize ,  &vidWidth, &vidHeight) ;
	int isize =  (vidHeight * vidWidth) *  colorBytesPerPixel;
	vidimage_buff = new unsigned char [ isize ] ;
	vidimage_backbuff = new unsigned char [ isize ] ;
	
	memset(vidimage_buff, 0, isize);
	memset(vidimage_backbuff, 0, isize);

	m_IsVideoOn = true ;
	StartEventThread();
	return m_IsVideoOn;
}

bool MS_NUI_Kinect_Interface::StartDepthRecording(int resolution) 
{
	if( IsConnected() == false)
		return false ;

	if(  m_IsDepthOn == true )
		return true ;

	DepthResolution = resolution;

	NUI_IMAGE_RESOLUTION imagesize = GetImageRes(resolution) ;
	HRESULT hr;
	if(  bMultidevice )
	{
		if( ppInstance == NULL)
			return false ;
		hr = ppInstance->NuiImageStreamOpen(START_ARG NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, imagesize, 0,2, m_DepthFrameEvent, &m_pDepthStreamHandle );
	}
	else
	{
		hr = NuiImageStreamOpen( NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX, imagesize, 0,2, m_DepthFrameEvent, &m_pDepthStreamHandle );
	}
  
	if( FAILED( hr ) )
    {  return false ;  }

	if(  depth_buff_raw != NULL )
	   delete[]  depth_buff_raw ;
	
	if(  depth_backbuff_raw != NULL )
	   delete[]  depth_backbuff_raw ;
	
	ImageResolutionToSize( imagesize , &depthWidth, &depthHeight) ;
	
	int isize =  (depthWidth * depthHeight) * depthBytesPerPixel;
	depth_buff_raw = new unsigned char [ isize ] ;
	depth_backbuff_raw = new unsigned char [ isize ] ;
	
	memset(depth_buff_raw, 0, isize );
	memset(depth_backbuff_raw, 0, isize );

	m_IsDepthOn = true ;
	StartEventThread();
	return m_IsDepthOn ;

}



bool MS_NUI_Kinect_Interface::StartSkeletonTracking(void)
{
	if( IsConnected() == false )
		return false ;

	if( m_IsSkelTrackingOn == true )
		return true;
	
	HRESULT hr;
	if(  bMultidevice )
	{
		if( ppInstance == NULL)
			return false ;
		hr = ppInstance->NuiSkeletonTrackingEnable(START_ARG m_SkeletonEvent, 0 );
	}
	else
	{
		hr = NuiSkeletonTrackingEnable( m_SkeletonEvent, 0 );
	}

	if( FAILED( hr ) )
    {  return false ;  }
	
    m_IsSkelTrackingOn = true;
	StartEventThread();
	return m_IsSkelTrackingOn;
}



MSKinectColor MS_NUI_Kinect_Interface::getColorAt(int x, int y) 
{
	int index = (y * vidWidth + x) *  colorBytesPerPixel;

	MSKinectColor c ;
	c.r = vidimage_buff[index + 0];
	c.g = vidimage_buff[index + (3-1)/2];
	c.b = vidimage_buff[index + (3-1)];
	c.a = 255;

	return c;
}


void MS_NUI_Kinect_Interface::Close()
{
	if( isThreadRunning() )
	{
		stopThread() ;
		while( isThreadRunning()  )
			::Sleep( 2 ) ;
	}

	if(  bMultidevice )
	{
		if( ppInstance != NULL )
		{
			ppInstance->NuiShutdown();
			MSR_NuiDestroyInstance( (INuiInstance *)ppInstance );
			ppInstance = NULL;
		}
	}
	else
	{
   		NuiShutdown();
	}


	m_isConnected = false;

    if( m_SkeletonEvent && ( m_SkeletonEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_SkeletonEvent );
        m_SkeletonEvent = NULL;
		m_IsSkelTrackingOn = false ;
    }

    if( m_DepthFrameEvent && ( m_DepthFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_DepthFrameEvent );
        m_DepthFrameEvent = NULL;
		m_IsDepthOn = false;
    }

    if( m_VideoFrameEvent && ( m_VideoFrameEvent != INVALID_HANDLE_VALUE ) )
    {
        CloseHandle( m_VideoFrameEvent );
        m_VideoFrameEvent = NULL;
		m_IsVideoOn = false;
    }
}

void MS_NUI_Kinect_Interface::threadedFunction()
{
	HANDLE   hEvents[3];
    // Configure events to be listened on
    hEvents[0] = m_DepthFrameEvent;
    hEvents[1] = m_VideoFrameEvent;
    hEvents[2] = m_SkeletonEvent;
	    
	while( isThreadRunning()  )
    {
        int nEventIdx = WaitForMultipleObjects( sizeof(hEvents)/sizeof(hEvents[0]), hEvents, FALSE, 100);

        switch(nEventIdx)
        {
            case 0:  DepthEvent() ;	break;

            case 1:  VideoEvent() ;	break;

            case 2:  SkeletonEvent(); break;
        }
    }
}



void MS_NUI_Kinect_Interface::DepthEvent(void)
{
    const NUI_IMAGE_FRAME * pImageFrame = NULL;
	HRESULT hr;
	if(  bMultidevice )
	{
		if( ppInstance == NULL )
			return; 
		hr = ppInstance->NuiImageStreamGetNextFrame(START_ARG m_pDepthStreamHandle, 0,   &pImageFrame );
	}
	else
	{
   		hr = NuiImageStreamGetNextFrame( m_pDepthStreamHandle, 0,   &pImageFrame );
	}

	if( FAILED( hr ) )
    {  return;   }

    NuiImageBuffer * pTexture = pImageFrame->pFrameTexture;
    KINECT_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if( LockedRect.Pitch != 0 )
    {
    	lock();
		memcpy(depth_backbuff_raw, (BYTE*) LockedRect.pBits,  pTexture->BufferLen());
		m_DepthRefresh = true;
		unlock();
    }
  
   if(  bMultidevice )
	{
		ppInstance->NuiImageStreamReleaseFrame( START_ARG m_pDepthStreamHandle, pImageFrame );
	}
   else
   {
	    NuiImageStreamReleaseFrame( m_pDepthStreamHandle, pImageFrame );
   }

}

RGBQUAD MS_NUI_Kinect_Interface::ShortToQuad_Depth( USHORT s )
{
    USHORT RealDepth = (s & 0xfff8) >> 3;
    USHORT Player = s & 7;

    // transform 13-bit depth information into an 8-bit intensity appropriate
    // for display (we disregard information in most significant bit)
    BYTE l = 255 - (BYTE)(256*RealDepth/0x0fff);

    RGBQUAD q;
    q.rgbRed = q.rgbBlue = q.rgbGreen = 0;

    switch( Player )
    {
    case 0:
        q.rgbRed = l / 2;
        q.rgbBlue = l / 2;
        q.rgbGreen = l / 2;
        break;
    case 1:
        q.rgbRed = l;
        break;
    case 2:
        q.rgbGreen = l;
        break;
    case 3:
        q.rgbRed = l / 4;
        q.rgbGreen = l;
        q.rgbBlue = l;
        break;
    case 4:
        q.rgbRed = l;
        q.rgbGreen = l;
        q.rgbBlue = l / 4;
        break;
    case 5:
        q.rgbRed = l;
        q.rgbGreen = l / 4;
        q.rgbBlue = l;
        break;
    case 6:
        q.rgbRed = l / 2;
        q.rgbGreen = l / 2;
        q.rgbBlue = l;
        break;
    case 7:
        q.rgbRed = 255 - ( l / 2 );
        q.rgbGreen = 255 - ( l / 2 );
        q.rgbBlue = 255 - ( l / 2 );
    }

    return q;
}

void MS_NUI_Kinect_Interface::VideoEvent(void)
{
	const NUI_IMAGE_FRAME * pImageFrame = NULL;
	HRESULT hr ;

	if(  bMultidevice )
	{
	    if( ppInstance == NULL )
		 	return; 
		
		hr = ppInstance->NuiImageStreamGetNextFrame( START_ARG m_pVideoStreamHandle, 0, &pImageFrame );
	}
	else
	{
		hr = NuiImageStreamGetNextFrame( m_pVideoStreamHandle, 0, &pImageFrame );
	}
    
	if( FAILED( hr ) )
    {  return;  }
	
	int type = pImageFrame->eImageType;

    NuiImageBuffer * pTexture = pImageFrame->pFrameTexture;
    KINECT_LOCKED_RECT LockedRect;
    pTexture->LockRect( 0, &LockedRect, NULL, 0 );
    if( LockedRect.Pitch != 0 )
    {
		lock() ;
   		memcpy(vidimage_backbuff, (BYTE*) LockedRect.pBits, pTexture->BufferLen() );
		m_VidRefresh = true;
		unlock();
	} 

    if(  bMultidevice )
	{
		ppInstance->NuiImageStreamReleaseFrame( START_ARG m_pVideoStreamHandle, pImageFrame );
	}
	else
	{
		NuiImageStreamReleaseFrame( m_pVideoStreamHandle, pImageFrame );
	}
}


void MS_NUI_Kinect_Interface::SkeletonEvent(void)
{
   NUI_SKELETON_FRAME SkeletonFrame;
   bool bFoundSkeleton = false;
   
   HRESULT hr ;
   if(  bMultidevice )
	{
	    if( ppInstance == NULL )
		 	return;
		hr = ppInstance->NuiSkeletonGetNextFrame(START_ARG  0, &SkeletonFrame );
   }
   else
   {
	    hr = NuiSkeletonGetNextFrame( 0, &SkeletonFrame );
   }

   
	if( !FAILED( hr ) )
    { 
		bool bSmoothed= false;
		lock();
		m_skelEngine.ClearTrackingFlags() ; 
		unlock() ;
	
		for( int i = 0 ; i < NUI_SKELETON_COUNT; i++ )
		{
			 if( SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_TRACKED ||
				 SkeletonFrame.SkeletonData[i].eTrackingState == NUI_SKELETON_POSITION_ONLY)
			 {
			     bFoundSkeleton = true;
				 if( bSmoothed == false )
				 {
					 bSmoothed = true; 
					  if(  bMultidevice )
					  {
						 ppInstance->NuiTransformSmooth(START_ARG &SkeletonFrame, &m_smooth);
					  }
					  else
					  {
						 NuiTransformSmooth(&SkeletonFrame,&m_smooth);
					  }
				 }
			
				 lock();
				 m_skelEngine.Update(SkeletonFrame.dwFlags, SkeletonFrame.dwFrameNumber, SkeletonFrame.liTimeStamp,
					 SkeletonFrame.vFloorClipPlane, SkeletonFrame.vNormalToGravity, &SkeletonFrame.SkeletonData[i], i ); 
				 unlock() ;
			 }
		}
	}
   
    if( bFoundSkeleton == false )
    {
		m_skelEngine.LostSkeleton() ;
    }
}


void  MS_NUI_Kinect_Interface::ImageResolutionToSize( NUI_IMAGE_RESOLUTION res, int *idWidth, int *idHeight )
{
    switch( res )
    {
    case NUI_IMAGE_RESOLUTION_80x60:		*idWidth = 80;   *idHeight = 60;     break;
    case NUI_IMAGE_RESOLUTION_320x240:		*idWidth = 320;  *idHeight = 240;    break;
    case NUI_IMAGE_RESOLUTION_640x480:		*idWidth = 640;  *idHeight = 480;    break;
    case NUI_IMAGE_RESOLUTION_1280x1024:	*idWidth = 1280; *idHeight = 1024;   break;
    default:    *idWidth = 0;  *idHeight = 0;     break;
    }
}

void MS_NUI_Kinect_Interface::getColorPixelPositionFromDepthPixel(long depthX, long depthY, long* colorX, long* colorY, short depthVal)
{
	 if(  bMultidevice )
	{
		if( ppInstance != NULL )
		{
		HRESULT hres = ppInstance->NuiImageGetColorPixelCoordinatesFromDepthPixel(START_ARG GetImageRes(VideoResolution), NULL, depthX, depthY, NUI_IMAGE_DEPTH_MINIMUM, colorX, colorY);
		}
	 }
	 else
	 {
		HRESULT hres = NuiImageGetColorPixelCoordinatesFromDepthPixel(GetImageRes(VideoResolution), NULL, depthX, depthY, NUI_IMAGE_DEPTH_MINIMUM, colorX, colorY);
	 }
}

void MS_NUI_Kinect_Interface::TransFormToDepth(float x, float y, float z, float *fx, float *fy)
{
	 Vector4 vPoint;
	 vPoint.x = x;  vPoint.y = y;  vPoint.z = z; 
  	 NuiTransformSkeletonToDepthImageF( vPoint, fx, fy );
}

void MS_NUI_Kinect_Interface::TransFormToDepth(ofVec3f vec3, float *fx, float *fy)
{
	 Vector4 vPoint;
	 vPoint.x = vec3.x;  vPoint.y = vec3.y;  vPoint.z = vec3.z; 
  	 NuiTransformSkeletonToDepthImageF( vPoint, fx, fy );
}

/*
bool MS_NUI_Kinect_Interface::GetSkeleton( ofxMSKinectSkeleton *ptr_in)
{
	if( ptr_in == NULL )
		return false;
	
	lock() ;
	bool bret =  m_skelEngine.GetSkeleton(ptr_in);
	unlock();
	return bret;
}
*/
bool MS_NUI_Kinect_Interface::GetSkeleton( int index, ofxMSKinectSkeleton *ptr_in)
{
	if( ptr_in == NULL )
		return false;

	lock() ;
	bool bret = m_skelEngine.GetSkeleton( index, ptr_in);
	unlock();
	return bret;
}

bool MS_NUI_Kinect_Interface::GetSkeletonByTrackId(long id, ofxMSKinectSkeleton *ptr_in)
{
	if( ptr_in == NULL )
		return false;
	
	lock() ;
	bool bret = m_skelEngine.GetSkeletonByTrackId(id, ptr_in);
	unlock();
	return bret;
}

void MS_NUI_Kinect_Interface::GetAllTrackedSkeletons(vector<ofxMSKinectSkeleton> *skeletons)
{
	if( skeletons != NULL )
	{
		lock() ;
		m_skelEngine.GetAllTrackedSkeletons(skeletons);
		unlock();
	}
}


void MS_NUI_Kinect_Interface::GetAllPositionOnlySkeletons(vector<ofxMSKinectSkeleton> *skeletons)
{
	if( skeletons != NULL )
	{
		lock() ;
		m_skelEngine.GetAllPositionOnlySkeletons(skeletons);
		unlock();
	}
}

void MS_NUI_Kinect_Interface::GetSkeletons(vector<ofxMSKinectSkeleton> *tracked_skeletons,
	vector<ofxMSKinectSkeleton> *positiononly_skeletons)
{
		lock() ;
		if( tracked_skeletons != NULL )
			m_skelEngine.GetAllTrackedSkeletons(tracked_skeletons);
		if( positiononly_skeletons != NULL )
			m_skelEngine.GetAllPositionOnlySkeletons(positiononly_skeletons);
		unlock();
	
}


bool MS_NUI_Kinect_Interface::HasSeletions()
{
	return m_skelEngine.HasSeletions();
}


int MS_NUI_Kinect_Interface::NumberSkeletonsTracked(void)
{
	return m_skelEngine.NumberSkeletonsTracked();
}


void  MS_NUI_Kinect_Interface::GetTransformSmoothParamaters(NUI_TRANSFORM_SMOOTH_PARAMETERS *inptr ) 
{
	if( inptr != NULL)
		*inptr = m_smooth;
}



void  MS_NUI_Kinect_Interface::SetTransformSmoothParamaters(NUI_TRANSFORM_SMOOTH_PARAMETERS *inptr )
{ 
	if( inptr != NULL)
	{
		SetSmoothing( inptr->fSmoothing );
		SetCorrection( inptr->fCorrection ) ;
		SetJitterRadius( inptr->fMaxDeviationRadius );
		SetMaxDeviationsRadius( inptr->fMaxDeviationRadius) ;
		SetPredition( inptr->fPrediction );
	}

} 

void  MS_NUI_Kinect_Interface::SetSmoothing(float f)
{
	if( f < 0 || f > 1.0 )
		return ;
	m_smooth.fSmoothing = f ; 
}


void  MS_NUI_Kinect_Interface::SetCorrection(float f)
{
	if( f < 0 || f > 1.0 )
		return ;
	m_smooth.fCorrection = f ; 
}
	