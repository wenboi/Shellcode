/********************************************************************************\
*                                                                               *\
*                                                                               *\
*            program name : tcpserver.c                               *\
*                   Author: liang gang & Hu xiao-qin                  *\
*                  Functon: the server of the remote control system   *\
*                      Aim: to tell students how to implement a simple*\
*                           remote control sysem                      *\
*                      date:03-14-2009                                *\
*              modifieddate:04-18-2009                                *\
**********************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/time.h>

#define PORT 8900
#define BUF_SIZE 2048

int execute(char* command,char* buf);


int main()
{
	int listend;
	int connected;
	int tempid;
	int sendnum;
	int recvnum;
       char send_buf[2048];
	char recv_buf[2048];
	char command[2048];
	int ret;
	char buf[BUF_SIZE];
	struct sockaddr_in server;
	struct sockaddr_in client;
	int count;//counter
	int counter;
	int num;//count the ready I/o device
	int maxfd;//to storage the max file describe sign
	int collection[FD_SETSIZE];
	fd_set rset;
	fd_set allset;	
	int backnum;
	int length;
	int port;
	int opt;

	port =PORT;
	opt = SO_REUSEADDR;
	
	/*creating socket*/
	
	if (-1==(listend=socket(AF_INET,SOCK_STREAM,0)))
	{
		perror("create socket error\n");
		return -1;

	}
	setsockopt(listend,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));

	/*binding server socket*/

	memset(&server,0,sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	if (-1==bind(listend,(struct sockaddr*)&server,sizeof(struct sockaddr)))
	{
		perror("binding error\n");
		close(listend);
		return -1;
	}

	/*listening*/
	backnum = 5;
	if (-1==listen(listend,5))
	{
		perror("listening error\n");
		close(listend);
		return -1;

	}
	
	length = sizeof(struct sockaddr);
	maxfd=listend;
	counter=-1;
	for(count=0;count<FD_SETSIZE;count++)
	{
		collection[count]=-1;
	}
	FD_ZERO(&allset);
	FD_SET(listend,&allset);
	
	
	
	while(1)
	{
		rset = allset;
		num = select(maxfd+1,&rset,NULL,NULL,NULL);

		/*aim to listen socket and build connection socket*/
		if (FD_ISSET(listend,&rset))
		{
			if (-1==(connected=accept(listend,(struct sockaddr*)&client,&length )))
			{
				perror("accepting error\n");
				continue;
			}
			
			for(count=0;count<FD_SETSIZE;count++)
			{
				if (-1==collection[count])
				{
					collection[count]=connected;
					break;

				}					

			}
			if (FD_SETSIZE==count)
				fprintf(stderr,"too many clients\n");

			FD_SET(connected,&allset);

			if (connected>maxfd)
				maxfd = connected;
			if (count>counter)
				counter = count;

			if (--num<=0)
				continue;	
				

		}
		/*aim to connection socket and communate with client*/
		for(count=0;count<=counter;count++)
		{
			if(0>(tempid=collection[count]))
				continue;
			/*broken pipe*/	
			if(FD_ISSET(tempid,&rset))
			{
				
				memset(send_buf,0,2048);
				memset(recv_buf,0,2048);
				memset(command,0,2048);
				
				if (0>=(recvnum=recv(tempid,recv_buf,sizeof(recv_buf),0)))
				{
					perror("receiving data error\n");
					close(tempid);
					FD_CLR(tempid,&allset);
					collection[count]=-1;
					continue;
				}
				recv_buf[recvnum]='\0';
		
				if (0==strcmp(recv_buf,"quit"))
				{
					perror("quiting remote controling\n");
					close(tempid);
					FD_CLR(tempid,&allset);
					collection[count]=-1;
					continue;

				}
				strcpy(command,"/bin/");
				strcat(command,recv_buf);
				execute(command,send_buf);
		
				#ifdef DEBUG
					printf("1:send_buf%s\n",send_buf);
				#endif

				if ('\0'==*send_buf)
				{
			
					memset(command,0,2048);
					strcpy(command,"/sbin/");
					strcat(command,recv_buf);
					execute(command,send_buf);
			
				#ifdef DEBUG
					printf("1:send_buf%s\n",send_buf);
				#endif
				}
				if ('\0'==*send_buf)
				{
					memset(command,0,2048);
					strcpy(command,"/usr/bin/");
					strcat(command,recv_buf);
					execute(command,send_buf);
				#ifdef DEBUG
					printf("1:send_buf%s\n",send_buf);
				#endif
				}
				if ('\0'==*send_buf)
				{
					memset(command,0,2048);
					strcpy(command,"/usr/sbin/");
					strcat(command,recv_buf);
					execute(command,send_buf);
					#ifdef DEBUG
						printf("1:send_buf%s\n",send_buf);
					#endif
				}
					
	
				if ('\0'==*send_buf)
				sprintf(send_buf,"command is not  valid \n");
				int len = strlen(send_buf);
				send_buf[len]='\0';
				if (0>=(sendnum=send(tempid,send_buf,sizeof(send_buf),0)))
				{
					perror("send data error\n");
					close(tempid);
					FD_CLR(tempid,&allset);
					collection[count]=-1;
					continue;

				}
		
					
			}

			//if (--num<0) break;

		}
		
		
	}
	close(listend);	

}
int execute(char * command,char* buf)
{
	FILE *fp;
	int count;

	if (NULL==(fp=popen(command,"r")))
	{
		perror("create pipe error\n");
		exit(-1);

	}

	count = 0;
	while(((buf[count]=fgetc(fp))!=EOF)&&(count<2047))
	count++;
	


	buf[count]='\0';
	
	pclose(fp);
	return count;
}

