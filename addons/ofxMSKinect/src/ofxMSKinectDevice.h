#pragma once

#include "MS_NUI_Def.h"
#include "MS_NUI_Kinect_Interface.h"
#include "ofxMSKinectSkeleton.h"
#include "ofMain.h"

typedef enum IMAGE_RESOLUTION
{
  IMAGE_RESOLUTION_INVALID = -1,
  IMAGE_RESOLUTION_80x60 = 0,
  IMAGE_RESOLUTION_320x240,
  IMAGE_RESOLUTION_640x480,
  IMAGE_RESOLUTION_1280x1024          // for hires color only
} IMAGE_RESOLUTION;

class ofxMSKinectDevice
{
	public :
		ofxMSKinectDevice(bool bUseMultidevice );

		bool Init(int deviceindex);
		void Close(void);
		bool IsConnected(void);
	
		void Draw(int padding);
	
		void Update();
		bool UpdateRGBVideo() ;
		bool UpdateDepth() ;
		bool UpdateSkeletons() ;
	
		long GetCameraElevationAngle(void);
		void SetCameraElevationAngle(long angle);

		bool StartRGB(IMAGE_RESOLUTION  resolution);
		bool StartDepth(IMAGE_RESOLUTION resolution);
		bool StartSkeletonTracking(void);

		ofImage &GetvidImage() { return vidImage; } ;
		ofTexture &GetDepthTexture() { return depthTexture; } ;

		void DrawVidImage(float x, float y, float w, float h)  { vidImage.draw(x, y, w, h) ; } ; 
		void DrawVidImage(float x, float y )  { vidImage.draw(x, y, VideoSize.x, VideoSize.y ) ; } ; 
		
		void DrawDepthTexture(float x, float y, float w, float h)  { depthTexture.draw(x, y, w, h) ; } ; 
		void DrawDepthTexture(float x, float y )  { depthTexture.draw(x, y, DepthSize.x, DepthSize.y ) ; } ; 
	

		unsigned char* GetDepthPixels(void);
		unsigned char* GetColorPixels(void);
		unsigned short* GetRawDepthDataAsShorts(void);
		unsigned char* GetRawDepthData(void);

		ofPixels & GetofDepthPixels(void);
		ofPixels & GetofColorPixels(void);
		ofPixels & GetofRawDepthData(void);
	
		bool DepthHasPlayers() { return depthHasPlayers; }  ;

	//	bool GetSkeletonByTrackId(long id, ofxMSKinectSkeleton* skelData) ;
	//	bool GetFirstTrackedSkeleton(ofxMSKinectSkeleton* skelData) ;
	//	bool GetSkeleton(int index, ofxMSKinectSkeleton *ptr_in);
		bool HasSkeleton();
		
		//void GetAllTrackedSkeletons(vector<ofxMSKinectSkeleton> *skeletons);
		
		static void TransFormToDepth(float x, float y, float z, float *fx, float *fy);
		static void TransFormToDepth(ofVec3f vec3, float *fx, float *fy);

		int NumberSkeletonsTracked(void);

		string GetDeviceName();
		ofVec2f GetColorPixelPositionFromDepthPixel(long depthX, long depthY, short depthValue);

		ofVec2f VideoSize;
		ofVec2f DepthSize;

		vector<ofxMSKinectSkeleton> tracked_skeletons;
		vector<ofxMSKinectSkeleton> positiononly_skeletons;

		void  SetDefaultSmoothing(); 
		void  SetTransformSmoothParamaters(NUI_TRANSFORM_SMOOTH_PARAMETERS *inptr );
		void  GetTransformSmoothParamaters(NUI_TRANSFORM_SMOOTH_PARAMETERS *inptr );

		float GetSmoothing() ;
		void  SetSmoothing(float f);
		float GetCorrection() ;
		void  SetCorrection(float f);
		float GetJitterRadius() ;
		void  SetJitterRadius(float f);
		float GetMaxDeviationRadius() ;
		void  SetMaxDeviationsRadius(float f) ;
	




private :
	     MS_NUI_Kinect_Interface* kinectInterface;
		 
		 bool GetRGBVideoFrameAsImage();
		 void setDepthColorFromDistance(long distance, int index);
		 void modifyDepthColors(int width, int height, unsigned short* shortData);

		 ofPixels depthPixels;
		 ofPixels colorPixels;
		 ofShortPixels shortRawDepth;
		 ofPixels rawDepth;
		
		 static const int BlueIndex = 0;
	 	 static const int GreenIndex = 1;
		 static const int RedIndex = 2;
		
		 ofImage   vidImage;
		 ofTexture depthTexture;
		 unsigned char* depthColorData;

		 bool depthHasPlayers;
		 bool bMultidevice ; 
};

