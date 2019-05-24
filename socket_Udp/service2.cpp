#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main(int argc,char **argv)
{
    int ser_sockfd;
    int len;
	int PORT;
	
	printf("please enter the port\n");
	scanf("%d",&PORT); 
	printf("listening\n"); 
    socklen_t addrlen;
	char recv_buf[2048];
    struct sockaddr_in ser_addr;
   
    
	memset(recv_buf,0,2048);
	 /*����socket*/
    ser_sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(ser_sockfd<0)
    {
        printf("I cannot socket success\n");
        return 1;
    }
    /*��дsockaddr_in �ṹ*/
    addrlen=sizeof(struct sockaddr_in);
    bzero(&ser_addr,addrlen);
    ser_addr.sin_family=AF_INET;
    ser_addr.sin_addr.s_addr=htonl(INADDR_ANY);
    ser_addr.sin_port=htons(PORT);
    //�󶨿ͻ���
    if(bind(ser_sockfd,(struct sockaddr *)&ser_addr,addrlen)<0)
    {
	    printf("connect");
	    return 1;
    }
    
    bzero(recv_buf,sizeof(recv_buf));
    
    while(1)
    {
    
	    len=recvfrom(ser_sockfd,recv_buf,sizeof(recv_buf),0,(struct sockaddr*)&ser_addr,&addrlen);
	    
	    if (0==strcmp(recv_buf,"quit1"))
		        {
				perror("the client break the server process\n");
				close(ser_sockfd);
				break;
			}
	    //��ʾclient�˵������ַ
	    printf("receive from %s\n",inet_ntoa(ser_addr.sin_addr));
	    /*��ʾ�ͻ��˷������ִ�*/ 
	    printf("recevce:%s\n",recv_buf);
	    /*���ִ����ظ�client��*/
	    sendto(ser_sockfd,recv_buf,sizeof(recv_buf),0,(struct sockaddr*)&ser_addr,addrlen);
	    
	    bzero(recv_buf,sizeof(recv_buf));
	}
}
