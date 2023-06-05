#include "MyHsah.h"
using std::string;


MyHsah::MyHsah(HashType type) {
	m_type = type;
	switch (m_type) {
	case M_MD5:
		MD5_Init(&m_ctx.m_md5);
		break;
	case M_SHA1:
		SHA1_Init(&m_ctx.m_sha1);
		break;
	case M_SHA224:
		SHA224_Init(&m_ctx.m_sha224);
		break;
	default:
		MD5_Init(&m_ctx.m_md5);
		break;
	}
};

MyHsah::~MyHsah() {};

void MyHsah::addData(string data) {
	switch (m_type) {
	case M_MD5:
		MD5_Update(&m_ctx.m_md5,data.data(),data.size());
		break;
	case M_SHA1:
		SHA1_Update(&m_ctx.m_sha1,data.data(), data.size());
		break;
	case M_SHA224:
		SHA224_Update(&m_ctx.m_sha224,data.data(), data.size());
		break;
	default:
		MD5_Update(&m_ctx.m_md5,data.data(), data.size());
		break;
	}
};

string MyHsah::result() {
	string result;
	switch (m_type) {
	case M_MD5:
		result = md5Result();
		break;
	case M_SHA1:
		result = sha1Result();
		break;
	case M_SHA224:
		result = sha224Result();
		break;
	default:
		result = md5Result();
		break;
	}
	return result;
};

string MyHsah::md5Result() {
	unsigned char md[MD5_DIGEST_LENGTH]{};
	char res[2 * MD5_DIGEST_LENGTH + 1]{};
	MD5_Final(md, &m_ctx.m_md5);
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		snprintf(&res[i * 2], static_cast<size_t>(2 * MD5_DIGEST_LENGTH + 1), "%02x", md[i]);
	}
	return string(res);
}

string MyHsah::sha1Result()
{
	unsigned char md[SHA_DIGEST_LENGTH]{};
	char res[2 * SHA_DIGEST_LENGTH + 1]{};
	SHA1_Final(md, &m_ctx.m_sha1);
	for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
		snprintf(&res[i * 2], static_cast<size_t>(2 * SHA_DIGEST_LENGTH + 1), "%02x", md[i]);
	}
	return string(res);
}

string MyHsah::sha224Result()
{
	unsigned char md[SHA224_DIGEST_LENGTH]{};
	char res[2 * SHA224_DIGEST_LENGTH + 1]{};
	SHA224_Final(md, &m_ctx.m_sha224);
	for (int i = 0; i < SHA224_DIGEST_LENGTH; i++) {
		snprintf(&res[i * 2], static_cast<size_t>(2 * SHA224_DIGEST_LENGTH + 1), "%02x", md[i]);
	}
	return string(res);
}

