#include "OCCIOP.h"
#include <iostream>
#include <mysql_driver.h>
#include <mysql_connection.h>
#include <cppconn/resultset.h>
#include <cppconn/prepared_statement.h>
#include <chrono>

OCCIOP::OCCIOP()
{
}

OCCIOP::~OCCIOP()
{
}

bool OCCIOP::connectDB(std::string user, std::string passwd, std::string connStr)
{
	sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
	conn = driver->connect(connStr, user, passwd);
	if (conn != nullptr && conn->isClosed() == 0) {
		std::cout << "数据库连接成功！" << std::endl;
		return false;
	}
	return true;
}

int OCCIOP::getKeyID()
{
	const sql::SQLString sql = "select ikeysn from KEYSN for update";  //临时对数据库进行加锁
	sql::Statement* stmt = conn->createStatement();
	sql::ResultSet* res = stmt->executeQuery(sql);
	int keyID = -1;
	if (res->next()) {
		keyID = res->getInt(1);
	}
	res->close();
	stmt->close();
	return keyID;
}

bool OCCIOP::updataKeyID(int keyID)
{
	const sql::SQLString sql = "update KEYSN set ikeysn = ?";
	sql::PreparedStatement* pstmt = conn->prepareStatement(sql);
	pstmt->setInt(1, keyID);
	// 设置事务自动提交
	conn->setAutoCommit(true);
	int ret = pstmt->executeUpdate();
	pstmt->close();
	if (ret != 1) {
		std::cout << "插入失败！" << std::endl;
		return false;
	}
	return true;
}

bool OCCIOP::writeSecKey(SeckeyNodeInfo* pNode)
{
	const sql::SQLString sql = "INSERT INTO SECKEYINFO VALUES(?, ?, ?, ?, ?, ?)";
	sql::PreparedStatement* pstmt = conn->prepareStatement(sql);
	pstmt->setString(1, sql::SQLString(pNode->clientID));
	pstmt->setString(2, sql::SQLString(pNode->serverID));
	pstmt->setInt(3, pNode->seckeyID);
	pstmt->setDateTime(4, getCurTime());
	pstmt->setInt(5, pNode->status);
	pstmt->setString(6, sql::SQLString(pNode->seckey));
	// 设置事务自动提交
	conn->setAutoCommit(true);
	int ret = pstmt->executeUpdate();
	pstmt->close();
	if (ret != 1) {
		return false;
	}
	return true;
}

void OCCIOP::closeDB()
{
	conn->close();
	delete conn;
}

std::string OCCIOP::getCurTime()
{
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	std::time_t timestamp = std::chrono::system_clock::to_time_t(now);
	std::tm* timeinfo = std::localtime(&timestamp);
	char buf[64] = { 0 };
	std::strftime(buf,sizeof(buf),"%Y-%m-%d %H:%M:%S",timeinfo);
	return std::string(buf);
}
