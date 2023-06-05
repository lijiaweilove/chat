#pragma once
#include <string>


//  Tcp socket¡¨Ω”≥ÿ¿‡
static const int TIMEOUT = 10000;
class TcpSocket
{
private:
	int m_connfd;
public:
	enum ErrorType { ParamError = 3001, TimeoutError, PeerCloseError, MallocError };
	TcpSocket();
	TcpSocket(int fd);
	~TcpSocket();
	int connectToHost(std::string ip, unsigned short port, int timeout = TIMEOUT);
	void disConnect();
	int sendMsg(std::string sendMsg, int sendtime = TIMEOUT);
	std::string recvMsg(int timeout = TIMEOUT);
private:
	int setNonBlock(int fd);
	int setBlock(int fd);
	int connectTimeout(struct sockaddr_in* cliAddr, int wait_seconds);
	int writeTimeout(int sendtime);
	int wirten(const void* netdata, int dataLen);
	int readTimeout(int recvtime);
	int readn(void* netdata, int dataLen);
};

