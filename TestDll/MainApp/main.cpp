#include "dll.h"
#include <iostream>

int main()
{
	//TestExtDll * test = new TestExtDll;
	int i = 2;
	//int o = test->testInt (i);
	int o = testInt(i);
	std::cout << "output is " << o << "\n";
	return 0;
}