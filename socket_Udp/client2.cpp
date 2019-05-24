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

int main(int argc,char **argv)
{
    int cli_sockfd;
    int len;
    socklen_t addrlen;
    int port;
    struct sockaddr_in server_addr;
    char buffer_send[2048];
    char buffer_recv[2048];
    int choose=1;

    cli_sockfd=socket(AF_INET,SOCK_DGRAM,0);
    
    if ((2>argc)|| (argc >3))
	{
		print_usage(argv[0]);
		exit(1);

	}
	if (3==argc) 
    {
		port = atoi(argv[2]);
    }

    
    if(cli_sockfd<0)
    {
        printf("I cannot socket success\n");
        return 1;
    }
    
    addrlen=sizeof(struct sockaddr_in);
    bzero(&server_addr,addrlen);
    server_addr.sin_family=AF_INET;
    server_addr.sin_addr.s_addr=inet_addr(argv[1]);
    server_addr.sin_port=htons(port);
    bzero(buffer_send,sizeof(buffer_send));
    
	
	
	while(choose)
	{
		bzero(buffer_send,sizeof(buffer_send));
		printf("what words do  you want to tell to server:\n");
		
		scanf("%s",&buffer_send);
		
		
		if (0>(len= sendto(cli_sockfd,buffer_send,sizeof(buffer_send),0,(struct sockaddr*)&server_addr,addrlen)))
		{
			perror("send data error\n");
			continue;
		}
		
		buffer_send[len]='\0';
		
		if (0>(len=recvfrom(cli_sockfd,buffer_recv,sizeof(buffer_recv),0,(struct sockaddr*)&server_addr,&addrlen)))
		{
			perror("recv data error\n");
			continue;
		}
	    
	
	    printf("receive from server: %s\n",buffer_recv);
	    
	    if (0==strcmp(buffer_send,"quit"))
	        {
			perror("break the client process\n");
			close(cli_sockfd);
			exit(0);
		}
	}
	close(cli_sockfd);
}
