#include "clientOP.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <json/json.h>
#include "Message.pb.h"
#include "CodecFactory.h"
#include "RequestFactory.h"
#include "RespondFactory.h"
#include "RsaCrypto.h"
#include "TcpSocket.h"
#include "MyHsah.h"
#include "MyHsah.h"
using namespace Json;
using namespace std;

//  获取本地json文件配置
clientOP::clientOP(string jsonFile)
{
	ifstream ifs(jsonFile);
	Value root;
	Reader read;
	read.parse(ifs, root);
	ifs.close();
	m_info.clientID = root["clientID"].asString();
	m_info.serverID = root["serverID"].asString();
	m_info.ip = root["ip"].asString();
	m_info.port = root["port"].asInt();
}

clientOP::~clientOP()
{
}

//  秘钥协商
bool clientOP::seckeyAgree()
{
	//  生成公钥
	RsaCrypto rsa;
	rsa.generateRsaKey(2048);
	//  读取公钥文件
	ifstream ifs("public_key.pem");
	stringstream str;
	str << ifs.rdbuf();
	ifs.close();
	//  组织数据
	RequestInfo reqInfo;
	reqInfo.cmd = Agree;
	reqInfo.clientId = m_info.clientID;
	reqInfo.serverId = m_info.serverID;
	reqInfo.data = str.str();  //非对称加密的公钥(原数据)
	MyHsah sha1(M_SHA1);
	sha1.addData(str.str());
	reqInfo.sign = rsa.rsaSign(sha1.result());  //对公钥的hash值签名
	cout << "签名完成...." << endl;
	//  使用数据和工厂类创建客户端编解码对象
	CodecFactory* factory = new RequestFactory(&reqInfo);
	Codec* client = factory->createCodec();
	string encStr = client->encodeMsg();
	//释放资源
	delete factory;
	delete client;
	//发送序列化后的数据
	TcpSocket* tcp = new TcpSocket;
	if (tcp->connectToHost(m_info.ip, m_info.port) != 0) return false;
	cout << "连接服务器成功..." << endl;
	if (tcp->sendMsg(encStr) != 0) return false;
	//等待服务器回复,并解码服务器数据
	string out = tcp->recvMsg();  // 没收到数据
	CodecFactory* fa = new RespondFactory(out);
	Codec* req = fa->createCodec();
	RespondMsg* msg = (RespondMsg*)req->decodeMsg();  // 成员变量
	if (!msg->status()) {
		cout << "秘钥协商失败" << endl;
		return false;
	}
	//解密 私钥是rsa的成员变量
	string key = rsa.decryptByPrivateKey(msg->data());
	cout << "对称加密秘钥:" << key << endl;
	//释放资源
	delete fa;
	delete req;
	//  这是一个短连接
	tcp->disConnect();
	delete tcp;

	return true;
}

//  密钥校验
bool clientOP::seckeyCheck()
{
	cout << "密钥校验" << endl;
}

//  密钥注销
bool clientOP::seckeyCancel()
{
	cout << "密钥注销" << endl;
}
