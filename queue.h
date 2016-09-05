#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct QUEUE_NODE{
	char* data;
	unsigned int length;
	struct QUEUE_NODE * next;
}NODE;

typedef struct QUEUE{
	NODE * head;
	NODE * tail;
	unsigned int lenth;
}QUEUE;

extern QUEUE* newQueue();
extern int dataInsertQueue(QUEUE* queue,char* data,unsigned int lenth);
extern char* dataQuitQueue(QUEUE* queue);
extern unsigned int getQueueLen(QUEUE* queue);
extern NODE * quitQueue(QUEUE* queue);
extern int rollBackQuitQueue(NODE* node,QUEUE* queue);
extern int destoryNode(NODE* abandonedNode);
extern void dataReadNode(char* readBuffer,int readLen,NODE* curNode);
extern char* dataReadQueue(QUEUE* queue,int readPos,int readLen);
#endif
