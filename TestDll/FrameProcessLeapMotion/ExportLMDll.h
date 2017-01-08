#ifndef _EXPORT_Leap_Motion_DLL_H
#define _EXPORT_Leap_Motion_DLL_H

#if defined (EXPORTBUILD_LEAP_MOTION)  
# define _DLLExport __declspec (dllexport)  
# else  
# define _DLLExport __declspec (dllimport)  
#endif

// newest 2014-4-4 15:38:34

#include "ARGlassListener.h"

//extern Leap::Controller g_ctrl;

//extern "C" void _DLLExport getAction(LeapMotionAction * action);
extern "C" bool _DLLExport getAction(ARGlassAction * action,LeapHand * bondInfos,long * frameId, bool * isAction,  LeapMotionAction status);

extern "C" void _DLLExport initLeapMotion();

extern "C" void _DLLExport init();

extern "C" void _DLLExport exitLeapMotion();

void calCamposFromLeappos(ARGlassAction * action, int index);
// // 根据当前所需手势的状态出发 得到手势 手势数量默认为1 及操作数比如菜单手势的手掌位置 RST的移动位置
// bool getMenuAction(ARGlassAction * action);
// bool getSingleAction(ARGlassAction * action, LeapMotionAction status);
// bool getDoubleAction(ARGlassAction * action, LeapMotionAction status);

#endif