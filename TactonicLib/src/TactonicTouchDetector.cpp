#include <stdio.h>
#include <sstream>
#include <string.h>
#include <math.h>

#include "TactonicTouchDetector.h"

using namespace std;


namespace tactonic
{
	TactonicTouchDetector::TactonicTouchDetector():
    nextID(0),
    stopping(false),
    touchCallbackLen(0),
    gestureCallbackLen(0),
	filterCallbackLen(0),
    flickRightCount(0),
    flickLeftCount(0),
    flickUpCount(0),
    flickDownCount(0),
    pinchInCount(0),
    pinchOutCount(0),
    twistClockCount(0),
    twistCounterCount(0),
    clickCount(0),
    tapCount(0)
	{
        
	}
    
	TactonicTouchDetector::~TactonicTouchDetector()
	{
		stop();
	}
    
	void TactonicTouchDetector::setTouchInfo(TactonicDevice deviceInfo)
	{
		device.serialNumber = deviceInfo.serialNumber;
		device.rows = deviceInfo.rows;
		device.cols = deviceInfo.cols;
        device.rowSpacingUM = deviceInfo.rowSpacingUM;
        device.colSpacingUM = deviceInfo.colSpacingUM;
        frameEvent.device = device;
		for(int i = 0; i < MAX_GESTURES; i++){
			gestureEvents[i].device = device;
			gestureEvents[i].gesture = new TactonicGesture();
			gestureEvents[i].gesture->type = (TactonicGestureType) i;
			if(i < 10){
				gestureEvents[i].gesture->state = GESTURE_PERFORMED;
			}
			else{
				gestureEvents[i].gesture->state = GESTURE_UPDATE;
			}
			gestureEvents[i].frame = new TactonicTouchFrame();
			
		}

		deviceRegion.x = 0;
		deviceRegion.y = 0;
		deviceRegion.width = device.cols*device.colSpacingUM/1000.0f;
		deviceRegion.height = device.rows*device.rowSpacingUM/1000.0f;
		for(int i = 0; i < BUFFER_FRAMES; i++){
			forceFrames[i].forces = new int[device.rows*device.cols];
			memset(forceFrames[i].forces, 0, device.rows*device.cols);
			forceFrames[i].numForces = device.rows*device.cols;
			forceFrames[i].frameNumber = 0;
			forceFrames[i].time = 0;
			touchFrames[i].frameNumber = 0;
			touchFrames[i].touches= new TactonicTouch[MAX_TOUCHES];
			touchFrames[i].time = 0;
			touchFrames[i].numTouches = 0;
		}
	}
    
	void TactonicTouchDetector::start()
	{
		readFrame = 0;
		fillFrame = 0;
		
		stopping = false;
#ifdef _WIN32
		threadRead = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)eventLoop, (void *)this, 0, NULL);
		SetThreadPriority(threadRead, THREAD_PRIORITY_HIGHEST);
#else
        pthread_attr_init(&pthread_custom_attr);
        pthread_create(&readThread,&pthread_custom_attr, eventLoop,(void *)this);
#endif
	}
    
	void TactonicTouchDetector::stop()
	{
		stopping = true;
#ifdef _WIN32
		DWORD code;
		WaitForSingleObject(threadRead, INFINITE);
		GetExitCodeThread(threadRead, &code);
		CloseHandle(threadRead);
#else
#endif
	}
	
	void TactonicTouchDetector::update(TactonicFrame* frame)
	{
        frameEvent.frame = frame;
		 for(int i = 0; i < filterCallbackLen; i++){
            if(filterCallbackList[i] != 0)
                frameEvent.frame = filterCallbackList[i](&frameEvent);
        }
        Tactonic_CopyFrame(frameEvent.frame, &forceFrames[fillFrame]);
		if((fillFrame+1)%BUFFER_FRAMES != readFrame)
			fillFrame = (fillFrame+1)%BUFFER_FRAMES;
		else
			fprintf(stderr, "Warning, data loss experienced. Increase buffer size or decrease update frame rate.");
	}
    
	void TactonicTouchDetector::find(){
		detectTouches();
		trackTouches();
        computeMovement();
        detectGestures();
		readFrame = (readFrame+1)%BUFFER_FRAMES;
		if(touchCallbackLen > 0){
            touchEvent.frame = &touchFrames[readFrame];
            for(int i =0; i < touchCallbackLen; i++){
                if(touchCallbackList[i] != 0){
                    touchCallbackList[i](&touchEvent);
                }
            }
		}
	}
    
	void* TactonicTouchDetector::eventLoop(void* touchDetector)
	{
		TactonicTouchDetector *detector = static_cast<TactonicTouchDetector *>(touchDetector);
		while(!detector->stopping){
			while((detector->readFrame+1)%BUFFER_FRAMES != detector->fillFrame)
				detector->find();
            Sleep(1);
		}
        return (void*) true;
	}
    
	void TactonicTouchDetector::detectTouches(){
		int writeFrame = (readFrame+1)%BUFFER_FRAMES;
		int i, j, v, u, val, val_ij;
		float xx, yy, ww;
		touchFrames[writeFrame].numTouches = 0;
		for(i = 0; i < device.rows; i++) {
			for(j = 0; j < device.cols; j++) {
				if(isPeak(i, j, device.rows , device.cols)) {
					int counter = 0;
					val_ij = forceFrames[writeFrame].forces[i*device.cols+j];
					xx = 0, yy = 0, ww = 0;
					for(v = -1; v <= 1; v++) {
						for(u = -1; u <= 1; u++) {
							if(inBounds(i+u, j+v, device.rows, device.cols)){
								val =forceFrames[writeFrame].forces[(i+u)*device.cols+(j+v)];
								if(val <= val_ij) {
									yy += val * (i + u)/(device.rows*1.0f);
									xx += val * (j + v)/(device.cols*1.0f);
									ww += val;
									counter++;
								}
							}
						}
					}
					if(ww > 200) {
						xx = device.cols*device.colSpacingUM*xx/(1000.0f*ww);
						yy = device.rows*device.rowSpacingUM*yy/(1000.0f*ww);
						touchFrames[writeFrame].touches[touchFrames[writeFrame].numTouches].x = xx;
						touchFrames[writeFrame].touches[touchFrames[writeFrame].numTouches].y = yy;
						touchFrames[writeFrame].touches[touchFrames[writeFrame].numTouches].force = ww;
						touchFrames[writeFrame].numTouches= touchFrames[writeFrame].numTouches+1;
						if(touchFrames[writeFrame].numTouches >= MAX_TOUCHES-1){
							return;
						}
					}
				}
			}
		}
	}
    
	bool TactonicTouchDetector::isPeak(int i, int j, int rows, int cols) {
		int writeFrame = (readFrame+1)%BUFFER_FRAMES;
		int v, u;
		int val;
		int val0 = max(forceFrames[writeFrame].forces[i*cols+j],0);
		for(v = -1; v <= 1; v++) {
			for(u = -1; u <= 1; u++) {
				if((u != 0 || v != 0) && inBounds(i+u, j+v, rows, cols)) {
					val = max(forceFrames[writeFrame].forces[(i+u)*cols+(j+v)],0);
					if(val > val0)
						return false;
					if(val == val0 && (i+u)*cols+(j+v) < (i)*cols+(j))
						return false;
				}
			}
		}
		return true;
	}
    
	bool TactonicTouchDetector::inBounds(int i, int j, int rows, int cols) {
		return i >= 0 && i < rows && j >= 0 && j < cols;
	}
	
	void TactonicTouchDetector::trackTouches(){
		TactonicTouch *t, *prevT;
		int writeFrame = (readFrame+1)%BUFFER_FRAMES;
        float blur;
		float closeDist, dist;
		int closeIndex, i , j;
		for(i = 0; i < MAX_TOUCHES; i++){
			touchFrames[writeFrame].touches[i].id = -1;
		}
		for(i = 0; i < touchFrames[readFrame].numTouches; i++){
			closeDist = 16.0f*device.colSpacingUM/1000.0f;
			closeIndex = -1;
			prevT = &touchFrames[readFrame].touches[i];
			if(prevT->touchState != TOUCH_END){
				for(j = 0; j < touchFrames[writeFrame].numTouches; j++){
					t = &touchFrames[writeFrame].touches[j];
					if(t->id == -1){
						dist =(prevT->x+prevT->dx-t->x)*(prevT->x+prevT->dx-t->x)+(prevT->y+prevT->dy-t->y)*(prevT->y+prevT->dy-t->y);
						if(dist < closeDist){
							closeDist = dist;
							closeIndex = j;
						}
					}
				}
				if(closeIndex != -1){
					t = &touchFrames[writeFrame].touches[closeIndex];
					t->id = prevT->id;
					t->touchState = TOUCH_UPDATE;
					t->dx = t->x - prevT->x;
					t->dy = t->y - prevT->y;
					blur = (t->dx*t->dx+t->dy*t->dy+0.5f)/(9.5f*device.colSpacingUM/1000.0f);
					t->x = ((1.0f-blur)*prevT->x + blur*t->x);
					t->y = ((1.0f-blur)*prevT->y + blur*t->y);
					t->force = (15*prevT->force + t->force)/16.0f;
					t->dx = t->x - prevT->x;
					t->dy = t->y - prevT->y;
					t->dForce = t->force - prevT->force;
				}
				else{
					t = &touchFrames[writeFrame].touches[touchFrames[writeFrame].numTouches];
					t->x = prevT->x;
					t->y = prevT->y;
					t->dx = 0;
					t->dy = 0;
					t->dForce = 0;
					t->id = prevT->id;
					t->force = prevT->force;
					t->touchState = TOUCH_END;
					touchFrames[writeFrame].numTouches++;
				}
			}
            
		}
		for(i = 0; i < touchFrames[writeFrame].numTouches; i++){
			t = &touchFrames[writeFrame].touches[i];
			if(t->id == -1){
				t->id = nextID;
				t->touchState = TOUCH_START;
				t->dx = 0;
				t->dy = 0;
				nextID++;
			}
		}
	}
	
	void TactonicTouchDetector::getTouchFrame(TactonicTouchFrame* frame){
		int i, touchIndex = 0;
		TactonicTouch t;
		int prevFrame = (readFrame);
		frame->frameNumber = touchFrames[prevFrame].frameNumber;
		frame->numTouches = touchFrames[prevFrame].numTouches;
		frame->time = touchFrames[prevFrame].time;
		for(i = 0; i < frame->numTouches; i++){
			t = touchFrames[prevFrame].touches[i];
            frame->touches[touchIndex].id = t.id;
            frame->touches[touchIndex].x = t.x;
            frame->touches[touchIndex].y = t.y;
            frame->touches[touchIndex].force = t.force;
            frame->touches[touchIndex].dx = t.dx;
            frame->touches[touchIndex].dy = t.dy;
            frame->touches[touchIndex].dForce = t.dForce;
            frame->touches[touchIndex].touchState = t.touchState;
            touchIndex++;
		}
	}
	
	void TactonicTouchDetector::addTouchCallback(TactonicTouchCallback callback, TactonicTouchRegion region){
        if(touchCallbackLen > 31)
            return;
		touchCallbackList[touchCallbackLen] = callback;
		touchRegionList[touchCallbackLen] = region;
        touchCallbackLen++;
	}
    
	void TactonicTouchDetector::removeTouchCallback(TactonicTouchCallback callback, TactonicTouchRegion region){
        for(int i = 0; i < touchCallbackLen; i++){
            if(callback == touchCallbackList[i])
                touchCallbackList[i] = 0;
        }
	}
	
	void TactonicTouchDetector::addGestureCallback(TactonicGestureCallback callback, TactonicTouchRegion region, TactonicGestureType gestureType){
        if(gestureCallbackLen > 31)
            return;
		gestureCallbackList[gestureCallbackLen] = callback;
		gestureRegionList[gestureCallbackLen] = region;
		gestureTypeList[gestureCallbackLen] = gestureType;
        gestureCallbackLen++;
	}
    
	void TactonicTouchDetector::removeGestureCallback(TactonicGestureCallback callback, TactonicTouchRegion region, TactonicGestureType gestureType){
        for(int i = 0; i < gestureCallbackLen; i++){
            if(callback == gestureCallbackList[i])
                gestureCallbackList[i] = 0;
        }
	}
    
	void TactonicTouchDetector::addFilterCallback(TactonicTouchFilterCallback callback){
        if(filterCallbackLen > 31)
            return;
		filterCallbackList[filterCallbackLen] = callback;
        filterCallbackLen++;
	}
    
	void TactonicTouchDetector::removeFilterCallback(TactonicTouchFilterCallback callback){
        for(int i = 0; i < filterCallbackLen; i++){
            if(callback == filterCallbackList[i])
                filterCallbackList[i] = 0;
        }
	}

    void TactonicTouchDetector::computeMovement() {
		int writeFrame = (readFrame+1)%BUFFER_FRAMES;

		TactonicGesture *moveX = gestureEvents[GESTURE_TRANSLATE_X].gesture;
		TactonicGesture *moveY = gestureEvents[GESTURE_TRANSLATE_Y].gesture;
		TactonicGesture *rotate = gestureEvents[GESTURE_ROTATE].gesture;
		TactonicGesture *scale = gestureEvents[GESTURE_SCALE].gesture;
		TactonicGesture *leanX = gestureEvents[GESTURE_LEAN_X].gesture;
		TactonicGesture *leanY = gestureEvents[GESTURE_LEAN_Y].gesture;
		
		float leanXVal=0;
		float leanYVal=0;
		float totalForce = 0;
		float rotateSum = 0;
		float scaleSum = 1.0f;
		float angle, u, v,forceVal;
		float prevRotate = rotate->dValue;
		float prevScale = scale->dValue;
        bool onlyTouchUpdate = true;

		moveX->dValue = 0;
		moveY->dValue = 0;
		leanX->dValue = 0;
		leanY->dValue = 0;
        centerX[writeFrame] = 0;
        centerY[writeFrame] = 0;
       
		if(touchFrames[writeFrame].numTouches > 0) {
			for(int i = 0; i < touchFrames[readFrame].numTouches; i++) {
				if(touchFrames[readFrame].touches[i].touchState != TOUCH_UPDATE)
                    onlyTouchUpdate = false;
			}

			for(int i = 0; i < touchFrames[writeFrame].numTouches; i++) {
				centerX[writeFrame] += touchFrames[writeFrame].touches[i].x;
				centerY[writeFrame] += touchFrames[writeFrame].touches[i].y;
				moveX->dValue += touchFrames[writeFrame].touches[i].dx;
				moveY->dValue += touchFrames[writeFrame].touches[i].dy;
				if(touchFrames[writeFrame].touches[i].touchState != TOUCH_UPDATE)
                    onlyTouchUpdate = false;
			}
			
			centerX[writeFrame] /= (1.0f)*touchFrames[writeFrame].numTouches;
			centerY[writeFrame] /= (1.0f)*touchFrames[writeFrame].numTouches;
			moveX->dValue /= 1.0f*touchFrames[writeFrame].numTouches;
			moveY->dValue /= 1.0f*touchFrames[writeFrame].numTouches;
			sendGestureEvent(GESTURE_TRANSLATE_X);
			sendGestureEvent(GESTURE_TRANSLATE_Y);

			if(!onlyTouchUpdate){
                for(int i = 0; i < touchFrames[writeFrame].numTouches; i++) {
					u = touchFrames[writeFrame].touches[i].x - centerX[writeFrame];
					v = touchFrames[writeFrame].touches[i].y - centerY[writeFrame];
					rotateTouches[writeFrame][i] = (float) atan2f(u, -v);
					scaleTouches[writeFrame][i] = ((float) sqrt(u * u + v * v));
				}
				leanX->centerX = centerX[writeFrame];
				leanX->centerY = centerY[writeFrame];
				leanY->centerX = centerX[writeFrame];
				leanY->centerY = centerY[writeFrame];
				leanX->value = 0;
				leanY->value = 0;
				rotate->value = 0;
				scale->value = 1.0f;
				rotate->dValue = 0;
				scale->dValue = 1.0f;
				return;
            }

			for(int i = 0; i < touchFrames[writeFrame].numTouches; i++) {
				forceVal = touchFrames[writeFrame].touches[i].force;
				leanXVal += forceVal*(touchFrames[writeFrame].touches[i].x - leanX->centerX);
				leanYVal += forceVal*(touchFrames[writeFrame].touches[i].y - leanY->centerY);
				totalForce += forceVal;
			}
			leanXVal = leanXVal/(1.0f*totalForce);
			leanYVal = leanYVal/(1.0f*totalForce);
			
			leanX->dValue = leanXVal-leanX->value;
			leanX->value = leanXVal;
			leanY->dValue = leanYVal-leanY->value;
			leanY->value = leanYVal;
			
			sendGestureEvent(GESTURE_LEAN_X);
			sendGestureEvent(GESTURE_LEAN_Y);

			if(touchFrames[writeFrame].numTouches > 1 && touchFrames[writeFrame].numTouches == touchFrames[readFrame].numTouches) {
				for(int i = 0; i < touchFrames[writeFrame].numTouches; i++) {
					u = touchFrames[writeFrame].touches[i].x - centerX[writeFrame];
					v = touchFrames[writeFrame].touches[i].y - centerY[writeFrame];
					rotateTouches[writeFrame][i] = (float) atan2f(u, -v);
					scaleTouches[writeFrame][i] = ((float) sqrt(u * u + v * v));
					angle = rotateTouches[writeFrame][i] - rotateTouches[readFrame][i];
					if(angle > 6.0)
						angle-=3.14159f*2.0f;
					if(angle < -6.0)
						angle+=3.14159f*2.0f;
					rotateSum += angle;
					scaleSum *= scaleTouches[writeFrame][i] / scaleTouches[readFrame][i];
				}
				rotateSum /= (2.0f * 3.14159f);
                
				rotate->dValue = 3.0f*prevRotate/4.0f + ROTATE_MULT*rotateSum/4.0f;
				rotate->value = rotate->value+rotate->dValue;

				scale->dValue =scaleSum/4.0f + 3.0f*prevScale/4.0f;
				scale->value = scale->value*scale->dValue;
				
				sendGestureEvent(GESTURE_ROTATE);
				sendGestureEvent(GESTURE_SCALE);
            }
        }
		
	}

	void TactonicTouchDetector::setGestureCount(int val){
		
            flickRightCount=(val);
            flickLeftCount=(val);
            flickUpCount=(val);
            flickDownCount=(val);
            pinchInCount=(val);
            pinchOutCount=(val);
            twistClockCount=(val);
            twistCounterCount=(val);
            clickCount=(val);
            tapCount = val;
	}
    
    void TactonicTouchDetector::sendGestureEvent(TactonicGestureType gestureType){
        if(gestureCallbackLen > 0){
            for(int i =0; i < gestureCallbackLen; i++){
				if(gestureCallbackList[i] != 0 && (gestureTypeList[i]==gestureType || gestureTypeList[i] == GESTURE_ALL)){
                    gestureCallbackList[i](&gestureEvents[gestureType]);
                }
            }
		}
    }
    
    void TactonicTouchDetector::detectGestures() {
		int writeFrame = (readFrame+1)%BUFFER_FRAMES;

		
		for(int i = 0; i < touchFrames[writeFrame].numTouches;i++){
			TactonicTouch *t = &touchFrames[writeFrame].touches[i];
			TactonicGesture *touch;
			if(t->touchState == TOUCH_START){
				touch = gestureEvents[GESTURE_TOUCH_START].gesture;
				touch->centerX = t->x;
				touch->centerY = t->y;
				touch->totalForce = t->force;
				touch->id = t->id;
				sendGestureEvent(GESTURE_TOUCH_START);
			}
			else if(t->touchState == TOUCH_END){
				touch = gestureEvents[GESTURE_TOUCH_END].gesture;
				touch->centerX = t->x;
				touch->centerY = t->y;
				touch->totalForce = t->force;
				touch->id = t->id;
				sendGestureEvent(GESTURE_TOUCH_END);
			}
		}
        
		TactonicGesture *moveX = gestureEvents[GESTURE_TRANSLATE_X].gesture;
		TactonicGesture *moveY = gestureEvents[GESTURE_TRANSLATE_Y].gesture;
		TactonicGesture *rotate = gestureEvents[GESTURE_ROTATE].gesture;
		TactonicGesture *scale = gestureEvents[GESTURE_SCALE].gesture;
        if(touchFrames[writeFrame].numTouches >= 2){
            
			if(rotate->dValue> TWIST_THRESHOLD)
                twistClockCount++;
            else if(twistClockCount > 0)
                twistClockCount--;
			if(twistClockCount > TWIST_TIME_MIN && (touchFrames[writeFrame].touches[0].touchState == TOUCH_END||touchFrames[writeFrame].touches[1].touchState == TOUCH_END)){
                sendGestureEvent(GESTURE_TWIST);
				setGestureCount(-8);
            }
            
            if(rotate->dValue < -TWIST_THRESHOLD)
                twistCounterCount++;
            else if(twistCounterCount > 0)
                twistCounterCount--;
            if(twistCounterCount > TWIST_TIME_MIN && (touchFrames[writeFrame].touches[0].touchState == TOUCH_END||touchFrames[writeFrame].touches[1].touchState == TOUCH_END)){
                sendGestureEvent(GESTURE_TWIST);
				setGestureCount(-8);
            }

            if(scale->dValue < 1.0f-PINCH_THRESHOLD)
                pinchInCount++;
            else if(pinchInCount > 0)
                pinchInCount--;
            if(pinchInCount > 2 && (touchFrames[writeFrame].touches[0].touchState == TOUCH_END||touchFrames[writeFrame].touches[1].touchState == TOUCH_END)){
                sendGestureEvent(GESTURE_PINCH);
				setGestureCount(-8);
            }
            
            if(scale->dValue > 1.0f+PINCH_THRESHOLD)
                pinchOutCount++;
            else if(pinchOutCount > 0)
                pinchOutCount--;
            if(pinchOutCount >  PINCH_TIME_MIN && (touchFrames[writeFrame].touches[0].touchState == TOUCH_END||touchFrames[writeFrame].touches[1].touchState == TOUCH_END)){
                sendGestureEvent(GESTURE_PINCH);
				setGestureCount(-8);
            }
        }
        if(touchFrames[writeFrame].numTouches == 1){
            if(moveX->dValue > FLICK_THRESHOLD){
                flickRightCount++;
				flickLeftCount = 0;
			}
            else if(flickRightCount > 0)
                flickRightCount--;
            if(flickRightCount > FLICK_TIME_MIN && touchFrames[writeFrame].touches[0].touchState == TOUCH_END && tapCount < FLICK_TIME_MAX){
                sendGestureEvent(GESTURE_FLICK);
				setGestureCount(-8);
            }
            
            if(moveX->dValue < -FLICK_THRESHOLD){
                flickLeftCount++;
				flickRightCount = 0;
			}
            else if(flickLeftCount > 0)
                flickLeftCount--;
            if(flickLeftCount > FLICK_TIME_MIN && touchFrames[writeFrame].touches[0].touchState == TOUCH_END && tapCount < FLICK_TIME_MAX ){
                sendGestureEvent(GESTURE_FLICK);
				setGestureCount(-8);
            }
            
            if(moveY->dValue > FLICK_THRESHOLD){
                flickUpCount++;
				flickDownCount = 0;
			}
            else if(flickUpCount > 0)
                flickUpCount--;
            if(flickUpCount > FLICK_TIME_MIN && touchFrames[writeFrame].touches[0].touchState == TOUCH_END && tapCount < FLICK_TIME_MAX){
                sendGestureEvent(GESTURE_FLICK);
				setGestureCount(-8);
            }
            
            if(moveY->dValue < -FLICK_THRESHOLD){
                flickDownCount++;
				flickUpCount = 0;
			}
            else if(flickDownCount > 0)
                flickDownCount--;
            if(flickDownCount > FLICK_TIME_MIN && touchFrames[writeFrame].touches[0].touchState == TOUCH_END && tapCount < FLICK_TIME_MAX){
                sendGestureEvent(GESTURE_FLICK);
				setGestureCount(-8);
            }
            
            if(touchFrames[writeFrame].touches[0].force >= CLICK_THRESHOLD_PRESS){
                clickCount++;
				tapCount = 0;
			}
			if(touchFrames[writeFrame].touches[0].force < CLICK_THRESHOLD_RELEASE || touchFrames[writeFrame].touches[0].touchState == TOUCH_END){
                if(clickCount <= CLICK_TIME_MAX && CLICK_TIME_MIN < clickCount){
                    sendGestureEvent(GESTURE_CLICK);
					setGestureCount(-8);
				}
            }
            
            if(touchFrames[writeFrame].touches[0].touchState == TOUCH_END){
                if(tapCount <= TAP_TIME_MAX && tapCount > TAP_TIME_MIN){
                    sendGestureEvent(GESTURE_TAP);
					setGestureCount(-8);
				}
            }
            if(tapCount < 500)
                tapCount++;
		}
		
		if(touchFrames[writeFrame].numTouches == 0){
			setGestureCount(0);
		}
    }
}
