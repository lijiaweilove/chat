//数据库操作类
#pragma once
#include <string>
#include <mysql_connection.h>
#include "SeckeyNodeInfo.h"

class OCCIOP
{
public:
	OCCIOP();
	~OCCIOP();
	bool connectDB(std::string user, std::string passwd, std::string connStr);
	int getKeyID();
	bool updataKeyID(int keyID);
	bool writeSecKey(SeckeyNodeInfo* pNode);
	void closeDB();
private:
	std::string getCurTime();
private:
	sql::Connection* conn;
};

