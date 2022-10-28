#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

#define BUFSIZE 1024

int main(int argc, char **argv)
{
	int sd;
	struct sockaddr_in raddr;
	FILE *fp; 
	int len;
	char rbuf[BUFSIZE];

	if(argc < 2)
	{
		fprintf(stderr, "Usage: ./Client ip\n");
		exit(1);
	}

	sd = socket(AF_INET, SOCK_STREAM, 0/* IPPROTO_TCP, IPPROTO_SCTP */);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

//bind();

	raddr.sin_family = AF_INET;
	raddr.sin_port = htons(80);
	inet_pton(AF_INET, argv[1], &raddr.sin_addr);
	if(connect(sd, (void *)&raddr, sizeof(raddr)) < 0)
	{
		perror("connect()");
		exit(1);
	}
	
	//没有用recv实现，一切皆文件的设计原理， 可以用文件io打开，操作
	fp = fdopen(sd, "r+");
	if(fp == NULL)
	{
		perror("fdopen()");
		exit(1);
	}
	
	fprintf(fp, "Get /test.jpg\r\n\r\n");
	fflush(fp);
	
	while(1)
	{
		len = fread(rbuf, 1, BUFSIZE, fp);
		if(len <= 0)
			break;
		
		fwrite(rbuf, 1, len, stdout);
	}

	fclose(fp);
	exit(0);
}

