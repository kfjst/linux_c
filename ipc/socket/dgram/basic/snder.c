#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#include "proto.h"

#define IPSTRSIZE 16

int main(int argc, char **argv)
{
	int sd;
	struct sockaddr_in ipaddr;
	struct msg_st rbuf; 
	
	if(argc < 2)
	{
		fprintf(stderr, "Usage: ./xxx ip\n");
		exit(1);
	}

	sd = socket(AF_INET, SOCK_DGRAM, 0 /* IPPROTO_UDP */);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

#if 0
	ipaddr.sin_family = AF_INET;
	ipaddr.sin_port = htons(atoi(RCVPORT));
	inet_pton(AF_INET, "0.0.0.0", &ipaddr.sin_addr);
	if(bind(sd, (void *)&ipaddr, sizeof(ipaddr)) < 0)
	{
		perror("bind()");
		exit(1);
	}
#endif
	memset(&rbuf, 0, sizeof(rbuf));
 	strcpy(rbuf.name, "Alen");
	rbuf.chinese = htonl(rand()%100);
	rbuf.math = htonl(rand()%100);
	
	ipaddr.sin_family = AF_INET;
	ipaddr.sin_port = htons(atoi(RCVPORT));
	inet_pton(AF_INET, argv[1], &ipaddr.sin_addr);
	if(sendto(sd, &rbuf, sizeof(rbuf), 0, (void *)&ipaddr, sizeof(ipaddr)) < 0)
	{
		perror("sendto");
		exit(1);
	}

	puts("OK");

	close(sd);

	exit(0);
}
