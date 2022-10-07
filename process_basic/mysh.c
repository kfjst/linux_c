#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <glob.h>
#include <string.h>

static void promt(void)
{
	printf("mysh0.1:~$ ");
}

struct cmd_st
{
	glob_t globress;
};
#define DELIMS " \t\n"
static void prase(char *line, struct cmd_st *cmd)
{
	char *tok = NULL;
	int i = 0;

	while(1)
	{
		tok = strsep(&line, DELIMS);
		if(tok == NULL)
			break;
		if(tok[0] == '\0')
			continue;
		
		glob(tok, GLOB_NOCHECK|GLOB_APPEND * i, NULL, &cmd->globress);
		i = 1;
	}
}


int main()
{
	pid_t pid;
	char *line = NULL;
	size_t len = 0;
	struct cmd_st cmd;
	
	while(1)
	{
		promt();

		if(getline(&line, &len, stdin) < 0)
		{
			break;
		}

		if(memcmp(line, "esc", 3) == 0)
		{
			exit(0);
		}

		prase(line, &cmd);
		
		pid = fork();
		if(pid < 0)
		{
			perror("fork();");
			exit(1);
		}

		if(0 == pid)
		{
			/* child  */
			//puts(cmd.globress.gl_pathv[0]);
			//fflush(NULL);
			execvp(cmd.globress.gl_pathv[0], cmd.globress.gl_pathv);
			perror("execvp()");
			exit(1);
		}
		else
		{
			wait(NULL);
		}	
	}

	exit(0);
}



