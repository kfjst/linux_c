#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>

#include "proto.h"

#define IPSTRSIZE 16

int main(int argc, char **argv)
{
	int sd;
	struct sockaddr_in ipaddr;
	struct msg_st *sbufp; 
	int size;

	if(argc < 2)
	{
		fprintf(stderr, "Usage: ./xxx name\n");
		exit(1);
	}

	sd = socket(AF_INET, SOCK_DGRAM, 0 /* IPPROTO_UDP */);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

	
	struct ip_mreqn mreq;
	inet_pton(AF_INET, MGROUP, &mreq.imr_multiaddr);
	inet_pton(AF_INET, "0.0.0.0", &mreq.imr_address);
	mreq.imr_ifindex = if_nametoindex("eth0");
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, &mreq, sizeof(mreq)) < 0)
	{
		perror("setsockopt()");
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
	size = sizeof(struct msg_st) + strlen(argv[1]);
	sbufp = malloc(size);
	if(sbufp == NULL)
	{
		perror("malloc()");
		exit(1);
	}

	memset(sbufp, 0, size);
 	strcpy(sbufp->name, argv[1]);
	sbufp->chinese = htonl(rand()%100);
	sbufp->math = htonl(rand()%100);
	
	ipaddr.sin_family = AF_INET;
	ipaddr.sin_port = htons(atoi(RCVPORT));
	inet_pton(AF_INET, MGROUP, &ipaddr.sin_addr);
	if(sendto(sd, sbufp, size, 0, (void *)&ipaddr, sizeof(ipaddr)) < 0)
	{
		perror("sendto");
		exit(1);
	}

	puts("OK");

	close(sd);

	exit(0);
}
