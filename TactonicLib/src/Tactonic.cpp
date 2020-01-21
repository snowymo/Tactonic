
#include "TactonicUSB.h"
#include "TactonicDev.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

using namespace tactonic;

TactonicDeviceList tileList;
TactonicUSB **tileUSB;

TACTONIC_LIB TactonicDeviceList* WINAPI Tactonic_GetDeviceList()
{
	printf( "====================================================================\n");
	printf( "Copyright 2010-2013 Tactonics Technologies, LLC. All rights reserved\n");
	printf( "====================================================================\n\n");

    char serialBuff[8][64];
	char *serialList[8 + 1];
    char desBuff[8][64];
	char *desList[8 + 1];
	int numDes;
    
	for(int i = 0; i < 8; i++) {
		serialList[i] = serialBuff[i];
		desList[i] = desBuff[i];
	}
    
	if (FT_ListDevices(serialList, &tileList.numDevices, FT_LIST_ALL | FT_OPEN_BY_SERIAL_NUMBER) != FT_OK){
		tileList.numDevices = 0;
		return &tileList;
	}
	
	if(tileList.numDevices > 0){
		FT_ListDevices(desList, &numDes, FT_LIST_ALL | FT_OPEN_BY_DESCRIPTION);
		tileList.devices = new TactonicDevice[tileList.numDevices];
		int devIndex = 0;
		tileUSB = new TactonicUSB*[tileList.numDevices];

		for(int i = 0; i < tileList.numDevices; i++){
			tileUSB[devIndex] = new TactonicUSB();
			if ((tileUSB[devIndex]->openDevice(serialBuff[i]))){ 
				tileUSB[devIndex]->getDeviceInfo(&tileList.devices[devIndex]);
				if(tileList.devices[devIndex].serialNumber == 0){
					tileList.devices[devIndex].serialNumber = i+1;
					tileUSB[devIndex]->device.serialNumber = i+1;
				}
				printf("Initializing Device %lu\n", tileList.devices[devIndex].serialNumber);
				printf("Device Firmware %d.%d\n",tileUSB[devIndex]->firmwareVersion[0], tileUSB[devIndex]->firmwareVersion[1]);
				printf("Tactonic Library %d.%d\n",TACTONIC_LIBRARY_VERSION_MAJOR, TACTONIC_LIBRARY_VERSION_MINOR);
				devIndex++;
			}
		}
		tileList.numDevices = devIndex;



	}
	return &tileList;
}


TACTONIC_LIB void WINAPI Tactonic_StartDevice(TactonicDevice device)
{	
	for(int i = 0; i < tileList.numDevices; i++){
		if(device.serialNumber == tileList.devices[i].serialNumber){
			tileUSB[i]->startDevice();
		}
	}
}

TACTONIC_LIB void WINAPI Tactonic_StopDevice(TactonicDevice device)
{
	for(int i = 0; i < tileList.numDevices; i++){
		if(device.serialNumber == tileList.devices[i].serialNumber){
			tileUSB[i]->stopDevice();
		}
	}
}


TACTONIC_LIB TactonicFrame* WINAPI Tactonic_CreateFrame(TactonicDevice device)
{
    TactonicFrame *frame = new TactonicFrame();
	frame->forces = new int[device.rows * device.cols];
    frame->numForces = device.rows*device.cols;
    frame->rows = device.rows;
    frame->cols = device.cols;
	frame->frameNumber = 0;
	frame->time = 0;
	memset(frame->forces,0, device.rows*device.cols*sizeof(int));
    return frame;
}

TACTONIC_LIB void WINAPI Tactonic_DestroyFrame(TactonicFrame *frame)
{
	delete (frame->forces);
    frame->numForces = 0;
    frame->cols = 0;
    frame->rows = 0;
	frame->frameNumber = 0;
	frame->time = 0;
    delete (frame);
}

TACTONIC_LIB void WINAPI Tactonic_PollFrame(TactonicDevice device, TactonicFrame *frame)
{
	for(int i = 0; i < tileList.numDevices; i++){
		if(device.serialNumber == tileList.devices[i].serialNumber){
			tileUSB[i]->getFrame(&frame[0]);
		}
	}
}

TACTONIC_LIB void WINAPI Tactonic_CopyFrame(TactonicFrame *srcFrame, TactonicFrame *dstFrame)
{
	if(srcFrame->numForces != dstFrame->numForces){
        free(dstFrame->forces);
        dstFrame->forces = new int[srcFrame->numForces];
        dstFrame->numForces = srcFrame->numForces;
	}
    memcpy(dstFrame->forces, srcFrame->forces, srcFrame->numForces*sizeof(int));
    dstFrame->frameNumber = srcFrame->frameNumber;
    dstFrame->time = srcFrame->time;
}

TACTONIC_LIB void WINAPI Tactonic_AddFrameCallback(TactonicDevice device, TactonicFrameCallback callback)
{
	for(int i = 0; i < tileList.numDevices; i++){
		if(device.serialNumber == tileList.devices[i].serialNumber){
			tileUSB[i]->addFrameCallback(callback);
		}
	}
}


TACTONIC_LIB void WINAPI Tactonic_RemoveFrameCallback(TactonicDevice device, TactonicFrameCallback callback)
{
	for(int i = 0; i < tileList.numDevices; i++){
		if(device.serialNumber == tileList.devices[i].serialNumber){
			tileUSB[i]->removeFrameCallback(callback);
		}
	}
}
    
    
TACTONIC_LIB void WINAPI Tactonic_AddDeviceCallback(TactonicDevice device, TactonicDeviceCallback callback)
{
    for(int i = 0; i < tileList.numDevices; i++){
		if(device.serialNumber == tileList.devices[i].serialNumber){
			tileUSB[i]->addDeviceCallback(callback);
		}
	}
}
    
    
TACTONIC_LIB void WINAPI Tactonic_RemoveDeviceCallback(TactonicDevice device, TactonicDeviceCallback callback)
{
    for(int i = 0; i < tileList.numDevices; i++){
		if(device.serialNumber == tileList.devices[i].serialNumber){
			tileUSB[i]->removeDeviceCallback(callback);
		}
	}
}

TACTONIC_LIB void WINAPI Tactonic_ResetDevice(TactonicDevice device)
{
	for(int i = 0; i < tileList.numDevices; i++){
		if(device.serialNumber == tileList.devices[i].serialNumber){
			tileUSB[i]->resetDevice();
		}
	}
}

TACTONIC_LIB void WINAPI Tactonic_SetDeviceControl(TactonicDevice device, int deviceControl, int deviceValue){
	for(int i = 0; i < tileList.numDevices; i++){
		if(device.serialNumber == tileList.devices[i].serialNumber){
			tileUSB[i]->setDeviceControl(deviceControl, deviceValue);
		}
	}
}

