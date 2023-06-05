#include "RespondFactory.h"
#include <string>
using namespace std;

RespondFactory::RespondFactory(string enc) : CodecFactory() {
	m_flag = false;
	m_encStr = enc;
};

RespondFactory::RespondFactory(RespondInfo* info) : CodecFactory() {
	m_flag = true;
	m_info = info;
};

RespondFactory::~RespondFactory() {};

Codec* RespondFactory::createCodec() {
	Codec* codec = nullptr;
	if (m_flag) {
		codec = new RespondCodec(m_info);
	}
	else {
		codec = new RespondCodec(m_encStr);
	}
	return codec;
};