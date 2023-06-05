#include "ServerOP.h"
#include "TcpServer.h"
#include "TcpSocket.h"
#include "Message.pb.h"
#include "CodecFactory.h"
#include "RequestFactory.h"
#include "RespondFactory.h"
#include "RsaCrypto.h"
#include "MyHsah.h"
#include <string>
#include <memory>
#include <fstream>
#include <json/json.h>
#include <fstream>
#include <unistd.h>
#include <iostream>
#include "RespondCodec.h"
using namespace std;
using namespace Json;
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
	m_server.reset(new TcpServer);
	m_server->setListen(m_port);
	while (1) {
		cout << "等待客户端连接....." << endl;
		std::unique_ptr<TcpSocket> socket(m_server->acceptConnect());//socket可能有多个
		if (socket == nullptr) continue;
		cout << "与客户端连接成功....." << endl;
		pthread_t tid;
		pthread_create(&tid, nullptr, workHard, this);
		m_umap.insert({ tid, std::move(socket) });
	}
	m_server->closefd();
}

void* ServerOP::workHard(void* arg)
{
	sleep(1);
	//接收并反序列化数据
	ServerOP* op = (ServerOP*)arg;
	std::unique_ptr<TcpSocket> tcp(op->m_umap[pthread_self()].release());
	string recvData = tcp->recvMsg();
	std::unique_ptr<CodecFactory> factory(new RequestFactory(recvData));
	std::unique_ptr<Codec> request(factory->createCodec());
	RequestMsg* reqMsg = (RequestMsg*)request->decodeMsg();  //成员变量
	//判断数据
	string data;
	switch (reqMsg->cmdtype()) {
	case 0:
		data = op->seckeyAgree(reqMsg);
		break;
	case 1:
		op->seckeyCheck();
		break;
	case 2:
		op->seckeyCancel();
		break;
	case 3:
		std::cout << "密钥查看" << std::endl;
		break;
	default:
		break;
	}
	//  发送数据并关闭套接字
	tcp->sendMsg(data);
	tcp->disConnect();
	op->m_umap.erase(pthread_self());
	return nullptr;
}

string ServerOP::seckeyAgree(RequestMsg* Msg)
{
	RespondInfo respInfo;
	ofstream ofs("public_key.pem");
	ofs << Msg->data();
	ofs.close();  // 关闭来强制刷新数据
	//  校验签名
	RsaCrypto rsa("public_key.pem");
	MyHsah sha1(M_SHA1);
	sha1.addData(Msg->data());
	if (!rsa.rsaVerify(Msg->data(), Msg->sign())) {
		respInfo.status = false;
		cout << "签名校验失败!!!" << endl;
	}else{
		//  组织数据
		cout << "签名校验成功......" << endl;
		respInfo.status = true;
		string randStr = getRandStr(Len16);
		string secKey = rsa.encryptByPublicKey(randStr);  //加密成二进制数据,会存在不可见的/0.导致read结束
		respInfo.data = secKey;  //生成对称加密的公钥并使用非对称加密的公钥加密
		respInfo.serverId = m_serverID;
		respInfo.clientId = Msg->clientid();
		respInfo.sekeyId = 1;
	}
	std::unique_ptr<CodecFactory> fa(new RespondFactory(&respInfo));
	//  使用数据和工厂类创建服务器编解码对象
	std::unique_ptr<Codec> resp(fa->createCodec());
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

//  要求：字符串包含：a-z, A-Z, 0-9, 特殊字符
string ServerOP::getRandStr(keyLen len)
{
	srand(time(NULL));	// 以当前时间为种子,防止会生成重复的随机数
	string retStr;
	char* buf = "~`@#$%^&*()_+=-{}[];':";
	for (int i = 0; i < len; ++i)
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
