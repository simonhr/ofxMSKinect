#include <Windows.h>
#include "ofxMSKinect.h"
#include "ofMain.h"

const float MIN_DEPTH_DISTANCE = 800;
const float MAX_DEPTH_DISTANCE = 4000;
const float DEPTH_DISTANCE_OFFSET = MAX_DEPTH_DISTANCE - MIN_DEPTH_DISTANCE;

//*************************************************
//Call this method first in your app initialize. 
// It can take a little while to find the kinect device(s)
//*************************************************
bool ofxMSKinect::Init(DEVICEINTERFACE devInterface )
{
	devices.clear();

	//TODO: Look for and init all devices.
	//Just adding first device for now.

	int count = MS_NUI_Kinect_Interface::GetDeviceCount();
	if( count == 0 )
		return false ;

	bool success = false ;

	if( devInterface ==   MULTIPLE_DEVICES )
	{
		for(int i =0 ; i < count ; i++)
		{
			ofxMSKinectDevice* dev = new ofxMSKinectDevice(true);
			success = dev->Init(i);
	
			if(success)
				devices.push_back(dev);
		}
	}
	else
	{
			ofxMSKinectDevice* dev = new ofxMSKinectDevice(false);
			success = dev->Init(0);
	
			if(success)
				devices.push_back(dev);
	}
	return success;
}

//*************************************************
//Get a ofVec2f representation of the resolution supplied
//*************************************************
ofVec2f ofxMSKinect::GetSizeFromResolution(int resolution)
{
	ofVec2f vec;
	
	if(resolution == IMAGE_RESOLUTION_640x480)
		vec.set(640, 480);
	
	if(resolution == IMAGE_RESOLUTION_320x240)
		vec.set(320, 240);

	return vec;
}

//*************************************************
//Shuts down the connection to the devices.
//*************************************************
void ofxMSKinect::Close()
{
	if(devices.size() > 0)
	{
		devices[0]->Close();
		devices.clear();
	}
}

//*************************************************
//Gets the number of kinect devices attached
//*************************************************
int ofxMSKinect::GetKinectDeviceCount()
{
	return devices.size();
}
//*************************************************
//Gets wether there is at least one device
//*************************************************
bool ofxMSKinect::HasKinectDevice()
{
	return devices.size() > 0;
}

//*************************************************
//Returns the first kinect device
//*************************************************
ofxMSKinectDevice* ofxMSKinect::GetKinectDeviceDefault()
{
	return GetKinectDevice(0);
}

//*************************************************
//Get the kinect device with the given index
//*************************************************
ofxMSKinectDevice* ofxMSKinect::GetKinectDevice(int index)
{
	if(devices.size() < index)
		return NULL;

	return devices[index];
}

//*************************************************
//Returns the distance in millimeters of the supplied Depth data
//*************************************************
short ofxMSKinect::GetDepthDistance(short val)
{
	short realDepth = (val & 0xfff8) >> 3;
	return realDepth;
}

//*************************************************
//Gets the player index from the supplied Depth data if exists. Returns 0 if no player was found.
//*************************************************
int ofxMSKinect::GetPlayerIndex(short val)
{
    short Player = val & 7;
	return Player;
}

