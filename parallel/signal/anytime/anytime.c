#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include "anytime.h"

#define ID_MAX_NUM 1024

struct anytime_st
{
	int id;
	int sec;
	int flag;
	at_jobfunc_t *fun;
	void *privat;
};

typedef void (*sighandler_t)(int);
static struct anytime_st *job[ID_MAX_NUM];
static int inited = 0;
static sighandler_t alrm_handler_save;

static void alrm_handler(int s)
{
	int i = 0;

	alarm(1);

	for(i = 0; i < ID_MAX_NUM; i++)
	{
		if(job[i] != NULL)
		{
			if(job[i]->flag == 0)
			{
				job[i]->sec--;
				if(job[i]->sec == 0)
				{
					job[i]->fun(job[i]->privat);
					job[i]->flag = 1;
				}
			}
		}
	}
}


static void module_unload(void)
{
	int i;

	signal(SIGALRM, alrm_handler_save);
	alarm(0);
	
	for(i = 0; i < ID_MAX_NUM; i++)
	{
		free(job[i]);
		job[i] = NULL;
	}	
}

static void module_load(void)
{
	alrm_handler_save = signal(SIGALRM, alrm_handler);
	alarm(1);

	atexit(module_unload);
}

static int get_free_pos(void)
{
	int i;

	for(i = 0; i < ID_MAX_NUM; i++)
	{
		if(job[i] == NULL)
		{
			return i;
		}
	}
	
	return -1;
}

/*
	return >= 0 成功，返回任务ID
	       == -EINVAL 失败，参数非法
		   == -ENOSPC 失败，数组满
		   == -ENOMEM 失败，内存空间不足
*/
int at_addjob(int sec, at_jobfunc_t *fun, void *arg)
{
	int id;

	if(sec <=0 || fun == NULL)
	{
		return -EINVAL;
	}

	if(inited == 0)
	{
		module_load();
		inited = 1;
	}
	
	id = get_free_pos();
	if(id < 0)
		return -ENOSPC;
	
	job[id] = malloc(sizeof(struct anytime_st));
	if(job[id] == NULL)
		return -ENOMEM;
	
	job[id]->id = id;
	job[id]->sec = sec;
	job[id]->fun = fun;
	job[id]->privat = arg;
	job[id]->flag = 0;

	return id;
}

/*
	return >= 0          成功，指定任务成功取消
	       == -EINVAL    失败，参数非法
		   == -EBUSY     失败，指定任务已完成
		   == -ECANCELED 失败，指定任务重复取消
*/
int at_canceljob(int id)
{

}

/*
	return == 0          成功，指定任务成功释放
	       == -EINVAL    失败，参数非法
*/
int at_waitjob(int id)
{
	if(id >= ID_MAX_NUM)
		return -EINVAL;

	if(job[id] == NULL)
		return -EINVAL;

	free(job[id]);
	return 0;
}

