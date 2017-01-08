#ifndef _EXPORT_ARPATTERN_DLL_H
#define _EXPORT_ARPATTERN_DLL_H

#if defined (EXPORTBUILD_AR_PATTERN)
# define _DLLExport __declspec (dllexport)  
# else  
# define _DLLExport __declspec (dllimport)  
#endif

#include "ARPattern.h"
#include <vector>

extern "C" bool _DLLExport detectPattern(unsigned char * inputOutputLeft,unsigned char * inputOutputRight, ARTCSPattern* results);

extern "C" void _DLLExport initPattern();

#endif