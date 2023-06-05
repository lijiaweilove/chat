#pragma once
#include <string>

struct clientInfo
{
	std::string serverID;
	std::string clientID;
	std::string ip;
	unsigned short port;
};

// �ͻ�����
enum handle{Agree,Check,Cancel};
class clientOP
{
private:
	clientInfo m_info;
public:
	//  ��ȡ����json�ļ�����
	clientOP(std::string jsonFile = "config.json");
	~clientOP();
	//��ԿЭ��,У��,ע��
	bool seckeyAgree();
	bool seckeyCheck();
	bool seckeyCancel();
};