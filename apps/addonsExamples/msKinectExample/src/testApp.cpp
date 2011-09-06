
#pragma once

#include <stdlib.h>
#include <Windows.h>
#include "testApp.h"
#include "ofxMSKinect.h"
#include "ofxMSSkeletonDraw.h"

ofxMSKinect* kinect;
ofxMSKinectDevice* firstDevice;
int padding = 12;

ofxSkeletonRenderer* skeletonDrawer; // simple class to draw skeletons 

void testApp::setup()
{
	//this will initialize everything we need for basic kinect functionality
	//this will also check if the device is connected and handle gracefully
	kinect = new ofxMSKinect();
	kinect->Init(SINGLE_DEVICE); //Pass MULTIPLE_DEVICES if you intend to use more than 1 kinect.
	
	if(kinect->HasKinectDevice())
	{
		skeletonDrawer = new ofxSkeletonRenderer();

		firstDevice = kinect->GetKinectDeviceDefault();

    //	NUI_TRANSFORM_SMOOTH_PARAMETERS inptr;    // get and set skeleton smoothing params
	//	firstDevice->GetTransformSmoothParamaters( &inptr );
	//	firstDevice->SetTransformSmoothParamaters( &inptr );
    //  or call individual functions for each of NUI_TRANSFORM_SMOOTH_PARAMETERS properties e.g 
	//	firstDevice->SetSmoothing(f);
	//	float f  = firstDevice->GetSmoothing() ;
		
		// image res for RGB has to be this set by the NUI api .lower res returns failure
		bool isOk = firstDevice->StartRGB(IMAGE_RESOLUTION_640x480) ;
	
		// default depth resolution is 320x240
		isOk = firstDevice->StartDepth(IMAGE_RESOLUTION_320x240);

		//start tracking skeleton
		isOk = firstDevice->StartSkeletonTracking();

		kinectFailedToInit = false;
	}
	else
	{
		kinectFailedToInit = true;
	}
}


void testApp::update()
{
	if(kinectFailedToInit)
		return;

	bool isOk = firstDevice->UpdateRGBVideo();
	isOk = firstDevice->UpdateDepth() ;
	isOk = firstDevice->UpdateSkeletons();

	// or call the single  firstDevice->Update() convenience function ;
}


void testApp::draw()
{
	ofSetHexColor(0xffffff);

	if(kinectFailedToInit)
	{
		ofSetHexColor(0xff0000);
		ofDrawBitmapString("No Kinect Device Found", padding, padding, 0);
		return;
	}

	int dwidth  = firstDevice->DepthSize.x;
	int dheight = firstDevice->DepthSize.y;

	//Draw depth
	firstDevice->DrawDepthTexture(padding, padding, dwidth, dheight);

	ofSetHexColor(0xffffff);
	
	//Draw video (scale to size of depth for debug)
	firstDevice->DrawVidImage(padding + padding + dwidth, padding, dwidth, dheight);

//  Examples of drawing skeleton draw tracked and position only skels
	skeletonDrawer->Draw(padding, firstDevice->DepthSize, firstDevice->HasSkeleton(),  &firstDevice->tracked_skeletons, &firstDevice->positiononly_skeletons);
	
//	skeletonDrawer->Draw(padding, firstDevice->DepthSize, firstDevice->HasSkeleton(),  &firstDevice->tracked_skeletons);
//	skeletonDrawer->DrawJoints(padding, firstDevice->DepthSize.x, firstDevice->DepthSize.y, firstDevice->HasSkeleton(),  &firstDevice->skeletons );
//	skeletonDrawer->DrawLimbs(padding, firstDevice->DepthSize.x, firstDevice->DepthSize.y, firstDevice->HasSkeleton(),  &firstDevice->skeletons );

	drawDebug();
	// or call firstDevice->Draw( padding ) to call convenience Draw function ;
}


void testApp::drawDebug()
{
	ofSetHexColor(0x0);
	int dwidth = firstDevice->DepthSize.x;

	int leftOffset = padding + (dwidth * 2) + padding + padding;

	string name = firstDevice->GetDeviceName();
	int yPos = padding;

	ostringstream ostr;

	ostr << "Device Name: " << name << "\r\n";

	ostr << "Connected :" << (firstDevice->IsConnected() ? "yes" : "no") << "\r\n";

	ostr << "RGB Size: " << firstDevice->VideoSize.x << " - " << firstDevice->VideoSize.y << "\r\n";

	ostr << "Depth Size: " << firstDevice->DepthSize.x << " - " << firstDevice->DepthSize.y << "\r\n";

	ostr << "Depth Has Player Info: " << (firstDevice->DepthHasPlayers() ? "yes" : "no") << "\r\n";

	bool hasSkeleton = firstDevice->HasSkeleton();
	ostr << "Has Skeletons: " << (hasSkeleton? "yes" : "no") << "\r\n";
	ofDrawBitmapString(ostr.str(), leftOffset, yPos, 0);
}

long val;
void testApp::keyPressed(int key)
{
	if(key == OF_KEY_UP)
	{
		val++;
		firstDevice->SetCameraElevationAngle(val);
	}
	if(key == OF_KEY_DOWN)
	{
		val--;
		firstDevice->SetCameraElevationAngle(val);
	}
	if(key == 'm')
	{
		
	}
	printf("Key %d\r\n", key);
}

void testApp::keyReleased(int key)
{

}

void testApp::mouseMoved(int x, int y )
{

}

void testApp::mouseDragged(int x, int y, int button){

}

void testApp::mousePressed(int x, int y, int button){

}

void testApp::mouseReleased(int x, int y, int button){

}

void testApp::windowResized(int w, int h){

}

void testApp::gotMessage(ofMessage msg){

}

void testApp::dragEvent(ofDragInfo dragInfo){

}

void testApp::exit()
{
	if(kinect != NULL)
		kinect->Close();

	delete kinect;
	kinect = NULL;

	delete firstDevice;
	firstDevice = NULL;
	
	delete skeletonDrawer;
	skeletonDrawer = NULL;
}
