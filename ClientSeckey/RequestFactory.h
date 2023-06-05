#pragma once
#include "CodecFactory.h"
#include "RequestCodec.h"
#include <string>

// 创建客户端类对象
class RequestFactory : public CodecFactory
{
private:
	bool m_flag;
	RequestInfo* m_info;
	std::string m_encStr;
public:
	RequestFactory(std::string m_encStr);
	RequestFactory(RequestInfo* info);
	~RequestFactory();
	Codec* createCodec();
};

