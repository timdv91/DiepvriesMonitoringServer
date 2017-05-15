#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <time.h>
#include <sstream>
#include <vector>
#include<stdio.h> //printf
#include<string.h>    //strlen
#include<sys/socket.h>    //socket
#include<arpa/inet.h> //inet_addr
#include<unistd.h>
#include<stdlib.h> //exit(0);
#include <iostream>

using namespace std;

class udpDiscover
{
public:
	udpDiscover();
	~udpDiscover();

	vector<vector<string>> getDiscoveryBroadcastData();
};

