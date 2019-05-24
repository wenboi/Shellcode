/**************************************************************************\
*                                                                         *\
*                                                                         *\
*            program name : tcpclient.c                                   *\
*                   Author: liang gang & Hu xiao-qin                      *\
*                  Functon: the server of the remote control system       *\
*                      Aim: to tell students how to implement a simple    *\
*                           remote control sysem                          *\
*                      date:03-14-2009                                    *\
*                                                                         *\
***************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>

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

	port =PORT;
	
	if (argc!=2) print_usage(argv[0]);

	/*creating socket*/
	
	if (-1==(sockfd=socket(AF_INET,SOCK_STREAM,0)))
	{
		perror("create socket error\n");
		exit(1);

	}

	/*connectting server socket*/

	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(argv[1]);
	server.sin_port = htons(port);

	if (-1==connect(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr)))
	{
		perror("connectting error\n");
		close(sockfd);
		exit(1);
	}


	while(1)
	{
		memset(send_buf,0,2048);
		memset(recv_buf,0,2048);
    		
		fprintf(stdout,"tcp>");
		gets(send_buf);
	
		if (0>(sendnum = send(sockfd,send_buf,sizeof(send_buf),0)))
		{
			perror("sending data error\n");
			close(sockfd);
			exit(1);
		}

		if (0==strcmp(send_buf,"quit"))
		{
			perror("quitting remoet controling\n");
			close(sockfd);
			exit(0);
		}
		if (0>(recvnum=recv(sockfd,recv_buf,sizeof(recv_buf),0)))
		{
			perror("receiving data error\n");
			close(sockfd);
			exit(1);

		}

		recv_buf[recvnum]='\0';
		printf("%s\n",recv_buf);
		fflush(stdout);


	}

			

}

