#ifndef _RELAYER_H__
#define _RELAYER_H__

#define REL_JOBMAX    1024

enum
{
	STATE_RUNNING = 1,
	STATE_CANCELEN,
	STATE_OVER,
};

struct rel_stat_st
{
	int state;
	int fd1;
	int fd2;
	int64_t count12, count21;
	// struct timerval start, end;
};

int rel_addjob(int fd1, int fd2);
/*
 *	return >=0        成功，返回当前任务ID
 *	       == -EINVAL 失败，参数非法
 *         == -ENOSPC 失败，任务数值满
 *         == -ENOMEM 失败，内存分配有误
*/

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

#endif

