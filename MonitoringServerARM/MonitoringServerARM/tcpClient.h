#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")

#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<unistd.h>

#include <iostream>

using namespace std;

#pragma once

class tcpClient
{
public:
	tcpClient(string,int);
	~tcpClient();

	bool getSocketResult();
	string tcpClientRequestCMD(string);
	void setSocketTimeout(int);

private:
	int sock;
	string iResult;
	struct sockaddr_in server;
	int _timeOut = 5;
};

