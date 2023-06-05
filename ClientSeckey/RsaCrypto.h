#pragma once
#include <string>
#include <openssl/rsa.h>
#include <openssl/pem.h>

//签名使用的hash算法
enum sigHashType {
	level1 = NID_md5,
	level2 = NID_sha1,
	level3 = NID_sha224
};

//非对称加密RSA
class RsaCrypto
{
private:
	RSA* pubKey;
	RSA* priKey;
public:
	RsaCrypto();  //需要调用generateRsaKey生成公/私钥
	RsaCrypto(std::string fileName, bool isPrivate = true);
	//RsaCrypto(std::string pubFile,std::string priFile);  //从本地获取公/私钥
	~RsaCrypto();
	void generateRsaKey(int keyLength,std::string pubFile="public_key.pem",std::string priFile= "private_key.pem");
	std::string encryptByPublicKey(std::string text);
	std::string decryptByPrivateKey(std::string str);
	std::string rsaSign(std::string text,sigHashType type = level2);
	bool rsaVerify(std::string text, std::string sigBuf,sigHashType type= level2);
private:
	std::string toBase64(const char* buf, int len);
	char* fromBase64(std::string str);
};

