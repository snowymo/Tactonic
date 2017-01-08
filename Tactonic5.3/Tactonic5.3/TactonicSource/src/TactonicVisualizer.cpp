// ====================================================================
// Copyright 2010-2012 Tactonics Technologies, LLC. All rights reserved
// PROPRIETARY/CONFIDENTIAL - DO NOT DISTRIBUTE!
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
// ====================================================================

#include "Tactonic.h"
#include "TactonicTouch.h"
#include "DisplayViews.h"

TactonicDevice device;              // The device that we are monitoring
TactonicFrame *frame;               // An Anti-Aliased Frame of data from the device
TactonicTouchFrame *touchFrame;     // A list of touches that were detected for a given TactonicFrame
DisplayViews viewer;                // Renders the Visualizer Views 

void frameCallback(TactonicFrameEvent* evt);
void touchCallback(TactonicTouchEvent* evt);
void display();
void keyEvent(unsigned char key, int x, int y);
void reshape(GLsizei width, GLsizei height);

int main(int argc, char *argv[]) {
    
	glutInit(&argc,argv);                                       // Set up OpenGL for rendering the Visualization
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);                // Standard OpenGL Initialization
	glutInitWindowSize(WIDTH, HEIGHT);                          // Standard OpenGL Initialization
	glutCreateWindow("Tactonic Technologies");                  // Standard OpenGL Initialization
	glutKeyboardFunc(keyEvent);                                 // Register OpenGL Key Handler		
	glutDisplayFunc(display);                                   // Register OpenGL Rendering Handler
	glutIdleFunc(display);                                      // Register OpenGL Idle Handler
	glutReshapeFunc(reshape);                                   // Register OpenGL Reshape Handler
	TactonicDeviceList *deviceList;                              // Data Structure for retrieving available devices
	deviceList = Tactonic_GetDeviceList();                        // Get available devices
    
    Sleep(1000);
	if(deviceList->numDevices > 0){                              // Set up the Tactonic Device and register the callback function
		device = deviceList->devices[0];                         // Get the device
		viewer.init(device, argc, argv);							// Initialize the View Renderer
        frame = Tactonic_CreateFrame(device);                   // Create a TactonicFrame for this device
        touchFrame = TactonicTouch_CreateFrame(device);         // Create a TactonicTouchFrame for this device
		TactonicTouch_CreateDetector(device);                   // Create the touch detector for the device
		TactonicTouch_AddTouchCallback(device, touchCallback);  // Add a TactonicTouchFrame callback method
		TactonicTouch_StartDetector(device);                    // Start the touch detector
		Tactonic_AddFrameCallback(device, frameCallback);       // Add a TactonicFrame callback method
		Tactonic_StartDevice(device);                           // Start the device
		glutMainLoop();                                         // Start the GLUT display loop
	}
	else{                                                       // If no devices, close the program
		printf("No devices available.");
		Sleep(1000);
	}
	return 0;
}

void frameCallback(TactonicFrameEvent* evt){            // TactonicFrame callback registered in the main
    Tactonic_CopyFrame(evt->frame,frame);               // Copy the callback frame to a local frame
}

void touchCallback(TactonicTouchEvent* evt){            // TactonicTouchFrame callback registered in the main
    TactonicTouch_CopyFrame(evt->frame, touchFrame);    // Copy the callback touch frame to a local frame
    viewer.registerTouches(touchFrame);                 // Register the touch frame in the display views
}

void display(){											// Glut display loop
	viewer.display(frame, touchFrame);                  // Render the current frame
}

void keyEvent(unsigned char key, int x, int y){			// Glut key event
	viewer.keyEvent(key);								// Send the key event to the visualizer so it may change modes
}

void reshape(GLsizei width, GLsizei height){			// Glut window resized
	viewer.reshape(width,height);						// Window was resized, send new shape to renderer
}