
#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#define BUF_LEN 512
#define PORT "60"

struct VisionPacketEntry
{
	unsigned int id;
	double centerX;
	double centerY;
	int area;
	double rCenterX;
	double rCenterY;
};

class Server
{
	WSADATA wsaData;
	int iRes;
	SOCKET sockListen;
	SOCKET client;
	struct addrinfo *result;
	struct addrinfo hints;
	int sendRes;
	char recvBuf[BUF_LEN];
	int recvBufLen;
	char *sendBuf;
	int sendBufLen;
	int Listen();
	int Recv();
	int Accept();
	int respond();
public:
	int sendPacket(std::vector<VisionPacketEntry> *p);
	Server();
	~Server();

};


