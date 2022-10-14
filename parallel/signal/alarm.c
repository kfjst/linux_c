#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
	alarm(10);
	alarm(1);  
	alarm(5);  //只有最后一个生效

	while(1)
	{
		pause(); //阻塞信号调用， 使用pause，可防止设备全部占用cpu, 不使用，CPU将100%
	}


	exit(0);
}


