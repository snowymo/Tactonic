// ====================================================================
// Copyright 2010-2012 Tactonics Technologies, LLC. All rights reserved
// PROPRIETARY/CONFIDENTIAL - DO NOT DISTRIBUTE!
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
// ====================================================================

#ifdef _WIN32
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma  common(lib, "ws2_32.lib")
#else
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#define Sleep(a) usleep(a*1000)
#endif

#include <stdio.h>
#include <string>
#include <iostream>
#include "Tactonic.h"
#include "TactonicTouch.h"
#include "FeetStatistics.h"
#include "DataSender.h"

using namespace std;

TactonicDevice device;              // The device that we are monitoring
TactonicFrame *frame;               // An Anti-Aliased Frame of data from the device
TactonicTouchFrame *touchFrame;     // A list of touches that were detected for a given TactonicFrame
DataSender dataSender;

void frameCallback(TactonicFrameEvent* evt);
void touchCallback(TactonicTouchEvent* evt);
void sendData(TactonicFrame* frame);


int main(int argc, char *argv[]) {
	TactonicDeviceList *deviceList;                              // Data Structure for retrieving available devices
	deviceList = Tactonic_GetDeviceList();                        // Get available devices

	Sleep(1000);
	if (deviceList->numDevices > 0){                              // Set up the Tactonic Device and register the callback function
		device = deviceList->devices[0];                         // Get the device
		frame = Tactonic_CreateFrame(device);                   // Create a TactonicFrame for this device
		TactonicTouch_CreateDetector(device);                   // Create the touch detector for the device
		TactonicTouch_StartDetector(device);                    // Start the touch detector
		Tactonic_AddFrameCallback(device, frameCallback);       // Add a TactonicFrame callback method
		Tactonic_StartDevice(device);                           // Start the device
		while (1) {
			sendData(frame);
			Sleep(15);
		}

	}
	else{                                                       // If no devices, close the program
		printf("No devices available.");
		Sleep(1000);
	}

	return 0;
}

void frameCallback(TactonicFrameEvent* evt){            // TactonicFrame callback registered in the main
	Tactonic_CopyFrame(evt->frame, frame);               // Copy the callback frame to a local frame
}

FeetStatistics feetStatistics;
CENTROID c_Bothfeet;//centroid of both feet
CENTROID c_Left;//centroid of left foot
CENTROID c_Right;//centroid of right foot

void sendData(TactonicFrame* frame){
	//initialize all data to zero
	c_Bothfeet.col = 0;
	c_Bothfeet.row = 0;
	c_Left.col = 0;
	c_Left.row = 0;
	c_Right.col = 0;
	c_Right.row = 0;
	string msg = "";

	feetStatistics.computeCentroid(frame->forces, c_Bothfeet, 0, 80);//compute centroid of both feet
	feetStatistics.computeCentroid(frame->forces, c_Left, 0, 40);//compute centroid of left foot
	feetStatistics.computeCentroid(frame->forces, c_Right, 40, 80);//compute centroid of right foot

	msg = to_string(c_Bothfeet.col) + "  " + to_string(c_Bothfeet.row) + "  " \
		+ to_string(c_Left.col) + "  " + to_string(c_Left.row) + "  "\
		+ to_string(c_Right.col) + "  " + to_string(c_Right.row);

	dataSender.sendTo("127.0.0.1", 9090, msg.c_str(), msg.size(), 0);//post msg to localhost
}
//
//#include "DataSender.h"
//#include "Tactonic.h"
//#include "TactonicTouch.h"
//#include "DisplayViews.h"
//
//TactonicDevice device;              // The device that we are monitoring
//TactonicFrame *frame;               // An Anti-Aliased Frame of data from the device
//TactonicTouchFrame *touchFrame;     // A list of touches that were detected for a given TactonicFrame
//DisplayViews viewer;                // Renders the Visualizer Views 
//DataSender dataSender;
//
//void frameCallback(TactonicFrameEvent* evt);
//void touchCallback(TactonicTouchEvent* evt);
//void display();
//void keyEvent(unsigned char key, int x, int y);
//void reshape(GLsizei width, GLsizei height);
//
//int main(int argc, char *argv[]) {
//    
//	glutInit(&argc,argv);                                       // Set up OpenGL for rendering the Visualization
//	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);                // Standard OpenGL Initialization
//	glutInitWindowSize(WIDTH, HEIGHT);                          // Standard OpenGL Initialization
//	glutCreateWindow("Tactonic Technologies");                  // Standard OpenGL Initialization
//	glutKeyboardFunc(keyEvent);                                 // Register OpenGL Key Handler		
//	glutDisplayFunc(display);                                   // Register OpenGL Rendering Handler
//	glutIdleFunc(display);                                      // Register OpenGL Idle Handler
//	glutReshapeFunc(reshape);                                   // Register OpenGL Reshape Handler
//	TactonicDeviceList *deviceList;                              // Data Structure for retrieving available devices
//	deviceList = Tactonic_GetDeviceList();                        // Get available devices
//    
//    Sleep(1000);
//	if(deviceList->numDevices > 0){                              // Set up the Tactonic Device and register the callback function
//		device = deviceList->devices[0];                         // Get the device
//		viewer.init(device, argc, argv);							// Initialize the View Renderer
//        frame = Tactonic_CreateFrame(device);                   // Create a TactonicFrame for this device
//        touchFrame = TactonicTouch_CreateFrame(device);         // Create a TactonicTouchFrame for this device
//		TactonicTouch_CreateDetector(device);                   // Create the touch detector for the device
//		TactonicTouch_AddTouchCallback(device, touchCallback);  // Add a TactonicTouchFrame callback method
//		TactonicTouch_StartDetector(device);                    // Start the touch detector
//		Tactonic_AddFrameCallback(device, frameCallback);       // Add a TactonicFrame callback method
//		Tactonic_StartDevice(device);                           // Start the device
//		glutMainLoop();                                         // Start the GLUT display loop
//	}
//	else{                                                       // If no devices, close the program
//		printf("No devices available.");
//		Sleep(1000);
//	}
//	return 0;
//}
//
//void frameCallback(TactonicFrameEvent* evt){            // TactonicFrame callback registered in the main
//    Tactonic_CopyFrame(evt->frame,frame);               // Copy the callback frame to a local frame
//}
//
//void touchCallback(TactonicTouchEvent* evt){            // TactonicTouchFrame callback registered in the main
//    TactonicTouch_CopyFrame(evt->frame, touchFrame);    // Copy the callback touch frame to a local frame
//    viewer.registerTouches(touchFrame);                 // Register the touch frame in the display views
//}
//
//void display(){											// Glut display loop
//	viewer.display(frame, touchFrame);                  // Render the current frame
//	string buffer = viewer.compressData(frame);
//	cout << buffer << endl;
//	dataSender.sendTo("127.0.0.1", 9090, buffer.c_str(), buffer.size(), 0);
//}
//
//void keyEvent(unsigned char key, int x, int y){			// Glut key event
//	viewer.keyEvent(key);								// Send the key event to the visualizer so it may change modes
//}
//
//void reshape(GLsizei width, GLsizei height){			// Glut window resized
//	viewer.reshape(width,height);						// Window was resized, send new shape to renderer
//}