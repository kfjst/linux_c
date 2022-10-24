#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "proto.h"


int main()
{
	key_t key;
	int msgid;
	struct msg_st rbuf;

	key = ftok(KEYPATH, KEYPROJ);
	if(key < 0)
	{
		perror("ftok()");
		exit(1);
	}

	//rcver.c 已经创建了，这里不需要重复创建，直接获取msgid
	msgid = msgget(key, 0);
	if(msgid < 0)
	{
		perror("msgget()");
		exit(1);
	}
	
	rbuf.mtype = 1;
	strcpy(rbuf.name, "alan");
	rbuf.math = rand()%100;
	rbuf.chinese = rand()%100;
	if(msgsnd(msgid, &rbuf, sizeof(rbuf) - sizeof(long), 0) < 0) // len sizeof(rbug) - sizeof((long), 接受实际数据
	{
		perror("msgsnd()");
		exit(1);
	}
	
	puts("ok");
	//msgctl(); 谁创建谁销毁，在recv中销毁

	exit(0);
}


