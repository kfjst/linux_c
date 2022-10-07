#include "stdio.h"
#include "stdlib.h"
#include "unistd.h"

/* date +%s  */

int main()
{
	
	puts("Bingen !");
	fflush(NULL); /*!!!*/


	execl("/bin//date", "date", "+%s", NULL);
	perror("execl()");
	exit(1);
	
	puts("end!");


	exit(0);
}
