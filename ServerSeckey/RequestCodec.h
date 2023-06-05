#pragma once
#include "Codec.h"
#include "Message.pb.h"
#include <string>

//  �ͻ����跢�͵����ݸ�ʽ
struct RequestInfo
{
	int cmd;
	std::string clientId;
	std::string serverId;
	std::string sign;
	std::string data;
};

//  �ͻ�����,���뷢�͵�����,������յ�����
class RequestCodec : public Codec
{
private:
	RequestMsg m_msg;
	std::string m_encStr;
public:
	RequestCodec();
	RequestCodec(RequestInfo* m_msg);
	RequestCodec(std::string m_encStr);
	void initMessage(std::string encstr);
	void initMessage(RequestInfo* info);
	~RequestCodec();
	std::string encodeMsg();
	void* decodeMsg();
};

