#include "configRead.h"

configRead::configRead()
{
	readConfigFile();
}


configRead::~configRead()
{
}

void configRead::readConfigFile()
{
	cout << "\033[1;32m ";
	cout << "====================READING CONFIG FILE:=====================" << endl;

	ifstream file(_configFileName);
	if (file.is_open())
	{
		//sizeof(array)/sizeof(int)
		for (int i = 0; i <= (sizeof(configArray)/sizeof(string)); ++i)
		{
			file >> configArray[i];
			cout << "\t" << configArray[i] << endl;
		}
		file.close();
		cout << "\tConfig file loaded." << endl;
	}
	else {
		cout << "\tWARNING, config file does not exist. --> Creating default config file." << endl;
		createDefaultConfigFile();
		readConfigFile(); // do a new read so settings are loaded.
	}

	cout << "\033[1;0m ";
}

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
	//Can't get correct size of the vector. Seems to be caused by mysql connector lib. Try/catch as fast fix:

	// Add data to array:
	mCPU[0] = MICRO_CTRL_CFG[0][ID];
	mCPU[1] = MICRO_CTRL_CFG[1][ID];
	mCPU[2] = MICRO_CTRL_CFG[2][ID];

	return mCPU; //return array;
}

string configRead::getConfigDataLine(int ID)
{
	string retVal;
	istringstream iss(configArray[ID]);
	string s;
	while (getline(iss, s, ':'))
	{
		//printf("`%s'\n", s.c_str());
		retVal = s.c_str();
	}
	return retVal;
}
