#include "tcpClient.h"

tcpClient::tcpClient(string IP, int PORT)
{
	cout << "\033[1;36m";
	cout << "  ====================STARTING TCP CONNECTION=====================" << endl;
	cout << "\033[1;0m";
	cout << "\tConnecting to device on: " << IP << ":" << PORT;

	//Create socket
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		printf("Could not create socket");
		getchar();
	}

	server.sin_addr.s_addr = inet_addr(IP.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(PORT);

	//Connect to remote server
	if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		iResult = "SOCKET_ERROR";
		cout << "\033[1;31m";
		cout << "\t --> Failed: (10060)" << endl;
		cout << "\033[1;0m ";
		return;
	}

	cout << "\t --> Success!" << endl;
}


tcpClient::~tcpClient()
{
	cout << "\tClosing TCP connection. ";
	close(sock);

	cout << "\t --> Success!" << endl;
	cout << "\033[0;36m ";
	cout << "======================TCP CONNECTION ENDED=======================" << endl;
	cout << "\033[1;0m ";
}

bool tcpClient::getSocketResult()
{
	if (iResult == "SOCKET_ERROR")
		return false;
	else
		return true;
}

string tcpClient::tcpClientRequestCMD(string strMsg)
{
	char message[16]; //Create buffer with text as char;
	strcpy(message, strMsg.c_str()); //copy message inside CMD char array;

	//set receive timeout:
	struct timeval tv;
	tv.tv_sec = _timeOut;  /* 30 Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(struct timeval));

	cout << "\tsending data.";

	strMsg += "  "; //Server removes 2 last chars. It sees it as carage return and newline. In linux not needed.
	send(sock, message, strlen(message), 0); //Send some data

	cout << "\t\t --> Success! \n\tReceiving data.";

	memset(&message[0], 0, sizeof(message)); //clear the array.
	recv(sock, message, sizeof(message), 0); //Receive a reply from the server
	if(strlen(message) == 0)
		strcpy(message, "ERROR: RTIMEOUT"); //copy message inside CMD char array;

	cout << "\t\t --> Success!" << endl;

	return message;
}

void tcpClient::setSocketTimeout(int sockTimeout)
{
	_timeOut = sockTimeout;
}


