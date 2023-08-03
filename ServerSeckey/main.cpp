#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "ServerOP.h"
#include <fcntl.h>
using namespace std;

//设置守护进程
void setDaemon()
{
	pid_t pid = fork();
	if (pid > 0){
		exit(0);
	}
	setsid();
	chdir("/home/lee/projects/ServerSeckey");
	umask(0);
	int fd = open("/dev/null", O_RDWR);
	dup2(fd, STDIN_FILENO);
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);
}	

int main()
{
	setDaemon();
    ServerOP server;
    server.startServer();
}