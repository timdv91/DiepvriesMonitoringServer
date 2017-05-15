#include "sendmail.h"



sendmail::sendmail()
{
}


sendmail::~sendmail()
{
}

void sendmail::setContacts(vector<vector<string>> Contacts)
{
	_Contacts = Contacts;
}

void sendmail::setUpperLowerLimits(vector<vector<int>> dbLowUpTemps)
{
	_lowUpTemps = dbLowUpTemps;
}

void sendmail::setRecentFreezerData(int temp, int relais, bool state, string error)
{
	_rTemp = temp;
	_rRelais = relais;
	_rState = state;
	_rError = error;
}

void sendmail::checkFreezerHealth(string freezerName, int ID)
{
	cout << "\033[1;31m "; //set console to red color

	//check if freezer ID is stored inside mail locklist:
	if (ID >= 0) //id smaller than 0 --> mail with variating text. --> Always allowed to send this.
	{
		int INDEX = getMailLockListID(ID);
		if (INDEX >= 0)
		{
			_mailSendTime = _vMailSendTime[INDEX];
			//prevent sending mail each loop:
			if (time(0) < (_mailSendTime + _sendMailEachSeconds))
			{
				cout << "\tSend mail locked for the next: " << (_mailSendTime + _sendMailEachSeconds) - time(0) << " seconds" << endl;
				cout << "\033[1;0m "; //return to default color
				return;
			}
		}
	}

	//determine connection problems:
	//if rstate = false, then there is a connection problem!
	if (ID < 0)
	{
		sendMail("FreezerMonitoring System information", _rError, ID); //send standard mail wit variating text.
	}
	else if (_rState != true)
	{
		stringstream ss;
		ss << "Monitoring system detected possible hardware failure on the monitoring device connected to: " << freezerName << endl;
		ss << "ERROR: " << _rError << endl << endl;
		ss << "As this error is not freezer related this is not urgent.";
		sendMail("Freezer: Monitoring system hardware failure!", ss.str(), ID);
	}
	else if (_rRelais == 0) // if the hardware relais is triggerd:
	{
		stringstream ss;
		ss << "Monitoring system detected a freezer in ALERT mode: " << freezerName << endl;
		ss << "This means a freezer has possible hardware malfunctions and stopped working. " << endl;
		ss << "As the problem was detected early on, the temperature will probably be stabel for the next couple of hours. " << endl;
		ss << "At this moment, the temperature inside this freezer is: " << _rTemp << "°C";
		sendMail("Freezer: ALERT mode detected!", ss.str(), ID);
	}
	else //if temperature is out of range:
	{
		if (_rTemp < _lowUpTemps[ID][0]) //when to cold:
		{
			stringstream ss;
			ss << "Monitoring system detected a freezer to cold: " << freezerName << endl;
			ss << "This freezer is to cold!" << endl;
			ss << "At this moment, the temperature inside this freezer is: " << _rTemp << "°C" << endl;
			ss << "The minimum allowed preset temperature is: " << _lowUpTemps[ID][0] << "°C";
			sendMail("Freezer: to cold!", ss.str(), ID);
		}
		else if (_rTemp > _lowUpTemps[ID][1]) //when to hot:
		{
			stringstream ss;
			ss << "Monitoring system detected a freezer to hot: " << freezerName << endl;
			ss << "This freezer is to hot!" << endl;
			ss << "At this moment, the temperature inside this freezer is: " << _rTemp << "°C" << endl;
			ss << "The maximum allowed preset temperature is: " << _lowUpTemps[ID][1] << "°C";
			sendMail("Freezer: to hot!", ss.str(), ID);
		}
	}

	cout << "\033[1;0m "; //return to default color
}

void sendmail::sendMail(string subject, string msg, int ID)
{
	for (int i = 0; i < _Contacts[0].size(); i++)
	{
		//cout << "\t" << _Contacts[0][i] << ":" << _Contacts[1][i] << ":" << _Contacts[2][i] << endl;

		string mail("sh /root/projects/MonitoringServerARM/bin/ARM/Debug/sendMail.sh ");
		mail += "\"" + subject + "\" ";
		mail += "\"" + msg + "\" ";
		mail += _Contacts[1][i];
		system(mail.c_str());
		cout << mail;
	}

	if (ID < 0) //skip code underneath if ID is -1 --> this is a mail with variating text and can be send at any time.
		return; 

	_mailSendTime = time(0); //add mail send time to var

	int INDEX = getMailLockListID(ID);
	if (INDEX >= 0)
	{
		_vMailSendTime[INDEX] = _mailSendTime; //add mail send time to existing index of vector for this freezer
	}
	else //if this is the first time this freezer go's tutut
	{ 
		_vID.push_back(ID);
		_vMailSendTime.push_back(_mailSendTime);
	}
}

int sendmail::getMailLockListID(int ID)
{
	//find ID in vector list:
	for (int i = 0; i < _vID.size(); i++)
	{
		if (_vID[i] == ID) //if freezer ID is found:
		{
			return i; //freezer found inside locklist on ID --> i
		}
	}

	return -1; //freezer not found inside locklist
}
