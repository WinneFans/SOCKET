#include "queue.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"

NODE * cteateNode(void* data,unsigned int length)
{
	if(NULL == data && 0 == length)
	{
		return NULL;
	}
	NODE * newNode = (NODE *)malloc(sizeof(NODE));
	if(newNode)
	{
		newNode->data = (char*)calloc(sizeof(char),length+1);
		if(!newNode->data)
		{
			free(newNode);
			return NULL;
		}
		memcpy(newNode->data,data,length);
		newNode->length = length;
		newNode->next = NULL;
	}
	return newNode;
}

int destoryNode(NODE* abandonedNode)
{
	if(NULL == abandonedNode)
	{
		return -1;
	}
	if(NULL == abandonedNode->data)
	{
		free(abandonedNode->data); 
	}
	if(NULL == abandonedNode)
	{
		free(abandonedNode);
	}
	return 0;
}

int insertQueue(QUEUE* queue,NODE* newNode)
{
	if(NULL == queue || NULL == newNode)
	{
		return -1;
	}
	if(NULL == queue->head)
	{
		queue->head = newNode;
		queue->tail = newNode;
		queue->tail->next = NULL;
		queue->lenth = 1;
		return 0;
	}	

	else if(NULL != queue->tail)
	{
		queue->tail->next = newNode;
		queue->tail = newNode;
		queue->tail->next = NULL;
		queue->lenth ++ ;
		return 0;
	}
	return -1;	
}


NODE * quitQueue(QUEUE* queue)
{
	if(NULL == queue)
	{
		return NULL;
	}
	NODE* quitNote = queue->head;
	queue->head = queue->head->next;
	queue->lenth --;
	return quitNote;
}

int	rollBackQuitQueue(NODE* node,QUEUE* queue)
{
	if(NULL!= node && NULL != queue)
	{
		node->next = queue->head;
		queue->head = node;
		queue->lenth ++;
		return 0;
	}
	return -1;
}

int initQueue(QUEUE* queue)
{
	if(NULL == queue)
	{
		return -1;
	}
	queue->head = NULL;
	queue->tail = NULL;
	queue->lenth = 0;
	return 0;
}

QUEUE* newQueue()
{
	QUEUE* queue = (QUEUE*)calloc(sizeof(QUEUE),1);
	if(queue)
	{
		int res = initQueue(queue);
		if(0 == res)
		{
			return queue;
		}
	}
	return NULL;
}

int dataInsertQueue(QUEUE* queue,char* data,unsigned int lenth)
{
	if(NULL == queue)
	{
		printf("please create a queue fist\n");
		return - 1;
	}
	NODE* newNode = cteateNode(data,lenth);
	if(newNode)
	{
		int res =insertQueue(queue,newNode);
		return res;
	}
	return -1;
}


char* dataQuitQueue1(QUEUE* queue)
{
	if(NULL == queue)
	{
		return NULL;
	}
	NODE* quitNode =  quitQueue(queue);
	if(quitNode->length<=0)
	{
		return NULL;
	}
	char* dataOut = (char*)calloc(quitNode->length+1,1);
	if(NULL == dataOut)
	{
		return NULL;
	}
	memcpy(dataOut,quitNode->data,quitNode->length);
	dataOut[quitNode->length] = '\0';
	destoryNode(quitNode);
	return dataOut;
}

//demo:char* data = dataQuitQueue(queue);
char* dataQuitQueue(QUEUE* queue)
{
	if(queue)
	{
		NODE* quitNode =  quitQueue(queue);
		if(quitNode)
		{
			char* dataOut = (char*)calloc(quitNode->length+1,1);
			if(dataOut)
			{
				memcpy(dataOut,quitNode->data,quitNode->length);
				dataOut[quitNode->length] = '\0';
			}
			destoryNode(quitNode);
			return dataOut;
		}
	}
	return NULL;
}

unsigned int getQueueLen(QUEUE* queue)
{
	if(queue)
	{
		return queue->lenth;
	}
	return 0;
}

void dataReadNode(char* readBuffer,int readLen,NODE* curNode)
{
	if(NULL == readBuffer || readLen < 1 || NULL == curNode)
	{
		return;
	}
	int copyLen = 0;
	copyLen=readLen>curNode->length?curNode->length:readLen;
	printf("copyLen = %d\n",copyLen);
	memcpy(readBuffer,curNode->data,copyLen);
	if(readLen>curNode->length)
	{
		dataReadNode(readBuffer+curNode->length,readLen-curNode->length,curNode->next);
	}
	return;
}

char* dataReadQueue(QUEUE* queue,int readPos,int readLen)
{
	if(NULL == queue || queue->lenth <1 || readPos<0 || readLen <1)
	{
		return NULL;
	}
	char* readBuffer = (char*)calloc(sizeof(char),readLen);
	if(NULL == readBuffer)
	{
		return NULL;
	}
	NODE* curNode = queue->head;
	int curNodeLen = curNode->length;
	int nodeLenCnt = curNode->length;
	int remReadLen = readLen;
	//reach
	while(nodeLenCnt<readPos && NULL!=curNode->next)
	{
		curNode = curNode->next;
		nodeLenCnt += curNode->length;
		curNodeLen = curNode->length;
	}
	if(nodeLenCnt<readPos)
	{
		return NULL;
	}
	//copy
	char* pStart =  curNode->data;
	pStart+= (readPos - (nodeLenCnt - curNodeLen));
	int remCurNodeLen = nodeLenCnt - readPos;

	if(nodeLenCnt>=readPos+readLen)
	{
		memcpy(readBuffer,pStart,readLen);
	}
	else//need to use the next few nodes
	{
		memcpy(readBuffer,pStart,remCurNodeLen);
		remReadLen -= remCurNodeLen;
		printf("remReadLen=%d\n",remReadLen);
		 dataReadNode(readBuffer+remCurNodeLen,remReadLen,curNode->next);
	}
	return readBuffer;
}

/*
int main()
{
	FILE* fd = fopen("./t1.txt","a+b");
	QUEUE* queue = newQueue();
	
	while(1)
	{
		char buf[10];
		int n = fread(buf,1,10,fd);
		if(n<=0)
			break;
		else
			dataInsertQueue(queue,buf,n);
		printf("queue->lenth=%d\n",queue->lenth);
	}
	fclose(fd);
	printf("queue->lenth=%d\n",queue->lenth);

	FILE* fd2 =fopen("./t2.txt","a+b");
	while(queue->lenth)
	{
		char* buf= dataQuitQueue(queue);
		fwrite(buf,1,strlen(buf),fd);
		free(buf);
	}
	fclose(fd2);


	QUEUE* queue = newQueue();
	char buf1[] = "abcdefghijklmnopqrstuvwxyz";
	char buf2[] = "0123456789";
	char buf3[] = "1qazXSW@";
	char buf4[] = "3edcVFR$";
	char buf5[] = "abcdefghijklmnopqrstuvwxyz";
	char buf6[] = "0123456789";
	char buf7[] = "1qazXSW@";
	char buf8[] = "3edcVFR$";
	dataInsertQueue(queue,buf1,strlen(buf1));
	dataInsertQueue(queue,buf2,strlen(buf2));
	dataInsertQueue(queue,buf3,strlen(buf3));
	dataInsertQueue(queue,buf4,strlen(buf4));
	dataInsertQueue(queue,buf5,strlen(buf5));
		dataInsertQueue(queue,buf6,strlen(buf6));
		dataInsertQueue(queue,buf7,strlen(buf7));
		dataInsertQueue(queue,buf8,strlen(buf8));
	char* buf = dataReadQueue(queue,0,75);
	printf("buf=%s\n",buf);
	return 0;
}*/

