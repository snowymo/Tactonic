#pragma once

#include <Ws2tcpip.h>
#include <winsock2.h>
#include <stdio.h>
#include <string>
#include <iostream>

#define DEFAULT_PORT 27015

class UDPClient
{
public:
    UDPClient(char* serverName = NULL);
    ~UDPClient();

    bool sendMsg(std::string msg);
    void closeConnection();
    void setAckFromServer(bool b) { ackFromServer = b; }


private:
    WSADATA wsaData;
    SOCKET socketC;
    int len;
    struct sockaddr_in serverInfo;
    bool ackFromServer;
};

