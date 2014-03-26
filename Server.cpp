#include "stdafx.h"
#include "Server.h"
#include <sstream>
#include <iostream>
#include <iomanip>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

using namespace std;
Server::Server()
{
	this->sd = INVALID_SOCKET;
	
	result = NULL;
	recvBufLen = BUF_LEN;
	sendBufLen = BUF_LEN;

	iRes = WSAStartup(MAKEWORD(2,2), &wsaData);
	if(iRes !=0) 
	{
		cout<<"WSAStartup failed: " << iRes	<<endl;
		return;
	}


	this->sd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sd == INVALID_SOCKET)
	{
		cout << "socket creation failed: " << WSAGetLastError() << endl;
		closesocket(this->sd);
		WSACleanup();
	}

	memset((void *)&server, '\0', sizeof(struct sockaddr_in));
	server.sin_family=AF_INET;
	server.sin_port=htons(PORT);

	server.sin_addr.S_un.S_un_b.s_b1 = (unsigned char)192;
	server.sin_addr.S_un.S_un_b.s_b2 = (unsigned char)168;
	server.sin_addr.S_un.S_un_b.s_b3 = (unsigned char)1;
	server.sin_addr.S_un.S_un_b.s_b4 = (unsigned char)60;

	if(bind(this->sd,(struct sockaddr *) &server, sizeof(struct sockaddr_in)) ==-1)
	{
		cout << "bind failed: " << WSAGetLastError() << endl;
		closesocket(sd);
		WSACleanup();
	}
	memset((void*)&this->recvBuf, '\0', sizeof(char)*512);
	int client_length = (int)sizeof(struct sockaddr_in);
	//blocking call
	int bytes_received = recvfrom(this->getSock(), this->recvBuf, recvBufLen, 0,
		(struct sockaddr *)&client, &client_length);

	cout << "got: " << this->recvBuf << " from: " << 
		inet_ntoa(this->client.sin_addr) <<":"<< ntohs(client.sin_port) << endl;
	if (bytes_received < 0)
	{
		cout << "Could not receive datagram: " << WSAGetLastError() << endl;
		closesocket(sd);
		WSACleanup();
   
	}
}


SOCKET Server::getSock()
{
	const SOCKET sock = this->sd;
	return sock;
}

/*
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
	return 0;
}*/

int Server::respond()
{
	//send contents of sendBuf.

	int iSRes = sendto(this->sd, this->sendBuf, (int)this->sendBufLen, 0,
		(struct sockaddr*)&this->client, sizeof(this->client));
	if(iSRes == SOCKET_ERROR)
	{
		cout << "send failed with error: " << WSAGetLastError() << endl;
		closesocket(this->sd);
		WSACleanup();
		return 1;
	}
	cout << "Bytes sent: " << iSRes << endl;
	return 0;
}


Server::~Server()
{
	this->iRes = shutdown(this->sd, SD_SEND);
	if(this->iRes == SOCKET_ERROR)
	{
		cout << "shutdown failed with error: " << WSAGetLastError() << endl;
		closesocket(this->sd);
		WSACleanup();
		return;
	}
	closesocket(this->sd);
	WSACleanup();
}

int Server::sendPacket(VisionPacketEntry p)
{
	stringstream ss;
	/// each field consists of 16 characters, first 8 for an ident,
	/// second 8 for the value.
	/// any remaing spaces are '0' filled.
	ss 
		<<  setfill('0') << right //fill right of data with '0'
		<< setw(8) << "ID"
		<< setw(8) << p.id 
		<< setw(8) << "centerX"
		<< setw(8) << p.centerX
		<< setw(8) << "centerY"
		<< setw(8) << p.centerY
		<< setw(8) << "area"
		<< setw(8) << p.area
		<< setw(8) << "rCenterX"
		<< setw(9) << p.rCenterX
		<< setw(8) << "rCenterY"
		<< setw(9) << p.rCenterY 
		<< endl
		;
	string buf = ss.str();
	this->sendBuf = buf.c_str();
	this->sendBufLen = buf.length()*sizeof(char);
	this->respond();
	//this->Listen();
	//this->Accept();
	//this->Recv();
	return 0;
}
