// ====================================================================
// Copyright 2010-2012 Tactonics Technologies, LLC. All rights reserved
// PROPRIETARY/CONFIDENTIAL - DO NOT DISTRIBUTE!
// DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
// ====================================================================
#ifndef __TACTONICDEV_H__
#define __TACTONICDEV_H__

#ifdef _WIN32
#include <windows.h>
#include "Tactonic.h"
#define TACTONIC_API __declspec(dllexport)
#else
#define TACTONIC_API
#include "Tactonic.h"
#define WINAPI
#endif

#define CALIBRATE_OFF 0xA0
#define CALIBRATE_ON 0xB0



extern "C" {
	TACTONIC_LIB void WINAPI Tactonic_SetDeviceControl(TactonicDevice device, int deviceControl, int controlValue);
    
	TACTONIC_LIB void WINAPI Tactonic_ResetDevice(TactonicDevice device);
}
	
#endif