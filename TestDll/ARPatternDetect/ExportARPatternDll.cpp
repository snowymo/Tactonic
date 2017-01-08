//ºê¶¨Òå  
#define EXPORTBUILD_AR_PATTERN

#include "ExportARPatternDll.h"
#include "ARPatternDetect.h"

extern "C" bool _DLLExport detectPattern (unsigned char * inputOutputLeft,unsigned char * inputOutputRight, ARTCSPattern* results)
{
	bool isDetect = ARPatternDetect::getInstance ()->detectPattern (inputOutputLeft);
	ARPatternDetect::getInstance ()->getResult (results);
	return isDetect;
}

extern "C" void _DLLExport initPattern ()
{
	ARPatternDetect::getInstance ()->init ();
	ARPatternDetect::getInstance ()->cleanup ();
}
