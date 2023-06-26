#pragma once
#include <string>
#include "SeckeyShm.h"

struct clientInfo
{
	std::string serverID;
	std::string clientID;
	std::string ip;
	unsigned short port;
};

// 客户端类
enum handle{Agree,Check,Cancel};
class clientOP
{
private:
	clientInfo m_info;
	SeckeyShm *m_shm;
public:
	//  获取本地json文件配置
	clientOP(std::string jsonFile = "config.json");
	~clientOP();
	//秘钥协商,校验,注销
	bool seckeyAgree();
	bool seckeyCheck();
	bool seckeyCancel();
};