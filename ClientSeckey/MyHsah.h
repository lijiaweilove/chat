#pragma once
#include <string>
#include <openssl/md5.h>
#include <openssl/sha.h>

enum HashType{M_MD5,M_SHA1,M_SHA224};

//不可逆,结果定长,输入不同,输出也不同,MD5(16字节),sha1(20字节)
class MyHsah
{
private:
	HashType m_type;
	union MyUnion
	{
		MD5_CTX m_md5;
		SHA_CTX m_sha1;
		SHA256_CTX m_sha224;
	}m_ctx;
public:
	MyHsah(HashType type);
	~MyHsah();
	void addData(std::string data);
	std::string result();
private:
	std::string md5Result();
	std::string sha1Result();
	std::string sha224Result();
};

