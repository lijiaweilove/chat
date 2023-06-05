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
using namespace Json;
using namespace std;

//  ��ȡ����json�ļ�����
clientOP::clientOP(string jsonFile)
{
	ifstream ifs(jsonFile);
	Value root;
	Reader read;
	read.parse(ifs, root);
	m_info.clientID = root["clientID"].asString();
	m_info.serverID = root["serverID"].asString();
	m_info.ip = root["ip"].asString();
	m_info.port = root["port"].asInt();
}

clientOP::~clientOP()
{
}

//  ��ԿЭ��
bool clientOP::seckeyAgree()
{
	//  ���ɹ�Կ
	RsaCrypto rsa;
	rsa.generateRsaKey(2048);
	//  ��ȡ��Կ�ļ�
	ifstream ifs("public_key.pem");
	stringstream str;
	str << ifs.rdbuf();
	str << ifs.rdbuf();
	//  ��֯����
	RequestInfo reqInfo;
	reqInfo.cmd = Agree;
	reqInfo.clientId = m_info.clientID;
	reqInfo.serverId = m_info.serverID;
	reqInfo.sign = rsa.rsaSign(str.str());  //��Կǩ��
	reqInfo.data = str.str();  //�ǶԳƼ��ܵĹ�Կ(ԭ����)
	cout << "ǩ�����...." << endl;
	//  ʹ�����ݺ͹����ഴ���ͻ��˱�������
	CodecFactory* factory = new RequestFactory(&reqInfo);
	Codec* client =  factory->createCodec();
	string encStr = client->encodeMsg();
	//�ͷ���Դ
	delete factory;
	delete client;
	cout << "���л��ַ���:" << encStr << endl;
	//�������л��������
	TcpSocket* tcp = new TcpSocket;
	if(tcp->connectToHost(m_info.ip, m_info.port) != 0) return false;
	cout << "���ӷ������ɹ�:" << encStr << endl;
	if (tcp->sendMsg(encStr) != 0) return false;
	//�ȴ��������ظ�,���������������
	string out = tcp->recvMsg();
	CodecFactory* fa = new RespondFactory(out);
	Codec* req = fa->createCodec();
	RespondMsg* msg = (RespondMsg*)req->decodeMsg();
	if (!msg->status()) {
		cout << "��ԿЭ��ʧ��" << endl;
		return false;
	}
	//���� ˽Կ��rsa�ĳ�Ա����
	string key = rsa.decryptByPrivateKey(msg->data());
	cout << "�ԳƼ��ܵ���Կ" << key << endl;
	//�ͷ���Դ
	delete fa;
	delete msg;
	//  ����һ��������
	tcp->disConnect();
	delete tcp;

	return true;
}

//  ��ԿУ��
bool clientOP::seckeyCheck()
{
	cout << "��ԿУ��" << endl;
}

//  ��Կע��
bool clientOP::seckeyCancel()
{
	cout << "��Կע��" << endl;
}
