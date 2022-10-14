#ifndef ANYTIME_H__
#define ANYTIME_H__

typedef void at_jobfunc_t(void*);

/*
	return >= 0 成功，返回任务ID
	       == -EINVAL 失败，参数非法
		   == -ENOSPC 失败，数组满
		   == -ENOMEM 失败，内存空间不足
*/
int at_addjob(int sec, at_jobfunc_t *fun, void *arg);

/*
	return >= 0          成功，指定任务成功取消
	       == -EINVAL    失败，参数非法
		   == -EBUSY     失败，指定任务已完成
		   == -ECANCELED 失败，指定任务重复取消
*/
int at_canceljob(int id);

/*
	return == 0          成功，指定任务成功释放
	       == -EINVAL    失败，参数非法
*/
int at_waitjob(int id);

#endif

