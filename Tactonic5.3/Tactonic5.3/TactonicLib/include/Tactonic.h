// ====================================================================
// Copyright 2010-2012 Tactonics Technologies, LLC. All rights reserved
// PROPRIETARY/CONFIDENTIAL - DO NOT DISTRIBUTE!
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
// =====================================================================
#ifndef __TACTONIC_H__
#define __TACTONIC_H__

#ifdef _WIN32
#include <windows.h>
#define TACTONIC_LIB __declspec(dllexport)
#else
#define TACTONIC_LIB
#define WINAPI
#endif


extern "C" {
    
	enum TactonicDeviceEventType{			// The type of device event
		TACTONIC_DEVICE_STARTED,			// Device was started
		TACTONIC_DEVICE_STOPPED,			// Device was stopped
		TACTONIC_DEVICE_ERROR,				// Device error occured
		TACTONIC_DEVICE_TOPOLOGY_CHANGE		// Device topology has changed
	};
    
	enum TactonicErrorCode{					// For a device error, a more specific description
		TACTONIC_ERROR_OPEN,				// Device communication was not opened
		TACTONIC_ERROR_WRITE,				// Cannot write to the device
		TACTONIC_ERROR_READ,				// No data received from the device
		TACTONIC_ERROR_FIRMWARE,			// Incompabile firmware installed on device
		TACTONIC_ERROR_COMMUNICATION,		// Unexpected communication value recieved
	};
    
	typedef struct tactonic_device_s{		// A Tactonic Device, containing informaiton about the device
		unsigned short rows;				// The number of rows for the device
		unsigned short cols;				// The number of columns for the device
		unsigned short rowSpacingUM;		// The spacing between the rows in micrometers
		unsigned short colSpacingUM;		// The spacing between the cols in micrometers
		unsigned long serialNumber;         // The serial number of this device
	} TactonicDevice;

	
	typedef struct tactonic_device_list_s{	// A list of all the Tactonic Devices and the number of devices
		TactonicDevice *devices;			// An array of Tactonic Devices
		int numDevices;						// The number of Tactonic Devices
	} TactonicDeviceList;

	typedef struct tactonic_frame_s{		// Frame of data, containing the forces, the frame number and time stamp
        int* forces;						// An array of forces
        int numForces;                      // Number of values in the force array
        int rows;                           // Number of rows in the force array
        int cols;                           // Number of columns in the force array
		unsigned long frameNumber;			// The frame number for this data
		double time;						// The frame time since the device was started
	} TactonicFrame;
	
	typedef struct tactonic_frame_event_s{  // A Tactonic Frame Event, containing a TactonicDevice and a TactonicFrame
        TactonicDevice device;              // The TactonicDevice for the TactonicFrame
		TactonicFrame *frame;               // A pointer to a TactonicFrame
	} TactonicFrameEvent;
	
	typedef struct tactonic_device_event_s{ // A Tactonic Device Event, contains information about the event
        TactonicDevice device;              // The TactonicDevice for the device event
		TactonicDeviceEventType eventType;  // The type of Tactonic Device Event
        int eventCode;                      // A specific code for the Tactonic Device Event
	} TactonicDeviceEvent;
	
	// Typedef of a TactonicFrameCallback method used for obtaining frame events
	typedef void (*TactonicFrameCallback)(TactonicFrameEvent* frameEvent);
    
	// Typedef of a TactonicDeviceCallback method used for obtaining device events
	typedef void (*TactonicDeviceCallback)(TactonicDeviceEvent* deviceEvent);

	// Get the list of Tactonic Devices
	TACTONIC_LIB TactonicDeviceList* WINAPI Tactonic_GetDeviceList();
	
	// Start the device
	TACTONIC_LIB void WINAPI Tactonic_StartDevice(TactonicDevice device);

	// Stop the device
	TACTONIC_LIB void WINAPI Tactonic_StopDevice(TactonicDevice device);
	
	// Allocate a frame of data for the device
	TACTONIC_LIB TactonicFrame* WINAPI Tactonic_CreateFrame(TactonicDevice device);
	
	// Deallocate a frame of data for the device
	TACTONIC_LIB void WINAPI Tactonic_DestroyFrame(TactonicFrame* frame);
	
	// Poll the most recent frame of data from the device
	TACTONIC_LIB void WINAPI Tactonic_PollFrame(TactonicDevice device, TactonicFrame* frame);

	// Copy an allocated frame from a source to a destantation
	TACTONIC_LIB void WINAPI Tactonic_CopyFrame(TactonicFrame* srcFrame, TactonicFrame* dstFrame);
	
	// Add a frame callback method for a TactonicDevice
	TACTONIC_LIB void WINAPI Tactonic_AddFrameCallback(TactonicDevice device, TactonicFrameCallback callback);
	
	// Remove a frame callback method for a TactonicDevice
	TACTONIC_LIB void WINAPI Tactonic_RemoveFrameCallback(TactonicDevice device, TactonicFrameCallback callback);
    
    // Add a device callback method for a TactonicDevice
	TACTONIC_LIB void WINAPI Tactonic_AddDeviceCallback(TactonicDevice device, TactonicDeviceCallback callback);
	
    // Remove a device callback method for a TactonicDevice
	TACTONIC_LIB void WINAPI Tactonic_RemoveDeviceCallback(TactonicDevice device, TactonicDeviceCallback callback);
}

#endif // __TACTONIC_H__
