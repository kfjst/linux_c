#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

#include "mytbf.h"


struct mytbf_st
{
	int cps;
	int burst;
	int token;
	int pos;
};

typedef void (*sighandler_t)(int);
static struct mytbf_st *job[MYTBF_MAX];
static sighandler_t alrm_handler_save;

static int get_free_pos()
{
	int i = 0;

	for(i = 0; i < MYTBF_MAX; i++)
	{
		if(job[i] == NULL)
		{
			return i;
		}
	}

	return -1;
}

static void alrm_handle(int s)
{
	int i = 0;
	alarm(1);
	
	for(i = 0; i < MYTBF_MAX; i++)
	{
		if(job[i] != NULL)
		{
			job[i]->token+=job[i]->cps;
			if(job[i]->token > job[i]->burst)
				job[i]->token = job[i]->burst;
		}
	}
}

static void module_unload(void)
{
	int i;
	signal(SIGALRM, alrm_handler_save);
	alarm(0);

	for(i = 0; i < MYTBF_MAX; i++)
	{
		free(job[i]);
		job[i] = NULL;
	}
}

static void module_load(void)
{
	alrm_handler_save = signal(SIGALRM, alrm_handle);
	alarm(1);

	atexit(module_unload);
}

mytbf_t *mytbf_init(int cps, int burst)
{
	struct mytbf_st *me;
	static int inited = 0;
	int pos = 0;

	if(inited == 0)
	{
		module_load();
		inited = 1;
	}

	pos = get_free_pos();
	if(pos < 0)
	{
		return NULL;
	}

	me = malloc(sizeof(*me));
	if(me == NULL)
		return NULL;
	
	me->token = 0;
	me->cps = cps;
	me->burst = burst;
	me->pos = pos;	
	job[pos] = me;

	return me;
}

int mytbf_fetchtoken(mytbf_t *ptr, int size)
{
	struct mytbf_st *me = ptr;
	int n = 0;

	if(size <= 0)
		return -EINVAL;
	
	while(me->token <= 0)  //等待有token
		pause;
	
	n = (me->token) > size ? size : me->token;
	me->token -= n;

	return n;
}

int mytbf_returntoken(mytbf_t *ptr, int size)
{
	struct mytbf_st *me = ptr;
	
	if(size <= 0)
		return -EINVAL;

	me->token += size;
	if(me->token > me->burst)
		me->token = me->burst;
	
	return size;
}

int mytbf_destroy(mytbf_t *ptr)
{
	struct mytbf_st *me = ptr;

	job[me->pos] = NULL;
	free(me);

	return 0;
}


	

