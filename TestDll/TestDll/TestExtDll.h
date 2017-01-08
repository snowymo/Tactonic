/************************************************************************/
/* Author:hehe */
/* test unity with extra dll */
/************************************************************************/

#ifndef _TEST_EXT_DLL
#define _TEST_EXT_DLL

#include <string>

class TestExtDll
{
public:
	TestExtDll(void);
	~TestExtDll(void);

public:
	std::string testInputandOutput(std::string str);
	int testInt(int i);
};

#endif