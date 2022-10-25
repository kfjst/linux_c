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
	struct msg_st *sbufp; 
	int size;

	if(argc < 3)
	{
		fprintf(stderr, "Usage: ./xxx ip name\n");
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
	size = sizeof(struct msg_st) + strlen(argv[2]);
	sbufp = malloc(size);
	if(sbufp == NULL)
	{
		perror("malloc()");
		exit(1);
	}

	memset(sbufp, 0, size);
 	strcpy(sbufp->name, argv[2]);
	sbufp->chinese = htonl(rand()%100);
	sbufp->math = htonl(rand()%100);
	
	ipaddr.sin_family = AF_INET;
	ipaddr.sin_port = htons(atoi(RCVPORT));
	inet_pton(AF_INET, argv[1], &ipaddr.sin_addr);
	if(sendto(sd, sbufp, size, 0, (void *)&ipaddr, sizeof(ipaddr)) < 0)
	{
		perror("sendto");
		exit(1);
	}

	puts("OK");

	close(sd);

	exit(0);
}
