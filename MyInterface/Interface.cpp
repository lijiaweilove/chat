#include "Interface.h"
#include "AesCrypto.h"
#include "BaseShm.h"
#include "SeckeyShm.h"
#include "json/json.h"
#include "SeckeyNodeInfo.h"
#include "fstream"
using namespace Json;

Interface::Interface(string json)
{
    //  从json中获取秘钥
    ifstream ifs(json);
    Reader reader;
    Value root;
    reader.parse(ifs, root);
    string serverId = root["ServerID"].asString();
    string clientID = root["ClientID"].asString();
    string key = root["ShmKey"].asString();
    int maxNode = root["MaxNode"].asInt();
    //  从共享内存中获取秘钥
    SeckeyShm shm(key, maxNode);
    SeckeyNodeInfo node = shm.shmRead(serverId, clientID);
    m_key = string(node.seckey);
}

Interface::~Interface()
{
}

string Interface::encryptData(string data)
{
    AesCrypto aes(m_key);
    string str = aes.aesCBCEncrypt(data);
    return str;
}

string Interface::decryptData(string data)
{
    AesCrypto aes(m_key);
    std::string str = aes.aesCBCDecrypt(data);
    return str;
}
