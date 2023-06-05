#pragma once
#include "CodecFactory.h"
#include "RespondCodec.h"
#include <string>

// ���������������
class RespondFactory :public CodecFactory
{
private:
    bool m_flag;
    std::string m_encStr;  
    RespondInfo* m_info;  
public:
    RespondFactory(std::string enc);
    RespondFactory(RespondInfo* info);
    ~RespondFactory();
    Codec* createCodec()override;
};

