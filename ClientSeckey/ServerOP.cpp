#include "ServerOP.h"
#include "TcpServer.h"
#include "TcpSocket.h"
#include "Message.pb.h"
#include "CodecFactory.h"
#include "RespondFactory.h"
#include "RsaCrypto.h"
#include <string>
#include <memory>
#include <fstream>
#include <json/json.h>
using namespace Json;
using namespace std;
//监听, 提取(多线程 + io多路), 反序列化数据, 判断和验证, 序列化数据, 
//准备数据:对称秘钥 + 传公钥, 处理状态, id), 发送;


ServerOP::ServerOP(string jsonFile)
{
	ifstream ifs(jsonFile);
	Value root;
	Reader read;
	read.parse(ifs, root);
	m_port = root["port"].asInt();
	m_serverID = root["serverID"].asString();
}

ServerOP::~ServerOP()
{
}

bool ServerOP::startServer()
{
	std::unique_ptr<TcpServer> tcp(new TcpServer());
	if (tcp->setListen(m_port) != 0) return false;
	while (1) {
		std::unique_ptr<TcpSocket> socket(tcp->acceptConnect());//socket可能有多个
		if (socket == nullptr) continue;
		pthread_t tid;
		pthread_create(&tid, nullptr, workHard, this);
		m_umap.insert({ tid, std::move(socket) });
	}
	tcp->closefd();
}

void* ServerOP::workHard(void* arg)
{
	//接收并反序列化数据
	ServerOP* op = (ServerOP*)arg;
	std::unique_ptr<TcpSocket> tcp(op->m_umap[pthread_self()].release());
	string recvData = tcp->recvMsg();
	std::unique_ptr<CodecFactory> factory(new RespondFactory(recvData));
	std::unique_ptr<Codec> respond(factory->createCodec());
	std::unique_ptr<RequestMsg> reqMsg((RequestMsg*)respond->decodeMsg());
	//判断数据
	string data;
	switch (reqMsg->cmdtype()) {
	case 1:
		data = op->seckeyAgree(std::move(reqMsg));
		break;
	case 2:
		op->seckeyCheck();
		break;
	case 3:
		op->seckeyCancel();
		break;
	case 4:
		std::cout << "密钥查看" << std::endl;
		break;
	default:
		break;
	}
	tcp->sendMsg(data);
}

string ServerOP::seckeyAgree(std::unique_ptr<RequestMsg> reqMsg)
{
	RespondInfo respInfo;
	//  校验签名
	RsaCrypto rsa("public_key.pem");
	rsa.rsaVerify(reqMsg->data(), reqMsg->sign());
	if(!rsa.rsaVerify(reqMsg->data(), reqMsg->sign())){
		respInfo.status = false;
		cout << "签名校验失败!!!" << endl;
	}
	//  组织数据
	respInfo.status = true;
	string randStr = getRandStr(16);
	string secKey = rsa.encryptByPublicKey(randStr);
	respInfo.data = randStr;  //生成对称加密的公钥并使用非对称加密的公钥加密
	respInfo.serverId = m_serverID;
	respInfo.clientId = reqMsg->clientid();
	respInfo.sekeyId = 1;
	//  使用数据和工厂类创建服务器编解码对象
	std::unique_ptr<CodecFactory> factory(new RespondFactory(&respInfo));
	Codec* resp = factory->createCodec();
	string encStr = resp->encodeMsg();
	//  发送数据
	return encStr;
}

void ServerOP::seckeyCheck()
{
}

void ServerOP::seckeyCancel()
{
}

string ServerOP::getRandStr(int num)
{
	srand(time(NULL));	// 以当前时间为种子
	string retStr = string();
	char* buf = "~`@#$%^&*()_+=-{}[];':";
	for (int i = 0; i < num; ++i)
	{
		int flag = rand() % 4;
		switch (flag)
		{
		case 0:	// 0-9
			retStr.append(1, rand() % 10 + '0');
			break;
		case 1:	// a-z
			retStr.append(1, rand() % 26 + 'a');
			break;
		case 2:	// A-Z
			retStr.append(1, rand() % 26 + 'A');
			break;
		case 3:	// 特殊字符
			retStr.append(1, buf[rand() % strlen(buf)]);
			break;
		}
	}
	return retStr;
}
