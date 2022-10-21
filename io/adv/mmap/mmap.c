#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>

//映射文件，查找文件中某个字符个数

int main(int argc, char **argv)
{
	char *str;
	int fd;
	struct stat buf;
	int i, count;

	if(argc < 2)
	{
		fprintf(stdout, "Usage: ./mmap filename\n");
		exit(1);
	}

	fd = open(argv[1], O_RDONLY);
	if(fd < 0)
	{
		perror("open");
		exit(1);
	}
		
	if(fstat(fd, &buf) < 0)
	{
		perror("fstat()");
		exit(1);
	}

	str = mmap(NULL, buf.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if(str == MAP_FAILED)
	{
		perror("mmap()");
		exit(1);
	}
	
	close(fd);

	count = 0;
	for(i = 0; i < buf.st_size; i++)
	{
		if(str[i] == 'a')
			count++;
	}

	printf("char a count %d\n", count);
	munmap(str, buf.st_size);

	exit(0);
}
