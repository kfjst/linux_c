#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

#define FNAME "/tmp/out"

static int daemonize(void)
{
	pid_t pid;
	int fd;
	
	pid = fork();
	if(pid < 0)
		return -1;

	if(pid > 0) //parent
	{
		exit(0); 
	}
	
	//child
	fd = open("/dev/null", O_RDWR);
	if(fd < 0)
		return -1;
	
	dup2(fd, 0);
	dup2(fd, 1);
	dup2(fd, 2);
	if(fd > 2)
	{
		close(fd);
	}
	
	setsid();

	chdir("/"); //注意FNAME路径

	//umask(0);
	return 0;
}

/*
	root: cat /var/log/syslog
	syslog函数会自己控制输出格式，不用添加换行符
	
*/

int main()
{
	FILE *fp;
	int i = 0;

	openlog("mydaemon_log", LOG_PID, LOG_DAEMON);

	if(daemonize())
	{
		syslog(LOG_ERR, "Dameonize() faild!");
		exit(1);
	}
	else
	{
		syslog(LOG_INFO, "Daemonize() success!");
	}

	fp = fopen(FNAME, "w");
	if(fp == NULL)
	{
		syslog(LOG_ERR, "fopen():%s", strerror(errno));
		exit(1);
	}
	
	syslog(LOG_INFO, "%s was opened.",FNAME);
	for(i = 0; ; i++)
	{
		fprintf(fp, "%d\n", i);

		fflush(fp); //!!!
		syslog(LOG_DEBUG, "%d is printed.", i);
		sleep(1);
	}
	
	fclose(fp);
	exit(1);
}

