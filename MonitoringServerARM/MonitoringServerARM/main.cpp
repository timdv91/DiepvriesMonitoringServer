#include <cstdio>
#include <string>
#include "tcpClient.h"
#include "sqlConnect.h"
#include "configRead.h"
#include "udpDiscover.h"
#include "sendmail.h"
#include "sys/types.h"
#include "sys/sysinfo.h"

using namespace std;

void sqlConnectStore(string, string, string);
void reloadSqlConfigDB();
string* tcpClientRequest(tcpClient *tcpC);
int getFreezerCount(bool silentMode = false);
int getch_noblock();
void updateIpUsingMac();
void reloadSqlContactsDB();
void getSystemInfo();

sendmail sml = sendmail();
configRead cfgr = configRead();
sqlConnect sqlC = sqlConnect(cfgr.getConfigDataLine(1), cfgr.getConfigDataLine(2), cfgr.getConfigDataLine(3), cfgr.getConfigDataLine(4), cfgr.getConfigDataLine(5));

int main()
{	
	//load contacts before trying to send a bootup mail (facepalm):
	reloadSqlContactsDB();

	//send mail at startup of program:
	stringstream err;
	err << "System (re)booted." << endl << "the freezer \'monitoringSystem\' is running in optimal condition.";
	sml.setRecentFreezerData(NULL, NULL, false, err.str());
	sml.checkFreezerHealth("System(Re)boot", -1);
	
	do 
	{
		//get freezer count, size() not available and sizeof() returns fault values...
		int rowCount = 0;
		do {
			rowCount = getFreezerCount();
			if (rowCount <= 0)
				usleep(2500 * 1000);
			else
			{
				getSystemInfo();

				//update CONFIG table IP's
				updateIpUsingMac();

				//reload config file:
				reloadSqlConfigDB();
				
				//check alert:
				reloadSqlContactsDB();
			}

		} while (rowCount <= 0); //stay in loop when freezercount = 0;

		int i = 0;
		do 
		{
			tcpClient tcpC = tcpClient(cfgr.getMicrocontrollerConfigFromDB(i)[1], stoi(cfgr.getMicrocontrollerConfigFromDB(i)[2]));

			if (tcpC.getSocketResult() == true)
			{
				//When conection to microcontroller is made, read it out and store to DB: 
				string* data = tcpClientRequest(&tcpC); //buffer array;

				stringstream ss;
				ss << data[0] << " " << data[1];
				
				// when receiving failed:
				if (ss.str().find("ERROR") != std::string::npos)
				{
					cout << "\033[1;31m ";
					cout << "\t1) " << data[0] << endl;
					cout << "\t2) " << data[1] << endl;
					cout << "\tSkipping freezer with name: " << cfgr.getMicrocontrollerConfigFromDB(i)[0] << endl;
					cout << "\033[1;0m ";

					//set this temps to sendmail class, set bool to false --> somthing went wrong!
					stringstream err;
					err << "Receiving failed: timeout --> " << cfgr.getMicrocontrollerConfigFromDB(i)[0];
					sml.setRecentFreezerData(NULL, NULL, false, err.str());

					//check health of freezer: (give freezername in case we need to send mail):
					sml.checkFreezerHealth(cfgr.getMicrocontrollerConfigFromDB(i)[0], i);
				}
				// when receiving succeeds:
				else 
				{
					sqlConnectStore(cfgr.getMicrocontrollerConfigFromDB(i)[0], data[0], data[1]); //readout;

					//http://stackoverflow.com/questions/5947742/how-to-change-the-output-color-of-echo-in-linux
					cout << "\033[1;37m ";
					cout << endl << "\tMeasurements for: " << cfgr.getMicrocontrollerConfigFromDB(i)[0] << endl;
					cout << "\tMeasured values:" << endl;
					cout << "\tRoom temperature: " << data[0] << "*C" << endl;
					cout << "\tRelais state: " << data[1] << endl << endl;
					cout << "\033[0m";

					//set temp + relais status from this freezer:
					sml.setRecentFreezerData(stoi(data[0]), stoi(data[1]));

					//check health of freezer: (give freezername in case we need to send mail):
					sml.checkFreezerHealth(cfgr.getMicrocontrollerConfigFromDB(i)[0], i);
				}
			}
			else 
			{
				cout << "\033[1;31m ";
				cout << "\tSkipping freezer with name: " << cfgr.getMicrocontrollerConfigFromDB(i)[0] << endl;
				cout << "\033[1;0m ";

				//set this temps to sendmail class, set bool to false --> somthing went wrong!
				stringstream err;
				err << "Could not connect to microcontroller --> " << cfgr.getMicrocontrollerConfigFromDB(i)[0];
				sml.setRecentFreezerData(NULL, NULL, false, err.str());

				//check health of freezer: (give freezername in case we need to send mail):
				sml.checkFreezerHealth(cfgr.getMicrocontrollerConfigFromDB(i)[0], i);
			}
			i++;

		} while (i<rowCount); 
		//}while(cfgr.getMicrocontrollerConfigFromDB(i) != nullptr); --> not working on linux.

		cout << "\tSleeping for a while..." << endl;
		usleep(2500 * 1000);
		cout << string(50, '\n');

	} while (true);

    return 0;
}

void sqlConnectStore(string freezer, string strTemp, string strRelaisState)
{
	stringstream ss;
	ss << "INSERT INTO " << freezer << " SET temp=" << strTemp << ", " << "relais=" << strRelaisState << ";";
	//ss << "SELECT * FROM Freezer_1 WHERE temp>10 AND temp<15";
	if (sqlC.sendSQLquery(ss.str()) == true)
	{
		vector<vector<string>> test = sqlC.getSQLOutput();
		for (int I = 0; I < test[0].size(); I++)
		{
			cout << endl;
			for (int K = 0; K < (test.size() / test[0].size()); K++)
			{
				cout << "Data: " << test[K][I] << "\t";
			}
		}
	}
}

void reloadSqlConfigDB()
{
	cout << "\033[1;32m ";
	cout << "====================RELOADING CONFIG TABLE:=====================" << endl;
	cout << "\033[1;0m ";

	/* LOAD MICROCONTROLLER CONFIG FROM DB: */
	cout << "\tReloading CONFIG table. " << endl;

	if (getFreezerCount(true) <= 0) //prevent crashing of server software when 0 freezers are set.
		return;

	if (sqlC.sendSQLquery("SELECT * FROM CONFIG;") == false)
	{
		cout << "\tERROR, could not load config from database!" << endl;
		return;
	}

	//get return value from sqlDB:
	vector<vector<string>> config = sqlC.getSQLOutput();
	cfgr.setMicrocontrollerConfigFromDB(config); // Store sql output inside buffer.
	
	//set lower and uper limits inside int vector matrix:
	vector<vector<int>> lowUpLimits;
	for (int i = 0; i < config[0].size(); i++)
	{
		//store mac and Ip inside 2D vector: 
		lowUpLimits.push_back(vector<int>());
		lowUpLimits[i].push_back(stoi(config[4][i]));
		lowUpLimits[i].push_back(stoi(config[5][i]));
	}
	sml.setUpperLowerLimits(lowUpLimits);
	cout << "\tConfig table reloaded." << endl << endl;
}

string* tcpClientRequest(tcpClient *_tcpC)
{
	string* data = new string[2];

	data[0] = _tcpC->tcpClientRequestCMD("getTemp;  ");
	data[1] = _tcpC->tcpClientRequestCMD("getIO;  ");

	return data;
}

int getFreezerCount(bool silentMode)
{
	if(!silentMode)//if true, do not print text:
		cout << "\tGetting freezer count: " << endl;

	/* request row count: */
	if (sqlC.sendSQLquery("SELECT COUNT(*) FROM CONFIG;") == false)
	{
		cout << "\tERROR, could not load config from database!" << endl;
		return 0;
	}
	int freezerCount = stoi(sqlC.getSQLOutput()[0][0]);

	if (!silentMode) //if true, do not print text:
	{
		if (freezerCount > 1)
			cout << "\tThere are " << freezerCount << " freezers set." << endl;
		else if (freezerCount != 0)
			cout << "\tThere is " << freezerCount << " freezer set." << endl;
		else
			cout << "\tThere are no freezers in CONFIG! Trying again in some seconds..." << endl;
	}
	return freezerCount;
}

int getch_noblock()
{

}

void updateIpUsingMac()
{
	udpDiscover udpD = udpDiscover();
	vector<vector<string>> discoveryList = udpD.getDiscoveryBroadcastData();

	cout << "\033[1;36m ";
	cout << "====================UPDATING CONFIG TABLE:======================" << endl;
	cout << "\033[1;0m ";

	//Update CONFIG table IP's using mac's:
	for (int I = 0; I < discoveryList.size(); I++)
	{
		cout << "\tUPDATING: " << discoveryList[I][0] << ":" << discoveryList[I][1] << endl;

		stringstream ss;
		ss << "UPDATE CONFIG SET IP='" << discoveryList[I][1] << "' WHERE MAC='" << discoveryList[I][0] << "';";
		sqlC.sendSQLquery(ss.str());
	}

	cout << "\033[0;36m ";
	cout << endl << "  ================CONFIG TABLE UPDATE COMPLETED:==================" << endl;
	cout << "\033[1;0m ";
}

void reloadSqlContactsDB()
{
	cout << "\033[1;32m ";
	cout << "====================RELOADING CONTACTS TABLE:=====================" << endl;
	cout << "\033[1;0m ";

	/* LOAD CONTACTS FROM DB: */
	cout << "\tReloading CONTACTS table. " << endl;
	if (sqlC.sendSQLquery("SELECT * FROM CONTACTS;") == false)
	{
		cout << "\tERROR, could not load CONTACTS from database!" << endl;
		getchar();
		exit(2);
	}
	//get mysql return data into vector matrix: 
	sml.setContacts(sqlC.getSQLOutput());

	cout << "\tCONTACTS table reloaded." << endl << endl;
}

void getSystemInfo()
{
	struct sysinfo memInfo;
	sysinfo(&memInfo);

	int rebootPerc = 75;
	float RamUsedPerc = (float)((float)(((float)memInfo.totalram - (float)memInfo.freeram) / (float)memInfo.totalram) * 100);

	cout << "\tSystem monitoring: " << endl;
	cout << "\tSystem ram usage: " << RamUsedPerc << "% \n\tSystem reboot set at " << rebootPerc << "%" << endl;

	if (RamUsedPerc >= rebootPerc)
	{
		stringstream err;
		err << "System high memory usage. Going down for reboot in 10 seconds.";
		sml.setRecentFreezerData(NULL, NULL, false, err.str());
		sml.checkFreezerHealth("System(Re)boot", -1);

		system("sleep 10 && reboot");
	}
}