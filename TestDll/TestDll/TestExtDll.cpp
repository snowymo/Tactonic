#include "TestExtDll.h"
#include <iostream>

TestExtDll::TestExtDll(void)
{
}


TestExtDll::~TestExtDll(void)
{
}

std::string TestExtDll::testInputandOutput (std::string str)
{
	std::cout << "input is:" << str << "\n";
	return str;
}

int TestExtDll::testInt (int i)
{
	std::cout << "input is " << i;
	return 2 * i;
}
