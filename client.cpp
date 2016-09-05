/*
 * client.cpp
 *
 *  Created on: 2016年6月12日
 *      Author: fansiyuan
 */
#include "client.h"
#include "queue.h"

#define	DATA_BUF_LEN PACKAGE_DATA_LEN
#define	HEARTBEAT_INTERVAL 10

client* gClient;

client::client() {
	sendList = newQueue();
	sendListFile = newQueue();
	receiveList = newQueue();
	socketFd = -1;
	serverStat = 0;
}

void console()
{
	char consoleBuf[100] = {};
	do{
		system("clear");
		memset(consoleBuf,0x00,100);
		char s1[] = "Linking..";
		char s2[] = "Ready";
		char* serverStatCur;
		gClient->serverStat== 0?serverStatCur=s1:serverStatCur=s2;
		_DEBUG("server is %s %d",serverStatCur,gClient->serverStat);
		_DEBUG("cur packages count in queue is %d",gClient->sendList->lenth);
		_DEBUG("====================================================");
		_DEBUG("F\t--\tSend File");
		_DEBUG("M\t--\tmanua input data");
		_DEBUG("S\t--\tsend the queue data to server");
		_DEBUG("Q\t--\tquit out the client");
		_DEBUG("====================================================");
		scanf("%s",consoleBuf);
	}while(!(1 == strlen(consoleBuf) &&( consoleBuf [0] == 'F' || consoleBuf [0] == 'f' ||consoleBuf [0] == 'M' ||consoleBuf [0] == 'm' ||consoleBuf [0] == 'S' ||consoleBuf [0] == 's' ||consoleBuf [0] == 'Q' ||consoleBuf [0] == 'q')));

	if('F' == consoleBuf[0] || 'f' == consoleBuf[0])
	{
		char consoleFileDirBuf[150] = {};
		do{
			system("clear");
			_DEBUG("please input the dir");
			scanf("%s",consoleFileDirBuf);
			_DEBUG("consoleFileDirBuf=%s",consoleFileDirBuf);
			if(access(consoleFileDirBuf,F_OK)==0)//Need to judge authority
			{
				int res = -1;
				do{
					res = gClient->readDataFromFile(consoleFileDirBuf);
					if(0 == res)
					{
						break;
					}
				}while(1);
				_DEBUG("file load success");
				break;
			}
			else
			{
				_DEBUG("file does not exist");
				_DEBUG("B -- back other -- reinput");
				memset(consoleBuf,0x00,100);
				scanf("%s",consoleBuf);
				if(1 == strlen(consoleBuf)&&( consoleBuf [0] == 'B' || consoleBuf [0] == 'b' ))
				{
							break;
				}
			}
		sleep(3);
		}while(1);

	}
	else if('M' == consoleBuf[0] || 'm' == consoleBuf[0])
	{
		char consoleDataBuf[150] = {};
		do{
			system("clear");
			_DEBUG("please input the data");
			scanf("%s",consoleDataBuf);
			_DEBUG("input :%s",consoleDataBuf);
			_DEBUG("C -- save and continue; Y-- save and break; B -- back ; Other -- reinput;");
			memset(consoleBuf,0x00,100);
			scanf("%s",consoleBuf);
			if(1 == strlen(consoleBuf)&&(consoleBuf [0] == 'Y' || consoleBuf [0] == 'y'||consoleBuf [0] == 'c' ||consoleBuf [0] == 'C'))
			{
				gClient->Pack(consoleDataBuf,strlen(consoleDataBuf),CMD_STRING,NULL,0,0);
			}
			if(1 == strlen(consoleBuf)&&(consoleBuf [0] == 'B' ||consoleBuf [0] == 'b'||consoleBuf [0] == 'Y' ||consoleBuf [0] == 'y'))
			{
				break;
			}
		}while(1);
	}
	else if('S' == consoleBuf[0] || 's' == consoleBuf[0])
	{
		gClient->startSend();
	}
	else if('Q' == consoleBuf[0] || 'q' == consoleBuf[0])
	{
		_DEBUG("exit!");
		gClient->logout();
		exit(0);
	}
	sleep(1);
}

void* sendThread(void* para)
{
	if(NULL == gClient)
	{
		_DEBUG("please init your client before you send data");
	}
	while(gClient->sendList->lenth)
	{
		int res = -1;
		do{
			res = gClient->send();
			if(res ==-1)
			{
				_DEBUG("relink");
				gClient->reconnectServer();
			}
			else
			{
				_DEBUG("send data success %d",gClient->sendList->lenth);
				break;
			}
		}while(res == -1);
		DEBUG("pckg done");
	}
	return NULL;
}

void* client::CheckServerThread(void* para)
{
	client* curClient = (client*)para;
	while(1)
	{
		gClient->Pack(NULL,0,CMD_HEARTBEAT,NULL,0,0);
		sleep(HEARTBEAT_INTERVAL);
	}
	return NULL;
}


void* client::receiveThread(void* para)
{
	client* curClient = (client*)para;
	if(NULL == gClient)
	{
		_DEBUG("Please Init Your Client Before You Receive Data");
		return NULL;
	}

	while(1)
	{
		char acBuf[100] ={};
		int res = read(curClient->socketFd,acBuf,100);
		if(res == -1)
		{
			//DEBUG("Read Failed");
			//perror("read:");
			//gClient->reconnectServer();
		}
		else if(res>0)
		{
			_DEBUG("read = %s",acBuf);
		}
	}
	return NULL;
}

client::~client() {
}

int client::reconnectServer()
{
	do{
		if(-1 != connectServer())
		{
			//DEBUG("Connect Success")
			break;
		}
		else
		{
			//ERROR("Connect Error");
		}
		sleep(1);
	}while(1);
	return 0;
}

int client::connectServer()
{
	if( -1 != socketFd)
	{
		close(socketFd);
		socketFd = -1;
	}
	socketFd = socket(PF_INET,SOCK_STREAM,0);
	if(-1 == socketFd)
	{
		ERROR("Socket Error");
		return -1;
	}
	struct sockaddr_in addr;
	addr.sin_family = PF_INET;
	addr.sin_port = htons(SERVER_PORT);
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);

	if(-1 == connect(socketFd,(struct sockaddr*)&addr,sizeof(addr)))
	{
		return -1;
	}
	return 0;
}

int client::send()
{
	if(NULL == sendList && 0 == sendList->lenth)
	{
		_DEBUG("no data in send list");
		return 0;
	}
	if(sendList->lenth)
	{
		NODE* data =  quitQueue(sendList);
		_DEBUG("data len=%u %d",data->length,strlen(data->data));
		int res = write(socketFd,data->data,data->length);
		if(-1 == res)
		{
			serverStat = 0;
			rollBackQuitQueue(data,sendList);
			DEBUG("send error roll back");
			return -1;
		}
		return 0;
	}
	_DEBUG("send list length can't less than zero");
	return -1;
}
void client::sendImmediately(char* data,int dataLen)
{
	int res = -1;
	do{
		res = write(socketFd,data,dataLen);
		if(res != -1)
		{
			serverStat = 1;
			break;
		}
		else
		{
			serverStat = 0;
			reconnectServer();
		}
		sleep(1);
	}while(1);
}

int client::receiveData()
{
	return 0;
}

int client::logout()
{
	return Pack(NULL,0,CMD_DISCONNECT,NULL,0,0);
}

int client::Pack(char* stuffing,int len,int cmd,string* name,int total =0,int serialNo = 0)
{
	int headlen = sizeof(PACKAGE_HEAD);
	int taillen = sizeof(PACKAGE_TAIL);
	int datalen = len;
	int packagelen = headlen + datalen + taillen;

	char* package = (char*)calloc(sizeof(char),packagelen);
	if(NULL == package)
	{
		ERROR("calloc failed");
		return -1;
	}

	PACKAGE_HEAD head;
	memset(&head,1,sizeof(head));
	memcpy((head.VerificationCode),"abcd",sizeof("abcd"));
	head.PackageType = cmd;
	head.DataLen = datalen;
	head.SerialNo = serialNo;
	head.PckgTotal = total;
	if(NULL!=name&&name->length())
	{
		memcpy(head.FileName,name->c_str(),name->length());
	}else
	{
		memcpy(head.FileName,"abcdefghijklmnopqrst",sizeof("abcdefghijklmnopqrst"));
	}

	PACKAGE_TAIL tail;
	memcpy((tail.VerificationCode),"dcba",sizeof("abcd"));
	tail.SerialNo = head.SerialNo ;

	memcpy(package,&(head),headlen);
	memcpy(package+headlen,stuffing,datalen);
	memcpy(package+headlen+datalen,&(tail),taillen);


	if(cmd != CMD_DISCONNECT && cmd!=CMD_HEARTBEAT&&cmd!=CMD_FILE)
	{
		dataInsertQueue(sendList,package,packagelen);;
	}
	else if(cmd==CMD_FILE)
	{
		dataInsertQueue(sendListFile,package,packagelen);
	}
	else//Data which need to be sent immediately.
	{
		sendImmediately(package,packagelen);
	}//
	return 0;
}

void client::startSend()
{
	int result = -1;
	do{
		pthread_t sendThreadId;
		result=pthread_create(&sendThreadId,NULL,sendThread, NULL);
		if(result != 0)
		{
			ERROR("create  send thread failed");
			sleep(1);
		}
	}while(result != 0);
	_DEBUG("send thread ok");
}

void* ConsoleThread(void* para)
{
	while(1)
	{
		console();
		sleep(1);
	}
	return NULL;
}

int client::start()
{
	reconnectServer();
	int result = 0;
	pthread_t receiveThreadId,CheckServerThreadId,consoleThreadId;
	sigset_t signal_mask;
	sigemptyset(&signal_mask);
	sigaddset(&signal_mask,SIGPIPE);
	int rc = pthread_sigmask(SIG_BLOCK,&signal_mask,NULL);
	if(rc != 0)
	{
		ERROR("Block Sigpipe Error");
	}
	result=pthread_create(&receiveThreadId,NULL,receiveThread,this);
	if(result != 0)
	{
		ERROR("Create Receive Thread Failed");
	}
	result=pthread_create(&CheckServerThreadId,NULL,CheckServerThread,this);
	if(result != 0)
	{
		ERROR("Create Check Server Thread Failed");
	}
	result = pthread_create(&consoleThreadId,NULL,ConsoleThread,NULL);
	if(result!=0)
	{
		ERROR("Create Check Console Thread Failed");
	}
	pthread_join(CheckServerThreadId,NULL);
	pthread_join(receiveThreadId,NULL);
	pthread_join(consoleThreadId,NULL);
	return 0;
}

int client::readDataFromFile(char* dataPath)
{
	FILE* SourceFile = fopen(dataPath,"a+b");
	if(NULL == SourceFile)
	{
		ERROR("Open File Failed");
		return -1;
	}

	string fileName;
	unsigned long fileSize = get_file_size(dataPath);
	get_file_name(dataPath,fileName);
	int packcnt = fileSize/DATA_BUF_LEN +1;
	int serialNo = 1;

	while(1)
	{
		char readDataBuffer[DATA_BUF_LEN] = {};
		int readDataLen = fread(&readDataBuffer,1,DATA_BUF_LEN,SourceFile);
		if(readDataLen<=0)
		{
			break;
		}
		Pack(readDataBuffer,readDataLen,CMD_FILE,&fileName,packcnt,serialNo++);
	}
	_DEBUG("Data Is Ready");
	fclose(SourceFile);
	return 0 ;
}

int  main()
{
	gClient = new client;
	gClient->start();
}

