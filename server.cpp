#include "server.h"

server::server():tcpsocket()
{
	ipReuse = 0;
	lnkCnt = 0;
	linkGroup = NULL;
	MsgQid = -1;
	clientSocketFd = -1;
	serverPid = 0;
	recShardMemId = 100;
	sndShardMemId = 101;
	char HeadCode[]	= "abcd";
	char TailCode[]	= "dcba";
	string headcode(HeadCode);
	string tailcode(TailCode);
	headlen = sizeof(PACKAGE_HEAD);
	taillen = sizeof(PACKAGE_TAIL);

}

server::~server()
{
	for(int i=0;i<lnkCnt;i++)
	{
		linkGroup[i].~tcplink();
	}
}

int MsgqCreate(int& msqid,int msgkey)
{
	msqid = -1;
	msqid = msgget(msgkey,IPC_CREAT|0666);
	if(msqid <0)
	{
			ERROR("MsgQ Create Error");
			return -1;
	}
	DEBUG("MsgQ Create Success");
	return 0;
}

int MsgSender(int msgqId,int msgType,PID_STU* msgData)
{
	struct msgstru msgs;
	memset(&msgs,0x00,sizeof(struct msgstru));
	msgs.msgtype = msgType;
	msgs.data = *msgData;
	int res = msgsnd(msgqId,&msgs,sizeof(struct msgstru),IPC_NOWAIT);
	//DEBUG("message send res:%d msgType=%d pid=%d ppid=%d type=%d",res,msgType,msgs.data.pid,msgs.data.ppid,msgs.data.pidtype);
	return res;
}

int MsgSender1(int msgqId,int msgType,int* msgtext)
{
	struct msgstru1 msgs;
	memset(&msgs,0x00,sizeof(struct msgstru1));
	msgs.msgtype = msgType;
	msgs.msgtext = *msgtext;
	int res = msgsnd(msgqId,&msgs,sizeof(int),IPC_NOWAIT);
	//DEBUG("message send res:%d msgType=%d pid=%d ppid=%d type=%d",res,msgType,msgs.data.pid,msgs.data.ppid,msgs.data.pidtype);
	return res;
}


int MsgqClear(int msgqId)
{
	struct msgstru msgs;
	while(1)
	{
		memset(&msgs,0x00,sizeof( struct msgstru));
		if(msgrcv(msgqId, (void*)&msgs, BUFSIZ, 0,IPC_NOWAIT) != -1)
		{
			//
		}
		else
		{
			break;
		}
		sleep(1);
	}
	DEBUG("MsgQ Clear Complete");
	return 0;
}

int server::ChkPkg(const string& pkg)
{
	int i = 0,j = 0;
	i = pkg.find(headcode,i);
	j = pkg.find(tailcode,j);
	if(string::npos == i || string::npos == j)
	{
		return -1;
	}
	
	string headstr(pkg,i,headlen);
	string tailstr(pkg,j,taillen);	
	PACKAGE_HEAD head;
	PACKAGE_TAIL tail;	
	memcpy(&head,headstr.c_str(),sizeof(PACKAGE_HEAD));
	memcpy(&tail,tailstr.c_str(),sizeof(PACKAGE_TAIL));
	
	if(head.DataLen != pkg.length()-headlen-taillen)
	{
		return -1;
	}
	if(head.PackageType<0||head.PackageType>=CMD_CNT)
	{
		return -1;
	}
	if(tail.SerialNo!=head.SerialNo)
	{
		return -1;
	}
	return 0;
}

int server::UnPack(const string& headstr,const string& datastr)
{
	PACKAGE_HEAD head;
	memcmp(&head,headstr.c_str(),headlen);
	switch(head.PackageType)
	{
	case CMD_STRING:break;
	case CMD_STRUCT:break;
	case CMD_FILE:break;
	case CMD_DISCONNECT:break;
	case CMD_HEARTBEAT:break;
	}
	return 0;
}

int server::ReadNonBlock(int socketFd,char* recvBuf,int recvLen)
{
	if((-1 == socketFd)||(NULL == recvBuf)||(recvLen <= 0))
	{
		return -1;
	}
	fd_set fds;
	struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec =1000;
	int returnValue = 0;
	FD_ZERO(&fds);
	FD_SET(socketFd,&fds);
	int maxfd =(socketFd)+1;
	int res = select(maxfd,&fds,NULL,NULL,&timeout);
    switch(res)
    {
		case -1:
			returnValue = -1;break;
        case 0:
        	returnValue = 0;break;
        default:
        	if(FD_ISSET(socketFd,&fds))
        	{
				returnValue = read(socketFd,recvBuf,recvLen);
        	}
    }
    FD_CLR(socketFd,&fds);
	return returnValue;
}

void server::DataRecvProc()
{
	DEBUG("DataRecvPro Start");
	struct msgstru1 msgs;
	string recvfile;
	int msmtext;
	char* recvBuffer = (char*)calloc(sizeof(char),READ_BUFF_LEN);
	if(NULL == recvBuffer)
	{
		ERROR("Calloc Error");
	}
	while(1)
	{
		int res = msgrcv(MsgQidLnk,(void*)&msgs,BUFSIZ,PROCESS_DATARECV,IPC_NOWAIT);
		if(res!= -1)
		{
			if(1 == msgs.msgtext)
			{
				recvfile = "./01.txt";
				msmtext = 2;
			}
			else
			{
				recvfile = "./02.txt";
				msmtext = 1;
			}
			DEBUG("Change Recv File %s",recvfile.c_str());
			MsgSender1(MsgQidLnk,PROCESS_DATAPROC,&msmtext);
		}
		while(1)
		{
			memset(recvBuffer,0x00,READ_BUFF_LEN);
			int res = ReadNonBlock(clientSocketFd,recvBuffer,READ_BUFF_LEN);
			if(res > 0)
			{
				DEBUG("data com res=%d",res);
				string s(recvBuffer);
				ofstream out(recvfile.c_str());
				out<<s<<"\n";
	    		break;
			}
			else if(res <0)
			{
				ERROR("Read Failed");
			}
		}
	}
}

void server::DataSendProc()
{
	while(1)
	{
		//send
		//clear buff
		sleep(5);
	}
	DEBUG("DataSendProc End");
}

void server::GetPckg(string readBuff,int i)
{
	string headstr(readBuff,i,headlen);
	PACKAGE_HEAD head;
	memcpy(&head,headstr.c_str(),sizeof(headlen));
	int pckglen = headlen + taillen + head.DataLen;

	string stuffing(readBuff,i+headlen,head.DataLen);
	string package(readBuff,i,pckglen);
	int res = ChkPkg(package);
	if(res)
	{
		UnPack(headstr,stuffing);
	}
}

void server::DataProcProc()
{
	DEBUG("DataRecvPro Start");	
	string procfile = "null";
	int rmflag = 0;
	int msgtext;
	while(1)
	{
		if(!(procfile == "null"))
		{
			ifstream in(procfile.c_str());
			string s;
			while(getline(in,s))
			{
				cout<<s<<endl;
				//dataproc
			}
			if(1 == rmflag)
			{
				rmflag = 0; 
				remove(procfile.c_str());
				DEBUG("Remove File %s",procfile.c_str());
				if(procfile ==  "./01.txt")
				{
					msgtext = 1;
				}
				else if(procfile ==  "./02.txt")
				{
					msgtext = 2;
				}
				MsgSender1(MsgQidLnk,PROCESS_DATARECV,&msgtext);
			}
		}
		struct msgstru1 msgs;
		memset(&msgs,0,sizeof(struct msgstru1));
		int res = msgrcv(MsgQidLnk,(void*)&msgs,BUFSIZ,PROCESS_DATAPROC,IPC_NOWAIT);
		if(res!= -1)
		{
			if(1 == msgs.msgtext)
			{
				procfile = "./01.txt";
			}
			else
			{
				procfile = "./02.txt"; 
			}
			rmflag = 1;
		}
	}
}

int server::AcceptNonBlock(tcpsocket* lnk)
{
	if(NULL == lnk)
	{
		return -1;
	}
    int returnValue = 0;
	struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec =1000;
    fd_set fds;
	FD_ZERO(&fds);
	FD_SET(socketFd,&fds);
	int maxfd =(socketFd)+1;
	socklen_t len = sizeof(struct sockaddr_in);
	int res = select(maxfd,&fds,NULL,NULL,&timeout);
    switch(res)
    {
        case -1:
        	returnValue = -1;break;
        case 0:
        	returnValue = -1;break;
        default:
        	if(FD_ISSET(socketFd,&fds))
        	{
        		lnk->socketFd =accept(socketFd,(struct sockaddr*)&(lnk->socketAddr),&len);
        		DEBUG("%s Link The Server", inet_ntoa(lnk->socketAddr.sin_addr));
        	}
        	returnValue = 1;
        	break;
    }
    FD_CLR(socketFd, &fds);
	return returnValue;
}

void server::ClearAddrList(struct sockaddr_in* from)
{
	for(int i = 0 ; i < lnkCnt ; i ++)
	{
		if(0 == linkGroup[i].AddrCmp(from))
		{
			linkGroup[i].LinkClear();
		}
	}
}

int server::IsAddrListFull()
{
	for(int i=0;i<lnkCnt;i++)
	{
		if(0 == linkGroup[i].IsLinking())
		{
			return i;
		}
	}
	return -1;
}

int server::ProcessCreate(void(server::*newproc)())
{
	pid_t pid = fork();
	if(pid < 0)
	{
		ERROR("Fork Error");
	}
	else if(0 == pid)
	{
		(this->*newproc)();
		exit(0);
	}
	return pid;
}

void server::ClientServiceProcess()
{
	DEBUG("recShardMemId=%d sndShardMemId=%d",recShardMemId,sndShardMemId);
	
	MsgqCreate(MsgQidLnk,getpid());
	MsgqClear(MsgQidLnk);

	int msmtext1 = 1;
	int msmtext2 = 2;
	
	MsgSender1(MsgQidLnk,PROCESS_DATARECV,&msmtext1);
	MsgSender1(MsgQidLnk,PROCESS_DATARECV,&msmtext2);
	pid_t recvPid = ProcessCreate(&server::DataRecvProc);
	pid_t sendPid = ProcessCreate(&server::DataSendProc);
	pid_t procPid = ProcessCreate(&server::DataProcProc);
	
	int ppid = getpid();

	PID_STU pidInfo0;
	pidInfo0.pidtype = RECE_DATA;
	pidInfo0.pid = recvPid;
	pidInfo0.ppid = ppid;
	PID_STU pidInfo1;
	pidInfo1.pidtype = SEND_DATA;
	pidInfo1.pid = sendPid;
	pidInfo1.ppid = ppid;
	PID_STU pidInfo2;
	pidInfo2.pidtype = PROC_DATA;
	pidInfo2.pid = procPid;
	pidInfo2.ppid = ppid;

	MsgSender(MsgQid,PROCESS_CREATE,&pidInfo0);
	MsgSender(MsgQid,PROCESS_CREATE,&pidInfo1);
	MsgSender(MsgQid,PROCESS_CREATE,&pidInfo2);
	
	//DataRecvProc();
	while(1)
	{
		sleep(5);
	}
}

void  server::SetLinkPid()
{
	for(int i=0;i<3;i++)
	{
		struct msgstru msgs;
		memset(&msgs,0x00,sizeof(struct msgstru));
		int res = msgrcv(MsgQid,(void*)&msgs,BUFSIZ,0,IPC_NOWAIT);
		if(res!= -1){
			if(msgs.msgtype == PROCESS_CREATE){
				PID_STU curpidinf = msgs.data;
				for(int i=0;i<lnkCnt;i++){
					if(linkGroup[i].mainpid == curpidinf.ppid){
						switch(curpidinf.pidtype)
						{
						case RECE_DATA:linkGroup[i].recvdatapid=curpidinf.pid;DEBUG("Recv Process Ok");break;
						case SEND_DATA:linkGroup[i].senddatapid=curpidinf.pid;DEBUG("Send Process Ok");break;
						case PROC_DATA:linkGroup[i].procdatapid=curpidinf.pid;DEBUG("Proc Process Ok");break;
						default:ERROR("Unknow Process Type %d",curpidinf.pidtype);break;
						}
					}
				}
			}
		}
		else{
			break;
		}
	}
}

int server::ClientProcessAllocation()
{
	int pid = -1;
	while(1)
	{
		tcpsocket lnk;
		int res = AcceptNonBlock(&lnk);	
		if(res > 0)
		{
			clientSocketFd = lnk.socketFd;
			ClearAddrList(&(lnk.socketAddr));
			int curlinkcnt =  IsAddrListFull();
			if(-1 == curlinkcnt){
				ERROR("Link Error");
				continue;
			}
			pid =ProcessCreate(&server::ClientServiceProcess);
			if(pid<0)
			{
				ERROR("Porcess Create Failed");
			}
			else if(pid>0)
			{
				/*linkGroup[curlinkcnt].mainpid = pid;
				linkGroup[curlinkcnt].curclient.socketFd = lnk.socketFd;
				linkGroup[curlinkcnt].curclient.socketAddr = lnk.socketAddr;
				linkGroup[curlinkcnt].recShardMemId = recShardMemId ;
				linkGroup[curlinkcnt].sndShardMemId =  sndShardMemId;
				recShardMemId += 2;
				 sndShardMemId += 2;*/
			}
		}else
		{
			//SetLinkPid();
		}
		sleep(1);
	}
	return 0;
}

int server::Init(CONF_SERVER* CONF = NULL)
{
	FdInit();
	if(NULL == CONF){
		ipReuse = 1;
		lnkCnt = MAX_LINS_CNT;
		socketAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
		socketAddr.sin_port = htons(SERVER_PORT);
	}else{
		ipReuse = CONF->ipreuse;
		lnkCnt = CONF->linklimit;
		socketAddr.sin_addr.s_addr = inet_addr(CONF->serveraddr.data());
		socketAddr.sin_port = htons(CONF->serverport);
	}
	setsockopt(socketFd,SOL_SOCKET,SO_REUSEADDR,&ipReuse,sizeof(ipReuse));
	if(-1 == bind(socketFd,(struct sockaddr*)&socketAddr,sizeof(socketAddr)))
	{
		ERROR("Bind Error");
		return -1;
	}
	linkGroup = new tcplink[lnkCnt];
	serverPid = getpid();
	MsgqCreate(MsgQid,getpid());
	MsgqClear(MsgQid);
	listen(socketFd,lnkCnt);
	signal(SIGCHLD,SIG_IGN);
	return 0;
}

int main()
{

	server s1;
	s1.Init();
	s1.ClientProcessAllocation();
	return 0;
}
