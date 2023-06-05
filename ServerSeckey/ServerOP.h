#pragma once
#include "TcpSocket.h"
#include "Message.pb.h"
#include "TcpServer.h"
#include <unordered_map>
#include <memory>


class ServerOP
{
private:
	unsigned short m_port;
	std::string m_serverID;
	std::unique_ptr<TcpServer> m_server;
	std::unordered_map<pthread_t, std::unique_ptr<TcpSocket>> m_umap;
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

