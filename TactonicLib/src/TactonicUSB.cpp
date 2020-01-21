#include <stdio.h>
#include <sstream>
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "TactonicUSB.h"
int stackPath[16];

using namespace std;

namespace tactonic{
	const unsigned int TactonicUSB::DEVICE_TIMEOUT(1000);

	TactonicUSB::TactonicUSB() :
		ready(false),
		numFrameCallback(0),
		numDeviceCallback(0),
		isConfigRead(false),
		isStopped(false),
		isTiles(false),
		numTiles(1){
			frameEvent.frame = new TactonicFrame();
	}

	TactonicUSB::~TactonicUSB(){
		if (ready)
			stopDevice();
	}
	bool TactonicUSB::openDevice(char* index){
		if (ready)
			return false;
		int openError = 0;
		openError = FT_OpenEx(index, FT_OPEN_BY_SERIAL_NUMBER, &ftHandle)  | openError;
		baudRate =921600;
		openError = FT_SetBaudRate(ftHandle, baudRate) | openError; 
		openError = FT_SetDataCharacteristics(ftHandle, FT_BITS_8, FT_STOP_BITS_1, FT_PARITY_NONE) | openError;
		DWORD inTransferSize = 4096;
		DWORD outTransferSize = 16;
		openError = FT_SetUSBParameters(ftHandle, inTransferSize, outTransferSize) | openError;
		openError = FT_SetFlowControl(ftHandle, FT_FLOW_NONE, 0, 0)  | openError;
		openError =FT_SetTimeouts(ftHandle, 1000, 1000) | openError;
		openError = FT_SetLatencyTimer(ftHandle, 2) | openError;
		openError= FT_Purge(ftHandle, FT_PURGE_RX | FT_PURGE_TX)| openError;
		if(openError != FT_OK){
			FT_Close(ftHandle);            
			callDeviceEventCallback(TACTONIC_DEVICE_ERROR, TACTONIC_ERROR_OPEN);			
			return false;		
		}
		memset(&deviceFrameInfo, 0, sizeof(TactonicDevice));
		if (!(readDeviceInformation(&deviceFrameInfo,sizeof(TactonicDeviceInfo)))){
			FT_Close(ftHandle);
			callDeviceEventCallback(TACTONIC_DEVICE_ERROR, TACTONIC_ERROR_READ);
			return false;
		}
		isConfigRead = true;
		return true;
	}

	bool TactonicUSB::readDeviceInformation(TactonicDeviceInfo *deviceInfo, unsigned int sizeofInfoStruct){

		unsigned char command = COMMAND_FIRMWARE_VERSION;
		master = 0;
		for(int i = 0; i < 16; i++){
			command = COMMAND_ACK|i;
			writeUSB(&command, 1);
			Sleep(10);
		}
		command = 1;
		bool commandSet = false;
		while(command != 0){
			command = 0;
			readUSB(&command, 1);
			if((command&0xF0) == COMMAND_NACK && !commandSet){
				printf("Master ID: %d\n", command&0x0F);
				master = command&0x0F;
				commandSet = true;
			}
		}
		command = COMMAND_FIRMWARE_VERSION+master;
		int counter = 0;
		memset(firmwareVersion, 0, 3);
		while ((firmwareVersion[0] < TACTONIC_COMPATABLE_FIRMWARE_LOW || firmwareVersion[0] > TACTONIC_COMPATABLE_FIRMWARE_HIGH) && counter < 4){
			memset(firmwareVersion, 0, 3);
			writeUSB(&command, 1);
			readUSB(firmwareVersion, 2);
			if(firmwareVersion[0] == 6)
				readUSB(&firmwareVersion[2],1);
			else
				firmwareVersion[2] = 0;
			counter++;
		}

		if(firmwareVersion[0] < TACTONIC_COMPATABLE_FIRMWARE_LOW || firmwareVersion[0] > TACTONIC_COMPATABLE_FIRMWARE_HIGH){
			callDeviceEventCallback(TACTONIC_DEVICE_ERROR, TACTONIC_ERROR_FIRMWARE);
			return false;
		}

		if(firmwareVersion[2] != 0)
			isTiles = true;

		if(isTiles){
			numTiles = 0;
			int stackPos = -1;

			for(int i = 0; i < 16; i++){
				tileX[i] = -100;
				tileY[i] = -100;
				tileID[i] = -100;
				stackPath[i] = -100;

			}
			for(int i = 0; i < 16; i++){
				command = COMMAND_ACK|i;
				writeUSB(&command, 1);
				Sleep(10);
			}
			command = 1;
			while(command != 0){
				command = 0;
				readUSB(&command, 1);
				if((command&0xF0) == COMMAND_NACK){
					tileID[numTiles] = command&0x0F;
					printf("Tiles: %d %d\n", numTiles, command&0x0F);
					numTiles++;
				}
			}


			tileWidth = 1;
			tileHeight = 1;
			tileX[0] = 0;
			tileY[0] = 0;
			int tileMinX = 0;
			int tileMinY = 0;
			int currX = 0;
			int currY = 0;
			bool flip = true;
			int next = 0;
			int curr = 0;
			unsigned char buffer[2];
			while(next != -1){
				curr = next;
				next = -1;

				currX = tileX[curr];
				currY = tileY[curr];
				command = COMMAND_NEIGHBORS_ON + tileID[curr];
				writeUSB(&command, 1);
				readUSB(&command, 1);
				Sleep(1);
				for(int n = 0; n<4; n++){
					command = COMMAND_NEIGHBOR_0+(n*0x10) + tileID[curr];
					writeUSB(&command, 1);
					readUSB(&command, 1);
					Sleep(1);

					for(int j = 0; j < numTiles; j++){
						if(j != curr){
							command = COMMAND_NEIGHBORS_REPORT + tileID[j];
							writeUSB(&command, 1);
							readUSB(&command, 1);
							Sleep(1);

							for(int m = 0; m<4; m++){
								command = COMMAND_NEIGHBOR_0+(m*0x10) + tileID[j];
								writeUSB(&command, 1);
								readUSB(&command, 1);
								if(command != 32){
									if(tileX[j] == -100 && next == -1){
										if(n == 0)
											currX++;
										else if(n==1)
											currX--;
										else if(n==2)
											currY++;
										else if(n==3)
											currY--;
										tileX[j] = currX;
										tileY[j] = currY;
										tileWidth = max(currX+1, tileWidth);
										tileHeight = max(currY+1, tileHeight);
										tileMinX = min(currX, tileMinX);
										tileMinY = min(currY, tileMinY);
										next = j;
									}
								}
							}
							command = COMMAND_NEIGHBORS_REPORT + tileID[j];
							writeUSB(&command, 1);
							readUSB(&command, 1);
							Sleep(1);
						}
					}

				}
			}


			if(tileMinX < 0){
				tileWidth = tileWidth - tileMinX;
				for(int i = 0; i < numTiles; i++){
					tileX[i] = tileX[i] - tileMinX;
				}
			}
			if(tileMinY < 0){
				tileHeight = tileHeight - tileMinY;
				for(int i = 0; i < numTiles; i++){
					tileY[i] = tileY[i] - tileMinY;
				}
			}
		}
		FT_Purge(ftHandle, FT_PURGE_RX);

		command = COMMAND_INFO+master;
		writeUSB(&command, 1);
		readUSB(&command, 1);

		if (command != COMMAND_ACK){
			callDeviceEventCallback(TACTONIC_DEVICE_ERROR, TACTONIC_ERROR_READ);
			return false;
		}

		unsigned char* buffer = new unsigned char[sizeofInfoStruct];

		readUSB(buffer, sizeofInfoStruct); 

		memcpy(&deviceFrameInfo, buffer, sizeofInfoStruct);
		memcpy(&tileFrameInfo, buffer, sizeofInfoStruct);

		if(isTiles){
			deviceFrameInfo.rows *= tileHeight;
			deviceFrameInfo.cols *= tileWidth;
		}
		getDeviceInfo(&device);
		delete [] buffer;

		return true;
	}

	bool TactonicUSB::startDevice(){
		unsigned char command = COMMAND_START+master;

		if(isTiles){
			for(int i = 0; i < numTiles; i++){
				command = COMMAND_START|tileID[i];
				writeUSB(&command, 1);
				command = 0;
				readUSB(&command, 1);
			}
		}
		else{
			writeUSB(&command, 1);
			command = 0;
			readUSB(&command, 1);
		}


		writeFrame = 0;
		prevFrame = 1;

		deviceFrames[writeFrame].forces = new int[device.rows*device.cols];
		deviceFrames[writeFrame].frameNumber = 0;
		deviceFrames[writeFrame].time = 0;
		deviceFrames[prevFrame].forces = new int[device.rows*device.cols];
		deviceFrames[prevFrame].frameNumber = 0;
		deviceFrames[prevFrame].time = 0;
		for(int i = 0; i < device.rows*device.cols; i++){
			deviceFrames[writeFrame].forces[i] = 0;
			deviceFrames[prevFrame].forces[i] = 0;
		}
		frameSize = deviceFrameInfo.rows*deviceFrameInfo.cols + 1;
		buffer         = new unsigned char[4096];
		readPos        = 0;
		fillPos        = 0;
		frameBufferPos = 0;
		frameBuffer    = new int[tileFrameInfo.rows*tileFrameInfo.cols];

		if(!createLoopThread())
			return false;
		ready = true;
		callDeviceEventCallback(TACTONIC_DEVICE_STARTED, 0);
		return true;
	}

	void* TactonicUSB::readLoopPoll(void *tactonicDevice)
	{
		TactonicUSB *deviceUSB = static_cast<TactonicUSB *>(tactonicDevice);
		unsigned char command = COMMAND_FRAME+deviceUSB->master;
		deviceUSB->tileNum = 0;		
		while(!deviceUSB->isStopped)
		{
			if(!deviceUSB->isTiles){
				deviceUSB->readUSB(&command, 1);
				command = COMMAND_FRAME+deviceUSB->master;
			}
			else
				command = COMMAND_FRAME|deviceUSB->tileID[deviceUSB->tileNum];
			deviceUSB->writeUSB(&command, 1);
			deviceUSB->readUSB(&command, 1);
			if (command == COMMAND_ACK)
				deviceUSB->readTile();
			else{
				deviceUSB->callDeviceEventCallback(TACTONIC_DEVICE_ERROR, TACTONIC_ERROR_COMMUNICATION);
				FT_Purge(deviceUSB->ftHandle, FT_PURGE_RX);
			}


			//deviceUSB->readUSB(&command, 1);
			/*command = 0xA0;
			deviceUSB->writeUSB(&command, 1);
			deviceUSB->readUSB(&command, 1);
			if(command == COMMAND_ACK){
			deviceUSB->readUSB(&command, 1);
			printf("%d\n", command);
			deviceUSB->readUSB(&command, 1);
			printf("%d\n", command);
			for(int i = 0; i < command; i++){
			deviceUSB->readUSB(deviceUSB->buffer, 10);
			for(int j = 0; j < 10; j++){
			printf("%d, ", deviceUSB->buffer[j]);
			}
			printf("\n");

			}
			deviceUSB->readUSB(&command, 1);
			printf("%d\n", command);
			}*/



			if(deviceUSB->isTiles){
				deviceUSB->tileNum ++;
				if(deviceUSB->tileNum == deviceUSB->numTiles){				
					deviceUSB->tileNum = 0;
					for(int i = 0; i < 100; i++){
						clock();
					}
				}
			}	
		}		
		return (void*) true;
	}

	void TactonicUSB::readTile()
	{

		inFrame = true;
		unsigned int readBytes = 0;

		readUSB(buffer, 2);

		readBytes = (((unsigned int)(buffer[0])) << 6);
		readBytes+= buffer[1];

		readUSB(buffer, readBytes+2);
		fillPos = (fillPos + readBytes+2);
		readPos = 0;
		frameBufferPos = 0;

		if(readBytes < 2000){
			if(!uncompress())
			{
				inFrame = false;
				FT_Purge(ftHandle, FT_PURGE_RX);	
			}
		}
		readPos = 0;
		fillPos = 0;
		frameBufferPos = 0;
	}

	bool TactonicUSB::uncompress()
	{
		int i,j;
		bool endOfFrame = false;
		int value;
		for(readPos = 0; readPos < fillPos; readPos++) 
		{

			if (buffer[readPos] == 0xFF)
			{
				endOfFrame = true;
				break;
			}
			value = 0xFF & buffer[readPos];

			if((value >> 6)== 2){
				for(j = 0; j < (0x3F&value); j++) {
					frameBuffer[frameBufferPos++] = 0;
				}
			}
			else if((value>>6)==1){
				frameBuffer[frameBufferPos] = (0x3F&value)<<6;
			}
			else if((value>>6)==0){
				frameBuffer[frameBufferPos] = frameBuffer[frameBufferPos] + (value);
				frameBufferPos++;
			}
			else if((value>>6)==3){
				frameBuffer[frameBufferPos] = value;
				frameBufferPos++;
			}
		}

		if (frameBufferPos != tileFrameInfo.rows*tileFrameInfo.cols+1)
		{
			if (endOfFrame)
			{
				readPos = 0;
				fillPos = 0;
				frameBufferPos = 0;
				return false;
			}
			return true;
		}

		readPos = 0;
		fillPos = 0;
		frameBufferPos = 0;

		int* const forces = deviceFrames[writeFrame].forces;
		deviceFrames[writeFrame].frameNumber = deviceFrames[prevFrame].frameNumber+1;

		value = 0;
		int frameIndex = 1;
		int forceIndex = 0;
		int rowWidth = 0;
		if(isTiles){
			forceIndex = tileFrameInfo.cols*tileFrameInfo.rows*tileWidth*tileY[tileNum]+tileX[tileNum]*tileFrameInfo.cols; 
			rowWidth = (tileWidth-1)*tileFrameInfo.cols;
		}
		for(i = 0; i < tileFrameInfo.rows; i++) {
			for(int j = 0; j < tileFrameInfo.cols; j++){
				value = (0xFFF&frameBuffer[frameIndex]);
				frameBuffer[frameIndex] = 0;
				frameIndex++;

				forces[forceIndex]=value;
				forceIndex++;
			}
			forceIndex += rowWidth;
		}

		if(tileNum == numTiles-1){
			deviceFrames[writeFrame].frameNumber = deviceFrames[prevFrame].frameNumber+1;
			updateFrameTime();
			updateFrameImage();

			if (numFrameCallback > 0)
			{
				frameEvent.device = device;
				frameEvent.frame->forces = deviceFrames[prevFrame].forces;
				frameEvent.frame->numForces = device.rows*device.cols;
				frameEvent.frame->rows = deviceFrameInfo.rows;
				frameEvent.frame->cols = deviceFrameInfo.cols;
				frameEvent.frame->frameNumber = deviceFrames[prevFrame].frameNumber;
				frameEvent.frame->time = deviceFrames[prevFrame].time;
				for(int c = 0; c < numFrameCallback; c++){
					if(frameCallbackList[c] != 0){
						frameCallbackList[c](&frameEvent);
					}
				}
			}
		}
		inFrame = false;
		return true;
	}

	void TactonicUSB::updateFrameImage()
	{
		if(prevFrame == 0){
			prevFrame = 1;
			writeFrame = 0;
		}
		else{
			prevFrame = 0;
			writeFrame = 1;
		}
	}

	void TactonicUSB::updateFrameTime()
	{
		double imageTime = deviceFrames[writeFrame].time;
#ifdef _WIN32
		double prevImageTime = deviceFrames[prevFrame].time;
		__int64 time;
		QueryPerformanceCounter((LARGE_INTEGER*)&time);
		deviceFrames[writeFrame].time =  (double)time / (double)freq;
#else
		deviceFrames[writeFrame].time =  (double)(clock()/(1.0*CLOCKS_PER_SEC));
#endif
		imageTime = deviceFrames[writeFrame].time;

	}

	bool TactonicUSB::stopDevice()
	{
		if (!ready)
			return false;
		unsigned char command = COMMAND_STOP+master;
		writeUSB(&command, 1);
		readUSB(&command, 1);
		isStopped = true;
		command = COMMAND_RESET;
		writeUSB(&command, 1);
#ifdef _WIN32
		FT_STATUS ftStatus;
		WaitForSingleObject(threadRead, INFINITE);
		GetExitCodeThread(threadRead, (DWORD*)&ftStatus);
		CloseHandle(threadRead);
#else
		pthread_join(readThread, NULL);
		pthread_exit(NULL);
#endif
		isStopped = false;
		readPos = 0;
		fillPos = 0;
		frameBufferPos = 0;
		FT_Close(ftHandle);	
		ready = false;
		isConfigRead = false;
		callDeviceEventCallback(TACTONIC_DEVICE_STOPPED, 0);
		return true;
	}

	bool TactonicUSB::initFrame(TactonicFrame* frame)
	{
		if (!isConfigRead)
			return false;
		if (!frame)
			return false;
		frame->forces = new int[deviceFrameInfo.rows * deviceFrameInfo.cols];
		frame->frameNumber = 0;
		frame->time = 0;
		return true;
	}

	bool TactonicUSB::getFrame(TactonicFrame* frame)
	{
		if (!ready || !frame)
			return false;
		memcpy(frame->forces, deviceFrames[prevFrame].forces, deviceFrameInfo.rows*deviceFrameInfo.cols * sizeof(int));
		frame->frameNumber = deviceFrames[prevFrame].frameNumber;
		frame->time = deviceFrames[prevFrame].time;
		return true;
	}

	bool TactonicUSB::getDeviceInfo(TactonicDevice *device)
	{
		device->serialNumber = deviceFrameInfo.serialNumber<<16;
		device->serialNumber += deviceFrameInfo.serialNumber2;
		device->rows = deviceFrameInfo.rows;
		device->cols = deviceFrameInfo.cols;
		device->rowSpacingUM = deviceFrameInfo.rowSpacingUM;
		device->colSpacingUM = deviceFrameInfo.colSpacingUM;
		return true;
	}

	bool TactonicUSB::addFrameCallback(TactonicFrameCallback newCallback)
	{
		if(numFrameCallback > 31)
			return false;
		frameCallbackList[numFrameCallback] = newCallback;
		numFrameCallback++;
		return true;
	}

	bool TactonicUSB::removeFrameCallback(TactonicFrameCallback callback)
	{
		for(int i = 0; i < numFrameCallback; i++){
			if(callback == frameCallbackList[i])
				frameCallbackList[i] = 0;
		}
		return true;
	}

	bool TactonicUSB::addDeviceCallback(TactonicDeviceCallback newCallback)
	{
		if(numDeviceCallback > 31){
			return false;
		}
		deviceCallbackList[numDeviceCallback] = newCallback;
		numDeviceCallback++;
		return true;
	}

	bool TactonicUSB::removeDeviceCallback(TactonicDeviceCallback callback)
	{
		for(int i = 0; i < numDeviceCallback; i++){
			if(callback == deviceCallbackList[i])
				deviceCallbackList[i] = 0;
		}
		return true;
	}

	bool TactonicUSB::resetDevice()
	{
		unsigned char command = COMMAND_RESET;
		writeUSB(&command, 1);
		readUSB(&command, 1);
		return true;
	}


	void TactonicUSB::setDeviceControl(int deviceControl, int controlValue)
	{
		if(deviceControl > 16){
			unsigned char command = (unsigned char)(0xFF & deviceControl);
			command = (0xFF & deviceControl);
			writeUSB( &command, 1);
			command = (unsigned char)(0xFF & controlValue);
			command = (0xFF & controlValue);
			writeUSB( &command, 1);
			//readUSB( &command, 1);
			return;
		}
		return;
	}

	void TactonicUSB::callDeviceEventCallback(TactonicDeviceEventType evtType, int evtCode)
	{
		deviceEvent.device = device;
		deviceEvent.eventType = evtType;
		deviceEvent.eventCode = evtCode;
		for(int i = 0 ; i < numDeviceCallback; i++){
			deviceCallbackList[i](&deviceEvent);
		}
	}

	void TactonicUSB::writeUSB(unsigned char* buffer, int bufferLen){
		DWORD       totalWrite;
		if ((FT_Write(ftHandle, buffer, bufferLen, &totalWrite)) != FT_OK)
		{
			callDeviceEventCallback(TACTONIC_DEVICE_ERROR, TACTONIC_ERROR_WRITE);
			return;
		}
	}

	void TactonicUSB::readUSB(unsigned char* buffer, int bufferLen){
		DWORD       totalWrite;

		if ((FT_Read(ftHandle, buffer, bufferLen, &totalWrite)) != FT_OK)
		{
			callDeviceEventCallback(TACTONIC_DEVICE_ERROR, TACTONIC_ERROR_READ);
			return;
		}
	}

	bool TactonicUSB::createLoopThread(){

#ifdef _WIN32
		QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		threadRead = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)readLoopPoll, (void *)this, 0, NULL);
		if (threadRead == NULL)
			return false;
		SetThreadPriority(threadRead, THREAD_PRIORITY_HIGHEST);
#else
		pthread_attr_init(&pthread_custom_attr);
		pthread_create(&readThread,&pthread_custom_attr, readLoopPoll,(void *)this);
#endif
		return true;
	}
}