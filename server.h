#include "link.h"

#ifndef _SERVER_H_
#define _SERVER_H_

enum{
	PROCESS_CREATE = 1,
};

enum{
	PROCESS_DATARECV = 1,
	PROCESS_DATAPROC,
	PROCESS_DATASEND,	
};

enum{
	RECE_DATA = 0,
	SEND_DATA,
	PROC_DATA,
};

typedef  struct CONF_SERVER{
	int ipreuse;
	int linklimit;
	int serverport;
	string serveraddr;
}CONF_SERVER;

typedef struct  pidstru
{
	int pidtype;
	pid_t ppid;
	pid_t pid;
}PID_STU;

struct msgstru
{
    int msgtype;
    PID_STU data;
};

struct msgstru1
{
    int msgtype;
    int msgtext;
};


class server:public tcpsocket
{
public:
	server();
	~server();
	int Init(CONF_SERVER* );
	int ClientProcessAllocation();
	void ClearAddrList(struct sockaddr_in*);
	int IsAddrListFull();
	int ProcessCreate(void(server::*)());
	void ClientServiceProcess();
	void SetLinkPid();
	int AcceptNonBlock(tcpsocket*);
	void DataRecvProc();
	void DataSendProc();
	void DataProcProc();
	int ReadNonBlock(int,char*,int);
	int ChkPkg(const string&);
	int UnPack(const string&,const string&);
	void GetPckg(string readBuff,int i);

	int clientSocketFd;
	int ipReuse;
	int lnkCnt;
	int MsgQid;
	int MsgQidLnk;
	int serverPid;
	int recShardMemId;
	int sndShardMemId;
	tcplink* linkGroup;

	string headcode;
	string tailcode;
	int headlen;
	int taillen;
private:
};

#endif

