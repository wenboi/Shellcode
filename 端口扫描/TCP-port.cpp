

#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

void print_usage(char * cmd)
{
	fprintf(stderr," %s usage:\n",cmd);
	fprintf(stderr,"%s IP_Addr [port]\n",cmd);
}

int main(int argc,char** argv)
{
	struct sockaddr_in server;
	struct servent *sptr;
	int ret;
	char *name;
	int start_port;
	int end_port;
	int len;
	int port;
	int sockfd;
       
    start_port = atoi(argv[2]);
    end_port = atoi(argv[3]);
       
	for(port = start_port;port<=end_port;port++)
	{
		if (-1==(sockfd=socket(AF_INET,SOCK_STREAM,0)))
		{
			perror("can not create socket\n");
			exit(1);
		}
	
		memset(&server,0,sizeof(struct sockaddr_in));
		server.sin_family = AF_INET;
		server.sin_addr.s_addr = inet_addr(argv[1]);
		server.sin_port = htons(port);
	
		if (0>(ret=connect(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr))))
		{
			close(sockfd);
			continue;
		}
		
		else
		{
			//连接建立成功
 			sptr = getservbyport(htons(port), NULL);
 			
 			if(sptr == NULL)
 			{
 				printf("%d   unknown\n",port);
			}
			else
			{
				name = sptr->s_name;
				printf("%d   %s\n",port,name);
			}
		}
	
		close(sockfd);
	}
	
}




