#ifndef _MYSEM_H__
#define _MYSEM_H__

typedef void mysem_t;

mysem_t *mysem_init(int initval);

int mysem_add(mysem_t *sem, int n);

int mysem_sub(mysem_t *sem, int n);

int mysem_destroy(mysem_t *sem);

#endif

