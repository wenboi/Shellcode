

#include<stdlib.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <pthread.h>  

#define SEG_LEN  655
#define MAX_PORT 65535
#define THREAD_NUM 100

typedef struct port_segment
{
	char *dest;
	 int max_port;
	 int min_port;
}port_segment;


void *scan(void *arg) 
{
	struct sockaddr_in server;
	struct servent *sptr;
	int sockfd;
	int ret;
	char *name;
	
	struct port_segment *PORT = (struct port_segment *)(arg);
	
	
	memset(&server,0,sizeof(struct sockaddr_in));
	
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = inet_addr(PORT->dest);
	
	for(int i=PORT->min_port;i<PORT->max_port;i++)
	{
		server.sin_port = htons(i);
		
		if (-1==(sockfd=socket(AF_INET,SOCK_STREAM,0)))
		{
			perror("can not create socket\n");
			exit(1);
		}
		
		ret=connect(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr));
		
		close(sockfd);
		
		if(ret < 0)
		{
			continue;
		}	
		
		
		else if(ret>0)
		{
			//连接建立成功
 			sptr = getservbyport(htons(i), NULL);
 			
 			if(sptr == NULL)
 			{
 				printf("%d   unknown\n",i);
			}
			else
			{
				name = sptr->s_name;
				printf("%d   %s\n",i,name);
			}
		}
	}

}

int main(int argc,char** argv)
{
	
	pthread_t* thread;
	
	thread = ( pthread_t * )malloc( THREAD_NUM * sizeof(pthread_t) );
	
	if(argc<2) //判断输入参数
	{
		printf("error,please enter again!");
		exit(1);	
	}	
	
	
	for ( int j = 1; j <=argc - 1; j++ ) 
	{
	 		
    	printf("%d-ip: %s\n",j,argv[j]);
    	
        for ( int i = 0; i < THREAD_NUM; i++ ) {
            port_segment port;
            port.dest = argv[j];
            port.min_port = i * SEG_LEN + 1;

            /* the last segment */
            if ( i == (THREAD_NUM - 1) )
                port.max_port = MAX_PORT;
            else
                port.max_port = port.min_port + SEG_LEN - 1;
                
            if ( pthread_create(&thread[i], NULL, scan, (void *)&port) != 0 )		
            {
                perror( "pthread_create failed\n" );
                free(thread);
                exit(-2);
   			}
			pthread_join(thread[i], NULL);  //回收当前的线程资源	
            }
         
        }
        free(thread);			//扫描结束，释放资源 
        
}




