
#include "stdafx.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define BUF_LEN 512
#define PORT "60"

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
	int Listen();
	int Recv();
	int Accept();
	void respond();
public:
	
	Server();
	~Server();

};


