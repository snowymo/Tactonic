// ====================================================================
// Copyright 2010-2012 Tactonics Technologies, LLC. All rights reserved
// PROPRIETARY/CONFIDENTIAL - DO NOT DISTRIBUTE!
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
// ====================================================================
#ifndef __TACTONICTOUCH_H__
#define __TACTONICTOUCH_H__

#ifdef _WIN32
#include <windows.h>
#define TACTONIC_TOUCH __declspec(dllexport)
#else
#define TACTONIC_TOUCH
#define WINAPI
#endif
#include <stdio.h>
#include "Tactonic.h"

enum  TactonicGestureType{		// Types of touches
	GESTURE_NONE,
	GESTURE_TOUCH_START,
	GESTURE_TOUCH_END,
	GESTURE_TOUCH_PRESS,
	GESTURE_TOUCH_RELEASE,
    GESTURE_TAP,
    GESTURE_CLICK,
	GESTURE_FLICK,
    GESTURE_TWIST,
    GESTURE_PINCH,
	GESTURE_TRANSLATE_X,
	GESTURE_TRANSLATE_Y,
	GESTURE_ROTATE,
	GESTURE_SCALE,
	GESTURE_LEAN_X,
	GESTURE_LEAN_Y,
	GESTURE_ALL
};

enum TactonicTouchState{
	TOUCH_START,
	TOUCH_UPDATE,
	TOUCH_END
};

enum TactonicGestureState{
	GESTURE_START,
	GESTURE_UPDATE,
	GESTURE_END,
	GESTURE_PERFORMED
};

enum TactonicGestureDirection{
    GESTURE_DIRECTION_NONE,
	GESTURE_CLOCKWISE,
	GESTURE_COUNTER_CLOCKWISE,
	GESTURE_IN,
	GESTURE_OUT,
	GESTURE_NORTH,
	GESTURE_SOUTH,
	GESTURE_EAST,
	GESTURE_WEST,
};

enum TactonicGestureUnit{
	GESTURE_UNIT_NONE,
	GESTURE_UNIT_MILLIMETERS,
	GESTURE_UNIT_RADIANS
};

enum TactonicShape{
    SHAPE_RECTANGLE,
    SHAPE_OVAL
};

enum TactonicPivot{
    PIVOT_NONE,
	PIVOT_CENTER
};

extern "C" {

	typedef struct tactonic_touch_s{
		long id;					
		float x, y, force;		
		float dx, dy, dForce;
		TactonicTouchState touchState;
	} TactonicTouch;

	typedef struct tactonic_touch_frame_s{
		TactonicTouch *touches;        
		int numTouches;					
		long frameNumber;				
		double time;	
	} TactonicTouchFrame;
	
	typedef struct tactonic_touch_region_s{
		long id;												
		float x, y;
		float width, height;
		TactonicShape shape;
		TactonicPivot pivotPoint;
	} TactonicTouchRegion;

    typedef struct tactonic_touch_frame_event_s{ 
        TactonicDevice device;
		TactonicTouchFrame *frame;  
        TactonicTouchRegion *region; 
	} TactonicTouchEvent;
    
	typedef struct tactonic_gesture_s{
		int id;
		TactonicGestureType type;
		TactonicGestureState state;
		float value;
		float dValue;
		TactonicGestureUnit valueUnit;
		TactonicGestureDirection direction;
		float centerX, centerY, totalForce;
	} TactonicGesture;

    typedef struct tactonic_gesture_event_s{
        TactonicDevice device;
		TactonicGesture *gesture;
		TactonicTouchFrame *frame;  
        TactonicTouchRegion *region; 
	} TactonicGestureEvent;
    
	typedef void (*TactonicTouchCallback)(TactonicTouchEvent* touchEvent);

	typedef void (*TactonicGestureCallback)(TactonicGestureEvent* gestureEvent);

	typedef TactonicFrame* (*TactonicTouchFilterCallback)(TactonicFrameEvent* evt);
    
	TACTONIC_TOUCH void WINAPI TactonicTouch_CreateDetector(TactonicDevice device);

	TACTONIC_TOUCH void WINAPI TactonicTouch_DestroyDetector(TactonicDevice device);

	TACTONIC_TOUCH void WINAPI TactonicTouch_StartDetector(TactonicDevice device);

	TACTONIC_TOUCH void WINAPI TactonicTouch_StopDetector(TactonicDevice device);

	TACTONIC_TOUCH TactonicTouchFrame* WINAPI TactonicTouch_CreateFrame(TactonicDevice device);

	TACTONIC_TOUCH void WINAPI TactonicTouch_DestroyFrame(TactonicTouchFrame* frame);

	TACTONIC_TOUCH void WINAPI TactonicTouch_AddTouchCallback(TactonicDevice device, TactonicTouchCallback callback);

	TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveTouchCallback(TactonicDevice device, TactonicTouchCallback callback);

	TACTONIC_TOUCH void WINAPI TactonicTouch_AddTouchRegionCallback(TactonicDevice device, TactonicTouchRegion touchRegion, TactonicTouchCallback callback);

	TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveTouchRegionCallback(TactonicDevice device, TactonicTouchRegion touchRegion, TactonicTouchCallback callback);

	TACTONIC_TOUCH void WINAPI TactonicTouch_AddGestureCallback(TactonicDevice device, TactonicGestureType gestureType, TactonicGestureCallback callback);

	TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveGestureCallback(TactonicDevice device, TactonicGestureType gestureType,  TactonicGestureCallback callback);

	TACTONIC_TOUCH void WINAPI TactonicTouch_AddGestureRegionCallback(TactonicDevice device, TactonicTouchRegion touchArea, TactonicGestureType gestureType, TactonicGestureCallback callback);

	TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveGestureRegionCallback(TactonicDevice device, TactonicTouchRegion touchArea, TactonicGestureType gestureType,  TactonicGestureCallback callback);

	TACTONIC_TOUCH void WINAPI TactonicTouch_AddFilterCallback(TactonicDevice device, TactonicTouchFilterCallback callback);

	TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveFilterCallback(TactonicDevice device, TactonicTouchFilterCallback callback);
   
	TACTONIC_TOUCH void WINAPI TactonicTouch_PollFrame(TactonicDevice device, TactonicTouchFrame* frame);

	TACTONIC_TOUCH void WINAPI TactonicTouch_CopyFrame(TactonicTouchFrame* srcFrame, TactonicTouchFrame* dstFrame);
}

#endif // __TACTONICTOUCH_H__
