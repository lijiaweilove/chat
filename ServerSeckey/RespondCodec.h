#pragma once
#include "Codec.h"
#include "Message.pb.h"
#include <string>

//  服务器需发送的数据格式
struct RespondInfo
{
	bool status;
	int sekeyId;
	std::string clientId;
	std::string serverId;
	std::string data;
};

//  服务器类,将获取的字符串解码,编码响应的数据
class RespondCodec :public Codec
{
private:
	std::string m_encStr;
	RespondMsg m_msg;
public:
	RespondCodec();
	RespondCodec(std::string encstr);
	RespondCodec(RespondInfo* info);
	void initMessage(std::string encstr);
	void initMessage(RespondInfo* info);
	std::string encodeMsg() override;
	void* decodeMsg() override;
	~RespondCodec();
};

