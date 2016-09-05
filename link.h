#include "tcpsocket.h"
#ifndef _LINK_H_
#define _LINK_H_ 

class tcplink{
public:
	tcplink();
	~tcplink();
	tcpsocket curclient;
	pid_t mainpid;
	pid_t recvdatapid;
	pid_t senddatapid;
	pid_t procdatapid;
	int recShardMemId;
	int sndShardMemId;
	int AddrCmp(struct sockaddr_in *);
	void LinkClear();
	int IsLinking();
private:
};
#endif
