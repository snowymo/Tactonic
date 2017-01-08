#ifndef _EXPORT_DLL_H
#define _EXPORT_DLL_H

#if defined (EXPORTBUILD_OPENCV)  
# define _DLLExport __declspec (dllexport)  
# else  
# define _DLLExport __declspec (dllimport)  
#endif

#include "FrameProcess.h"

extern "C" void _DLLExport processDoubleFrame(uchar * inputOutputLeft,uchar * inputOutputRight, uchar * inputOutputClickLeft,uchar * inputOutputClickRight,int width, int height, int step, bool isPostCalib,bool isStereoRectify, ARGlassAction * menuAction, int* menuActionCnt, ARGlassAction * clickAction, int * clickActionCnt);

extern "C" int _DLLExport adjustYUVcolor(FrameProcess::YUV yuvPara, int value);

extern "C" void _DLLExport sethsv(int hValueBot,int hValueTop,int sValueBot, int sValueTop,int vValueBot, int vValueTop);

extern "C" void _DLLExport setElementSize(int eSize);

extern "C" int _DLLExport setBMPara(double uniquenessRatio);

extern "C" void _DLLExport initOCAM();

//extern "C" void _DLLExport stereoRectify(uchar * inputOutputLeft,uchar * inputOutputRight,int width, int height, int step);

#endif