#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main()
{
	/*

	www.OpenMP.org
	是一套跨语言的简单的并发标准
	借助编译器，实现并发，通过一些语法标记，GCC4.0以后都能识别openMP的语法标记

	有几个CPU,就会有几次打印，扔出来几个线程同时进行 同时打印hello world
	需要在编译选项中设置识别关键字
	CFLAGS+=-fopenmp -Wall
	和环境有关
	*/
	
// sections 以段落进行并发
#pragma omp parallel sections
{
#pragma omp section
	printf("[%d]hello\n", omp_get_thread_num());	
#pragma omp section
	printf("[%d]world\n", omp_get_thread_num());
}

	exit(0);
}
