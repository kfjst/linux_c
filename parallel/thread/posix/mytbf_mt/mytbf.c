#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include "mytbf.h"


struct mytbf_st
{
	int cps;
	int burst;
	int token;
	int pos;
	pthread_mutex_t mut;
	pthread_cond_t cond;
};

//typedef void (*sighandler_t)(int);
static struct mytbf_st *job[MYTBF_MAX];
//static sighandler_t alrm_handler_save;
static pthread_mutex_t mut_job = PTHREAD_MUTEX_INITIALIZER;
static pthread_t alrm_tid;

static int get_free_pos_unlocked()
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

static void *alrm_thread(void *p)
{
	int i = 0;
	struct timespec req;
//	alarm(1);
	
	req.tv_sec = 1;
	req.tv_nsec = 0;

	while(1)
	{
		pthread_mutex_lock(&mut_job);
		for(i = 0; i < MYTBF_MAX; i++)
		{
			if(job[i] != NULL)
			{
				pthread_mutex_lock(&(job[i]->mut));
				job[i]->token+=job[i]->cps;
				if(job[i]->token > job[i]->burst)
					job[i]->token = job[i]->burst;
				pthread_cond_broadcast(&(job[i]->cond));
				pthread_mutex_unlock(&(job[i]->mut));
			}
		}
		pthread_mutex_unlock(&mut_job);
		
		nanosleep(&req, NULL);
	}

	pthread_exit(NULL);
}

static void module_unload(void)
{
	int i;
//	signal(SIGALRM, alrm_handler_save);
//	alarm(0);
	
	pthread_cancel(alrm_tid);
	pthread_join(alrm_tid, NULL);

	for(i = 0; i < MYTBF_MAX; i++)
	{
		if(job[i] != NULL)
		{
			mytbf_destroy(job + i);
		}
	}

	pthread_mutex_destroy(&mut_job);
}

static void module_load(void)
{
//	alrm_handler_save = signal(SIGALRM, alrm_handle);
//	alarm(1);
	int err;

	err = pthread_create(&alrm_tid, NULL, alrm_thread, NULL);
	if(err > 0)
	{
		fprintf(stderr, "pthread_create():%s\n", strerror(err));
		exit(1);
	}

	atexit(module_unload);
}

mytbf_t *mytbf_init(int cps, int burst)
{
	struct mytbf_st *me;
	int pos = 0;
	static pthread_once_t once_control = PTHREAD_ONCE_INIT;

	pthread_once(&once_control, module_load);

	me = malloc(sizeof(*me));
	if(me == NULL)
		return NULL;
	
	me->token = 0;
	me->cps = cps;
	me->burst = burst;
	me->pos = pos;	
	pthread_mutex_init(&me->mut, NULL);
	pthread_cond_init(&me->cond, NULL);

	pthread_mutex_lock(&mut_job);
	pos = get_free_pos_unlocked();
	if(pos < 0)
	{
		pthread_mutex_unlock(&mut_job);
		pthread_mutex_destroy(&me->mut);
		pthread_cond_destroy(&me->cond);
		free(me);
		return NULL;
	}

	job[pos] = me;
	pthread_mutex_unlock(&mut_job);
	
	return me;
}

int mytbf_fetchtoken(mytbf_t *ptr, int size)
{
	struct mytbf_st *me = ptr;
	int n = 0;

	if(size <= 0 || me == NULL)
		return -EINVAL;
	
	pthread_mutex_lock(&me->mut);
	while(me->token <= 0)  //等待有token
		pthread_cond_wait(&me->cond, &me->mut);
	
	n = (me->token) > size ? size : me->token;
	me->token -= n;
	pthread_mutex_unlock(&me->mut);

	return n;
}

int mytbf_returntoken(mytbf_t *ptr, int size)
{
	struct mytbf_st *me = ptr;
	
	if(size <= 0 || me == NULL)
		return -EINVAL;

	pthread_mutex_lock(&me->mut);
	me->token += size;
	if(me->token > me->burst)
		me->token = me->burst;
	pthread_cond_broadcast(&me->cond);
	pthread_mutex_unlock(&me->mut);

	return size;
}

int mytbf_destroy(mytbf_t *ptr)
{
	struct mytbf_st *me = ptr;
	
	if(me == NULL)
	{
		return -1;
	}
	
	pthread_mutex_lock(&mut_job);
	job[me->pos] = NULL;
	pthread_mutex_unlock(&mut_job);
	pthread_cond_destroy(&me->cond);
	pthread_mutex_destroy(&me->mut);
	free(me);

	return 0;
}


	

