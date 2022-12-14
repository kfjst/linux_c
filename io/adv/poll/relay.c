#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <poll.h>

#define TTY1 "/dev/pts/0"
#define TTY2 "/dev/pts/1"

#define BUFSIZE 1024

enum
{
	STATE_R = 1,
	STATE_W,
	STATE_AUTO,
	STATE_Ex,
	STATE_T
}E_FsmState;

struct fsm_st
{
	int state;
	int sfd;
	int dfd;
	int len;
	int pos;
	char buf[BUFSIZE];
	char *errstr;
};

static int fsm_driver(struct fsm_st *fsm)
{
	int ret;

	if(fsm == NULL)
		return -1;

	switch(fsm->state)
	{
		case STATE_R:
		{
			fsm->len = read(fsm->sfd, fsm->buf, BUFSIZE);
			if(fsm->len == 0)
			{
				fsm->state = STATE_T;
			}
			else if(fsm->len < 0)
			{
				if(errno != EAGAIN)
				{
					fsm->errstr = "read()";
					fsm->state = STATE_Ex;
				}
				else
				{
					fsm->state = STATE_R;
				}

			}
			else
			{
				fsm->pos = 0;
				fsm->state = STATE_W;
			}
			break;
		}
		case STATE_W:
		{
			ret = write(fsm->dfd, &fsm->buf[fsm->pos], fsm->len);
			if(ret < 0)
			{
				if(errno != EAGAIN)
				{
					fsm->errstr = "write()";
					fsm->state = STATE_Ex;
				}
				else
				{
					fsm->state = STATE_W;
				}
			}
			else
			{
				fsm->pos += ret;
				fsm->len -= ret;
				if(fsm->len == 0)
				{
					fsm->state = STATE_R;
				}
				else
				{
					fsm->state = STATE_W;
				}
			}

			break;
		}
		case STATE_Ex:
		{
			perror(fsm->errstr);
			fsm->state = STATE_T;
			break;
		}
		case STATE_T:
		{
			/* do sth */
			break;
		}
		default:
			abort();
			break;
	}

	return 0;
}

static int relay(int fd1, int fd2)
{
	int fd1_save, fd2_save;
	struct fsm_st fsm12, fsm21;	
	struct pollfd fds[2];

	if(fd1 < 0 || fd2 < 0)
	{
		return -1;
	}
	
	fd1_save = fcntl(fd1, F_GETFL);
	fcntl(fd1, F_SETFL, fd1_save | O_NONBLOCK);

	fd2_save = fcntl(fd2, F_GETFL);
	fcntl(fd2, F_SETFL, fd2_save | O_NONBLOCK);

	fsm12.state = STATE_R;
	fsm12.len = 0;
	fsm12.sfd = fd1;
	fsm12.dfd = fd2;

	fsm21.state = STATE_R;
	fsm21.len = 0;
	fsm21.sfd = fd2;
	fsm21.dfd = fd1;

	fds[0].fd = fd1;
	fds[1].fd = fd2;

	while(fsm12.state != STATE_T || fsm21.state != STATE_T)
	{
		//??????????????????
		fds[0].events = 0;
		if(fsm12.state == STATE_R)
			fds[0].events |= POLLIN;
		if(fsm21.state == STATE_W)
			fds[0].events |= POLLOUT;

		fds[1].events = 0;
		if(fsm21.state == STATE_R)
			fds[1].events |= POLLIN;
		if(fsm12.state == STATE_W)
			fds[1].events |= POLLOUT;

		//??????
		if(fsm12.state < STATE_AUTO || fsm21.state < STATE_AUTO)
		{
			while(poll(fds, 2, -1) < 0)
			{
				if(errno == EINTR)
					continue;
				perror("poll()");
				exit(1);
			}
		}
			
		//??????????????????
		if(fsm12.state > STATE_AUTO
		|| (fds[0].revents & POLLIN)
		|| (fds[1].revents & POLLOUT))
		{
				fsm_driver(&fsm12);
		}

		if(fsm21.state > STATE_AUTO
		|| (fds[1].revents & POLLIN)
		|| (fds[0].revents & POLLOUT))
		{
			fsm_driver(&fsm21);
		}
	}

	fcntl(fd1, F_SETFL, fd1_save);
	fcntl(fd2, F_SETFL, fd2_save);
	
	return 0;
}


int main()
{
	int fd1, fd2;
	
	fd1 = open(TTY1,O_RDWR);
	if(fd1 < 0)
	{
		perror("open(TTY1):");
		exit(1);
	}
	 
	write(fd1, "TTY1\n", 5);

	fd2 = open(TTY2,O_RDWR);
	if(fd2 < 0)
	{
		perror("open(TTY2):");
		exit(1);
	}

	
	write(fd2, "TTY2\n", 5);
	relay(fd1, fd2);

	close(fd1);
	close(fd2);

	exit(0);
}
