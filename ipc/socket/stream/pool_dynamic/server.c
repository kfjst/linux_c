#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <sys/mman.h>
#include <time.h>

#include "proto.h"

#define BUFSIZE 50
#define IPSTRSIZE 20

#define MINSPARSERVE 5
#define MAXSPARSERVE 10
#define MAXCLIENT 20
#define SIG_NOTIFY  SIGUSR2 

enum
{
	STATE_IDLE = 0,
	STATE_BUSY,
};

struct server_st
{
	pid_t pid;
	int state;
	//int reuse; // 服务多少次数后重启，防止出现异常
};

static struct server_st *server_pool;
static int idle_count = 0, busy_count = 0;
static int sd;

static void usr2_handler(int p)
{
	return;
}

static void server_job(int slot);
static int add_1_server(void)
{
	pid_t pid;
	int solt;

	if(idle_count + busy_count >= MAXCLIENT)  // >= !!!!!
		return -1;
	
	for(solt = 0; solt < MAXCLIENT; solt++)
	{
		if(server_pool[solt].pid == -1)
		{
			break;
		}
	}
	server_pool[solt].state = STATE_IDLE;

	pid = fork();
	if(pid < 0)
	{
		perror("fork()");
		exit(1);
	}
	
	if(pid == 0)
	{
		
		server_job(solt);
		exit(0);
	}
	else
	{
		server_pool[solt].pid = pid;
		idle_count++;   //!!!
	}

	return 1;
}

static int del_1_server(void)
{
	int i;

	if(idle_count == 0)
		return -1;
	
	for(i = 0; i < MAXCLIENT; i++)
	{
		if(server_pool[i].pid != -1 && server_pool[i].state == STATE_IDLE)
		{
			kill(server_pool[i].pid, SIGTERM);
			server_pool[i].pid = -1;
			idle_count--;   // !!!!
			break;
		}
	}

	return 1;
}

static void server_job(int slot)
{
	int client_sd;
	struct sockaddr_in raddr;
	socklen_t raddr_len;
	//char ipstr[IPSTRSIZE];
	char buf[BUFSIZE];
	int len;
	int ppid;

	ppid = getppid();
	
	while(1)
	{
		server_pool[slot].state = STATE_IDLE;
		kill(ppid, SIG_NOTIFY);
	
//		puts("accept");
		raddr_len = sizeof(raddr);
		client_sd = accept(sd, (void *)&raddr, &raddr_len);
		if(client_sd < 0)
		{
			if(errno != EAGAIN || errno != EINTR)
			{
				perror("accept()");
				exit(1);
			}
		}
	
		server_pool[slot].state = STATE_BUSY;
		kill(ppid, SIG_NOTIFY);

		//inet_ntop(AF_INET, &raddr.sin_addr, ipstr, IPSTRSIZE);
		//printf("[%d] : client: %s: %d\n", getpid(), ipstr, ntohs(raddr.sin_port));
		
		len = sprintf(buf, FMT_STAMP, (long long)time(NULL)); 
		if(send(client_sd, buf, len, 0) < 0)
		{
			perror("send()");
			exit(1);
		}
		
		sleep(5);

		close(client_sd);
	}
}

static void scan_pool(void)
{
	int busy = 0, idle = 0;
	int i;
	for(i = 0; i < MAXCLIENT; i++)
	{
		if(server_pool[i].pid == -1)
			continue;
		if(kill(server_pool[i].pid, 0))
		{
			/* 查看进程是否存在 */
			server_pool[i].pid = -1;
			continue;
		}
		if(server_pool[i].state == STATE_IDLE)
			idle++;
		else if(server_pool[i].state == STATE_BUSY)
			busy++;
		else
		{
			fprintf(stderr,"err state\n");
			abort();
		}
	}

	idle_count = idle;
	busy_count = busy;
}

int main()
{
	struct sigaction sa, osa;
	sigset_t set, oset;

	// 子进程消亡后自行进行收尸
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_NOCLDWAIT; // 阻止进程进入僵尸态
	if(sigaction(SIGCHLD, &sa, &osa) < 0)
	{
		perror("sigaction()");
		exit(1);
	}

	sa.sa_handler = usr2_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0; // 阻止进程进入僵尸态
	if(sigaction(SIG_NOTIFY, &sa, &osa) < 0)
	{
		perror("sigaction()");
		exit(1);
	}

	sigemptyset(&set);
	sigaddset(&set, SIG_NOTIFY);
	if(sigprocmask(SIG_BLOCK, &set, &oset) < 0)
	{
		perror("sigprocmask()");
		exit(1);
	}

	server_pool = mmap(NULL, sizeof(struct server_st) * MAXCLIENT, PROT_WRITE | PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	if(server_pool == MAP_FAILED)
	{
		perror("mmap()");
		exit(1);
	}

	sd = socket(AF_INET, SOCK_STREAM, 0);
	if(sd < 0)
	{
		perror("socket()");
		exit(1);
	}

	int val = 1;
	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) < 0)
	{
		perror("setsockopt()");
		exit(1);
	}

	struct sockaddr_in laddr;
	laddr.sin_family = AF_INET;
	laddr.sin_port = htons(atoi(SERVERPORT));
	inet_pton(AF_INET, "0.0.0.0", &laddr.sin_addr);
	if(bind(sd, (void *)&laddr, sizeof(laddr)) < 0)
	{
		perror("bind()");
		exit(1);
	}

	if(listen(sd, 100) < 0)
	{
		perror("listen()");
		exit(1);
	}
	

	int i;
	
	for(i = 0; i < MAXCLIENT; i++)
	{
		server_pool[i].pid = -1;
		server_pool[i].state = STATE_IDLE;
	}

	for(i = 0; i < MINSPARSERVE; i++)
	{
		add_1_server();
	}

	while(1)
	{
//		puts("2");
		sigsuspend(&oset);
//		puts("3");
		scan_pool();	
//		printf("idle %d busy %d\n", idle_count, busy_count);
//		sleep(10);


		if(idle_count > MAXSPARSERVE)
		{
			//空闲池多，杀掉空闲server
			for(i = 0; i < (idle_count - MAXSPARSERVE); i++)
			{
				del_1_server();
			}
		}
		else if(idle_count < MINSPARSERVE)
		{
			//太少了，增加一些
			for(i = 0; i < (MINSPARSERVE - idle_count); i++)
			{
				add_1_server();
			}
		}

		for(i = 0; i < MAXCLIENT; i++)
		{
			if(server_pool[i].pid == -1)
			{
				putchar(' ');
			}
			else
			{
				if(server_pool[i].state == STATE_IDLE)
					putchar('.');
				else
					putchar('x');
			}
		}

		putchar('\n');
	}	
	
	sigprocmask(SIG_SETMASK, &oset, NULL);
	
	for(i = 0; i < MAXCLIENT; i++)
	{
		if(server_pool[i].pid != -1)
			wait(NULL);
	}

	exit(0);
}
