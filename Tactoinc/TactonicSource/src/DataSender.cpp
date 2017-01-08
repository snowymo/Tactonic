#include "DataSender.h"


DataSender::DataSender()
{
	int status = WSAStartup(MAKEWORD(2, 2), &data);
	if (status != 0)
		throw system_error(WSAGetLastError(), system_category(), "WSAStartup Failed!");

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (sock == INVALID_SOCKET)
		throw system_error(WSAGetLastError(), system_category(), "Error Opening Socket!");
}


DataSender::~DataSender()
{
	WSACleanup();
	closesocket(sock);
}

void DataSender::sendTo(const string& address, unsigned short port, const char* buffer, int length, int flags = 0){
	sockaddr_in add;
	add.sin_family = AF_INET;
	in_addr s;
	inet_pton(AF_INET, address.c_str(), (void*)&s);
	add.sin_addr = s;
	add.sin_port = htons(port);
	int statusCode = sendto(sock, buffer, length, flags, reinterpret_cast<SOCKADDR*>(&add), sizeof(add));
	if (statusCode < 0)
		throw system_error(WSAGetLastError(), system_category(), "Data Sending Failed!");
}

