#include <stdlib.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <command>\n", argv[0]);
		exit(1);
	}

	pid_t pid;
	int status;
	pid = fork();
	if (pid == 0)
	{ /* proceso hijo */
		//execlp ( " ls " ," ls " ," -l " , NULL ) ;
		execlp("/bin/bash", "-c", "’ls -l’ ", NULL); //Esto habría que cambiarlo, pero es para ver si furula
		perror(" ");
		return -1;
	}
	else
	{ /* proceso padre */
		while (pid != wait(&status))
			;
	}
	return system(argv[1]);
}
