#pragma once
#include "Codec.h"

//  使用多态实现服务器和客户端类对象的创建
class CodecFactory
{
public:
	CodecFactory();
	virtual ~CodecFactory();
	virtual Codec* createCodec();
};

