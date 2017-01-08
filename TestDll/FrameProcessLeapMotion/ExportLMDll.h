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
// // ���ݵ�ǰ�������Ƶ�״̬���� �õ����� ��������Ĭ��Ϊ1 ������������˵����Ƶ�����λ�� RST���ƶ�λ��
// bool getMenuAction(ARGlassAction * action);
// bool getSingleAction(ARGlassAction * action, LeapMotionAction status);
// bool getDoubleAction(ARGlassAction * action, LeapMotionAction status);

#endif