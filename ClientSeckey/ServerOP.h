#pragma once
#include "TcpSocket.h"
#include "Message.pb.h"
#include <unordered_map>
#include <memory>


class ServerOP
{
private:
	unsigned short m_port;
	std::string m_serverID;
	std::unordered_map<pthread_t, std::unique_ptr<TcpSocket>> m_umap;
public:
	ServerOP(std::string jsonFile = "config.json");
	~ServerOP();
	bool startServer();
	static void* workHard(void* arg);
	std::string seckeyAgree(std::unique_ptr<RequestMsg> reqMsg);
	void seckeyCheck();
	void seckeyCancel();
private:
	std::string getRandStr(int num);
};

