#include "RespondCodec.h"
using std::string;

RespondCodec::RespondCodec() {};

RespondCodec::RespondCodec(string encstr) {
	initMessage(encstr);
};

RespondCodec::RespondCodec(RespondInfo* info) {
	initMessage(info);
};

void RespondCodec::initMessage(string encstr) {
	m_encStr = encstr;
};

void RespondCodec::initMessage(RespondInfo* info) {
	m_msg.set_status(info->status);
	m_msg.set_seckeyid(info->sekeyId);
	m_msg.set_clientid(info->clientId);
	m_msg.set_serverid(info->serverId);
	m_msg.set_data(info->data);
};

string RespondCodec::encodeMsg() {
	string output;
	m_msg.SerializeToString(&output);
	return output;
};

void* RespondCodec::decodeMsg() {
	m_msg.ParseFromString(m_encStr);
	return &m_msg;
};

RespondCodec::~RespondCodec() {};