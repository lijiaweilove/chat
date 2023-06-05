#include "RsaCrypto.h"
#include <iostream>
using std::string;

//extern "C" {
//#include <openssl/applink.c>
//};

RsaCrypto::RsaCrypto() {
	pubKey = RSA_new();
	priKey = RSA_new();
}

RsaCrypto::RsaCrypto(string fileName, bool isPrivate)
{
	FILE* fd = nullptr;
	pubKey = RSA_new();
	priKey = RSA_new();
	if (isPrivate) {
		fd = fopen(fileName.data(), "r");
		priKey = PEM_read_RSAPrivateKey(fd, nullptr, nullptr, nullptr);
		fclose(fd);
	}else{
		fd = fopen(fileName.data(), "r");
		pubKey = PEM_read_RSAPublicKey(fd, nullptr, nullptr, nullptr);
		fclose(fd);
	}
}


RsaCrypto::~RsaCrypto(){
	RSA_free(pubKey);
	RSA_free(priKey);
}

void RsaCrypto::generateRsaKey(int keyLength,string pubFile,string priFile)
{
	BIGNUM* bn = nullptr;
	RSA* rsa = nullptr;
	bn = BN_new();
	BN_set_word(bn, RSA_F4);
	rsa = RSA_new();
	RSA_generate_key_ex(rsa, keyLength, bn, nullptr);
	//将密钥对写入磁盘,生成的在内存中
	FILE* fd = nullptr;
	fd = fopen(pubFile.data(), "w");
	PEM_write_RSAPublicKey(fd, rsa);
	fclose(fd);
	fd = fopen(priFile.data(), "w");
	PEM_write_RSAPrivateKey(fd, rsa, nullptr, nullptr, 0, nullptr, nullptr);
	fclose(fd);
	pubKey = RSAPublicKey_dup(rsa);
	priKey = RSAPrivateKey_dup(rsa);
	BN_free(bn);
	RSA_free(rsa);
}

std::string RsaCrypto::encryptByPublicKey(std::string text)
{
	//数据加密后,长度和密钥长度相同,使用函数生成密钥长度
	int keyLen = RSA_size(pubKey);
	char* buf = new char[keyLen];
	string res = string();
	//len返回加密后数据长度==密钥长度
	int len = RSA_public_encrypt(text.size(), (const unsigned char*)text.data(),
		(unsigned char*)buf, pubKey, RSA_PKCS1_PADDING);
	if (len >= 0) res = toBase64(buf, len);
	delete[] buf;
	return res;
}

std::string RsaCrypto::toBase64(const char* buf, int len)
{
	BIO* b64 = BIO_new(BIO_f_base64());
	BIO* bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_write(b64, buf, len);
	BIO_flush(b64);
	BUF_MEM* bptr = nullptr;
	BIO_get_mem_ptr(b64, &bptr);
	string res = string(bptr->data, bptr->length);
	BIO_free_all(b64);
	return res;
}

char* RsaCrypto::fromBase64(std::string str)
{
	BIO* b64 = BIO_new(BIO_f_base64());
	BIO* bmem = BIO_new_mem_buf(str.data(), str.size());
	bmem = BIO_push(b64, bmem);
	char* buf = new char[str.size()];
	BIO_read(b64, buf, str.size());
	BIO_free_all(b64);
	return buf;
}

std::string RsaCrypto::decryptByPrivateKey(std::string str)
{
	//数据加密后,长度和密钥长度相同,使用函数生成密钥长度
	char* text = fromBase64(str);
	int keyLen = RSA_size(priKey);
	char* buf = new char[keyLen];
	//len加密后数据长度
	string res = string();
	int len = RSA_private_decrypt(keyLen, (const unsigned char*)text,
		(unsigned char*)buf, priKey, RSA_PKCS1_PADDING);
	if (len >= 0) res = string(buf, len);
	delete[] buf;
	delete[] text;
	return res;
}

std::string RsaCrypto::rsaSign(string text,sigHashType type)
{
	unsigned int outLen = 0;
	char* out = new char[RSA_size(priKey) + 1];
	RSA_sign(type, (const unsigned char*)text.data(), text.size(), (unsigned char*)out, &outLen, priKey);
	string res = toBase64(out, outLen);
	delete[] out;
	return res;
}

bool RsaCrypto::rsaVerify(string text,string sigBuf,sigHashType type)
{
	int kenLen = RSA_size(pubKey);
	char* sign = fromBase64(sigBuf);
	if (RSA_verify(type, (const unsigned char*)text.data(), text.size(),
		(unsigned char*)sign, kenLen, pubKey) == -1)
		return false;
	return true;
}
