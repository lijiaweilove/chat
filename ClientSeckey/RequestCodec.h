#pragma once
#include "Codec.h"
#include "Message.pb.h"
#include <string>

//  客户端需发送的数据格式
struct RequestInfo
{
	int cmd;
	std::string clientId;
	std::string serverId;
	std::string sign;
	std::string data;
};

//  客户端类,编码发送的数据,解码接收的数据
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

