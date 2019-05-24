
#include<stdio.h>
#include<unistd.h>
#include<sys/socket.h>
#include<string.h>
#include <sys/types.h>
#include<stdlib.h>
#include<netdb.h>
#include<sys/wait.h>
#include<signal.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define PORT 8900
#define BUF_SIZE 2048

int execute(char* command,char* buf);

void sig_chld(int);


int main()
{
	int sockfd;
	struct sockaddr_in client;
	struct sockaddr_in server;
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	char cmd[2048];
	int sendnum;
	int listened;
	int len;
	int recvnum;
	int connectd;
	int port;
	

	memset(send_buf,0,2048);
	memset(recv_buf,0,2048);

	port = PORT;

	
   if (-1==(sockfd=socket(AF_INET,SOCK_STREAM,0)))
      {
		 perror("create listen socket error\n");
		 exit(1);
      }	

	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	if (-1==bind(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr)))
	{
		perror("binding error\n");
		close(sockfd);
		exit(1);

	}
	
	if (-1==(listened=listen(sockfd,5)))
    {
		perror("listen error\n");
		exit(1);
    }
	
//	length = sizeof(struct sockaddr_in);
	//协议处理：
	//-----------------------------------------------
	
	while(1)
	{
		memset(recv_buf,0,2048);
		memset(send_buf,0,2048);
		
		if(-1 == (connectd = accept(sockfd,(struct sockaddr*)&client,(socklen_t*)&len)))
		{
			perror("error in recving data...\n");
			break;

		}
		signal(SIGCHLD, sig_chld);
		pid_t fpid; 
		fpid = fork();

		if(fpid == 0)
		{
			close(listened);
			while(1)
			{
				 if (0>(recvnum = recv(connectd,recv_buf,sizeof(recv_buf),0)))
					{
						perror("recv error\n");
						close(connectd);
						continue;
					}
		//执行命令，返回结果存入缓冲区 send_buf
		//*****************************************
				if (1)
				{
				recv_buf[recvnum]='\0';
		
				if (0==strcmp(recv_buf,"quit"))
				{
					perror("quitting remote controling\n");
					break;
		
				}
		
				strcpy(cmd,"/bin/");
				strcat(cmd,recv_buf);
				execute(cmd,send_buf);
		
				if ('\0'==*send_buf)
				{
					memset(cmd,0,sizeof(cmd));
					strcpy(cmd,"/sbin/");
					strcat(cmd,recv_buf);
					execute(cmd,send_buf);
					
					if ('\0'==*send_buf)
					{	
						memset(cmd,0,sizeof(cmd));
						strcpy(cmd,"/usr/bin/");
						strcat(cmd,recv_buf);
						execute(cmd,send_buf);
					}
					
					if ('\0'==*send_buf)
					{	
						memset(cmd,0,sizeof(cmd));
						strcpy(cmd,"/usr/sbin/");
						strcat(cmd,recv_buf);
						execute(cmd,send_buf);
					}
				}
				if ('\0'==*send_buf)
					sprintf(send_buf,"command is not vaild,check it please\n");
		
				printf("the server message is:%s\n",send_buf);
			}
		//*************************************************************
		
				sendnum = strlen(send_buf);
				if(0>send(connectd,send_buf,sendnum,0))
				{
					perror("error!");
					break;
				}
			}
			close(connectd);
			exit(1);
		}
		else if(fpid > 0)
		{
			close(connectd);
			continue;
		}
	}
	close(sockfd);
	exit(1);
}

void sig_chld(int signo) 
{	
	pid_t pid;	
	int stat;//	printf("signal ...\n");	
	while((pid=waitpid(-1, &stat, WNOHANG)) > 0)		
	printf("child %d terminated.\n", pid);	
	return;
}


//执行命令
int execute(char* command,char* buf)
{
	FILE * 	fp;
	int count;

	if (NULL==(fp = popen(command,"r")))
	{
		perror("creating pipe error\n");
		exit(1);

	}
	
	count = 0 ;

	while(((buf[count] = fgetc(fp))!=EOF)&&count<2047)
	count++;
	buf[count]='\0';

	pclose(fp);
	return count;
	

}


