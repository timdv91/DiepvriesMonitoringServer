#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include <time.h>
#include <sstream>
#include <vector>


using namespace std;

class udpDiscover
{
public:
	udpDiscover();
	~udpDiscover();

	vector<vector<string>> getDiscoveryBroadcastData();
};

