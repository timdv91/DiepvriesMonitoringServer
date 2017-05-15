#include "udpDiscover.h"

udpDiscover::udpDiscover()
{
}


udpDiscover::~udpDiscover()
{
}

vector<vector<string>> udpDiscover::getDiscoveryBroadcastData()
{
	cout << "\033[1;36m ";
	cout << " =================STARTING DISCOVERY BROADCAST:==================" << endl;
	cout << "\033[1;0m ";

	#define BUFLEN 36  //Max length of buffer

	int sock;
	struct sockaddr_in broadcastAddr;
	char *broadcastIP;
	unsigned short broadcastPort;
	char *sendString;
	int broadcastPermission;
	int sendStringLen;
	socklen_t clientLen = BUFLEN;

	broadcastIP = "255.255.255.255";
	broadcastPort = 30303;

	sendString = "D";             /*  string to broadcast */


	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
		fprintf(stderr, "socket error");
		exit(1);
	}

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0) {
		perror("Error");
	}

	broadcastPermission = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (void *)&broadcastPermission, sizeof(broadcastPermission)) < 0) {
		fprintf(stderr, "setsockopt error");
		exit(1);
	}

	/* Construct local address structure */
	memset(&broadcastAddr, 0, sizeof(broadcastAddr));
	broadcastAddr.sin_family = AF_INET;
	broadcastAddr.sin_addr.s_addr = inet_addr(broadcastIP);
	broadcastAddr.sin_port = htons(broadcastPort);

	sendStringLen = strlen(sendString);

	cout << "\n\tDiscovery broadcast send.\n" << endl;
	/* Broadcast sendString in datagram to clients */
	if (sendto(sock, sendString, sendStringLen, 0, (struct sockaddr *)&broadcastAddr, sizeof(broadcastAddr)) != sendStringLen) {
		fprintf(stderr, "sendto error");
		exit(1);
	}

	int server_length;
	server_length = sizeof(struct sockaddr_in);

	vector<vector<string>> retArray;
	int I = 0;
	string Ip;
	while (true)
	{
		//receive answer from client:
		char recMessage[BUFLEN];
		if (recvfrom(sock, recMessage, BUFLEN, 0, (struct sockaddr *) &broadcastAddr, &clientLen) == -1)
		{
			//fprintf(stderr, "recvfrom error");
			break;
		}

		Ip = inet_ntoa(broadcastAddr.sin_addr); //store ip adress from sender;
		
		//Message received contains useless data, remove this data and keep the mac adress:
		string MACadr[3];
		istringstream iss(recMessage);
		string s;
		int K = 0;
		while (getline(iss, s, '\n'))
		{
			MACadr[K] = s.c_str();
			K++;
		}

		//remove \r from back of string:
		MACadr[1] = MACadr[1].substr(0, MACadr[1].size() - 1);
		
		
		//store mac and Ip inside 2D vector: 
		retArray.push_back(vector<string>());
		retArray[I].push_back(MACadr[1]);
		retArray[I].push_back(Ip);

		//cout << endl << MACadr[1] << ":" << Ip << endl;
		cout << "\tDevice MAC: " << retArray[I][0] << "\n\tOn IP: " << retArray[I][1] << endl;

		I++;
	}

	close(sock);

	cout << endl;
	cout << "\033[0;36m ";
	cout << " ==================ENDING DISCOVERY BROADCAST:===================" << endl;
	cout << "\033[1;0m ";
	return retArray;
}