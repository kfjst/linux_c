#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>
#include "relayer.h"
#include <string.h>

#define BUFSIZE 1024

enum
{
	STATE_R = 1,
	STATE_W,
	STATE_Ex,
	STATE_T
}E_FsmState;

struct rel_fsm_st
{
	int state;
	int sfd;
	int dfd;
	int len;
	int pos;
	char buf[BUFSIZE];
	char *errstr;
};

struct rel_job_st
{
	int id;
	int job_state;
	int fd1;
	int fd2;
	struct rel_fsm_st fsm12, fsm21;
	int fd1_save, fd2_save;
//strcut timerval start,end;
};

static struct rel_job_st* rel_job[REL_JOBMAX];
static pthread_mutex_t job_mut = PTHREAD_MUTEX_INITIALIZER;
static pthread_t rel_tid;

static int fsm_driver(struct rel_fsm_st *fsm)
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

static int get_free_pos_unlocked(void)
{
	int i;
	for(i = 0; i < REL_JOBMAX; i++)
	{
		if(rel_job[i] == NULL)
		{
			return i;
		}
	}

	return -1;
}

static void *thr_relay(void *p)
{
	int i;

	while(1)
	{
		pthread_mutex_lock(&job_mut);
		
		for(i = 0; i < REL_JOBMAX; i++)
		{
			if(rel_job[i] != NULL)
			{
				if(rel_job[i]->job_state == STATE_RUNNING)
				{
					fsm_driver(&rel_job[i]->fsm12);
					fsm_driver(&rel_job[i]->fsm21);
					if(rel_job[i]->fsm12.state == STATE_T \
					&& rel_job[i]->fsm21.state == STATE_T)
					{
						rel_job[i]->job_state == STATE_OVER;
					}
				}
			}
		}

		pthread_mutex_unlock(&job_mut);
	}

	pthread_exit(NULL);
}
static void module_unload(void)
{
	int i;

	pthread_cancel(rel_tid);
	pthread_join(rel_tid, NULL);
	
	for(i = 0; i < REL_JOBMAX; i++)
	{
		if(rel_job[i] != NULL)
		{

			fcntl(rel_job[i]->fd1, F_SETFL, rel_job[i]->fd1_save);
			fcntl(rel_job[i]->fd2, F_SETFL, rel_job[i]->fd2_save);

			free(rel_job[i]);
		}
	}

	pthread_mutex_destroy(&job_mut);
}

static void module_load(void)
{
	int err;
	err == pthread_create(&rel_tid, NULL, thr_relay, NULL);
	if(err > 0)
	{
		fprintf(stderr,"pthread_create():%s\n", strerror(err));
		exit(1);
	}

	atexit(module_unload);
}

/*
 *	return >=0        成功，返回当前任务ID
 *	       == -EINVAL 失败，参数非法
 *         == -ENOSPC 失败，任务数值满
 *         == -ENOMEM 失败，内存分配有误
*/
int rel_addjob(int fd1, int fd2)
{
	struct rel_job_st *me;
	int pos;
	static pthread_once_t once;

	if(fd1 < 0 || fd2 < 0)
	{
		return -EINVAL;
	}
	
	pthread_once(&once, module_load);

	me = malloc(sizeof(*me));
	if(me == NULL)
		return -ENOMEM;
	
	me->fd1 = fd1;
	me->fd2 = fd2;
	me->job_state = STATE_RUNNING;

	me->fd1_save = fcntl(me->fd1, F_GETFL);
	fcntl(me->fd1, F_SETFL, me->fd1_save | O_NONBLOCK);

	me->fd2_save = fcntl(me->fd2, F_GETFL);
	fcntl(me->fd2, F_SETFL, me->fd2_save | O_NONBLOCK);

	me->fsm12.state = STATE_R;
	me->fsm12.len = 0;
	me->fsm12.pos = 0;
	me->fsm12.sfd = fd1;
	me->fsm12.dfd = fd2;

	me->fsm21.state = STATE_R;
	me->fsm21.len = 0;
	me->fsm21.pos = 0;
	me->fsm21.sfd = fd2;
	me->fsm21.dfd = fd1;
		
	pthread_mutex_lock(&job_mut);
	pos = get_free_pos_unlocked();
	if(pos < 0)
	{
		pthread_mutex_unlock(&job_mut);
		fcntl(me->fd1, F_SETFL, me->fd1_save);
		fcntl(me->fd2, F_SETFL, me->fd2_save);
		free(me);
		return -ENOSPC;
	}
	rel_job[pos] = me;
	me->id = pos;
	pthread_mutex_unlock(&job_mut);
	
	return pos;
}

#if 0
int rel_canceljob(int id);
/*
 *	return == 0       成功, 指定任务成功取消
 *	       == -EINVAL 失败，参数非法
 *         == -EBUSY  失败，任务早已被取消
*/


int rel_waitjob(int id, struct rel_stat_st *);
/*
 *	return == 0       成功，指定任务已终止并返回状态
 *	       == -EINVAL 失败，参数非法
*/


int rel_statjob(int id, int rel_stat_st *);
/*
 *	return == 0       成功，指定任务状态已经返回
 *	       == -EINVAL 失败，参数非法
*/
#endif



