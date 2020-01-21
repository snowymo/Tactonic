
#include "TactonicTouch.h"
#include "TactonicTouchDetector.h"
#include "stdlib.h"

using namespace tactonic;

TactonicTouchDetector *detector;
TactonicDevice deviceHandle;
void detectCallback(TactonicFrameEvent* evt);

TACTONIC_TOUCH void WINAPI TactonicTouch_CreateDetector(TactonicDevice device)
{
    
    printf("Touch Touch Library %d.%d\n",TACTONIC_TOUCH_VERSION_MAJOR, TACTONIC_TOUCH_VERSION_MINOR);
	detector = new TactonicTouchDetector();
	detector->setTouchInfo(device);
	deviceHandle.serialNumber = device.serialNumber;
}

TACTONIC_TOUCH void WINAPI TactonicTouch_DestroyDetector(TactonicDevice device)
{ 
	Tactonic_RemoveFrameCallback(device, detectCallback);
    delete detector;
}

TACTONIC_TOUCH void WINAPI TactonicTouch_StartDetector(TactonicDevice device)
{
	if(device.serialNumber == deviceHandle.serialNumber){
		detector->start();
        Tactonic_AddFrameCallback(device, detectCallback);
	}
}

TACTONIC_TOUCH void WINAPI TactonicTouch_StopDetector(TactonicDevice device)
{
	if(device.serialNumber == deviceHandle.serialNumber){
        Tactonic_RemoveFrameCallback(device, detectCallback);
		detector->stop();
	}
}

TACTONIC_TOUCH TactonicTouchFrame* WINAPI TactonicTouch_CreateFrame(TactonicDevice device)
{
    TactonicTouchFrame *frame = new TactonicTouchFrame();
	frame->touches = new TactonicTouch[MAX_TOUCHES];
	frame->frameNumber = 0;
	frame->time = 0;
	frame->numTouches = 0;
    return frame;
}

TACTONIC_TOUCH void WINAPI TactonicTouch_DestroyFrame(TactonicTouchFrame* frame)
{
    delete (frame->touches);
	frame->frameNumber = 0;
	frame->time = 0;
	frame->numTouches = 0;
    delete (frame);
}

TACTONIC_TOUCH void WINAPI TactonicTouch_AddTouchCallback(TactonicDevice device, TactonicTouchCallback callback)
{
	if(device.serialNumber == deviceHandle.serialNumber){
		detector->addTouchCallback(callback, detector->deviceRegion);
	}
}

TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveTouchCallback(TactonicDevice device, TactonicTouchCallback callback)
{
    if(device.serialNumber == deviceHandle.serialNumber){
		detector->removeTouchCallback(callback, detector->deviceRegion);
	}
}

TACTONIC_TOUCH void WINAPI TactonicTouch_AddTouchRegionCallback(TactonicDevice device, TactonicTouchRegion region, TactonicTouchCallback callback)
{
	if(device.serialNumber == deviceHandle.serialNumber){
		detector->addTouchCallback(callback, region);
	}
}

TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveTouchRegionCallback(TactonicDevice device, TactonicTouchRegion region, TactonicTouchCallback callback)
{
    if(device.serialNumber == deviceHandle.serialNumber){
		detector->removeTouchCallback(callback, region);
	}
}

TACTONIC_TOUCH void WINAPI TactonicTouch_AddGestureCallback(TactonicDevice device, TactonicGestureType gestureType, TactonicGestureCallback callback)
{
	if(device.serialNumber == deviceHandle.serialNumber){
		detector->addGestureCallback(callback, detector->deviceRegion, gestureType);
	}
}

TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveGestureCallback(TactonicDevice device, TactonicGestureType gestureType, TactonicGestureCallback callback)
{
	if(device.serialNumber == deviceHandle.serialNumber){
		detector->removeGestureCallback(callback, detector->deviceRegion, gestureType);
	}
}

TACTONIC_TOUCH void WINAPI TactonicTouch_AddGestureRegionCallback(TactonicDevice device, TactonicTouchRegion region, TactonicGestureType gestureType, TactonicGestureCallback callback)
{
	if(device.serialNumber == deviceHandle.serialNumber){
		detector->removeGestureCallback(callback, region, gestureType);
	}
}

TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveGestureRegionCallback(TactonicDevice device, TactonicTouchRegion region, TactonicGestureType gestureType, TactonicGestureCallback callback)
{
	if(device.serialNumber == deviceHandle.serialNumber){
		detector->removeGestureCallback(callback, region, gestureType);
	}
}

TACTONIC_TOUCH void WINAPI TactonicTouch_CopyFrame(TactonicTouchFrame *srcFrame, TactonicTouchFrame *dstFrame)
{ 
    dstFrame->numTouches = srcFrame->numTouches;
    for(int i = 0; i < srcFrame->numTouches; i++){
        dstFrame->touches[i].dForce = srcFrame->touches[i].dForce;
        dstFrame->touches[i].dx = srcFrame->touches[i].dx;
        dstFrame->touches[i].dy= srcFrame->touches[i].dy;
        dstFrame->touches[i].force = srcFrame->touches[i].force;
        dstFrame->touches[i].x = srcFrame->touches[i].x;
        dstFrame->touches[i].y= srcFrame->touches[i].y;
        dstFrame->touches[i].id = srcFrame->touches[i].id;
        dstFrame->touches[i].touchState= srcFrame->touches[i].touchState;
    }
    dstFrame->frameNumber = srcFrame->frameNumber;
    dstFrame->time = srcFrame->time;
}

TACTONIC_TOUCH void WINAPI TactonicTouch_AddFilterCallback(TactonicDevice device, TactonicTouchFilterCallback callback)
{
	if(device.serialNumber == deviceHandle.serialNumber){
		detector->addFilterCallback(callback);
	}
}

TACTONIC_TOUCH void WINAPI TactonicTouch_RemoveFilterCallback(TactonicDevice device, TactonicTouchFilterCallback callback)
{
    if(device.serialNumber == deviceHandle.serialNumber){
		detector->removeFilterCallback(callback);
	}
}

TACTONIC_TOUCH void WINAPI TactonicTouch_PollFrame(TactonicDevice device, TactonicTouchFrame* frame)
{
	if(device.serialNumber == deviceHandle.serialNumber){
		detector->getTouchFrame(frame);
	}
}

void detectCallback(TactonicFrameEvent* evt){
    
	if(evt->device.serialNumber == deviceHandle.serialNumber){
        detector->update(evt->frame);
    }
}
