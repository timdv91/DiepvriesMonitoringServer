#include "sqlConnect.h"

sqlConnect::sqlConnect(string strUser, string strPassword, string strDatabase, string IP, string PORT)
{
	cout << "\033[1;36m ";
	cout << "====================STARTING SQL CONNECTION=====================" << endl;
	cout << "\033[1;0m ";

	const char *server = IP.c_str();
	const char *user = strUser.c_str();
	const char *password = strPassword.c_str();
	const char *database = strDatabase.c_str();
	const int port = stoi(PORT);

	conn = mysql_init(NULL);

	cout << "\tConnecting to SQL database on ip: " << server << ":" << port;

	/* Connect to database */
	if (!mysql_real_connect(conn, server, user, password, database, port, NULL, 0))
	{
		cout << endl << "\t\a" << stderr << "\t" << mysql_error(conn) << endl;
		getchar();
		exit(1);
	}

	cout << "\t --> Success!" << endl;
}


sqlConnect::~sqlConnect()
{
	/* close connection: */
	cout << "Closing sql connection..." << endl;
	mysql_free_result(res);
	mysql_close(conn);

	cout << "\033[0;36m ";
	cout << "====================SQL CONNECTION CLOSED========================" << endl;
	cout << "\033[1;0m ";
}


bool sqlConnect::sendSQLquery(string sqlQuery)
{
	cout << "\tSend query to database.";

	/* send SQL query */
	/*=================*/
	if (mysql_query(conn, sqlQuery.c_str()))
	{
		cout << stderr << "\t" << mysql_error(conn) << endl;
	}

	cout << "\t --> Success!" << endl;

	//mysql_free_result(res); // causes segmentation error in linux???
	res = mysql_use_result(conn);

	if (res == NULL)
		return false; //No data is returned from database;

	return true; //Data is returned from database;
}

vector<vector<string>> sqlConnect::getSQLOutput()
{
	/* if no data is inside res, we souldn't be here: */
	if (res == NULL)
	{
		cout << "\n\nERROR! --> There is no returned data! We should not be inside this function!!!" << endl << endl;
	}

	/* loop all returned sql data and push inside 2D vector: */
	vector<vector <string>> buffer;
	while ((row = mysql_fetch_row(res)) != NULL)
	{
		for (int I = 0; I<mysql_num_fields(res); I++)
			buffer.push_back(vector<string>()); //init second demention of vector;
		for (int I = 0; I<mysql_num_fields(res); I++)
			buffer[I].push_back(row[I]); //Push data inside vector;
	}

	return buffer;
}


