#include <string.h>

#include "Tactonic.h"
#include "TactonicJNI.h"

using namespace std;
TactonicDeviceList *deviceList;
TactonicFrame *frames[16];

JNIEnv *m_env;
JavaVM *m_vm;
void frameCallback(TactonicFrameEvent *evt);
void deviceCallback(TactonicDeviceEvent *evt);

void frameCallback(TactonicFrameEvent *evt){
			
	m_vm->AttachCurrentThread( (void**) &m_env, NULL );
	jclass cls = m_env->FindClass( "com/tactonic/api/TactonicJNI" );
	jmethodID mid = m_env->GetStaticMethodID(cls, "frameCallback", "(I)V");
	m_env->CallStaticVoidMethod(cls, mid, (int)evt->device.serialNumber);

}
void deviceCallback(TactonicDeviceEvent *evt){
			
}


JNIEXPORT void JNICALL Java_com_tactonic_api_TactonicJNI_getDeviceList(JNIEnv *env, jobject obj, jobject sensors)
{
	deviceList = Tactonic_GetDeviceList();
	jfieldID fid;
	jmethodID mid;
	jclass cls = env->GetObjectClass(sensors);

	fid = env->GetFieldID(cls, "numDevices", "I");
	env->SetIntField(sensors, fid, deviceList->numDevices);

	mid = env->GetMethodID(cls, "setDevice", "(IIII)V");
	
	for(int i = 0; i < deviceList->numDevices; i++){
		env->CallVoidMethod(sensors, mid, i, deviceList->devices[i].serialNumber, deviceList->devices[i].rows, deviceList->devices[i].cols);
        
        frames[i] = Tactonic_CreateFrame(deviceList->devices[i]);
	}
}

JNIEXPORT void JNICALL Java_com_tactonic_api_TactonicJNI_startDevice(JNIEnv *env, jobject obj, jobject handle)
{
	jmethodID     mid;
	jclass cls = env->GetObjectClass(handle);
	mid = env->GetMethodID(cls, "getSerialNumber", "()I");
	jint serial =  env->CallIntMethod(handle, mid);
	for(int i = 0; i < deviceList->numDevices; i++){
		if(deviceList->devices[i].serialNumber == serial){
			Tactonic_StartDevice(deviceList->devices[i]);
			
		} 
	}
}

JNIEXPORT void JNICALL Java_com_tactonic_api_TactonicJNI_stopDevice(JNIEnv *env, jobject obj, jobject handle)
{
	jmethodID     mid;
	jclass cls = env->GetObjectClass(handle);
	mid = env->GetMethodID(cls, "getSerialNumber", "()I");
	jint serial =  env->CallIntMethod(handle, mid);
	for(int i = 0; i < deviceList->numDevices; i++){
		if(deviceList->devices[i].serialNumber == serial){
			Tactonic_StopDevice(deviceList->devices[i]);
		}
	}
}

JNIEXPORT void JNICALL Java_com_tactonic_api_TactonicJNI_pollFrame(JNIEnv *env, jobject obj, jobject handle, jobject jFrame)
{
	jmethodID     mid;
	jclass cls = env->GetObjectClass(handle);
	mid = env->GetMethodID(cls, "getSerialNumber", "()I");
	jint serial =  env->CallIntMethod(handle, mid);
	for(int i = 0; i < deviceList->numDevices; i++){
		if(deviceList->devices[i].serialNumber == serial){
			jfieldID     fid;
			Tactonic_PollFrame(deviceList->devices[i], frames[i]);
			jclass cls = env->GetObjectClass(jFrame);
			fid = env->GetFieldID(cls, "image", "[I");

			if(fid){
				jintArray arr = (jintArray)env->GetObjectField(jFrame, fid);
				jsize len = env->GetArrayLength(arr);
				if(len > 0){
					jboolean     isCopy;
					jint* tab = env->GetIntArrayElements(arr, &isCopy);
					for(int j = 0; j < deviceList->devices[i].rows*deviceList->devices[i].cols; j++){
						tab[j] = (int)(frames[i]->forces[j]);
					}
					env->ReleaseIntArrayElements((jintArray)arr, tab, 0);
					
					fid = env->GetFieldID(cls, "frameNumber", "J");
					env->SetLongField(jFrame, fid, frames[i]->frameNumber);

					fid = env->GetFieldID(cls, "time", "D");
					env->SetDoubleField(jFrame, fid, frames[i]->time);
				}
			}
		}
	}
}

JNIEXPORT void JNICALL Java_com_tactonic_api_TactonicJNI_addFrameCallback(JNIEnv *env, jobject obj, jobject handle, jobject jListener)
{
	jmethodID     mid;
	jclass cls = env->GetObjectClass(handle);
	mid = env->GetMethodID(cls, "getSerialNumber", "()I");
	jint serial =  env->CallIntMethod(handle, mid);
	for(int i = 0; i < deviceList->numDevices; i++){
		if(deviceList->devices[i].serialNumber == serial){
			env->GetJavaVM( &m_vm );
			Tactonic_AddFrameCallback(deviceList->devices[i],frameCallback);
		}
	}
}

JNIEXPORT void JNICALL Java_com_tactonic_api_TactonicJNI_removeFrameCallback(JNIEnv *env, jobject obj, jobject handle, jobject jListener)
{
	jmethodID     mid;
	jclass cls = env->GetObjectClass(handle);
	mid = env->GetMethodID(cls, "getSerialNumber", "()I");
	jint serial =  env->CallIntMethod(handle, mid);
	for(int i = 0; i < deviceList->numDevices; i++){
		if(deviceList->devices[i].serialNumber == serial){
			env->GetJavaVM( &m_vm );
			Tactonic_RemoveFrameCallback(deviceList->devices[i],frameCallback);
		}
	}
}

JNIEXPORT void JNICALL Java_com_tactonic_api_TactonicJNI_addDeviceCallback(JNIEnv *env, jobject obj, jobject handle, jobject jListener)
{
	jmethodID     mid;
	jclass cls = env->GetObjectClass(handle);
	mid = env->GetMethodID(cls, "getSerialNumber", "()I");
	jint serial =  env->CallIntMethod(handle, mid);
	for(int i = 0; i < deviceList->numDevices; i++){
		if(deviceList->devices[i].serialNumber == serial){
			env->GetJavaVM( &m_vm );
			//Tactonic_AddDeviceCallback(deviceList->devices[i],deviceCallback);
		}
	}
}

JNIEXPORT void JNICALL Java_com_tactonic_api_TactonicJNI_removeDeviceCallback(JNIEnv *env, jobject obj, jobject handle, jobject jListener)
{
	jmethodID     mid;
	jclass cls = env->GetObjectClass(handle);
	mid = env->GetMethodID(cls, "getSerialNumber", "()I");
	jint serial =  env->CallIntMethod(handle, mid);
	for(int i = 0; i < deviceList->numDevices; i++){
		if(deviceList->devices[i].serialNumber == serial){
			env->GetJavaVM( &m_vm );
			//Tactonic_RemoveDeviceCallback(deviceList->devices[i],deviceCallback);
		}
	}
}
