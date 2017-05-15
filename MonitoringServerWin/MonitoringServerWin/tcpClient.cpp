#include "tcpClient.h"

tcpClient::tcpClient(string IP, int PORT)
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	
	SetConsoleTextAttribute(hConsole, 9); // change color light_bleu;
	cout << "===================STARTING TCP CONNECTION======================" << endl;
	SetConsoleTextAttribute(hConsole, 7); // change color white;
	cout << "\tConnecting to device on: " << IP << ":" << PORT;

	
	//Winsock Startup:
	WSADATA wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) //if WSAStartup returns anything other than 0 then error has occured.
	{
		cout << " --> WINSOCK STARTUP FAILED!" << endl;
	}
	
	addr; //Address to be binded to our connection socket.
	int sizeofaddr = sizeof(addr); //Need sizeofaddr for the connect function.
	addr.sin_addr.s_addr = inet_addr(IP.c_str()); //IP adress.
	addr.sin_port = htons(PORT); //Port number;
	addr.sin_family = AF_INET; //IPv4 Socket;

	Connection = socket(AF_INET, SOCK_STREAM, NULL); //Set connection socket;
	setsockopt(Connection, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(int)); // setting send timeout
	setsockopt(Connection, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(int)); //setting receive timeout

	//connect to the server
	iResult = connect(Connection, (sockaddr*)&addr, sizeofaddr);
	if (iResult == SOCKET_ERROR)
	{
		u_long iError = WSAGetLastError();
		//check if error was WSAEWOULDBLOCK, where we'll wait
		if (iError == WSAEWOULDBLOCK)
		{
			cout << "Attempting to connect.\n";
			fd_set Write, Err;
			TIMEVAL Timeout;
			int TimeoutSec = (int)timeout/1000; // timeout after x seconds

			FD_ZERO(&Write);
			FD_ZERO(&Err);
			FD_SET(Connection, &Write);
			FD_SET(Connection, &Err);

			Timeout.tv_sec = TimeoutSec;
			Timeout.tv_usec = 0;

			iResult = select(0, NULL, &Write, &Err, &Timeout);

			if (iResult == 0)
			{
				cout << "Connect Timeout (" << TimeoutSec << " Sec).\n";
			}
			else
			{
				if (FD_ISSET(Connection, &Write))
				{
					cout << "Connected!\n";
				}
				if (FD_ISSET(Connection, &Err))
				{
					cout << "Select error.\n";
				}
			}
		}
		else
		{
			//http://stackoverflow.com/questions/9262270/color-console-output-with-c-in-windows
			HANDLE hConsole;
			hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

			cout << "\t";
			//set color, print, reset color:
			SetConsoleTextAttribute(hConsole, 12);
			cout << " --> Failed. (" << WSAGetLastError() << ")" << endl;
			SetConsoleTextAttribute(hConsole, 7);

			WSACleanup();
		}
	}
	else
	{
		//connected without waiting (will never execute) --> In my case, executed always if there is no error???
		cout << " --> Success!" << endl;
	}
}


tcpClient::~tcpClient()
{
	cout << "\tClosing connection.";
	
	closesocket(Connection);

	cout << "\t --> Success!" << endl;

	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, 1); // change color beu;
	cout << "=====================TCP CONNECTION ENDED=======================" << endl;
	SetConsoleTextAttribute(hConsole, 7); // change color white;
}

bool tcpClient::getSocketResult()
{
	if (iResult == SOCKET_ERROR || iResult == WSAEWOULDBLOCK)
		return false;
	else
		return true;
}

string tcpClient::tcpClientRequestCMD(string strMsg)
{
	setsockopt(Connection, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(int)); //setting send timeout
	setsockopt(Connection, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(int)); //setting the receive timeout

	cout << endl << "\tSending data.";

	char CMD[16]; //Create buffer with text as char;
	strcpy(CMD, strMsg.c_str()); //copy message inside CMD char array;
	send(Connection, CMD, sizeof(CMD), NULL); //Send the message buffer;
	
	cout << "\t\t --> Success! \n\tReceiving data.";

	memset(&CMD[0], 0, sizeof(CMD)); //clear the array.
	recv(Connection, CMD, sizeof(CMD), NULL); //receive message using same buffer;
	if(strlen(CMD) == 0)
		strcpy(CMD, "ERROR: RTIMEOUT"); //copy message inside CMD char array;
	else
		cout << "\t\t --> Success!" << endl;

	return CMD;
}

void tcpClient::setSocketTimeout(int _timeout)
{
	timeout = _timeout;
}


