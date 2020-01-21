// ====================================================================
// Copyright 2010-2012 Tactonics Technologies, LLC. All rights reserved
// PROPRIETARY/CONFIDENTIAL - DO NOT DISTRIBUTE!
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
// ====================================================================

#pragma once
#include <math.h>
#include <stdlib.h>
#include <fstream>
#ifdef _WIN32
#include "glut.h"
#else
#include <GLUT/glut.h>
#define Sleep(a) usleep(a*1000)
#endif
#include "Tactonic.h"
#include "TactonicTouch.h"
#include "Contours.h"
using namespace std;

#define WIDTH 1280          // Default width
#define HEIGHT 720          // Default height
#define RED_VALUE 2000.0f   // Red color display value

enum  TactonicViews{	// The different visualizer views
	GAUSSIAN_VIEW,		// View displays a gaussian blurred signal
	SENSEL_VIEW,		// View displays each sensel as a colored square
	TEXT_VIEW,			// View displays touches
	TOUCH_VIEW,			// View displays touches
	LINE_VIEW,			// View displays touches
	GESTURE_VIEW,
	CONTOUR_VIEW
};

class DisplayViews
{
public:
	DisplayViews();
	~DisplayViews();
	
	TactonicDevice device;      // Device that is being displayed
	int	wPix;						// Width of view
	int	hPix;						// Height of view
	float xOffsetPix;
	float yOffsetPix;
	float colSpacingPix;
	float rowSpacingPix;
	bool centered;

	Contours *contours;
	int	view;					// The view being displayed
	float* blurForces;			// Gaussian blur force values
	int* contourForces;			// Gaussian blur force values
	unsigned char* blurColors;			// Gaussian blur force values
	float red, green, blue;		// Red, green and blue for displaying colors
    
	int levels[8];
	int levelLen;

    int posIndex;               // Position being filled for drawing lines
    bool pen[100000];   
    double xPosition[100000];   // X postion of touch
    double yPosition[100000];   // Y position of touch
    double zPosition[100000];   // Z position of touch (Force)
    double dx[100000];          // Difference in X position of touch from last frame
    double dy[100000];          // Difference in Y position of touch from last frame
    double dz[100000];          // Difference in Z position of touch from last frame (Force)
    int lineFrame;              // Frame number of the line view, used for showing clear instruction
	double gestureX, gestureY, gestureRotate, gestureScale, gestureLeanX, gestureLeanY;
    
    char fullPath [200];        // Full path to executable and logo image
    char *imageData;            // Logo image data
    float logoScale;            // Scale of logo image
    GLuint logoID;                  // Logo texture ID
    GLuint cursorID;                  // Logo texture ID
    GLuint penID;                  // Logo texture ID
    GLuint gaussianID;                  // Logo texture ID
    struct tga_def
    {
        unsigned char len;      // Length of ID
        unsigned char type;     // TGA type
        unsigned char typeCode; // TGA type code
        unsigned char spec[5];  // TGA specifications
        unsigned short xStart;  // X origin location
        unsigned short yStart;  // Y origin location
        unsigned short w;       // Width of image
        unsigned short h;       // Height of image
        unsigned char bits;     // Bits per pixel
        unsigned char desc;     // Description of TGA image
    };
    tga_def tgadef;
	
	void init(TactonicDevice device, int argc, char** argv);                            // Initialize the display view for a device
	void display(TactonicFrame* frame, TactonicTouchFrame* touchFrame);		// Display a frame of data
	void drawDeviceBorder();                                                // Draw border
	void drawDeviceWires();                                                 // Draw wires
	void setColor(float color);                                             // Set color. blue to red
	void senselView(TactonicFrame *frame);                                   // Draw sensel rectangles
	void textView(TactonicFrame *frame);                                     // Draw sensel rectangles
	void gaussianView();                                                    // Draw a gaussian view
	void contourView();                                   // Draw sensel rectangles
	void registerTouches(TactonicTouchFrame *touchFrame);                    // Register the touches for line view
	void registerGesture(TactonicGesture *gesture);                    // Register the touches for line view
	void touchView(TactonicTouchFrame *touchFrame);                          // Draw the touch view
	void lineView(TactonicTouchFrame *touchFrame);                           // Draw the line view
	void blurValue(int x, int y, float value);                              // Blur a single value using a gaussian blur
	void applyGaussian(TactonicFrame *frame);                                // Apply a gaussian blur to the frame
	void keyEvent(unsigned char key);                                       // Key event, which switches the view
	void reshape(GLsizei width, GLsizei height);                            // The window was reshaped
	bool loadTGA(const char *filename, GLuint *texID);                      // Load the logo image
    void drawLogo();                                                        // Draw the logo image
    void drawCursor(TactonicTouch t, GLuint texID);
};
