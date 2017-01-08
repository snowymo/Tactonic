#pragma once
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <iostream>
#include <system_error>

#pragma comment(lib, "ws2_32.lib")

using namespace std;

class DataSender
{
public:
	DataSender();
	~DataSender();

	void sendTo(const string& address, unsigned short port, const char* buffer, int length, int flags);
	//void bindPort(unsigned short part);

private:
	WSADATA data;
	SOCKET sock;

};

