#include"link.h"
tcplink::tcplink()
{
	curclient.socketFd = -1;
	recvdatapid = -1;
	senddatapid = -1;
	procdatapid = -1;
	mainpid = -1;
	curclient.socketAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	curclient.socketAddr.sin_addr.s_addr = htons(0);
}
tcplink::~tcplink()
{
}

int tcplink::AddrCmp(struct sockaddr_in * taraddr)
{
	int res = -3;	
	if((curclient.socketAddr.sin_addr.s_addr == taraddr->sin_addr.s_addr))
	{
		res += 1;
	}
	if(curclient.socketAddr.sin_family == taraddr->sin_family)
	{
		res += 2;
	}
	return res;
}
void tcplink::LinkClear()
{
	close(curclient.socketFd);
	kill(recvdatapid,SIGINT);
	kill(senddatapid,SIGINT);
	kill(procdatapid,SIGINT);
	kill(mainpid,SIGINT);
	curclient.socketFd = -1;
	recvdatapid = -1;
	senddatapid = -1;
	procdatapid = -1;
	mainpid = -1;
	curclient.socketAddr.sin_addr.s_addr = inet_addr("0.0.0.0");
	curclient.socketAddr.sin_addr.s_addr = htons(0);
}
int tcplink::IsLinking()
{
	if(-1 == curclient.socketFd && -1 == mainpid)
	{
		return 0;
	}
	return -1;
}




