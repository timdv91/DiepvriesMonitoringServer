#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <ctime>

using namespace::std;

class sendmail
{
public:
	sendmail();
	~sendmail();

	void setContacts(vector<vector<string>>);
	void setUpperLowerLimits(vector<vector<int>>);
	void setRecentFreezerData(int, int, bool state=true, string error="");

	void checkFreezerHealth(string, int);

private:

	void sendMail(string,string,int);
	int getMailLockListID(int);

	vector<vector<string>> _Contacts;
	vector<vector<int>> _lowUpTemps;
	int _rTemp;
	int _rRelais;
	bool _rState;
	string _rError;

	vector<time_t> _vMailSendTime;
	vector<int> _vID;

	time_t _mailSendTime = 0;
	int _sendMailEachSeconds = 3600;
};

