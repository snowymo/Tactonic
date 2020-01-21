#include "UDPClient.h"


UDPClient::UDPClient(char* serverName)
{
    ackFromServer = false;

    WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (serverName == NULL)
        serverName = (char*)"127.0.0.1";

    len = sizeof(serverInfo);
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(DEFAULT_PORT);
    inet_pton(AF_INET, serverName, &(serverInfo.sin_addr.s_addr));

    socketC = socket(AF_INET, SOCK_DGRAM, 0);
    
}


UDPClient::~UDPClient()
{
}

bool UDPClient::sendMsg(std::string msg)
{    
    if (strcmp(msg.c_str(), "exit") == 0)
        return false;
    if (sendto(socketC, msg.c_str(), msg.length(), 0, (sockaddr*)&serverInfo, len) != SOCKET_ERROR)
    {
        char buffer[1024];
        ZeroMemory(buffer, sizeof(buffer));
        if (ackFromServer) {
            if (recvfrom(socketC, buffer, sizeof(buffer), 0, (sockaddr*)&serverInfo, &len) != SOCKET_ERROR)
            {
                printf("Receive response from server: %s\n", buffer);
            }
        }        
    }
    return true;
}

void UDPClient::closeConnection()
{
    closesocket(socketC);
}
