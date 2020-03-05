// =============================================-%""rtuio1=======================
// Copyright 2010-2012 Tactonics Technologies, LLC. All rights reserved
// PROPRIETARY/CONFIDENTIAL - DO NOT DISTRIBUTE!
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
// ====================================================================

#include "UDPClient.h"
#include "Tactonic.h"
#include "TactonicTouch.h"
#include "DisplayViews.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <string>
#include <ctime> 
using namespace std::chrono;

#pragma comment (lib, "Ws2_32.lib")

UDPClient* udpClient = new UDPClient("172.24.71.214");

TactonicDevice device;              // The device that we are monitoring
int deviceIndex = 0;
TactonicDevice originDev;
TactonicFrame *frame;               // An Anti-Aliased Frame of data from the device
TactonicFrame *originFrame;
TactonicTouchFrame *touchFrame;     // A list of touches that were detected for a given TactonicFrame
DisplayViews viewer;                // Renders the Visualizer Views 

bool record;						//is recording flag
string fileName0, fileName1;		//var for file names
ofstream recorderStream;
int counter;						//incrementor for filenames

void frameCallback(TactonicFrameEvent* evt);
void touchCallback(TactonicTouchEvent* evt);
void display();
void keyEvent(unsigned char key, int x, int y);
void reshape(GLsizei width, GLsizei height);

void writeData(TactonicFrame* frame);			//obviously for writing data to a file

std::string fileprefix = "";

int deviceCols = 32;
int deviceRows = 32;

int main(int argc, char *argv[]) {
    if (argc == 2)
        fileprefix = argv[1];
    glutInit(&argc, argv);                                       // Set up OpenGL for rendering the Visualization
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);                // Standard OpenGL Initialization
    glutInitWindowSize(WIDTH, HEIGHT);                          // Standard OpenGL Initialization
    glutCreateWindow("Tactonic Technologies");                  // Standard OpenGL Initialization
    glutKeyboardFunc(keyEvent);                                 // Register OpenGL Key Handler		
    glutDisplayFunc(display);                                   // Register OpenGL Rendering Handler
    glutIdleFunc(display);                                      // Register OpenGL Idle Handler
    glutReshapeFunc(reshape);                                   // Register OpenGL Reshape Handler
    TactonicDeviceList *deviceList;                              // Data Structure for retrieving available devices
    deviceList = Tactonic_GetDeviceList();                        // Get available devices

    record = false;												//initiate record as false
    counter = 0;												//initiate counter
    Sleep(1000);
    if (deviceList->numDevices > 0) {                              // Set up the Tactonic Device and register the callback function
        originDev = deviceList->devices[deviceIndex];                         // Get the device
        device = originDev;
        device.cols = deviceCols;
        device.rows = deviceRows;
        viewer.init(device, argc, argv);							// Initialize the View Renderer
        originFrame = Tactonic_CreateFrame(originDev);
        frame = Tactonic_CreateFrame(device);                   // Create a TactonicFrame for this device
        touchFrame = TactonicTouch_CreateFrame(originDev);         // Create a TactonicTouchFrame for this device
        Tactonic_AddFrameCallback(originDev, frameCallback);       // Add a TactonicFrame callback method
        TactonicTouch_CreateDetector(device);                   // Create the touch detector for the device
        TactonicTouch_AddTouchCallback(device, touchCallback);  // Add a TactonicTouchFrame callback method
        TactonicTouch_StartDetector(device);                    // Start the touch detector
        Tactonic_StartDevice(device);                           // Start the device
        glutMainLoop();                                         // Start the GLUT display loop
    }
    else {                                                       // If no devices, close the program
        printf("No devices available.");
        Sleep(1000);
    }
    return 0;
}

void frameCallback(TactonicFrameEvent* evt) {            // TactonicFrame callback registered in the main
    Tactonic_CopyFrame(evt->frame, originFrame);               // Copy the callback frame to a local frame
    int arrayIndex = 0;
    int largestForce = 0, largestRow = 0, largestCol = 0;
    for (int i = 0; i < deviceRows; i++)
//    for (int i = 16; i < 48; i++)
    {
        for (int j = 0; j < deviceCols; j++)
        //for (int j = 75; j > 63; j--)
        {
            frame->forces[arrayIndex] = originFrame->forces[i * originFrame->cols + j];
            if (frame->forces[arrayIndex] > largestForce) {
                largestForce = frame->forces[arrayIndex];
                largestRow = i;
                largestCol = j;
            }
            arrayIndex++;
        }
    }
    if(largestForce > 0)
        std::cout << largestRow << "," << largestCol << ":" << largestForce << "\n";

    // send the data
    //udpClient->sendMsg(std::to_string(frame->cols) + " " + std::to_string(frame->rows));
    milliseconds ms = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    long long msll = ms.count();
    // copy the force to my buffer, send it with the timestamp, add a new func for byte array
}

void touchCallback(TactonicTouchEvent* evt) {            // TactonicTouchFrame callback registered in the main
    TactonicTouch_CopyFrame(evt->frame, touchFrame);    // Copy the callback touch frame to a local frame
    viewer.registerTouches(touchFrame);                 // Register the touch frame in the display views
}

void display() {											// Glut display loop
    writeData(frame);
    viewer.display(frame, touchFrame);                  // Render the current frame
}

void keyEvent(unsigned char key, int x, int y) {			// Glut key event
    if (key == 'r' || key == 'R') {

        viewer.keyEvent((record ? '1' : '2'));
        if (record == false) {							//create the filename, open the file for recording
            /*if (deviceIndex == 0)
                fileName0 = counter + "987654321_LeftfootData.js";
            else
                fileName0 = counter + "987654321_RightfootData.js";*/
            time_t t = time(0);   // get time now
            struct tm * now = localtime(&t);
            char buffer[80];
            strftime(buffer, 80, "%Y-%m-%d-%H-%M-%S", now);

            fileName0 = std::string(buffer)+"_"+fileprefix+".js";
            recorderStream.open(fileName0);
            recorderStream << "window.footData = [" << endl;
        }
        else {											//close file, wipe file name, increment counter
            recorderStream << "];";
            recorderStream.close();
            counter++;
        }
        record = !record;								//toggle record flag
        //cout << "recording";
    }
    else viewer.keyEvent(key);								// Send the key event to the visualizer so it may change modes
}

void reshape(GLsizei width, GLsizei height) {			// Glut window resized
    viewer.reshape(width, height);						// Window was resized, send new shape to renderer
}

void writeData(TactonicFrame* frame) {
    int value = 0;
    recorderStream << "[";
    milliseconds ms = duration_cast<milliseconds>(
        system_clock::now().time_since_epoch()
        );
    recorderStream << std::to_string(ms.count()) << "\t";
    for (int i = 0; i < 384; i++) {

        value = frame->forces[i];
        recorderStream << value << ", ";
    }

    recorderStream << "]," << endl;
}