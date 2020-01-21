// ====================================================================
// Copyright 2010-2012 Tactonics Technologies, LLC. All rights reserved
// PROPRIETARY/CONFIDENTIAL - DO NOT DISTRIBUTE!
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
// ====================================================================
#ifdef _WIN32
#include <time.h>
#include <fstream>;
#include <iostream>;
#else
#include <sys/time.h>
#endif
#include "Tactonic.h"
#include "TactonicTouch.h"
#include "TactonicDev.h"
#include "DisplayViews.h"

TactonicDevice device;			// The device that we are monitoring
TactonicFrame *frame;			// An Anti-Aliased Frame of data from the device
TactonicTouchFrame *touchFrame;
DisplayViews viewer;
TactonicFrame *zeroFrame;
bool zeroed = false;
FILE *f1;
bool record = false;
int temp;

float xPos = 0.0f;
float yPos = 0.0f;

int	displayCount = 0;							// Count the number of display frames
long prevClock = 0;								// Previous clock time
unsigned long prevFrame = 0;					// Previous frame number

void touchCallback(TactonicTouchEvent* evt);
void gestureCallback(TactonicGestureEvent* evt);
void frameCallback(TactonicFrameEvent* evt);
void deviceCallback(TactonicDeviceEvent* evt);
void recordFrame(TactonicFrame* forceFrame);
void display();
//void newView();
//void setVert();
//void setHor();
void keyEvent(unsigned char key, int x, int y);
void updateFPS(TactonicFrame* frame);
void reshape(GLsizei width, GLsizei height);

void touchCallback(TactonicTouchEvent* evt){
	TactonicTouch_CopyFrame(evt->frame, touchFrame);
    viewer.registerTouches(touchFrame);
}

void gestureCallback(TactonicGestureEvent* evt){
	viewer.registerGesture(evt->gesture);
	if(evt->gesture->state == GESTURE_PERFORMED && evt->gesture->type >= GESTURE_TAP){
		printf("%d\n", evt->gesture->type);
	}
}

void frameCallback(TactonicFrameEvent* evt){
    Tactonic_CopyFrame(evt->frame,frame);
	if(record){
		recordFrame(evt->frame);
	}
}

void deviceCallback(TactonicDeviceEvent* evt){
  //  printf("%d %d\n",evt->eventType, evt->eventCode);
}

void recordFrame(TactonicFrame* forceFrame){
    int value;
    fprintf (f1, "[%d,%f]\n",  (int)(frame->frameNumber), frame->time);
		fprintf (f1, "[");
    for (int i = 0; i < device.rows; i++)
    {
        for (int j = 0; j < device.cols; j++)
        {
            value = (frame->forces[i*device.cols+j]);
            fprintf (f1, "%d", value);
            if(j != device.cols-1)
                fprintf (f1, ",");
        }
        if(i != device.rows-1)
            fprintf (f1, ",\n");
    }
    fprintf (f1, "]\n");
}

void display(){
    
    viewer.display(frame, touchFrame);
    updateFPS(frame);
}

void keyEvent(unsigned char key, int x, int y){				// Glut key event
	/*if(key == 'z' || key =='Z'){
        if(!zeroed){
            Tactonic_CopyFrame(frame, zeroFrame);
            zeroed = true;
        }
        else
            zeroed = false;
	}*/
    if(key == 'r' || key == 'R'){
        if(!record){
            record = !record;
            f1 = fopen("write.txt", "wt");
            fprintf (f1, "[%d,%d]\n",  device.rows, device.cols);
        }
        else{
            record = !record;
            fclose(f1);
        }
    }
	else if(key == 'f' || key == 'F'){
		if(record)
			recordFrame(frame);
	}
    else{
        viewer.keyEvent(key);
    }
}

void reshape(GLsizei width, GLsizei height){				// Glut window resized
	viewer.reshape(width,height);							// Window was resized, send new shape to renderer
    
}


void updateFPS(TactonicFrame* frame){
	if(displayCount > 100 || displayCount == 0){
#ifdef _WIN32
		SYSTEMTIME time;
		GetSystemTime(&time);
		unsigned long millis = time.wMinute*60000+(time.wSecond * 1000) + time.wMilliseconds;
#else
		timeval time;
		gettimeofday(&time, NULL);
		long millis = (time.tv_sec * 1000) + (time.tv_usec / 1000);
#endif
		if(displayCount > 100){
			double pollFPS = 1000*(frame->frameNumber-prevFrame)/(1.0*(millis-prevClock));
			printf("FPS: %f\n",pollFPS);
		}

		displayCount = 0;
		prevFrame =frame->frameNumber;
		prevClock = millis;
	}
	displayCount++;
}

int main(int argc, char *argv[]) {
	glutInit(&argc,argv);									// Standard OpenGL Initialization
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);			// Standard OpenGL Initialization
	glutInitWindowSize(WIDTH, HEIGHT);						// Standard OpenGL Initialization
	glutCreateWindow("Tactonic Technologies");				// Standard OpenGL Initialization
	glutKeyboardFunc(keyEvent);								// Register OpenGL Key Handler		
	glutDisplayFunc(display);								// Register OpenGL Rendering Handler
	glutIdleFunc(display);									// Register OpenGL Idle Handler
	glutReshapeFunc(reshape);                               // Register OpenGL Reshape Handler
	TactonicDeviceList *deviceList;							// Data Structure for retrieving available devices
	deviceList = Tactonic_GetDeviceList();					// Get available devices

	TactonicTouchRegion region;
	if(deviceList->numDevices > 0){	
		device = deviceList->devices[0];                         // Get the device	
		viewer.init(device, argc, argv);                           // Initialize the  View Renderer
        frame = Tactonic_CreateFrame(device);
        zeroFrame = Tactonic_CreateFrame(device);
        touchFrame = TactonicTouch_CreateFrame(device);
      //  Tactonic_SetDeviceControl(device, CALIBRATE_ON);
	//	Tactonic_AddDeviceCallback(device, deviceCallback);
		Tactonic_AddFrameCallback(device, frameCallback);       // Register a TactonicFrame Callback Method
        TactonicTouch_CreateDetector(device);
		TactonicTouch_AddTouchCallback(device, touchCallback);
		//TactonicTouch_AddGestureCallback(device, GESTURE_ALL, gestureCallback);
		Tactonic_StartDevice(device);                           // Tell the driver to start this device
		TactonicTouch_StartDetector(device);
		glutMainLoop();
	}
	else{
		printf("No devices available.");
		Sleep(1000);
	}
	return 0;
}