#include "common.h"

#ifndef _TCP_H_ 
#define _TCP_H_ 

class tcpsocket
{
public:
	tcpsocket();
	~tcpsocket();
	int FdInit();
	int socketFd;
	struct sockaddr_in socketAddr;
private:
};

#endif

