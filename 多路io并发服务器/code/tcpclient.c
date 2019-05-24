#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 8900
#define BUF_SIZE 2048
int print_usage(char * buf)
{
	perror("%s usage:\n");
	perror("%s Ipaddr\n");
	exit(0);
}
int main(int argc,char** argv)
{
	int sockfd;
	struct sockaddr_in server;
	struct sockaddr_in client;
	char send_buf[BUF_SIZE];
	char recv_buf[BUF_SIZE];
	char command[BUF_SIZE];
	int sendnum;
	int recvnum;
	int backnum;
	int length;
	int port;
	port = PORT;
	if (argc != 2) 
	print_usage(argv[0]);
	if (-1 == (sockfd = socket(AF_INET,SOCK_STREAM,0)))
	{
		perror("create socket error\n");
		exit(1);
	}
	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(port);
	if (-1 == connect(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr)))//连接
	{
		perror("connect socket error\n");
		close(sockfd);
		exit(1);
	}
	memset(send_buf,0,2048);
	memset(recv_buf,0,2048);
    		
	fprintf(stdout,"please enter your name: ");
	gets(send_buf);
	sendnum = send(sockfd,send_buf,sizeof(send_buf),0);
	fd_set sockset;
	FD_ZERO(&sockset);
	FD_SET(sockfd,&sockset);
	FD_SET(0,&sockset);
	while(1)
	{
		memset(send_buf,0,2048);
		memset(recv_buf,0,2048);
		select(sockfd+1,&sockset,NULL,NULL,NULL);
		if(FD_ISSET(sockfd,&sockset))
		{
			recvnum = recv(sockfd,recv_buf,sizeof(recv_buf),0);
			recv_buf[recvnum] = '\0';
			printf("receive message:%s\n",recv_buf);
		}
		if(FD_ISSET(0,&sockset))
		{
			fgets(send_buf,sizeof(send_buf),stdin);
			send_buf[strlen(send_buf)-1] = '\0';
			char chars[6];
			strncpy(chars,send_buf,6);
			if(strcmp(chars,"/help") == 0)//打印帮助信息
			{
				fprintf(stdout,"/quit:quit from the room\n");
				fprintf(stdout,"/who:printf the online users\n");
				fprintf(stdout,"/send name:send message to one user\n");
				continue;
			}
			if(strcmp(chars,"/quit") == 0)//退出
			{
				printf("quit the chat room......\n");
				send(sockfd,send_buf,sizeof(send_buf),0);
				return -1;
			}
			send(sockfd,send_buf,sizeof(send_buf),0);
		}

		FD_ZERO(&sockset);
		FD_SET(sockfd,&sockset);
		FD_SET(0,&sockset);
	}
}
