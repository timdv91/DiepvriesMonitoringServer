#pragma once
#include <winsock.h>
#include <stdio.h>
#include <mysql.h>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
// kernel32.lib;user32.lib;gdi32.lib;winspool.lib;comdlg32.lib;advapi32.lib;shell32.lib;ole32.lib;oleaut32.lib;uuid.lib;odbc32.lib;odbccp32.lib;%(AdditionalDependencies)

class sqlConnect
{
public:
	sqlConnect(string,string,string,string,string);
	~sqlConnect();

	bool sendSQLquery(string);
	vector<vector<string>> getSQLOutput();

private:
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
};

