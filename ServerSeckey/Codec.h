#pragma once
#include <string>

//  使用多态实现服务器和客户端编解码
class Codec
{
public:
	Codec();
	virtual ~Codec();
	virtual std::string encodeMsg();
	virtual void* decodeMsg();
};

