#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <Windows.h>

using namespace std;

class tcpClient
{
public:
	tcpClient(string, int);
	~tcpClient();

	bool getSocketResult();
	string tcpClientRequestCMD(string);
	void setSocketTimeout(int);

private:
	SOCKADDR_IN addr; //Address to be binded to our connection socket.
	SOCKET Connection;
	u_long iResult;
	int timeout = 5000;
};

