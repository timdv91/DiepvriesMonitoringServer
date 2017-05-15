#include "configRead.h"

configRead::configRead()
{
	HANDLE hConsole;
	hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(hConsole, 10); // change color light_green;
	readConfigFile();
	SetConsoleTextAttribute(hConsole, 7); // change color white;
}


configRead::~configRead()
{
}

/* Read the default config file: */
void configRead::readConfigFile()
{
	cout << "====================READING CONFIG FILE:=====================" << endl;

	ifstream file(_configFileName);
	if (file.is_open())
	{
		for (int i = 0; i <= size(configArray); ++i)
		{
			file >> configArray[i];
			cout << "\t" << configArray[i] << endl;
		}
		file.close();
		cout << "\tConfig file loaded." << endl << endl;
	}
	else {
		cout << "\tWARNING, config file does not exist. --> Creating default config file." << endl;
		createDefaultConfigFile();
		readConfigFile(); // do a new read so settings are loaded.
	}
}

/* Creates default config in case no config file exists: */
void configRead::createDefaultConfigFile()
{
	ofstream myfile;
	myfile.open(_configFileName);
	
	stringstream ss;
	ss << "Properties:" << endl;
	ss << "SQL_USER:" << "root" << endl; //1
	ss << "SQL_PASSWORD:" << "root" << endl; //2
	ss << "SQL_DATABASE:" << "monitoringTest" << endl; //3
	ss << "SQL_SERVER_IP:" << "127.0.0.1" << endl; //4
	ss << "SQL_SERVER_PORT:" << "3306" << endl; //5
	
	myfile << ss.str();
	myfile.close();
}

/* Stores db config inside private var in this class: */
void configRead::setMicrocontrollerConfigFromDB(vector<vector<string>> CFG_LIST)
{
	MICRO_CTRL_CFG = CFG_LIST;
}

/* Returns microcontroller database config as string array: */
string * configRead::getMicrocontrollerConfigFromDB(int ID)
{
	//create return array:
	string* mCPU = new string[MICRO_CTRL_CFG_WIDTH];

	//If array out of bounds, return nullptr:
	if (ID >= size(MICRO_CTRL_CFG[0]))
		return nullptr;

	// Add data to array:
	mCPU[0] = MICRO_CTRL_CFG[0][ID]; //contains freezer name
	mCPU[1] = MICRO_CTRL_CFG[1][ID]; //contains port
	mCPU[2] = MICRO_CTRL_CFG[2][ID]; //contains mac.

	return mCPU; //return array;
}

/* Returns 1 line of data from the config file, it is pre bufferd inside a private array: */
string configRead::getConfigDataLine(int ID)
{
	string retVal;
	istringstream iss(configArray[ID]);
	string s;
	while (getline(iss, s, ':'))
	{
		retVal = s.c_str();
	}
	return retVal;
}
