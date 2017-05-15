//#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

#pragma once
class configRead
{
public:
	configRead();
	~configRead();

	void setMicrocontrollerConfigFromDB(vector<vector<string>>);
	string* getMicrocontrollerConfigFromDB(int);
	string getConfigDataLine(int);

private:
	void readConfigFile();
	void createDefaultConfigFile();

private:
	/* MYSQL SERVER CONFIG FILE: */
	string configArray[5]; //increase size if properties are added!
	string _configFileName = "properties.cfg";

	/* MICROCONTROLLERS CONFIG FROM MYSQL DB: */
	vector<vector<string>> MICRO_CTRL_CFG;
	const int MICRO_CTRL_CFG_WIDTH = 3; //update width if vars are added to database!
};

