#pragma once
#include "MS_NUI_Def.h"
#include "MS_NUI_Kinect_Interface.h"
#include "ofxMSKinectDevice.h"
#include "ofMain.h"


typedef enum _DEVICEINTERFACE
{
    SINGLE_DEVICE = 0,   // use single devince interface functions
    MULTIPLE_DEVICES	// use multiple device interface functions NOTE: not supported in CodeBlocks
 } DEVICEINTERFACE;

class ofxMSKinect 
{
	public :
		bool Init(DEVICEINTERFACE devInterface );
		void Close();

		bool HasKinectDevice(void);
		int GetKinectDeviceCount(void);

		ofxMSKinectDevice* GetKinectDeviceDefault();
		ofxMSKinectDevice* GetKinectDevice(int index);

		static short GetDepthDistance(short val);
		static int GetPlayerIndex(short val);

		static ofVec2f GetSizeFromResolution(int resolution);
		

private :
	vector<ofxMSKinectDevice*> devices;
	ofxMSKinectDevice* dev;
};
