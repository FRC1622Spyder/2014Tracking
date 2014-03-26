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
	size_t idLen = to_string((long double)p.id).length();//there's a better way to do this, right?
	size_t areaLen = to_string((long double)p.area).length();
	size_t cXLen = to_string((long double)p.centerX).length();
	size_t cYLen = to_string((long double)p.centerY).length();
	size_t cRXLen = to_string((long double)p.rCenterX).length();
	size_t cRYLen = to_string((long double)p.rCenterY).length();
	stringstream ss;
	/// each field consists of a name, a data length, and the data:
	/// field names are 4 bytes wide, and are followed immedately by the length of the data string they tag. 
	/// the tag is a fixed width of 3 bytes. the data follows a delimiter (a colin) to seperate the size tag
	/// and the data. any empty space is '0' filled.

	///sample data string:
	/// 00ID01T0xCen005T366.5yCen005T283.5area003T143rXCe008T0.145313rYCe005T0.515
	///  The first triad will read "ID10T" for any ID with 10 characters. 
	///  If we do manage to get over a billion objects, someone was quite the ID10T 
	///  with the filtering.

	ss 
		<<  setfill('0') << right //fill right of data with '0'
		
		<< setw(4) << "ID"
		<< setw(2) << idLen << "T"
		<< setw(idLen) << p.id 
		
		<< setw(4) << "xCen"
		<< setw(2) << cXLen << "T"
		<< setw(cXLen) << p.centerX
		
		<< setw(4) << "yCen"
		<< setw(2) << cYLen << "T"
		<< setw(cYLen) << p.centerY
		
		<< setw(4) << "area"
		<< setw(2) << areaLen << "T"
		<< setw(areaLen) << p.area
		
		<< setw(4) << "rXCe"
		<< setw(2) << cRXLen << "T"
		<< setw(cRXLen) << p.rCenterX
		
		<< setw(4) << "rYCe"
		<< setw(2) << cRYLen << "T"
		<< setw(cRYLen) << p.rCenterY 
		
		<< endl
		;
	string buf = ss.str();
	this->sendBuf = buf.c_str();
	this->sendBufLen = (int)(buf.length()*sizeof(char));
	this->respond();
	//this->Listen();
	//this->Accept();
	//this->Recv();
	return 0;
}
