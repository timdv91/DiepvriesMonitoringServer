#include "udpDiscover.h"

udpDiscover::udpDiscover()
{
}


udpDiscover::~udpDiscover()
{
}

vector<vector<string>> udpDiscover::getDiscoveryBroadcastData()
{

	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, 11); // change color light_bleu;
	cout << "==================STARTING DISCOVERY BROADCAST==================" << endl;
	SetConsoleTextAttribute(hConsole, 7); // change color white;

	//init
	int server_length;
	int port = 30303;
	const int STRLEN = 256;
	char recMessage[STRLEN];
	char sendMessage[STRLEN];
	char *sendMes = "D";
	WSADATA wsaData;
	SOCKET mySocket;
	SOCKET myBackup;
	SOCKET acceptSocket;
	sockaddr_in myAddress;
	char broadcast = 1;
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;

	//create socket
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != NO_ERROR)
	{
		cerr << "Socket Initialization: Error with WSAStartup\n";
		system("pause");
		WSACleanup();
		exit(10);
	}


	mySocket = socket(AF_INET, SOCK_DGRAM, 0);
	setsockopt(mySocket, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
	setsockopt(mySocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&tv, sizeof(tv));

	if (mySocket == INVALID_SOCKET)
	{
		cerr << "Socket Initialization: Error creating socket" << endl;
		system("pause");
		WSACleanup();
		exit(11);
	}

	myBackup = mySocket;

	//bind
	myAddress.sin_family = AF_INET;
	myAddress.sin_addr.S_un.S_addr = INADDR_BROADCAST;
	myAddress.sin_port = htons(port);

	cout << endl;
	server_length = sizeof(struct sockaddr_in);

	sendto(mySocket, sendMes, strlen(sendMes), 0, (SOCKADDR*)&myAddress, server_length);
	cout << "\tDiscovery broadcast send." << endl << endl;

	string Ip, IpOld;
	vector<vector<string>> retArray;

	int I = 0; //counter for 2D vector list.
	while (true) 
	{
		//clear recMessage from empty chars:
		memset(&recMessage[0], 0, sizeof(recMessage));
		//receive discovery answer from devices:
		recvfrom(mySocket, recMessage, STRLEN, 0, (SOCKADDR*)&myAddress, &server_length);
		//get ip from this device:
		Ip = inet_ntoa(myAddress.sin_addr);

		if (Ip == IpOld)
		{
			break;
		}	
		else
		{
			//Message received contains useless data, remove this data and keep the mac adress:
			string MACadr;
			istringstream iss(recMessage);
			string s;
			while (getline(iss, s, '\n'))
			{
				MACadr = s.c_str();
			}

			//remove \r from back of string:
			MACadr = MACadr.substr(0, MACadr.size() - 1);

			//store mac and Ip inside 2D vector:
			retArray.push_back(vector<string>());
			retArray[I].push_back(MACadr);
			retArray[I].push_back(Ip);

			//Print on screen: 
			cout << "\tDevice MAC: " << retArray[I][0] << "\n\tOn IP: " << retArray[I][1] << endl;
			
			//set IpOld buffer to determine if this device has already been found:
			IpOld = Ip;
			I++;
		}
	}

	SetConsoleTextAttribute(hConsole, 3); // change color light_bleu;
	cout << endl << "==================ENDING DISCOVERY BROADCAST====================" << endl;
	SetConsoleTextAttribute(hConsole, 7); // change color white;

	return retArray; //return array to main.
}