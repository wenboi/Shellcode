#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main()
{
	pid_t pid;
	char input[10];
	char input2[10];

 	printf("this program was executed before fork\n");


	pid = fork();

	if(0>pid)
	{
		perror("fork error\n");
		exit(1);

	}
	else if(0==pid)
	{
		scanf("%s",input);
		fprintf(stdout,"this message is printed by child\n");
		
		exit(0);

	}
	else
	{
		scanf("%s",input2);
		fprintf(stdout,"this message is printed by parent\n");
		
		exit(0);

	}

//用ps -me可查看子进程；



}
