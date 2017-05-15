#pragma once
#include <cstdio>
#include <string>
#include <conio.h>
#include <vector>
#include "configRead.h"
#include "tcpClient.h"
#include "sqlConnect.h"
#include "udpDiscover.h"

using namespace std;

int getch_noblock();
string* tcpClientRequest(tcpClient *tcpC);
void sqlConnectStore(string,string,string);
void reloadSqlConfigDB();
void updateIpUsingMac();

/* Read config test: */
configRead cfgr = configRead();
sqlConnect sqlDB = sqlConnect(cfgr.getConfigDataLine(1), cfgr.getConfigDataLine(2), cfgr.getConfigDataLine(3), cfgr.getConfigDataLine(4), cfgr.getConfigDataLine(5));

int main()
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	/* LOOP: */
	int input = -1;

	//at boot of program, update all IP's inside the database using unique mac adresses,
	//this makes static IP's unnacecary.
	updateIpUsingMac();
	
	do 
	{
		reloadSqlConfigDB(); //Loads the CONFIG table from server, this contains IP's and ports from all mCPU's.
		int i = 0;
		do 
		{	//cfgr class manages all config data read from config file @ startup, and also config data loaded from database.
			tcpClient tcpC = tcpClient(cfgr.getMicrocontrollerConfigFromDB(i)[1], stoi(cfgr.getMicrocontrollerConfigFromDB(i)[2]));
			
			//when connection was successfull:
			if (tcpC.getSocketResult() == true) 
			{
				//When conection to microcontroller is made, read it out and store to DB: 
				string* data = tcpClientRequest(&tcpC); //buffer array;

				//but both vars inside stringstream:
				stringstream ss;
				ss << data[0] << " " << data[1];
				
				// when receiving failed:
				if (ss.str().find("ERROR") != std::string::npos)
				{
					//http://stackoverflow.com/questions/9262270/color-console-output-with-c-in-windows
					cout << endl << "\t";
					SetConsoleTextAttribute(hConsole, 12); // change color red;
					cout << "1) " << data[0];
					SetConsoleTextAttribute(hConsole, 7); // change color white;
					cout << endl << "\t";
					SetConsoleTextAttribute(hConsole, 12); // change color red;
					cout << "2) " << data[1] << endl;
					SetConsoleTextAttribute(hConsole, 7); // change color white;
					cout << endl << "\t";
					SetConsoleTextAttribute(hConsole, 12); // change color red;
					cout << "Skipping freezer with name: " << cfgr.getMicrocontrollerConfigFromDB(i)[0] << endl;

					SetConsoleTextAttribute(hConsole, 7); // change color white;
				}
				// when receiving succeeds:
				else 
				{
					sqlConnectStore(cfgr.getMicrocontrollerConfigFromDB(i)[0], data[0], data[1]); //readout;
																						  
					/* Show on cmdline: */
					SetConsoleTextAttribute(hConsole, 15); // change color bright_white;
					cout << endl << "\tMeasurements for: " << cfgr.getMicrocontrollerConfigFromDB(i)[0] << endl;
					cout << "\tMeasured values:" << endl;
					cout << "\tRoom temperature: " << data[0] << '\370' << "C" << endl;
					cout << "\tRelais state: " << data[1] << endl << endl;
					SetConsoleTextAttribute(hConsole, 7); // change color white;
				}
			}
			//when connection couldn't be made:
			else 
			{
				cout << "\t";
				SetConsoleTextAttribute(hConsole, 12); // change color;
				cout << "Skipping freezer with name: " << cfgr.getMicrocontrollerConfigFromDB(i)[0] << endl;
				SetConsoleTextAttribute(hConsole, 7); // change color white;
			}

			i++;
		}while (cfgr.getMicrocontrollerConfigFromDB(i) != nullptr); // when we reached last freezer in DB.

		cout << "\tSleeping for a while..." << endl;
		input = getch_noblock();
		Sleep(2500);
		cout << string(5, '\n');

	} while (input == -1);

	return 0;
}

void sqlConnectStore(string freezer, string temp, string relaisState)
{
	stringstream ss;
	ss << "INSERT INTO " << freezer << " SET temp=" << temp << ",relais=" << relaisState << ";";
	
	if (sqlDB.sendSQLquery(ss.str()) == false)
		return; //if querry doesn't return data skip all underneath;

	//if querry lets server return data, show it on screen:
	vector<vector <string >> sqlRetVal = sqlDB.getSQLOutput();
	for (int I = 0; I < sqlRetVal[0].size(); I++)
	{
		cout << endl;
		for (int K = 0; K < (sqlRetVal.size()/ sqlRetVal[0].size()); K++)
		{
			cout << "Data: " << sqlRetVal[K][I] << "\t";
		}
	}
}

void reloadSqlConfigDB()
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, 10); // change color light_green;
	cout << "====================RELOADING CONFIG TABLE:=====================" << endl;
	SetConsoleTextAttribute(hConsole, 7); // change color white;

	/* LOAD MICROCONTROLLER CONFIG FROM DB: */
	cout << "\tReloading config table. " << endl;
	if (sqlDB.sendSQLquery("SELECT * FROM CONFIG;") == false)
	{
		cout << "\tERROR, could not load config from database!" << endl;
		getchar();
		exit(2);
	}
	cfgr.setMicrocontrollerConfigFromDB(sqlDB.getSQLOutput()); // Store sql output inside buffer.
	cout << "\tConfig table reloaded." << endl << endl;
}

string* tcpClientRequest(tcpClient *_tcpC)
{
	string* data = new string[2];

	data[0] = _tcpC->tcpClientRequestCMD("getTemp;");
	data[1] = _tcpC->tcpClientRequestCMD("getIO;");

	return data;
}

int getch_noblock() 
{
	if (_kbhit()) //detect keyboard hit
		return _getch(); //if true, return the character.
	else
		return -1; //if nothing whas hit return -1;
}

void updateIpUsingMac()
{
	//send UDP discovery broadcast: 
	udpDiscover udpD = udpDiscover();
	vector<vector<string>> discoveryList = udpD.getDiscoveryBroadcastData();

	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, 11); // change color light_bleu;
	cout << "====================UPDATING CONFIG TABLE:======================" << endl;
	SetConsoleTextAttribute(hConsole, 7); // change color white;

	//Update CONFIG table IP's using mac's:
	for (int I = 0; I < discoveryList.size(); I++)
	{
		cout << "\tUPDATING: " << discoveryList[I][0] << ":" << discoveryList[I][1] << endl;
		
		stringstream ss;
		ss << "UPDATE CONFIG SET IP='" << discoveryList[I][1] << "' WHERE MAC='" << discoveryList[I][0] << "';";		
		sqlDB.sendSQLquery(ss.str());
	}

	SetConsoleTextAttribute(hConsole, 3); // change color light_bleu;
	cout << endl << "================CONFIG TABLE UPDATE COMPLETED:==================" << endl;
	SetConsoleTextAttribute(hConsole, 7); // change color white;
}