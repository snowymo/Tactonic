#ifndef __TactonicTouchDetector_H__
#define __TactonicTouchDetector_H__

#ifdef _WIN32
#include <windows.h>
#else
#include <pthread.h>
#define Sleep(a) usleep(a*1000)
#endif
#include "TactonicTouch.h"

#define BUFFER_FRAMES 8
#define MAX_TOUCHES 128
#define MAX_GESTURES 32
#define TACTONIC_TOUCH_VERSION_MAJOR 5
#define TACTONIC_TOUCH_VERSION_MINOR 9

#define DRAG_MULT 20.0f

#define ROTATE_MULT 180.0f
#define TWIST_THRESHOLD 0.7f
#define TWIST_TIME_MIN 2.0f

#define PINCH_THRESHOLD 0.03f
#define PINCH_TIME_MIN 2.0f

#define FLICK_THRESHOLD 0.5f
#define FLICK_TIME_MIN 2.0f
#define FLICK_TIME_MAX 100.0f

#define CLICK_THRESHOLD_PRESS 6000.0f
#define CLICK_THRESHOLD_RELEASE 4000.0f
#define CLICK_TIME_MIN  1.0f
#define CLICK_TIME_MAX  100.0f

#define TAP_TIME_MIN 2.0f
#define TAP_TIME_MAX 30.0f

namespace tactonic
{
	class TactonicTouchDetector
	{
	public:
		TactonicTouchCallback       touchCallbackList[32];
        int                         touchCallbackLen;
		TactonicTouchRegion         touchRegionList[32];
		TactonicGestureCallback     gestureCallbackList[32];
        int                         gestureCallbackLen;
		TactonicTouchRegion			gestureRegionList[32];
		TactonicGestureType			gestureTypeList[32];
		TactonicTouchFilterCallback	filterCallbackList[32];
        int                         filterCallbackLen;
        
#ifdef _WIN32
		HANDLE                      threadRead;
#else
        pthread_t                   readThread;
        pthread_attr_t              pthread_custom_attr;
#endif
		
		TactonicDevice              device;
		TactonicTouchRegion			deviceRegion;
		TactonicTouchEvent          touchEvent;
		TactonicFrame				forceFrames[BUFFER_FRAMES];
		TactonicTouchFrame			touchFrames[BUFFER_FRAMES];
        float						rotateTouches[BUFFER_FRAMES][MAX_TOUCHES];
        float						scaleTouches[BUFFER_FRAMES][MAX_TOUCHES];
		float						centerX[BUFFER_FRAMES], centerY[BUFFER_FRAMES];
		
        TactonicFrameEvent          frameEvent;
        
        TactonicGestureEvent        gestureEvents[MAX_GESTURES];
		int readFrame;
		int fillFrame;
		int nextID;
        
        int flickRightCount;
        int flickLeftCount;
        int flickUpCount;
        int flickDownCount;
        int pinchInCount;
        int pinchOutCount;
        int twistClockCount;
        int twistCounterCount;
        int clickCount;
        int tapCount;

		bool stopping;

		TactonicTouchDetector();
		~TactonicTouchDetector();

		void setTouchInfo(TactonicDevice);
		void start();
		void stop();
		void update(TactonicFrame*);
		void getTouchFrame(TactonicTouchFrame*);
		static void	*eventLoop(void*);
		void detectTouches();
		void addTouchCallback(TactonicTouchCallback callback, TactonicTouchRegion region);
		void removeTouchCallback(TactonicTouchCallback callback, TactonicTouchRegion region);
		void addGestureCallback(TactonicGestureCallback callback, TactonicTouchRegion region, TactonicGestureType gesture);
		void removeGestureCallback(TactonicGestureCallback callback, TactonicTouchRegion region, TactonicGestureType gesture);
		void find();
		void trackTouches();
        void computeMovement();
        void detectGestures();
        void sendGestureEvent(TactonicGestureType);
        void setGestureCount(int val);
		bool isPeak(int i, int j, int rows, int cols);
		bool inBounds(int i, int j, int rows, int cols);
		void addFilterCallback(TactonicTouchFilterCallback callback);
		void removeFilterCallback(TactonicTouchFilterCallback callback);
	};

}

#endif // __TactonicTouchDetector_H__
