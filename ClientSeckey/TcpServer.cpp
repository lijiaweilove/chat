#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include "TcpServer.h"

TcpServer::TcpServer() {};

TcpServer::~TcpServer() {};

int TcpServer::setListen(unsigned short port) {
    struct sockaddr_in servAddr;
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(port);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (m_lfd == -1)
        return -1;
    int on = 1;
    if (setsockopt(m_lfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
        return -1;
    if (bind(m_lfd, (struct sockaddr*)&servAddr, sizeof(servAddr)) == -1)
        return -1;
    if (listen(m_lfd, 128) == -1)
        return -1;
    return 0;
};

TcpSocket* TcpServer::acceptConnect(int wait_seconds) {
    int ret = 0;
    if (wait_seconds > 0) {
        fd_set accept_fdset;
        struct timeval timeout;
        FD_ZERO(&accept_fdset);
        FD_SET(m_lfd, &accept_fdset);
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do {  //等待一个wait_seconds的时间,只有当EINTR时才继续循环,select监听一个fd可读,然后accept
            ret = select(m_lfd + 1, &accept_fdset, nullptr, nullptr, &timeout);
        } while (ret < 0 && errno == EINTR);
        if (ret <= 0)
            return nullptr;
    }
    int connfd = accept(m_lfd, nullptr, nullptr);
    if (connfd == -1)
        return nullptr;
    return new TcpSocket(connfd);
};

void TcpServer::closefd() {
    close(m_lfd);
};