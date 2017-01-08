//ºê¶¨Òå  
#define  EXPORTBUILD  

#include "dll.h"
#include "TestExtDll.h"

int _DLLExport testInnOut (std::string str)
{
	TestExtDll* myDllClass = new TestExtDll;
	return atoi(myDllClass->testInputandOutput (str).c_str ());
}

extern "C" int _DLLExport testInt (int i)
{
	TestExtDll* myDllClass = new TestExtDll;
	return myDllClass->testInt (i);
}
