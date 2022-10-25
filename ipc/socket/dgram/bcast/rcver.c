#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "proto.h"

#define IPSTRSIZE 16

int main()
{
	int sd;
	struct sockaddr_in ipaddr;
	struct msg_st *rbufp;
	int size;
	int ipaddlen;
	char ipstr[IPSTRSIZE];

	sd = socket(AF_INET, SOCK_DGRAM, 0 /* IPPROTO_UDP */);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}
	
	int val = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_BROADCAST, &val, sizeof(val)) < 0)
	{
		perror("setsockopt()");
		exit(1);
	}

	ipaddr.sin_family = AF_INET;
	ipaddr.sin_port = htons(atoi(RCVPORT));
	inet_pton(AF_INET, "0.0.0.0", &ipaddr.sin_addr);
	if(bind(sd, (void *)&ipaddr, sizeof(ipaddr)) < 0)
	{
		perror("bind()");
		exit(1);
	}

	size = sizeof(struct msg_st) + NAMESIZE - 1;
	rbufp = malloc(size);
	if(rbufp == NULL)
	{
		perror("malloc");
		exit(1);
	}

	ipaddlen = sizeof(ipaddr);
	while(1)
	{
		recvfrom(sd, rbufp, size, 0, (void *)&ipaddr, &ipaddlen);
		
		inet_ntop(AF_INET, &ipaddr.sin_addr, ipstr, IPSTRSIZE);
		printf("---MESSAGE FROM %s :%d---\n", ipstr, ntohs(ipaddr.sin_port));
		printf("NAME = %s\n", rbufp->name);
		printf("MATH = %d\n", ntohl(rbufp->math));
		printf("CHINESE = %d\n", ntohl(rbufp->chinese));
	}

	close(sd);

	exit(0);
}
