// ====================================================================
// Copyright 2010-2012 Tactonics Technologies, LLC. All rights reserved
// PROPRIETARY/CONFIDENTIAL - DO NOT DISTRIBUTE!
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
// ====================================================================

#include "DisplayViews.h"

// Gausian blur values
double gaussian[11] = {0.0175, 0.0540, 0.1296, 0.2420, 0.3522, 0.4, 0.3522, 0.2420, 0.1296, 0.0540, 0.0175};
char escStr[25] = "Press ESC to Clear Lines";

// Set defaults and reset values
DisplayViews::DisplayViews(void)
{
	wPix = WIDTH;
	hPix = HEIGHT;
	view = GAUSSIAN_VIEW;
    lineFrame = 0;
	centered = true;
    
}

// Delete
DisplayViews::~DisplayViews(void)
{
    delete imageData;
	delete blurForces;
}

// Initialize the display view for a device
void DisplayViews::init(TactonicDevice deviceInfo, int argc, char** argv)
{
	char* dir = argv[0];
    int i, startName,offset = 0;
	device = deviceInfo;
	blurForces = new float[3*device.rows*3*device.cols];
	blurColors = new unsigned char[3*device.rows*3*device.cols*3];
    char logoName[100] = "TactonicVisualizerLogo.tga";
    char cursorName[100] = "cursor.tga";
    char penName[100] = "pen.tga";
    for (i=0; i<200 && dir[i]!=0; i++){
        fullPath[offset++] = dir[i];
    }
    while(!(fullPath[offset-1] == '/' || fullPath[offset-1] == '\\')){
        offset--;
    }
    if(fullPath[offset-1] == '/'){
        fullPath[offset++] = 'l';
        fullPath[offset++] = 'i';
        fullPath[offset++] = 'b';
        fullPath[offset++] = '/';
    }
       
    startName = offset;
    for (i=0; i<100 && logoName[i]!=0; i++){
        fullPath[offset++] = logoName[i];
    }
	fullPath[offset] = '\0';
    loadTGA(fullPath, &logoID);
    offset= startName;
    
    for (i=0; i<100 && cursorName[i]!=0; i++){
        fullPath[offset++] = cursorName[i];
    }
	fullPath[offset] = '\0';
    loadTGA(fullPath, &cursorID);
    offset= startName;
    
    for (i=0; i<100 && penName[i]!=0; i++){
        fullPath[offset++] = penName[i];
    }
	fullPath[offset] = '\0';
    loadTGA(fullPath, &penID);
	
    logoScale = 1.0f*wPix/(1.0f*WIDTH);
	rowSpacingPix = ((hPix-96*logoScale)/(1.0f*device.rows));
	if(rowSpacingPix*device.cols > wPix)
		rowSpacingPix = (wPix/(1.0f*device.cols));
	colSpacingPix = rowSpacingPix;
	xOffsetPix = (wPix-colSpacingPix*device.cols)/2;
	yOffsetPix = (hPix-96*logoScale-rowSpacingPix*device.rows)/2+96*logoScale-1;

	float diffX, diffY;
	for(int i = 0; i < argc; i++){
		if(argv[i][1] == 'p' && i+6 < argc){
			wPix = atoi(argv[i+1]);
			hPix = atoi(argv[i+2]);
			glutReshapeWindow(wPix, hPix);
			xOffsetPix = -atoi(argv[i+3]);
			yOffsetPix = -atoi(argv[i+4]);
			rowSpacingPix = atof(argv[i+5]);
			colSpacingPix = atof(argv[i+6]);
			centered = false;
		}
		if(argv[i][1] == 'm' && i+6 < argc){
			wPix = atoi(argv[i+1]);
			hPix = atoi(argv[i+2]);
			glutReshapeWindow(wPix, hPix);
			diffX = (atof(argv[i+5])*1000.0)/device.colSpacingUM;
			diffY = (atof(argv[i+6])*1000.0)/device.colSpacingUM;
			colSpacingPix = wPix/(diffX);
			rowSpacingPix = hPix/(diffY);
			xOffsetPix = -atoi(argv[i+3])*colSpacingPix*1000.0/(1.0*device.colSpacingUM);
			yOffsetPix = -atoi(argv[i+4])*rowSpacingPix*1000.0/(1.0*device.colSpacingUM);
			centered = false;
		}

	}
	
    glGenTextures(1, &gaussianID);
    glBindTexture(GL_TEXTURE_2D, gaussianID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 3*device.cols, 3*device.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, blurColors);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

// Display a frame of data
void DisplayViews::display(TactonicFrame* frame, TactonicTouchFrame* touchFrame)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
    
    if(view == GAUSSIAN_VIEW){
        applyGaussian(frame);
        gaussianView();
        memset(blurForces, 0, 3*device.rows*3*device.cols*sizeof(int));
        drawDeviceBorder();
    }
    else if(view == SENSEL_VIEW){
        drawDeviceWires();
        senselView(frame);
        drawDeviceBorder();
    }
    else if(view == TOUCH_VIEW){
        drawDeviceBorder();
        touchView(touchFrame);
    }
    else if(view == LINE_VIEW){
        lineView(touchFrame);
        drawDeviceBorder();
    }
	
    drawLogo();
	
   if(view == TEXT_VIEW){
        drawDeviceBorder();
        textView(frame);
    }
	glPopMatrix();
	glPopMatrix();
	glutSwapBuffers();
    Sleep(30);
}

void DisplayViews::drawDeviceWires(){
	glBegin(GL_LINES);
	int tileWidth = colSpacingPix*device.cols;
	int tileHeight = colSpacingPix*device.rows;
	glColor3f(0.5f, 0.5f, 0.5f);
	for (int i = 0; i < device.rows; i++){
		glVertex2d(xOffsetPix, yOffsetPix+(i+0.5f)*colSpacingPix);
		glVertex2d(xOffsetPix+tileWidth, yOffsetPix+(i+0.5f)*colSpacingPix);
	}
	for (int j = 0; j < device.cols; j++){
		glVertex2d(xOffsetPix+(j+0.5f)*colSpacingPix, yOffsetPix);
		glVertex2d(xOffsetPix+(j+0.5f)*colSpacingPix, yOffsetPix+tileHeight);
	}
	glEnd();
}

// Draw wires and border
void DisplayViews::drawDeviceBorder(){
	glBegin(GL_LINES);
	int tileWidth = colSpacingPix*device.cols;
	int tileHeight = colSpacingPix*device.rows;
	glColor3f(1.0f, 1.0f, 1.0f);
	glVertex2d(xOffsetPix+1, yOffsetPix);
	glVertex2d(xOffsetPix+1, yOffsetPix+tileHeight);
	glVertex2d(xOffsetPix+tileWidth, yOffsetPix);
	glVertex2d(xOffsetPix+tileWidth, yOffsetPix+tileHeight);
	glVertex2d(xOffsetPix, yOffsetPix+1);
	glVertex2d(xOffsetPix+tileWidth, yOffsetPix+1);
	glVertex2d(xOffsetPix, yOffsetPix+tileHeight);
	glVertex2d(xOffsetPix+tileWidth, yOffsetPix+tileHeight);
		
	glEnd();
}

// Set color, ranges between green and white
void DisplayViews::setColor(float color){
    red = 0;
    green = 0;
    blue = 0;
    if(color < 0.2f){	
        blue = color*5.0f;
	}
	else if(color < 0.4f){
		green = color*5.0f-1.0f;
		blue = 1.0f;
	}
	else if(color < 0.6f){	
		green = 1.0f;
		blue = 1.0f-(color*5.0f-2.0f);
	}
	else if(color < 0.8f){	
		green = 1.0f;
		red = color*5.0f-3.0f;
	}
	else if(color < 1.0f){	
		green = 1.0f-(color*5.0f-4.0f);
		red = 1.0f;
	}
    else{
        red = 1.0f;
    }
	glColor3f(red, green, blue);
}

// Draw sensel rectangles
void DisplayViews::senselView(TactonicFrame *frame){
	int value;
	glBegin(GL_QUADS);
	for (int i = 0; i < device.rows; i++){
		for (int j = 0; j < device.cols; j++){
			value = (frame->forces[i*device.cols+j]);
			setColor(value/RED_VALUE+0.1f);
			glVertex2d((j+0.25f)*colSpacingPix+xOffsetPix, (i+0.25f)*colSpacingPix+yOffsetPix);
			glVertex2d((j+0.25f)*colSpacingPix+colSpacingPix/2+xOffsetPix, (i+0.25f)*colSpacingPix+yOffsetPix);
			glVertex2d((j+0.25f)*colSpacingPix+colSpacingPix/2+xOffsetPix, (i+0.25f)*colSpacingPix+colSpacingPix/2+yOffsetPix);
			glVertex2d((j+0.25f)*colSpacingPix+xOffsetPix, (i+0.25f)*colSpacingPix+colSpacingPix/2+yOffsetPix);
		}
	}
			
	glEnd();
}

void DisplayViews::textView(TactonicFrame *frame){
    int value;
	glPushMatrix();
    for (int i = 0; i < device.rows; i++){
        for (int j = 0; j < device.cols; j++){
            value = frame->forces[i*device.cols+j];
            setColor(value/RED_VALUE+0.1f);
            if(value != 0){
                glRasterPos2i((j+0.25f)*colSpacingPix+xOffsetPix, (i+0.25f)*colSpacingPix+yOffsetPix);
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, (char)((value/1000)%10+'0'));
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, (char)((value/100)%10+'0'));
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, (char)((value/10)%10+'0'));
                glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_10, (char)(value%10+'0'));
            }
        }
    }
	glPopMatrix();
    glRasterPos2i(0,0);
    glColor3f(1.0, 1.0, 1.0);
}

// Draw a gaussian view
void DisplayViews::gaussianView(){
	
    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, gaussianID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 3*device.cols, 3*device.rows, 0, GL_RGB, GL_UNSIGNED_BYTE, blurColors);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0f, 0.0f);glVertex2f(xOffsetPix,yOffsetPix);
    glTexCoord2f(1.0f, 0.0f);glVertex2f(xOffsetPix+colSpacingPix*device.cols,yOffsetPix);
    glTexCoord2f(1.0f, 1.0f);glVertex2f(xOffsetPix+colSpacingPix*device.cols,yOffsetPix+rowSpacingPix*device.rows);
    glTexCoord2f(0.0f, 1.0f);glVertex2f(xOffsetPix,yOffsetPix+rowSpacingPix*device.rows);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

// Draw the touch view
void DisplayViews::touchView(TactonicTouchFrame *touchFrame){
	float delta_theta = 0.4;
	float pi = 3.14;
	int i;
	float angle;
	TactonicTouch t;
	for(i = 0; i < touchFrame->numTouches; i++) {
		t = touchFrame->touches[i];
		glBegin( GL_POLYGON );
		setColor(t.force/(4*RED_VALUE));
		for(angle = 0; angle < 2*pi; angle += delta_theta )
			glVertex2d((t.x+0.5f)*colSpacingPix+colSpacingPix*cos(angle)/2.0+xOffsetPix, (t.y+0.5f)*colSpacingPix+colSpacingPix*sin(angle)/2.0+yOffsetPix);
      
		glEnd();
	}
}


void DisplayViews::registerTouches(TactonicTouchFrame *touchFrame){
	int i;
    TactonicTouch t;
	if(view==LINE_VIEW || view==TOUCH_VIEW){
		for(i = 0; i < touchFrame->numTouches; i++) {
            t = touchFrame->touches[i];
            if(t.touchtype == TOUCH_DOWN){
                pen[t.id] = false;
            }
            if(t.force > 3000 || (pen[t.id] && t.force > 1500)) {
                xPosition[posIndex] = (t.x+0.5);
                yPosition[posIndex] = (t.y+0.5);
                zPosition[posIndex] = (t.force);
                dx[posIndex] = t.dx;
                dy[posIndex] = t.dy;
                dz[posIndex] = t.dForce;
                posIndex++;
                pen[t.id] = true;
            }
            else{
                pen[t.id] = false;
            }
		}
	}
}

// Draw the touch view
void DisplayViews::lineView(TactonicTouchFrame *touchFrame){
    int i;
    TactonicTouch t;
    
    double x, y;
    double xTouch, yTouch, dist, currForce, prevForce;
    double diffX, diffY;
    glColor3f(0, 0.0f, 1.0f);
    
    float delta_theta = 0.8;
    float pi = 3.14;
    float angle;
    
    
    for(i = 0; i < posIndex-1; i++){
       // glLineWidth(zPosition[i]/RED_VALUE);
        glColor3f(0, 1.0f, 1.0f);
        x = xPosition[i]*colSpacingPix+xOffsetPix;
        y = yPosition[i]*colSpacingPix+yOffsetPix;
        diffX = dx[i]*colSpacingPix;
        diffY = dy[i]*colSpacingPix;
        dist = sqrt(diffX*diffX+diffY*diffY);
        xTouch = diffY/dist;
        yTouch = diffX/dist;
        currForce = (zPosition[i]-1000.0f)/2000.0f;
        prevForce = dz[i]/2000.0f+currForce;
       
        glBegin( GL_POLYGON );
        for(angle = 0; angle < 2*pi; angle += delta_theta )
            glVertex2d(x+cos(angle)*currForce, y+sin(angle)*currForce);
            
        glEnd();
        
        glBegin( GL_POLYGON );
        glVertex2d(x-currForce*xTouch, y+currForce*yTouch);
        glVertex2d(x+currForce*xTouch, y-currForce*yTouch);
        glVertex2d(x-diffX+prevForce*xTouch, y-diffY-prevForce*yTouch);
        glVertex2d(x-diffX-prevForce*xTouch, y-diffY+prevForce*yTouch);
        glEnd();
    }
    
	for(i = 0; i < touchFrame->numTouches; i++) {
		t = touchFrame->touches[i];
        if(!pen[t.id]){
            drawCursor(t, cursorID);
        }
        else{
            drawCursor(t, penID);
        }
		glEnd();
	}
}

// Blur a single value using a gaussian blur
void DisplayViews::blurValue(int x, int y, float value){
	int i, j, x2, y2, n2, rows, cols;
	rows = device.rows;
	cols = device.cols;
	for (j=-5; j<=5; j++) {
		y2 = 3*y+j;
		for (i = -5; i <= 5; i++) {
			x2 = 3*x+i;
			if(j*j+i*i < 25){
				if (x2 >= 0 && x2 < 3*cols && y2 >= 0 && y2 < 3*rows) {
					n2 = 3*cols*(y2)+x2;
					blurForces[n2] += gaussian[i+5]*gaussian[j+5]*value;
				}
			}
		}
	}
}

// Apply a gaussian blur to the frame
void DisplayViews::applyGaussian(TactonicFrame *frame)
{
	int x, y, n=0;
	float value;
	if(view == GAUSSIAN_VIEW){
		for (y = 0; y < device.rows; y++) {
			for (x = 0; x < device.cols; x++) {
				value = 6*frame->forces[n++]/(1.0f*RED_VALUE);
				blurValue(x, y, value);
			}
		}
        float color;
		for (int i = 0; i < 3*device.rows*3*device.cols; i++) {
            color = blurForces[i];
            red = 0;
            green = 0;
            blue = 0;
            if(color < 0.2f){
                blue = color*5.0f;
            }
            else if(color < 0.4f){
                green = color*5.0f-1.0f;
                blue = 1.0f;
            }
            else if(color < 0.6f){
                green = 1.0f;
                blue = 1.0f-(color*5.0f-2.0f);
            }
            else if(color < 0.8f){
                green = 1.0f;
                red = color*5.0f-3.0f;
            }
            else if(color < 1.0f){	
                green = 1.0f-(color*5.0f-4.0f);
                red = 1.0f;
            }
            else{
                red = 1.0f;
            }
            blurColors[i*3] = (int)(red*255);
            blurColors[i*3+1] = (int)(green*255);
            blurColors[i*3+2] = (int)(blue*255);
        }
	}
}

// Key event, which switches the view
void DisplayViews::keyEvent(unsigned char key){
	if(key >= '1' && key <= '6'){
		view = key-'1';
        posIndex = 0;
    }
	if(key == 'w'){
		yOffsetPix+=4;
		centered = false;
	}
	if(key == 's'){
		yOffsetPix-=4;
		centered = false;
	}
	if(key == 'a'){
		xOffsetPix-=4;
		centered = false;
	}
	if(key == 'd'){
		xOffsetPix+=4;
		centered = false;
	}
	float prevR, prevC;
	if(key == 'z'){
		prevR = rowSpacingPix;
		prevC = colSpacingPix;
		rowSpacingPix*=1.2f;
		colSpacingPix*=1.2f;
		yOffsetPix = (yOffsetPix-hPix/2.0)*(rowSpacingPix)/(prevR)+hPix/2.0;
		xOffsetPix = (xOffsetPix-wPix/2.0)*(colSpacingPix)/(prevC)+wPix/2.0;
		centered = false;
	}
	if(key == 'x'){
		prevR = rowSpacingPix;
		prevC = colSpacingPix;
		rowSpacingPix/=1.2f;
		colSpacingPix/=1.2f;
		yOffsetPix = (yOffsetPix-hPix/2.0)*(rowSpacingPix)/(prevR)+hPix/2.0;
		xOffsetPix = (xOffsetPix-wPix/2.0)*(colSpacingPix)/(prevC)+wPix/2.0;
		centered = false;
	}
	if(key == 'p'){
		printf("-p %d %d %d %d %f %f\n", (int)(wPix),(int)(hPix), 
			(int)(-xOffsetPix),(int)(-yOffsetPix),colSpacingPix,rowSpacingPix);
	}
	if(key == 'm'){
		printf("-m %d %d %f %f %f %f\n", (int)(wPix),(int)(hPix), 
			(-xOffsetPix*device.colSpacingUM)/(1000.0*colSpacingPix),
			(-yOffsetPix*device.rowSpacingUM)/(1000.0*rowSpacingPix),
			(wPix*device.colSpacingUM)/(1000.0*colSpacingPix),
			(hPix*device.rowSpacingUM)/(1000.0*rowSpacingPix));
	}
	if(key == 27)
		posIndex = 0;
	if(key == 'c'){
		centered = true;
		logoScale = 1.0f*wPix/(1.0f*WIDTH);
		rowSpacingPix = ((hPix-96*logoScale)/(1.0f*device.rows));
		if(rowSpacingPix*device.cols > wPix)
			rowSpacingPix = (wPix/(1.0f*device.cols));
		colSpacingPix = rowSpacingPix;
		xOffsetPix = (wPix-colSpacingPix*device.cols)/2;
		yOffsetPix = (hPix-96*logoScale-rowSpacingPix*device.rows-1)/2+96*logoScale;
	}
}

// The window was reshaped
void DisplayViews::reshape(int width, int height){
	wPix = width;
    logoScale = 1.0f*wPix/(1.0f*WIDTH);
	hPix = height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0f, wPix, 0, hPix);
	
	if(centered){
		logoScale = 1.0f*wPix/(1.0f*WIDTH);
		rowSpacingPix = ((hPix-96*logoScale)/(1.0f*device.rows));
		if(rowSpacingPix*device.cols > wPix)
			rowSpacingPix = (wPix/(1.0f*device.cols));
		colSpacingPix = rowSpacingPix;
		xOffsetPix = (wPix-colSpacingPix*device.cols)/2;
		yOffsetPix = (hPix-96*logoScale-rowSpacingPix*device.rows-1)/2.0+96*logoScale;
	}
}

//Load the logo image
bool DisplayViews::loadTGA(const char* filename, GLuint* id)
{
	fstream filestr;
	filestr.open (filename, ios::in | ios::binary);								
	if (filestr.is_open())													
	{
		filestr.read((char*) &tgadef , sizeof(struct tga_def));
		int imageSize = tgadef.w * tgadef.h * tgadef.bits;
		this->imageData = (char*) malloc(imageSize);						
		filestr.read((char*) this->imageData, imageSize);
		filestr.close();
        glGenTextures(1, id);
        glBindTexture(GL_TEXTURE_2D, *id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tgadef.w, tgadef.h, 0, 0x80E1, GL_UNSIGNED_BYTE, this->imageData);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	}
	return 0;
}

// Draw the logo at the bottom of the window
void DisplayViews::drawLogo(){
    
    glColor3ub(255,255,255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, logoID);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0f, 0.0f);glVertex2f(0.0f,0);
    glTexCoord2f(1.0f, 0.0f);glVertex2f(wPix,0);
    glTexCoord2f(1.0f, 1.0f);glVertex2f(wPix,96.0f*logoScale);
    glTexCoord2f(0.0f, 1.0f);glVertex2f(0.0f,96.0f*logoScale);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
}

void DisplayViews::drawCursor(TactonicTouch t, GLuint texID){
    glColor3ub(255,255,255);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);
    glBegin(GL_POLYGON);
    glTexCoord2f(0.0f, 0.0f);glVertex2f((t.x+0.3f)*colSpacingPix+xOffsetPix,(t.y-0.4f)*colSpacingPix+yOffsetPix);
    glTexCoord2f(1.0f, 0.0f);glVertex2f((t.x+1.3f)*colSpacingPix+xOffsetPix,(t.y-0.4f)*colSpacingPix+yOffsetPix);
    glTexCoord2f(1.0f, 1.0f);glVertex2f((t.x+1.3f)*colSpacingPix+xOffsetPix,(t.y+0.6f)*colSpacingPix+yOffsetPix);
    glTexCoord2f(0.0f, 1.0f);glVertex2f((t.x+0.3f)*colSpacingPix+xOffsetPix,(t.y+0.6f)*colSpacingPix+yOffsetPix);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}