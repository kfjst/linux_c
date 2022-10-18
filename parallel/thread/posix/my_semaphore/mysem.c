#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include "mysem.h"

struct mysem_st
{
	int value;
	pthread_cond_t cond;
	pthread_mutex_t mut;
};

mysem_t *mysem_init(int initval)
{
	struct mysem_st *me;
	if(initval <= 0)
	{
		return NULL;
	}

	me = malloc(sizeof(*me));
	if(me == NULL)
	{
		return NULL;
	}

	me->value = initval;
	pthread_mutex_init(&me->mut, NULL);
	pthread_cond_init(&me->cond, NULL);
	
	return me;
}

int mysem_add(mysem_t *sem, int n)
{
	struct mysem_st *me = sem;
	
	if(n <= 0 || me == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&me->mut);
	(me->value) += n;
	pthread_cond_broadcast(&me->cond);
	pthread_mutex_unlock(&me->mut);
	
	return n;
}


int mysem_sub(mysem_t *sem, int n)
{
	struct mysem_st *me = sem;
	
	if(n <= 0 || me == NULL)
	{
		return -1;
	}

	pthread_mutex_lock(&me->mut);
	
	while(me->value < n)
	{
		pthread_cond_wait(&me->cond, &me->mut);
	}
	(me->value) -= n;
	pthread_mutex_unlock(&me->mut);
	
	return n;
}

int mysem_destroy(mysem_t *sem)
{
	struct mysem_st *me = sem;
	
	if(me == NULL)
	{
		return -1;
	}

	pthread_mutex_destroy(&me->mut);
	pthread_cond_destroy(&me->cond);
	free(me);

	return 0;
}

