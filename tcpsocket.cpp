#include "tcpsocket.h"
tcpsocket::tcpsocket()
{
	socketFd = -1;
	memset(&socketAddr,0x00,sizeof(struct sockaddr_in));
	socketAddr.sin_family = PF_INET;
}
tcpsocket::~tcpsocket()
{
}
int tcpsocket::FdInit()
{
	socketFd = socket(PF_INET,SOCK_STREAM,0);
	if(-1 == socketFd)
	{
		ERROR("socket\n");
		return -1;
	}
	return 0;
}



