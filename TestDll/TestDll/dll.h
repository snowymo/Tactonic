#if defined (EXPORTBUILD)  
# define _DLLExport __declspec (dllexport)  
# else  
# define _DLLExport __declspec (dllimport)  
#endif

#include <string>

extern "C" int _DLLExport testInnOut(std::string str);
extern "C" int _DLLExport testInt(int i);