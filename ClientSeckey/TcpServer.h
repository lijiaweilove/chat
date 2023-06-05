#pragma once
#include "TcpSocket.h"


class TcpServer
{
private:
	int m_lfd;
public:
	TcpServer();
	~TcpServer();
	int setListen(unsigned short port);
	TcpSocket* acceptConnect(int timeout = 10000);
	void closefd();
};

