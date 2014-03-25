#include "stdafx.h"
#include "Server.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

using namespace std;
Server::Server()
{
	this->sockListen = INVALID_SOCKET;
	this->client = INVALID_SOCKET;
	this->result = NULL;
	this->recvBufLen = BUF_LEN;
	this->sendBufLen = BUF_LEN;

	this->iRes = WSAStartup(MAKEWORD(2,2), &this->wsaData);
	if(iRes !=0) 
	{
		cout<<"WSAStartup failed: " << iRes	<<endl;
		return;
	}

	ZeroMemory(&this->hints, sizeof(this->hints));
	this->hints.ai_family = AF_INET;
	this->hints.ai_socktype = SOCK_DGRAM;
	this->hints.ai_protocol = IPPROTO_UDP;
	this->hints.ai_flags = AI_PASSIVE;

	this->iRes = getaddrinfo(NULL, PORT, &this->hints, &this->result);
	if(this->iRes !=0)
	{
		cout<< "getaddrinfo failed with error: " << this->iRes << endl;
		return;
	}

	this->sockListen = socket(this->result->ai_family, this->result->ai_socktype, this->result->ai_protocol);
	if(this->sockListen == INVALID_SOCKET)
	{
		cout << "socket failed with error: " << WSAGetLastError() << endl;
		return;
	}

	this->iRes = bind(this->sockListen, this->result->ai_addr, (int)this->result->ai_addrlen);
	if(this->iRes == SOCKET_ERROR)
	{
		cout << "bind failed with error: " << WSAGetLastError() <<endl;
		freeaddrinfo(this->result);
		closesocket(this->sockListen);
		WSACleanup();
		return;
	}

	freeaddrinfo(this->result);

	this->Listen();
	this->Accept();
	this->Recv();
	
}

int Server::Recv()
{
	do {
		this->iRes = recv(this->client, this->recvBuf, this->recvBufLen, 0);
		if(this->iRes > 0)
		{
			cout << "Got " << this->iRes << " bytes" <<endl;
			this->respond();
		}
		else if(this->iRes == 0)
		{
			cout << "Connection closing" <<endl;
		}
		else
		{
			cout << "recv failed with error: " << WSAGetLastError() << endl;
			closesocket(this->client);
			WSACleanup();
			return 1;
		}
	} while(this->iRes >0);
}

int Server::respond()
{
	

	//send contents of sendBuf.
	int iSRes = send(this->client, this->sendBuf, this->sendBufLen, 0);
	if(iSRes == SOCKET_ERROR)
	{
		cout << "send failed with error: " << WSAGetLastError() << endl;
		closesocket(this->client);
		WSACleanup();
		return 1;
	}
	cout << "Bytes sent: " << iSRes << endl;
}

int Server::Listen()
{
	//blocking call
	this->iRes = listen(this->sockListen, SOMAXCONN);
	if(this->iRes == SOCKET_ERROR)
	{
		cout << "listen failed with error: " << WSAGetLastError() <<endl;
		closesocket(this->sockListen);
		WSACleanup();
		return 1;
	}
}

int Server::Accept()
{
	//blocking call
	this->client = accept(this->sockListen, NULL, NULL);
	if(client == INVALID_SOCKET)
	{
		cout << "accept failed with error: " << WSAGetLastError() << endl;
		closesocket(this->client);
		WSACleanup();
		return 1;
	}
}

Server::~Server()
{
	this->iRes = shutdown(this->client, SD_SEND);
	if(this->iRes == SOCKET_ERROR)
	{
		cout << "shutdown failed with error: " << WSAGetLastError() << endl;
		closesocket(this->client);
		WSACleanup();
		return;
	}
	closesocket(this->client);
	WSACleanup();
}

int Server::sendPacket(vector<VisionPacketEntry> *p)
{
	this->sendBuf = reinterpret_cast<char*>(p); //this is probably on that list
	this->sendBufLen = sizeof(&p);              //of things one should never do.
}
