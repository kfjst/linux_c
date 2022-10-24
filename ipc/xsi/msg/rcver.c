
#include <stdio.h>
#include <stdlib.h>
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

	msgid = msgget(key, IPC_CREAT | 0600); //IPC_CREAT | 权限信息 (如: 0600)
	if(msgid < 0)
	{
		perror("msgget()");
		exit(1);
	}

	while(1)
	{
		if(msgrcv(msgid, &rbuf, sizeof(rbuf) - sizeof(long), 0, 0) < 0) // len sizeof(rbug) - sizeof((long), 接受实际数据
		{
			perror("msgrcv()");
			exit(1);
		}

		printf("name = %s\n", rbuf.name);
		printf("math = %d\n", rbuf.math);
		printf("chines = %d\n", rbuf.chinese);
	}
	
	msgctl(msgid, IPC_RMID, NULL);

	exit(0);
}
