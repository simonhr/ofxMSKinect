#pragma once

#include "ofMain.h"

typedef struct _ofMS_SKELETON_DATA
{
  NUI_SKELETON_TRACKING_STATE eTrackingState;
  long dwTrackingID;
  long dwEnrollmentIndex_NotUsed;
  long dwUserIndex;
  ofVec3f Position;
  ofVec3f SkeletonPositions[NUI_SKELETON_POSITION_COUNT];
  NUI_SKELETON_POSITION_TRACKING_STATE eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_COUNT];
  long dwQualityFlags;
  ofVec3f              vFloorClipPlane;
  ofVec3f              vNormalToGravity;
  long frame_number;
  LARGE_INTEGER time_stamp;
  long flags;
} ofxMSKinectSkeleton;



