#pragma once
#include <string>

//  ʹ�ö�̬ʵ�ַ������Ϳͻ��˱����
class Codec
{
public:
	Codec();
	virtual ~Codec();
	virtual std::string encodeMsg();
	virtual void* decodeMsg();
};

