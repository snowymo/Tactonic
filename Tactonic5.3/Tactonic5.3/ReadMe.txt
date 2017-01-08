====================================================================
Copyright 2010-2012 Tactonics Technologies, LLC. All rights reserved
DO NOT DISTRIBUTE!
DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
====================================================================

SETUP

	To modify/build the source provided you must have Microsoft Visual Studio C++ 2010 or Apple's Xcode 4.0 or later installed.

Note: Mac users should execute the following commands to remove conflicting FTDI libraries before running the sample visualizer or developing with a Tactonic Device.

cd /System/Library/Extensions
rm -r FTDIUSBSerialDriver.kext
cd /Library/Receipts
rm -r ftdiusbserialdriver.pkg
rm -r ftdiusbserialdriverinstallerPostflight.pkg
rm -r ftdiusbserialdriverinstallerPreflight.pkg

RUNNING THE SAMPLE VISUALIZER 

	1. Be sure the device is plugged into a USB Port
	2. Run TactonicVisualizerWin.exe for Windows or TactonicVisualizerMac for Mac
		- This program is found in <tactonic install>\TactonicDemos\

	3. The device is now ready to visualize touches and may be visualized in four views that may be toggled
	   by hitting the following keys
		
		'1' - Guassian Blurr View
	   	'2' - Sensel View, colors representing the actual pressure at each sensel in the grid
	   	'3' - Text Value View
	   	'4' - Sample Multi-Touch Detection
	   	'5' - Draw Line Mode
			*To clear the screen in Draw Mode, hit 'Esc'

	Note: The source code for this sample visualizer is provided. See notes below
		
FOR DEVELOPERS	

	Tactonic sensors, along with the provided support libraries, can act as a:		
		- Pressure Camera:  Providing Frames of Pressure Images		
		- Multi-Touch Device : Providing Pressure sensing Touch Events	

	There are two modes of obtaining either Pressure Images or Touch Events:	
		- Event Notification: Notifying client applications as data becomes available		
		- Polling:  Allowing client applications to request information as needed.	

	Typically event notification is the best choice providing the lowest latency

	To best understand the Tactonic API and programming model, we have provided the full commented source and support libraries for the Sample Visualizer as both a Microsoft Visual Studio and as an XCode project.		
		- The Visualizer uses the event driven programming model.		
		- Prior to experimenting with your own application, we strongly recommend that you do a clean build of the provided project and verify that it runs properly	

	If you have any questions of difficulty, contact: softwareSupport@tactonic.com


SAMPLE VISUALIZER PROJECT

	Full commented source and support libraries are provided for the Sample Visualizer (described above)
	
	A. The sample visualizer source code is provided as a Visual Studio project
		- TactonicSource.sln can be found in the "TactonicSource" folder

	B. The visualizer solution contains one project:
		TactonicVisualizer 		- Visualizer project that uses a callback to obtain frames

	C. Source Files

		TactonicVisualizer.cpp 		- Visualizer example that uses a callback to obtain frames
			
			* Initializes openGL for the visualization
			* Shows a typical Tactonic Device initialization sequence			
			* Registers callbacks to process frame
			* All data processing and visualization is done in DisplayViews.cpp

		DisplayViews.cpp		- Visualization
			
			* Processes all the data coming from the device			
			* Renders the data (based on the current view type 				
				'1' - Guassian Blurr View			   	
				'2' - Sensel View, colors representing the actual pressure at each sensel in the grid			   
				'3' - Text Value View			   	
				'4' - Sample Multi-Touch Detection			   	
				'5' - Draw Line Mode

		Tactonic.h			- Tactonic Core Library Header

		TactonicTouch.h			- Tactonic Touch Library Header

		DisplayViews.h			- Visualization Header

	D.   Primary Structs

		1. TactonicDeviceList contains a list of all available TactonicDevice.
			devices: An array of all available TactonicDevice.
			numDevices: An integer that indicates the number of available devices.
			
		2. TactonicDevice provides information about an attached device.
			rows: A short that gives the number of rows for the device.
			cols: A short that gives the number of cols for the device.
			rowSpacingUM: A short that gives the spacing in micrometers between rows (i.e. 6.5mm = 6500um).
			colSpacingUM: A short that gives the spacing in micrometers between cols (i.e. 6.5mm = 6500um).
			serialNumber: A long that gives the serial number for the device.
			
		3. TactonicFrame provides values for a single frame of force data.
			forces: An integer array with length rows*cols that holds one frame of force data.
			numForces: Number of values in the force array.
			rows: A short that gives the number of rows for the frame.
			cols: A short that gives the number of cols for the frame.
			frameNumber: A long that gives the frame number for this device.
			time: The time for the frame. Measured since the device was started (See Tactonic_StartDevice)
			initialized: A flag designating whether the forces array has been initialized and allocated. 
		
		4. TactonicFrameEvent is the stuct passed to a TactonicFrameCallback when a new frame is available
			device: The TactonicDevice for the event
			frame: A pointer to a TactonicFrame.

		5. TactonicDeviceEvent is the struct passed to a TactonicDeviceCallback when an event occurs on the device (Start, Stop, Error, etc)
			device: The TactonicDevice for this event.
			eventType: An enum that corresponds to the type of event.
			eventCode: An integer that contains specific information on the event type.

	E. Primary Method Calls:
		1.  Tactonic_GetDeviceList: Get the array of available TactonicDevice.
		2.  Tactonic_StartDevice: Start a TactonicDevice.
		3.  Tactonic_StopDevice: Stop a TactonicDevice.
		4.  Tactonic_CreateFrame: Create a frame of data for a device.
		5.  Tactonic_DestroyFrame: Destroy a frame of data for a device.
		6.  Tactonic_PollFrame: Get the most recent frame of data from the device.
		7.  Tactonic_CopyFrame: Copy a Tactonic Frame.
		8.  Tactonic_AddFrameCallback: Add a TactonicFrameCallback for a device.
		9.  Tactonic_RemoveFrameCallback: Remove a TactonicFrameCallback for a device.
		10. Tactonic_AddDeviceCallback: Add a TactonicDeviceCallback for a device.
		11. Tactonic_RemoveDeviceCallback: Remove a TactonicDeviceCallback for a device.

	F. Get frames from a Tactonic device from a callback:
		1. Request the current TaconicDeviceList with "Tactonic_GetDeviceList"
		2. Choose a device from the TactonicDeviceList and call "Tactonic_StartDevice" to start the device
		3. Allocate a TactonicFrame for the device with "Tactonic_CreateFrame"
		4. Register a callback by passing an instance of a TactonicCallback method to "Tactonic_RegisterCallback"
		5. Frames are sent to the TactonicCallback method and are copied using "Tactonic_CopyFrame" 

	Note: The Tactonic libraries are provided as part of this project.

MEMORY MANAGEMENT
	
	The following sections provides a brief overview of the behavior and safety of struct memory

	A. TactonicDeviceList:
		"Tactonic_GeteviceList" returns a pointer to the current list of devices available. 
		Memory is safe until "Tactonic_GetDeviceList" is called again.

	B. TactonicDevice: 
		TactonicDevice is always safe and treated as pass by value.

	C. TactonicDeviceEvent
		TactonicDeviceEvent memory is only safe within the TactonicDeviceCallback method.
		Once the method is exited, the event may change.

	D. TactonicFrameEvent
		TactonicFrame memory within a TactonicFrameEvent is only safe within the TactonicFrameCallback method.
		Proper allocation and copying of the TactonicFrame are described in the next section.

	E. TactonicFrame
		To save a TactonicFrame for safe use outside the callback method:
			1. Allocate a TactonicFrame using "Tactonic_CreateFrame" (Typically at the start of the program).
			2. Copy the TactonicFrame within the callback using "Tactonic_CopyFrame".
			3. Destroy a TactonicFrame using "Tactonic_DestroyFrame" (Typically at the end of the program).
		To poll the device and recieve a safe TactonicFrame copy.
			1. Allocate a TactonicFrame using "Tactonic_CreateFrame" (Typically at the start of the program).
			2. Poll the device using "Tactonic_PollFrame".
			3. Destroy a TactonicFrame using "Tactonic_DestroyFrame" (Typically at the end of the program).
		



	