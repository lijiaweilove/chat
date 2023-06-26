#pragma once
#include "TcpSocket.h"
#include "Message.pb.h"
#include "TcpServer.h"
#include "OCCIOP.h"
#include "SeckeyShm.h"
#include <unordered_map>
#include <memory>


class ServerOP
{
private:
	unsigned short m_port;
	std::string m_serverID;
	std::unique_ptr<TcpServer> m_server;
	std::unordered_map<pthread_t, std::unique_ptr<TcpSocket>> m_umap;
	std::string m_user;
	std::string m_passwd;
	std::string m_connStr;
	//创建数据库的实例化对象,因为好几个操作都需要对数据库进行操作
	OCCIOP m_occi;
	SeckeyShm *m_shm;
public:
	enum keyLen { Len16 = 16, Len24 = 24, Len32 = 32 };
	ServerOP(std::string jsonFile = "config.json");
	~ServerOP();
	bool startServer();
	static void* workHard(void* arg);
private:
	std::string seckeyAgree(RequestMsg* Msg);
	std::string getRandStr(keyLen len);
	void seckeyCheck();
	void seckeyCancel();
};

