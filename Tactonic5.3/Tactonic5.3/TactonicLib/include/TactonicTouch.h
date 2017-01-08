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

enum  TactonicTouchType{		// Types of touches
	TOUCH_DOWN,					// Touch started, put down
	TOUCH_MOVE,					// Touch moved, continued
	TOUCH_UP					// Touch released
};

extern "C" {
	typedef struct tactonic_touch_s{		// A TactonicTouch
		int id;								// An id for an individual touch					
		float x, y, force;					// Position and force on device
		float dx, dy, dForce;				// Changes in position and force from last framev
		TactonicTouchType touchtype;		// Type of touch
	} TactonicTouch;

	typedef struct tactonic_touch_frame_s{	// A frame of TactonicTouches
		TactonicTouch *touches;             // The array of touches
		int numTouches;						// The number of active touches in the array
		long frameNumber;					// The frame number of this touch frame
		double time;						// The time stamp for the touch frame
	} TactonicTouchFrame;
    
    typedef struct tactonic_touch_event_s{  // A event for a TactonicFrame
        TactonicDevice device;              // The device for the Tactonic Touch Frame
		TactonicTouchFrame *frame;          // A pointer to a Tactonic Touch Frame
	} TactonicTouchEvent;
    
    // Typedef of a TactonicTouchCallback method used for obtaining touch events
	typedef void (*TactonicTouchCallback)(TactonicTouchEvent* touchEvent);
    
    // Typedef of a TactonicTouchFilterCallback method
    // Return the TactonicFrame to be copied to the Tactonic Touch Detector Queue
	typedef TactonicFrame* (*TactonicTouchFilterCallback)(TactonicFrameEvent* evt);
    
	// Creates a touch detector for a TactonicDevice
	TACTONIC_TOUCH void WINAPI TactonicTouch_CreateDetector(TactonicDevice device);
    
    // Destroy a touch detector for a TactonicDevice
	TACTONIC_TOUCH void WINAPI TactonicTouch_DestroyDetector(TactonicDevice device);

	// Starts the touch detector thread
	TACTONIC_TOUCH void WINAPI TactonicTouch_StartDetector(TactonicDevice device);

	// Stops the touch detector thread
	TACTONIC_TOUCH void WINAPI TactonicTouch_StopDetector(TactonicDevice device);
    
    // Create a TactonicTouchFrame for a device
	TACTONIC_TOUCH TactonicTouchFrame* WINAPI TactonicTouch_CreateFrame(TactonicDevice device);
	
    // Destroy a TactonicTouchFrame for a device
	TACTONIC_TOUCH void WINAPI TactonicTouch_DestroyFrame(TactonicTouchFrame* frame);

	// Add a touch callback method for a detector
	TACTONIC_TOUCH void WINAPI TactonicTouch_AddTouchCallback(TactonicDevice device, TactonicTouchCallback callback);
    
    // Remove a touch callback method for a detector
	TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveTouchCallback(TactonicDevice device, TactonicTouchCallback callback);
    
	// Add a touch filter callback method for a detector
	TACTONIC_TOUCH void WINAPI TactonicTouch_AddFilterCallback(TactonicDevice device, TactonicTouchFilterCallback callback);
    
	// Remove a touch filter callback method for a detector
	TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveFilterCallback(TactonicDevice device, TactonicTouchFilterCallback callback);
    

	// Poll the most recent frame of touch data
	TACTONIC_TOUCH void WINAPI TactonicTouch_PollFrame(TactonicDevice device, TactonicTouchFrame* frame);
	
	// Copy on touch frame to another
	TACTONIC_TOUCH void WINAPI TactonicTouch_CopyFrame(TactonicTouchFrame* srcFrame, TactonicTouchFrame* dstFrame);
}

#endif // __TACTONICTOUCH_H__
