#pragma once
#include "Codec.h"

//  ʹ�ö�̬ʵ�ַ������Ϳͻ��������Ĵ���
class CodecFactory
{
public:
	CodecFactory();
	virtual ~CodecFactory();
	virtual Codec* createCodec();
};

