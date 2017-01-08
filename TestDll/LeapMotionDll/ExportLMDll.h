#ifndef _EXPORT_LM_DLL_H
#define _EXPORT_LM_DLL_H

#include "ARGlassListener.h"

#if defined (EXPORTBUILD_LM)  
# define _DLLExport __declspec (dllexport)  
# else  
# define _DLLExport __declspec (dllimport)  
#endif

extern Leap::Controller g_controller;
extern ARGlassListener g_listener;

enum LeapAction{
	NONE,
	MENU,
	CLICK
};

extern "C"{
	_DLLExport void LeapMotionInit();

	_DLLExport void LeapMotionExit();

	_DLLExport void processFrame();

	_DLLExport void getHandInfo(std::vector<Leap::Vector> & handInfo);

	_DLLExport void getAction(LeapAction * action);
};

#endif