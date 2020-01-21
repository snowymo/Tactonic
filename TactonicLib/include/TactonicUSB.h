#ifndef __TactonicUSB_H__
#define __TactonicUSB_H__

#ifdef _WIN32
#include <windows.h>
#include <ftd2xx.h>
#else
#include <pthread.h>
#include <ftd2xxMac.h>
#define Sleep(a) usleep(a*1000)
#endif

#include <ctime>
#include "Tactonic.h"
#include "TactonicDev.h"

#define COMMAND_ACK               0x10
#define COMMAND_NACK              0x20
#define COMMAND_RESET             0x30 
#define COMMAND_INFO              0x40
#define COMMAND_STOP              0x50
#define COMMAND_FRAME             0x60
#define COMMAND_FIRMWARE_VERSION  0x70 
#define COMMAND_NEIGHBORS_ON      0x80 
#define COMMAND_START             0x90
#define COMMAND_NEIGHBOR_0        0xA0
#define COMMAND_NEIGHBOR_1        0xB0
#define COMMAND_NEIGHBOR_2        0xC0
#define COMMAND_NEIGHBOR_3	      0xD0
#define COMMAND_NEIGHBORS_REPORT   0xE0
#define COMMAND_ALT			      0xF0

#define TACTONIC_LIBRARY_VERSION_MAJOR     6
#define TACTONIC_LIBRARY_VERSION_MINOR     1

#define TACTONIC_COMPATABLE_FIRMWARE_LOW 4
#define TACTONIC_COMPATABLE_FIRMWARE_HIGH 6


typedef struct tactonic_device_info_s{
	unsigned short rows;
	unsigned short cols;
	unsigned short rowSpacingUM;
	unsigned short colSpacingUM;
	unsigned short serialNumber;
	unsigned short serialNumber2;	
	unsigned char comment[16];		
} TactonicDeviceInfo;


namespace tactonic
{
	class TactonicUSB
	{

	private:
		static const unsigned int DEVICE_TIMEOUT;
		TactonicFrameCallback	frameCallbackList[32];
		int                     numFrameCallback;
		TactonicDeviceCallback	deviceCallbackList[32];
		int                     numDeviceCallback;
		FT_HANDLE			ftHandle;

#ifdef _WIN32
		HANDLE              threadRead;
		__int64				freq;
#else
        pthread_t           readThread;
        pthread_attr_t pthread_custom_attr;
#endif

		TactonicDeviceInfo	deviceFrameInfo;
		TactonicDeviceInfo	tileFrameInfo;
		TactonicFrame		deviceFrames[2];
        TactonicFrameEvent  frameEvent;
        TactonicDeviceEvent  deviceEvent;
		
		int					tileNum;
		int					prevFrame;
		int					writeFrame;
		int					baudRate;
		int					frameSize;
		bool				inFrame;

		bool				ready;        
		bool				isConfigRead;
		bool                isStopped;
		unsigned char*		buffer;
		int*				frameBuffer;
		unsigned int		readPos;
		unsigned int		fillPos;
		unsigned int		frameBufferPos;
		
		int tileID[16];
		int tileX[16];
		int tileY[16];
		int tileWidth;
		int tileHeight;
		
	public:
		TactonicDevice		device;

	public:
		bool				isTiles;
		int					numTiles;
		int					master;
		unsigned char       firmwareVersion[3];

		TactonicUSB();
		~TactonicUSB();
        
		bool			openDevice(char* index);
		bool			startDevice();
		bool			getDeviceInfo(TactonicDevice *deviceInfo);
		bool			initFrame(TactonicFrame* frame);
		bool			getFrame(TactonicFrame *frame);
		bool			stopDevice();
		bool			resetDevice();
		bool			addFrameCallback(TactonicFrameCallback callback);
		bool			removeFrameCallback(TactonicFrameCallback callback);
		bool			addDeviceCallback(TactonicDeviceCallback callback);
		bool			removeDeviceCallback(TactonicDeviceCallback callback);
		void			setDeviceControl(int deviceControl, int controlValue);
        void            callDeviceEventCallback(TactonicDeviceEventType evtType, int evtCode);

	private:
		bool			readDeviceInformation(TactonicDeviceInfo *deviceInfo, unsigned int sizeofInfoStruct);
		void			updateFrameImage();
		void			updateFrameTime();
		
		static void*	readLoopPoll(void *device);
		void			readTile();
		bool			fillReadBuffer(unsigned int num);
		bool			uncompress();
		void			writeUSB(unsigned char* buffer, int bufferLen);
		void			readUSB(unsigned char* buffer, int bufferLen);
		bool			createLoopThread();
	};

}

#endif // __TactonicDevice_H__
