#ifndef _COMM_H_
#define _COMM_H_ 

#include <sys/socket.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/types.h>
 #include <sys/stat.h>
#include <arpa/inet.h>
#include <string.h>//memset
#include <stdlib.h>//exit
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;

#define NONE			"\033[m"
#define RED				"\033[0;32;31m"
#define GREEN			"\033[0;32;32m"
#define BLUE			"\033[0;32;34m"
#define DARY_GRAY		"\033[1;30m"
#define CYAN			"\033[0;36m"
#define PURPLE			"\033[0;35m"
#define BROWN			"\033[0;33m"
#define YELLOW			"\033[1;33m"
#define WHITE			"\033[1;37m"
#define LIGHT_RED		"\033[1;31m"
#define LIGHT_GREEN		"\033[1;32m"
#define LIGHT_BLUE		"\033[1;34m"
#define LIGHT_CYAN		"\033[1;36m"
#define LIGHT_PURPLE	"\033[1;35m"
#define LIGHT_GRAY		"\033[0;37m"

#define ERROR(fmt,...) do{printf(RED);printf("[%s:%d]",__FILE__,__LINE__);printf(fmt,##__VA_ARGS__);printf(NONE);printf("\r\n");}while(0);
#define DEBUG(fmt,...) do{printf(CYAN);printf("[%s:%d]",__FILE__,__LINE__);printf(fmt,##__VA_ARGS__);printf(NONE);printf("\r\n");}while(0);
#define _DEBUG(fmt,...) do{printf(LIGHT_CYAN);printf(fmt,##__VA_ARGS__);printf(NONE);printf("\r\n");}while(0);

#define SERVER_IP		"192.168.32.151"//"172.16.82.24"//
#define SERVER_PORT	2014
#define MAX_LINS_CNT	4
#define MAX_SECS_WAIT	5
#define READ_BUFF_LEN	300
#define SHARE_BUFF_LEN	1000
#define SHARE_RECV_BUFF_ID	102
#define SHARE_SEND_BUFF_ID	103
#define FILE_NAME_LEN	20
#define PACKAGE_DATA_LEN 10

extern unsigned long get_file_size(const char *path);
extern int get_file_name(const char *path,string& name);

//package type
enum{
	CMD_STRING = 0,
	CMD_STRUCT,
	CMD_FILE,
	CMD_DISCONNECT,
	CMD_HEARTBEAT,
	CMD_CNT,
};

//endian
enum{
	ENDIAN_BIG = 0,
	ENDIAN_LITTLE,
};

//package head
typedef struct PACKAGEHEAD {
	char VerificationCode[4];//0xff 0xf0 0x00 0x0f
	int PackageType;
	int DataLen;
	int SerialNo;
	int PckgTotal;
	char FileName[FILE_NAME_LEN];
}PACKAGE_HEAD;

//package tail
typedef struct PACKAGETAIL{
	unsigned char VerificationCode[4];//0x00 0x0e 0xee 0xe0
	unsigned char SerialNo;
}PACKAGE_TAIL;
#endif
