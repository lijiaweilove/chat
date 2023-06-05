#include <string>
#include "TcpSocket.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>         
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
using std::string;

TcpSocket::TcpSocket() {};

TcpSocket::TcpSocket(int fd) :m_connfd(fd) {};  //传递进来的fd已经建立好连接了

TcpSocket::~TcpSocket() {};

int TcpSocket::connectToHost(std::string ip, unsigned short port, int wait_seconds) {
	int ret = 0;
	if (port <= 0 || port > 65535 || wait_seconds < 0)
		return ParamError;
	m_connfd = socket(AF_INET, SOCK_STREAM, 0);
	if (m_connfd == -1)
		return errno;
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(port);
	inet_pton(AF_INET, ip.data(), &servAddr.sin_addr.s_addr);
	//ret = connect(m_connfd, (struct sockaddr*)&servAddr, sizeof(servAddr));
	ret = connectTimeout(&servAddr, wait_seconds);
	return ret;
}

void TcpSocket::disConnect() {
	if (m_connfd > 0) {
		close(m_connfd);
	}
};

int TcpSocket::sendMsg(std::string sendMsg, int sendtime) {
	int ret = 0, writed = 0;
	ret = writeTimeout(sendtime);  //循环等待该fd可写
	if (ret == 0) {  // 发送带有包头的数据
		int dataLen = sendMsg.size() + 4;
		unsigned char* netdata = (unsigned char*)malloc(dataLen);
		if (netdata == nullptr)
			return MallocError;
		int netLen = htonl(sendMsg.size());  //发送int类型有字节序问题,字符串没有
		memcpy(netdata, &netLen, 4);
		memcpy(netdata + 4, sendMsg.data(), sendMsg.size());
		writed = wirten(netdata, dataLen);
		if (netdata != nullptr) {
			free(netdata);
		}
		if (writed < (dataLen))
			return writed;
	}
	if (ret < 0) {
		if (ret == -1 && errno == ETIMEDOUT)
			ret = TimeoutError;
	}
	return ret;
};

std::string TcpSocket::recvMsg(int recvtime) {
	int ret = 0;
	char* netdata = nullptr;
	ret = readTimeout(recvtime);  //循环等待该fd可读
	if (ret != 0) {
		if (ret == -1 && errno == ETIMEDOUT)
			ret = TimeoutError;
		std::cout << "readTimeout" << std::endl;
		return string();
	}
	int dataLen = 0;
	ret = readn(&dataLen, 4);  //读前四个字节
	if (ret == -1 || ret < 4) {
		std::cout << "func readn(1)" << std::endl;
		return string();
	}
	int len = ntohl(dataLen);
	netdata = (char*)malloc(len + 1);
	if (netdata == nullptr)
		return nullptr;
	ret = readn(netdata, len);
	if (ret == -1 || ret < len) {
		std::cout << "func readn(2)" << std::endl;
		return string();
	}
	netdata[len] = '\0';  //多分配一个字节内容，兼容可见字符串 字符串的真实长度仍然为n
	string data = string(netdata);
	free(netdata);
	netdata = nullptr;
	return data;
};

int TcpSocket::setNonBlock() {
	int flag = fcntl(m_connfd, F_GETFL);
	if (flag == -1)
		return -1;
	flag |= O_NONBLOCK;
	return fcntl(m_connfd, F_SETFL, flag);
}

int TcpSocket::setBlock() {
	int flag = fcntl(m_connfd, F_GETFL);
	if (flag == -1)
		return -1;
	flag &= ~O_NONBLOCK;
	return fcntl(m_connfd, F_SETFL, flag);
}

int TcpSocket::connectTimeout(sockaddr_in* cliAddr, int wait_seconds) {
	int ret = 0;
	socklen_t addrLen = sizeof(struct sockaddr_in);
	if (wait_seconds > 0)
		setNonBlock();
	ret = connect(m_connfd,(struct sockaddr*)cliAddr, addrLen);
	if (ret < 0 && errno == EINPROGRESS) {
		fd_set connect_fdset;
		struct timeval timeout;
		FD_ZERO(&connect_fdset);
		FD_SET(m_connfd, &connect_fdset);
		timeout.tv_sec = wait_seconds;
		timeout.tv_usec = 0;
		do {  //连接之后,等待一个wait_seconds的时间,只有当EINTR时才继续循环,select监听一个fd可写
			ret = select(m_connfd + 1, nullptr, &connect_fdset, nullptr, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret == 0)
			return -1;
		else if (ret == -1)
			return -1;
		else if (ret == 1) {
			//ret返回为1（表示套接字可写），可能有两种情况，一种是连接建立成功，一种是套接字产生错误
			//此时错误信息不会保存至errno变量中，因此，需要调用getsockopt来获取
			int err = 0;
			socklen_t len = sizeof(err);
			ret = getsockopt(m_connfd, SOL_SOCKET, SO_ERROR, &err, &len);
			if (err == 0)
				return 0;
			else
				return -1;
		}
	}
	if (wait_seconds > 0)
		setBlock();
	return ret;
}

int TcpSocket::writeTimeout(int sendtime) {
	int ret = 0;
	if (sendtime > 0) {
		fd_set write_fdset;
		struct timeval timeout;
		FD_ZERO(&write_fdset);
		FD_SET(m_connfd, &write_fdset);
		timeout.tv_sec = sendtime;
		timeout.tv_usec = 0;
		do {
			ret = select(m_connfd + 1, nullptr, &write_fdset, nullptr, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret == 0) {
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
			ret = 0;
	}
	return ret;
}

int TcpSocket::wirten(const void* netdata, int dataLen) {
	size_t nleft = dataLen;
	ssize_t nwritten = 0;
	char* bufp = (char*)netdata;
	while (nleft > 0) {
		if ((nwritten = write(m_connfd, bufp, dataLen)) < 0) {
			if (errno == EINTR) return -1;
		}else if (nwritten == 0) continue;
		bufp += nleft;
		nleft -= nwritten;
		}
	return dataLen;
}

int TcpSocket::readTimeout(int recvtime) {
	int ret = 0;
	if (recvtime > 0) {
		fd_set read_fdset;
		struct timeval timeout;
		FD_ZERO(&read_fdset);
		FD_SET(m_connfd, &read_fdset);
		timeout.tv_sec = recvtime;
		timeout.tv_usec = 0;
		do {
			ret = select(m_connfd + 1, &read_fdset, nullptr, nullptr, &timeout);
		} while (ret < 0 && errno == EINTR);
		if (ret == 0) {
			ret = -1;
			errno = ETIMEDOUT;
		}
		else if (ret == 1)
			ret = 0;
	}
	return ret;
}

int TcpSocket::readn(void* netdata, int dataLen) {
	size_t nleft = dataLen;
	ssize_t nread = 0;
	char* bufp = (char*)netdata;
	while (nleft > 0) {
		if ((nread = read(m_connfd, bufp, dataLen)) < 0) {
			if (errno == EINTR) continue;
			return -1;
		}else if (nread == 0) return dataLen - nleft;
		bufp += nleft;
		nleft -= nread;
	}
	return dataLen;
}