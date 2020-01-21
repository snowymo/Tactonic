// ===========================================
// Copyright 2010-2011 Tactonics Technologies, LLC. All rights reserved
// PROPRIETARY/CONFIDENTIAL - DO NOT DISTRIBUTE!
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
// ===========================================

#include <Windows.h>
#include <stdlib.h>
#include <mmsystem.h>
#include <tchar.h>
#include <math.h>
#include "Tactonic.h"
#include "TactonicTouch.h"
#include <time.h>

TactonicDevice device;			// The device that we are monitoring
TactonicFrame *frame;			// An Anti-Aliased Frame of data from the device
TactonicTouchFrame *touchFrame;

int PRESS = 3500;
int RELEASE = 2500;
int LEFT_DIFF = 200;
int RIGHT_DIFF = 200;

int width, height;
bool leftDown = false;
bool rightDown = false;
bool absolute = false;
bool relative = true;
int absoluteTrack = -1;
INPUT Input;
clock_t mouseClickTime;
double currentX, currentY;

void pressLeft(){
	if(!leftDown && !rightDown){
		ZeroMemory(&Input, sizeof(INPUT) * 1);
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
		SendInput(1, &Input, sizeof(INPUT));
		leftDown = true;
		mouseClickTime = clock();
		PlaySound("click.wav", NULL, SND_ASYNC);
	}
}

void releaseLeft(){
	if(leftDown){
		ZeroMemory(&Input, sizeof(INPUT) * 1);
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
		SendInput(1, &Input, sizeof(INPUT));
		leftDown = false;
		PlaySound("release.wav", NULL, SND_ASYNC);
	}
}

void pressRight(){
	if(!rightDown && !leftDown){
		ZeroMemory(&Input, sizeof(INPUT) * 1);
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
		SendInput(1, &Input, sizeof(INPUT));
		rightDown = true;
		mouseClickTime = clock();
		PlaySound("click.wav", NULL, SND_ASYNC);
	}
}

void releaseRight(){
	if(rightDown){
		ZeroMemory(&Input, sizeof(INPUT) * 1);
		Input.type = INPUT_MOUSE;
		Input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		SendInput(1, &Input, sizeof(INPUT));
		rightDown = false;
		PlaySound("release.wav", NULL, SND_ASYNC);
	}
}


void scrollWheel(double dist, double dist2){
	if(!rightDown){
		if(fabs(dist) > fabs(dist2)){
			ZeroMemory(&Input, sizeof(INPUT) * 1);
			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_WHEEL;
			Input.mi.mouseData  = (DWORD)dist;
			SendInput(1, &Input, sizeof(INPUT));
		}
		else{
			ZeroMemory(&Input, sizeof(INPUT) * 1);
			Input.type = INPUT_MOUSE;
			Input.mi.dwFlags = MOUSEEVENTF_HWHEEL;
			Input.mi.mouseData  = (DWORD)dist2;
			SendInput(1, &Input, sizeof(INPUT));
		}
	}
}

void checkClick(){
	if(touchFrame->numTouches == 1){
		releaseRight();
		TactonicTouch t = touchFrame->touches[0];
		if(t.force > PRESS){
			pressLeft();
		}
		else if(t.force < RELEASE){
			releaseLeft();
		}
	}
	else if(touchFrame->numTouches == 2){
		TactonicTouch t1 = touchFrame->touches[0];
		TactonicTouch t2 = touchFrame->touches[1];
		if(t1.force+t2.force > 1.5*PRESS){
			pressRight();
		}
		else if(t1.force+t2.force < 1.5*RELEASE){
			releaseRight();
		}
	}
	else{
		releaseLeft();
		releaseRight();
	}
}

void absoluteMouse(){
	if(touchFrame->numTouches == 1){
		TactonicTouch t = touchFrame->touches[0];
		
		if(absoluteTrack == -1){
			absoluteTrack = t.id;
		}
		if(t.force > 400 && absoluteTrack > 0 && t.dForce < LEFT_DIFF && t.dForce > -LEFT_DIFF && ((double)mouseClickTime-(double)clock())/(1.0*CLOCKS_PER_SEC) < -0.3){
			currentX = (int)((t.x)/(6.5*32.0)*width);
			currentY = (int)(((32.0*6.5-t.y)/(6.5*32.0))*height);
			SetCursorPos((int)currentX, (int)currentY);
		}
	}
	else if(touchFrame->numTouches == 2){
		absoluteTrack = -2;
		TactonicTouch t1 = touchFrame->touches[0];
		TactonicTouch t2 = touchFrame->touches[1];
		double dx = (t1.dx+t2.dx)*width/100;
		double dy = -(t1.dy+t2.dy)*height/100;
		if(t1.dForce+t2.dForce < RIGHT_DIFF && t1.dForce+t2.dForce > -RIGHT_DIFF && ((double)mouseClickTime-(double)clock())/(1.0*CLOCKS_PER_SEC) < -0.3){
			scrollWheel(-dy*8, dx*8);
		}
	}
	else{
		absoluteTrack = -1;
	}
	
	checkClick();
	
}


void relativeMouse(){
	if(touchFrame->numTouches == 1){
		TactonicTouch t = touchFrame->touches[0];
		float dx = t.dx*width/100;
		float dy = -t.dy*height/100;
		if(t.force > 400 && t.dForce < LEFT_DIFF && t.dForce > -LEFT_DIFF  && ((double)mouseClickTime-(double)clock())/(1.0*CLOCKS_PER_SEC) < -0.1){
			
			currentX = currentX + dx;
			currentY = currentY + dy;
			SetCursorPos((int)(currentX), (int)(currentY));
		}
	}
	else if(touchFrame->numTouches == 2){
		TactonicTouch t1 = touchFrame->touches[0];
		TactonicTouch t2 = touchFrame->touches[1];
		double dx = (t1.dx+t2.dx)*width/100;
		double dy = -(t1.dy+t2.dy)*height/100;
		if(t1.dForce+t2.dForce < RIGHT_DIFF && t1.dForce+t2.dForce > -RIGHT_DIFF && ((double)mouseClickTime-(double)clock())/(1.0*CLOCKS_PER_SEC) < -0.1){
			scrollWheel(-dy*8, dx*8);
		}
		
	}
	checkClick();
}


void touchCallback(TactonicTouchEvent* event){
	//if(device.serialNumber == event->device.serialNumber){
		TactonicTouch_CopyFrame(event->frame, touchFrame);
		if(relative)
			relativeMouse();
		if(absolute)
			absoluteMouse();
	//}
}

void frameCallback(TactonicFrameEvent* event){
		Tactonic_CopyFrame(event->frame,frame);
}



int _tmain(int argc, _TCHAR* argv[]){
    POINT pos;
	GetCursorPos(&pos);
	currentX = pos.x;
	currentY = pos.y;

	width = GetSystemMetrics(SM_CXSCREEN);
	height = GetSystemMetrics(SM_CYSCREEN);
	TactonicDeviceList *deviceList;							// Data Structure for retrieving available devices
	deviceList = Tactonic_GetDeviceList();					// Get available devices

	if(deviceList->numDevices > 0){	
		device = deviceList->devices[0];						// Get the device	
		
		frame = Tactonic_CreateFrame(device);
        touchFrame = TactonicTouch_CreateFrame(device);
		TactonicTouch_CreateDetector(device);
		TactonicTouch_AddTouchCallback(device, touchCallback);
		
		Tactonic_AddFrameCallback(device, frameCallback);	// Register a TactonicFrame Callback Method
		TactonicTouch_StartDetector(device);
		Tactonic_StartDevice(device);						// Tell the driver to start this device
	}
	else{
		printf("No devices available.");
		Sleep(1000);
	}
	char ch = 'r';
	while(ch != 'q'){
		if(relative){
			printf("You are in relative mode.\n");
		}
		if(absolute){
			printf("You are in absolute mode.\n");
		}
		printf("Enter a for absolute, r for relative, q to quit:\n");
		ch = getc(stdin);
		getc(stdin);
		if(ch == 'a'){
			absolute = true;
			relative = false;
		}
		if(ch == 'r'){
			relative = true;
			absolute = false;
		}
	}
	return 0;
}