/*
 * client.h
 *
 *  Created on: 2016年6月12日
 *      Author: fansiyuan
 */

#include "queue.h"
#include "common.h"
#ifndef CLIENT_H_
#define CLIENT_H_

class client {
public:
	QUEUE* sendList;
	QUEUE* sendListFile;
	QUEUE* receiveList;
	static	void*	receiveThread(void* para);
	static void* CheckServerThread(void* para);
	int socketFd;
	int serverStat;
	int readDataFromFile(char* dataPath);
	void sendImmediately(char* data,int dataLen);
	int send();
	int login();
	int start();
	int logout();
	void startSend();
	int reconnectServer();
	int connectServer();
	int disconnectServer();
	int receiveData();
	client();
	virtual ~client();


	int Pack(char*,int,int,string*,int,int);
private:

};
#endif /* CLIENT_H_ */
