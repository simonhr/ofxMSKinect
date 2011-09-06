#include <Windows.h>
#include "MS_NUI_Def.h"
#include "ofxMSKinect.h"
#include "ofxMSKinectDevice.h"
#include "ofMain.h"
#include "ofxMSSkeletonDraw.h"


ofxMSKinectDevice::ofxMSKinectDevice(bool bUseMultidevice)
{
	kinectInterface= NULL;
	bMultidevice = bUseMultidevice;
}

// ***********************************************************************
// Initializes the connection to the kinect device.
// ***********************************************************************
bool ofxMSKinectDevice::Init(int deviceindex )
{
	if( kinectInterface == NULL )
	{
		kinectInterface = new MS_NUI_Kinect_Interface(deviceindex, bMultidevice);
		kinectInterface->Initialize();
	}
	
	return kinectInterface->IsConnected();
}

string ofxMSKinectDevice::GetDeviceName()
{
	return string(kinectInterface->deviceName);
}

// ***********************************************************************
// Cleans up the connection to the kinect device
// ***********************************************************************
void ofxMSKinectDevice::Close(void)
{
	if( kinectInterface != NULL )
	{

		kinectInterface->Close();
		delete kinectInterface ;
		kinectInterface = NULL;

	}
	
	delete[] depthColorData;
	depthColorData = NULL;
}

// ***********************************************************************
// get's if the device is connected
// ***********************************************************************
bool ofxMSKinectDevice::IsConnected(void)
{
	if(kinectInterface == NULL)
		return false;

	return kinectInterface->IsConnected();
}

// ***********************************************************************
// Sets the Camera Elevation Angle (Range -27 - 27)
// ***********************************************************************
void ofxMSKinectDevice::SetCameraElevationAngle(long angle)
{
	if( kinectInterface != NULL )
	{
		kinectInterface->SetCameraElevationAngle(angle);
	}
}

// ***********************************************************************
// Gets the Camera Elevation Angle (Range -27 - 27)
// ***********************************************************************
long ofxMSKinectDevice::GetCameraElevationAngle(void)
{
	if(kinectInterface != NULL )
	{
		return kinectInterface->GetCameraElevationAngle();
	}
	return 0;
}


// ***********************************************************************
// Initiate Skeleton tracking 
// ***********************************************************************
bool  ofxMSKinectDevice::StartSkeletonTracking(void)
{
	if( kinectInterface != NULL )
		return kinectInterface->StartSkeletonTracking();
	return false;
}

// ***********************************************************************
// Initiate RGB (video) capture 
// ***********************************************************************
bool ofxMSKinectDevice::StartRGB(IMAGE_RESOLUTION resolution)
{
	if( kinectInterface != NULL )
	{
		VideoSize = ofxMSKinect::GetSizeFromResolution(resolution);
		vidImage.allocate(VideoSize.x, VideoSize.y, OF_IMAGE_COLOR_ALPHA); 
		return kinectInterface->StartVideoRecording( (int) resolution );
	}
	return false;
}

// ***********************************************************************
// Initiate Depth capture 
// ***********************************************************************
bool  ofxMSKinectDevice::StartDepth(IMAGE_RESOLUTION resolution)
{
	if( kinectInterface != NULL )
	{
		DepthSize = ofxMSKinect::GetSizeFromResolution(resolution);
		depthTexture.allocate(DepthSize.x, DepthSize.y, GL_RGB);

		int depthWidth =  DepthSize.x;
		int depthHeight = DepthSize.y;
		depthColorData = new unsigned char[depthWidth * depthHeight * 3]; 
		depthHasPlayers = false ;
		return kinectInterface->StartDepthRecording((int)resolution);
	}
	return false;
}


// ***********************************************************************
//   Convenience function  to Update all features
// ***********************************************************************
void ofxMSKinectDevice::Update()
{
	UpdateRGBVideo() ;
	UpdateDepth() ;
	UpdateSkeletons();
}

// ***********************************************************************
//  Updates Depth information
// ***********************************************************************
bool ofxMSKinectDevice::UpdateDepth() 
{
	unsigned short* datas = GetRawDepthDataAsShorts();
	if(datas == NULL)
		return false ;

	int width  = DepthSize.x;
	int height = DepthSize.y;

	modifyDepthColors(width, height, datas);
	
	depthTexture.loadData(depthColorData, width, height, GL_RGB);
	return true;
}

// ***********************************************************************
//  Updates RGB (video) information
// ***********************************************************************
bool ofxMSKinectDevice::UpdateRGBVideo()
{
	return GetRGBVideoFrameAsImage() ;
}


bool ofxMSKinectDevice::GetRGBVideoFrameAsImage()
{
	if( kinectInterface != NULL )
	{
		ofPixels pixels = GetofColorPixels();
		vidImage.setFromPixels(pixels);
		return true;
	}
	return false;
}

// ***********************************************************************
//  Return Depth display as ptr to unsigned char buffer
// ***********************************************************************
unsigned char* ofxMSKinectDevice::GetDepthPixels(void)
{
	if( kinectInterface != NULL )
	{
		return kinectInterface->GetDepthBuffer();
	}
	return (unsigned char* ) NULL;
}

//	ofPixels GetofDepthPixels(void);
//		ofPixels GetofColorPixels(void);
//		ofPixels GetofRawDepthDataAsShorts(void);
//		ofPixels GetofRawDepthData(void);


// ***********************************************************************
//  Return vid buffer as ofPixels object 
// ***********************************************************************
ofPixels &ofxMSKinectDevice::GetofColorPixels(void)
{
	if( kinectInterface != NULL )
	{
		colorPixels.setFromPixels(kinectInterface->GetVidBuffer(),
			kinectInterface->GetVidWidth(), 
			kinectInterface->GetVidHeight(), 
			OF_IMAGE_COLOR_ALPHA); 
	}
	else
	{
		colorPixels.clear();
	}
	return colorPixels;
}

// ***********************************************************************
//  Return depth buffer as ofPixels object 
// ***********************************************************************
ofPixels & ofxMSKinectDevice::GetofRawDepthData(void)
{
	if( kinectInterface != NULL )
	{
		rawDepth.setFromExternalPixels( kinectInterface->GetDepthBuffer(),
			kinectInterface->GetDepthWidth(), kinectInterface->GetDepthHeight(), kinectInterface->GetDepthChannels() ); 
	}
	else
	{
		rawDepth.clear();
	}
	return  rawDepth;
}

// ***********************************************************************
//  Return depth buffer as ofPixels object 
// ***********************************************************************
ofPixels & ofxMSKinectDevice::GetofDepthPixels(void)
{
	if( kinectInterface != NULL )
	{
		depthPixels.setFromExternalPixels( kinectInterface->GetDepthBuffer(),
			kinectInterface->GetDepthWidth(), kinectInterface->GetDepthHeight(), kinectInterface->GetDepthChannels()); 
	}
	else
	{
		rawDepth.clear();
	}
	return rawDepth;
}

/*
bool ofxMSKinectDevice::GetSkeleton( int index, ofxMSKinectSkeleton *ptr_in)
{
	if( kinectInterface != NULL )
	{
		return  kinectInterface->GetSkeletonByTrackId(index, ptr_in );
	}
	return false;
}


bool ofxMSKinectDevice::GetSkeletonByTrackId(long id,ofxMSKinectSkeleton *skelData) 
{
	if( kinectInterface != NULL )
	{
		return  kinectInterface->GetSkeletonByTrackId(id,skelData );
	}
	return false;
}

bool ofxMSKinectDevice::GetFirstTrackedSkeleton(ofxMSKinectSkeleton  *skelData ) 
{
	if( kinectInterface != NULL )
	{
		return  kinectInterface->GetSkeleton(skelData);
	}
	return false;
}
*/
/*
void ofxMSKinectDevice::GetAllTrackedSkeletons()// vector<ofxMSKinectSkeleton> *skeletons)
{
	if( kinectInterface != NULL && skeletons != NULL)
	{
		if(HasSkeleton() )
			kinectInterface->GetAllTrackedSkeletons(skeletons);
	}
}
*/
// ***********************************************************************
//  Update Skeletons information 
// ***********************************************************************
bool  ofxMSKinectDevice::UpdateSkeletons() //vector<ofxMSKinectSkeleton> *skeletons)
{
	if( kinectInterface != NULL )
	{
		if( HasSkeleton() )
		{
			//kinectInterface->GetAllTrackedSkeletons(&tracked_skeletons);
			//kinectInterface->GetAllPositionOnlySkeletons(&positiononly_skeletons);

			kinectInterface->GetSkeletons(&tracked_skeletons, &positiononly_skeletons);
			return true;
		}
	}
	return false;
}


void ofxMSKinectDevice::TransFormToDepth(float x, float y, float z, float *fx, float *fy)
{
		MS_NUI_Kinect_Interface::TransFormToDepth( x, y, z, fx, fy) ;
}

void ofxMSKinectDevice::TransFormToDepth(ofVec3f vec3, float *fx, float *fy)
{
		MS_NUI_Kinect_Interface::TransFormToDepth( vec3, fx, fy) ;
}

// ***********************************************************************
//  Return number of Skeletons being tracked 
// ***********************************************************************
int ofxMSKinectDevice:: NumberSkeletonsTracked(void)
{
	if( kinectInterface != NULL )
	{
		return kinectInterface->NumberSkeletonsTracked();
	}

	return 0;
}

// ***********************************************************************
//  Returns true if Skeletons dectect false if not  
// ***********************************************************************
bool ofxMSKinectDevice::HasSkeleton()
{
	if( kinectInterface != NULL )
	{
		return kinectInterface->HasSeletions();
	}
	return false;
}

// ***********************************************************************
//  Returns depth buffer as unsigned char ptr  
// ***********************************************************************
unsigned char* ofxMSKinectDevice::GetRawDepthData(void)
{
	if( kinectInterface != NULL )
	{
		return  kinectInterface->GetDepthBuffer();
	}
	return  (unsigned char* )NULL;
}

// ***********************************************************************
//  Returns depth buffer as unsigned short ptr   
// ***********************************************************************
unsigned short* ofxMSKinectDevice::GetRawDepthDataAsShorts(void)
{
	if( kinectInterface != NULL )
	{
		return (unsigned short* )  kinectInterface->GetDepthBuffer();
	}
	return  (unsigned short* ) NULL;
}

// ***********************************************************************
//  Returns RGB (vid) buffer  as unsigned short ptr   
// ***********************************************************************
unsigned char* ofxMSKinectDevice::GetColorPixels(void)
{
	if( kinectInterface != NULL )
	{
		return  kinectInterface->GetVidBuffer();
	}
	return (unsigned char* )NULL;
}

ofVec2f ofxMSKinectDevice::GetColorPixelPositionFromDepthPixel(long depthX, long depthY, short depthVal)
{
	ofVec2f vec ;
	if(kinectInterface != NULL)
	{
		long x = 0;
		long y = 0;
		kinectInterface->getColorPixelPositionFromDepthPixel(depthX, depthY, &x, &y, depthVal);
		vec.x = x;
		vec.y = y;
	}
	return vec;
}

// ***********************************************************************
//  internal to support convenience Draw functions    
// ***********************************************************************
void ofxMSKinectDevice::setDepthColorFromDistance(long distance, int index)
{

	if(distance <= 900)
	{
		depthColorData[index + BlueIndex] = 255;
		depthColorData[index + GreenIndex] = 0;
		depthColorData[index + RedIndex] = 0;
	}
	else if(distance > 900 && distance < 2000)
	{
		depthColorData[index + BlueIndex] = 0;
		depthColorData[index + GreenIndex] = 255;
		depthColorData[index + RedIndex] = 0;
	}
	else if(distance >= 2000)
	{
		depthColorData[index + BlueIndex] = 0;
		depthColorData[index + GreenIndex] = 0;
		depthColorData[index + RedIndex] = 255;
	}
}

// ***********************************************************************
//  internal to support convenience Draw functions    
// ***********************************************************************

void ofxMSKinectDevice::modifyDepthColors(int width, int height, unsigned short* shortData)
{
	int depthIndex = 0;
	
	int idxmax = (VideoSize.x * VideoSize.y) * 4;
	
	depthHasPlayers = false ;

	unsigned char* colorData = GetColorPixels();
	if(colorData == NULL)
		return;

	for(int y = 0; y < 240; y++)
	{
		int heighOffset = y * width;
		for(int x = 0; x < 320; x++)
		{
			int index = (x + heighOffset) * 3;  //((width - x - 1) + heighOffset) * 3;

			short depthDateItem = 0;

			int distance = ofxMSKinect::GetDepthDistance(shortData[depthIndex]);
			
			setDepthColorFromDistance(distance, index);

			if(ofxMSKinect::GetPlayerIndex(shortData[depthIndex]) > 0)
			{
				depthHasPlayers = true; 
				ofVec2f val = GetColorPixelPositionFromDepthPixel(x, y, distance);
				int idx = val.y * VideoSize.x + val.x;
				idx*=4;
				if( idx < idxmax ) // this cause we get a bug sometimes idx > max buffer size
				{
					depthColorData[index + BlueIndex]  = colorData[idx+BlueIndex];
					depthColorData[index + GreenIndex] = colorData[idx+GreenIndex];
					depthColorData[index + RedIndex]   = colorData[idx+RedIndex];
				}
			}

			depthIndex++;
		}
	}
}

// ***********************************************************************
// Convenience function to display depth, vid and skeleton on screen
// ***********************************************************************
void ofxMSKinectDevice::Draw(int padding )
{
	int dwidth  = DepthSize.x;
	int dheight = DepthSize.y;

	//Draw depth
	DrawDepthTexture(padding, padding, dwidth, dheight);

	ofSetHexColor(0xffffff);
	
	//Draw video (scale to size of depth for debug)
	DrawVidImage(padding + padding + dwidth, padding, dwidth, dheight);

//  Examples of drawing skeleton
	ofxSkeletonRenderer skeletonDrawer; // simple class to draw skeletons 
	skeletonDrawer.Draw(padding, DepthSize, HasSkeleton(),  &tracked_skeletons ,  &positiononly_skeletons);
}



// ***********************************************************************
// Smoothing functions SetDefaultSmoothing set to default for MSKinetic as per the SDK Docs.
// When set the values will take immediate effect on data returned from the Kinetic device.
// ***********************************************************************
void ofxMSKinectDevice::SetDefaultSmoothing()
{
	if( kinectInterface != NULL )
		kinectInterface->SetDefaultSmoothing();
}

void ofxMSKinectDevice::SetTransformSmoothParamaters(NUI_TRANSFORM_SMOOTH_PARAMETERS *inptr )
{
	if( kinectInterface != NULL && inptr != NULL )
	 	 kinectInterface->SetTransformSmoothParamaters( inptr )  ; 
}


void  ofxMSKinectDevice::GetTransformSmoothParamaters(NUI_TRANSFORM_SMOOTH_PARAMETERS *inptr ) 
{
		if( kinectInterface != NULL && inptr != NULL )
		 	 kinectInterface->GetTransformSmoothParamaters( inptr )  ; 
}


float ofxMSKinectDevice::GetSmoothing()
{
	if( kinectInterface != NULL )
		return kinectInterface->GetSmoothing()  ; 
	return 0;
}

void  ofxMSKinectDevice::SetSmoothing(float f)
{
	if( kinectInterface != NULL )
		kinectInterface->SetSmoothing(f) ; 
}

float ofxMSKinectDevice::GetCorrection() 
{
	if( kinectInterface != NULL )
		return kinectInterface->GetCorrection() ;
	return 0;
}


void ofxMSKinectDevice::SetCorrection(float f) 
{
	if( kinectInterface != NULL )
		kinectInterface->SetCorrection(f); 
}

float ofxMSKinectDevice::GetJitterRadius()
{
	if( kinectInterface != NULL )
		return kinectInterface->GetJitterRadius(); 
	return 0;
}
	
void ofxMSKinectDevice::SetJitterRadius(float f)
{
	if( kinectInterface != NULL )
		kinectInterface->SetJitterRadius(f) ; 
}


float ofxMSKinectDevice::GetMaxDeviationRadius() 
{ 
	if( kinectInterface != NULL )
		return kinectInterface->GetMaxDeviationRadius() ;
	return 0;
}

void ofxMSKinectDevice::SetMaxDeviationsRadius(float f)
{
	if( kinectInterface != NULL )
		kinectInterface->SetMaxDeviationsRadius(f) ;
}