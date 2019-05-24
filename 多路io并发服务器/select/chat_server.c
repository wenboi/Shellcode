/*	linux-socket-select-异步聊天室
	talk_server.c
	writed by hanzhongqiu 13/04/2009
	Using select() for I/O multiplexing 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/types.h>

/* port we're listening on */
#define SERVERPORT 1573
/*maximum cnnnect number*/
#define BACKLOG 10
/*maximum data buffer*/
#define BUFSIZE 2048

struct client_info			//用来存储客户端的所有信息 
{
	int client_id;
	char name[100];
	int first; 
	struct sockaddr_in client_address;
};

int main()
{
	/*master file descriptor list*/
	fd_set master_fds;
	/*temple file descriptor list for select()*/
	fd_set read_fds;
	/*server address*/
	struct sockaddr_in server_addr;
	/*client address*/
	struct sockaddr_in client_addr;
	/*maximum file descriptor number*/
	int max_fd;
	/*listening socket file descriptor*/
	int sockfd;
	/*newly accept()ed socket file descreptor*/
	int newfd;
	/*buffer for saving client data*/
	char data_buf[BUFSIZE];
	char send_buf[BUFSIZE];
	char stringfrom[2048];
	/*number of client data*/
	int nbytes;
	/*for set socket option*/
	int opt;
	int i;
	opt = SO_REUSEADDR;
    /*lenth of address*/
	int addr_len;
	/*for accept() to use*/
	int size;
	size = sizeof(struct sockaddr);
	/*temple varient*/
	int tmp_i, tmp_j;
	
	char str1[256];  
    char str2[256];  
    char str3[256];  
    int tmpid;
	struct client_info clientinfo[BACKLOG];
	
	/*clear the master and temple file descriptor*/
	FD_ZERO(&master_fds);
	FD_ZERO(&read_fds);
	
    memset(&data_buf, 0, BUFSIZE);
	memset(&send_buf, 0, BUFSIZE);

	/*create socket*/
	if (-1 == (sockfd = socket(AF_INET, SOCK_STREAM, 0)))
	{
		perror("create socket() error:");
		exit(1);
	}
	/*set the socket*/
    if (-1 == setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
			perror("setsockopt() error:");
			exit(1);
    }
    
	/*bind first config the socket then binding*/
	memset(&server_addr, 0, size);
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVERPORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bzero(&(server_addr.sin_zero), 8);
	if (-1 == bind(sockfd, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)))
	{
		perror("bind() socket error:");
		exit(1);
	}
	/*listen */
	if (-1 == listen(sockfd, BACKLOG))
	{
		perror("listen() error:");
		exit(1);
	}
	
	for(i=0;i<=BACKLOG;i++)  //初始化客户端的信息 
	{
		clientinfo[i].client_id=-1;
		memset(clientinfo[i].name,0,256);
		clientinfo[i].first=-1;
	} 
	
	/*add sockfd to master set*/
	FD_SET(sockfd, &master_fds);
	/*keep track the max file descriptor*/
	max_fd = sockfd;
	printf("server is ok!\n");
	/*loop*/
	while (1)
	{
		read_fds = master_fds;
		if (-1== select(max_fd + 1, &read_fds, NULL, NULL, NULL))
		{
			perror("select() error!\n");
			exit(1);
		}
		/*looking for data to read*/
		for (tmp_i = sockfd; tmp_i <= max_fd; tmp_i++)
		{
			/*got connect*/
			if (FD_ISSET(tmp_i, &read_fds))
			{
				if (tmp_i == sockfd)
				{
					newfd = accept(sockfd, (struct sockaddr*)&client_addr, &size);
					clientinfo[newfd].client_id = newfd;
					clientinfo[newfd].first = 1; 
					clientinfo[newfd].client_address.sin_addr = client_addr.sin_addr;
					if (-1 == newfd)
					{
						perror("accept() error:");
						exit(1);
					}
					else
					{
						FD_SET(newfd, &master_fds);
						if (newfd > max_fd)
						{
							max_fd = newfd;
						}
						printf("Get the new connect from %s\n", inet_ntoa(client_addr.sin_addr));
					}
				}
				else
				{/*get data from the client*/
							nbytes = read(tmp_i, data_buf, sizeof(data_buf));
							if(clientinfo[tmp_i].first==1)
							{
								strcpy(clientinfo[tmp_i].name,data_buf);
								clientinfo[tmp_i].first =-1;
							}
							if (0 > nbytes)
							{
								perror("recv() error:");
								exit(1);
							}
							if(nbytes == 0 || (0 == strcmp(data_buf, "exit")))
							{	
								printf("client: %s exit!\n", inet_ntoa(clientinfo[tmp_i].client_address.sin_addr));
								FD_CLR(tmp_i, &master_fds);
								close(tmp_i);
								strcat(send_buf, inet_ntoa(clientinfo[tmp_i].client_address.sin_addr));
								strcat(send_buf, "  was exit!");
								for (tmp_j = sockfd + 1; tmp_j <= max_fd; tmp_j++)
								{
									if (FD_ISSET(tmp_j, &master_fds))
									{

										if (-1 == write(tmp_j,send_buf, nbytes))
										{
											perror("send data error:");
										}
									}
								}// end for
							}
						
								if(data_buf[0]=='/')
								{
									if(data_buf[1]=='w')
									{
										for (tmp_j = sockfd ; tmp_j <= max_fd; tmp_j++)
										{
											if (FD_ISSET(tmp_j, &master_fds))
											{
												strcat(send_buf,clientinfo[tmp_j].name);
											}
										}
										send(tmp_i,send_buf,sizeof(send_buf),0);
									}
								
									if(data_buf[1]=='q')
									{
										 
				                            printf("client:%s exit!\n",clientinfo[tmp_i].name);  
				                            FD_CLR(tmp_i,&master_fds);  
				                            close(tmp_i);  
				                            strcat(send_buf,clientinfo[tmp_i].name);  
				                            strcat(send_buf," exit!"); 
									}
							
									memset(str1,0,sizeof(str1));  //cmd
						            memset(str2,0,sizeof(str2));  //username
						            memset(str3,0,sizeof(str3));  //message
						            sscanf(data_buf,"%s %s %s",str1,str2,str3);  
						            strcat(str2,"\n");
						            if(str1[1]=='s')
						            {
						            	tmpid=-1;
						            	for (tmp_j = sockfd ; tmp_j <= max_fd; tmp_j++)
										{
											if (FD_ISSET(tmp_j, &master_fds))
											{
												if(strcmp(str2,clientinfo[tmp_j].name)==0);
												tmpid = tmp_j;
											}
										}
										if(tmpid==-1)
										{
											strcat(send_buf,"user not online!\n");
											send(tmp_i,send_buf,sizeof(send_buf),0);
											continue;
										}
										else
										{
											strcat(send_buf,clientinfo[tmp_i].name);  
	                            			strcat(send_buf,str3);
	                            			send(tmpid,send_buf,sizeof(send_buf),0);  
	                           			 	continue; 
										}	
									}
							}
						
							
						
					
					
					
					//end else
						else
							{
								strcat(stringfrom,inet_ntoa(clientinfo[tmp_i].client_address.sin_addr);
								strcat(stringfrom,clientinfo[tmp_i].name);
								printf("get data:%s from the client :", data_buf);
								printf("%s\n", stringfrom);
								strcat(send_buf, inet_ntoa(clientinfo[tmp_i].client_address.sin_addr));
								strcat(send_buf, " said:  ");
								strcat(send_buf, data_buf);
								for (tmp_j = sockfd + 1; tmp_j <= max_fd; tmp_j++)
								{
									if (FD_ISSET(tmp_j, &master_fds))
									{

										if (-1 == write(tmp_j, send_buf, nbytes))
										{
											perror("send data error:");
										}
									}
								}// end for
							}//end else
			}//end if
		}
	}//end for 

    memset(&data_buf, 0, BUFSIZE);
	memset(&send_buf, 0, BUFSIZE);
	//FD_ZERO(&master_fds);
	//FD_SET(sockfd, &master_fds);
	}//end while

	return 0;
}


