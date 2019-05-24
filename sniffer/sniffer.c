//God bless my code

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pcap.h>
#include <sys/time.h>
#include <time.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <linux/if_ether.h>

#define BUFSIZ 1500
#define ETH_P_IP 0x0800 //IP协议
int count;
void print_mes(char *data, int len);
void print(char *line);

void pcap_callback(u_char *useless, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
  struct ethhdr *eptr;         //表示以太网帧的头部
  struct in_addr saddr, daddr;    //存放源和目的ip地址
  eptr = (struct ethhdr *)packet;
  char *data;
  int len, i;

  if((ntohs(eptr->h_proto) != ETH_P_IP ))
  {
    return;
  }

  printf("The packet id : %d\n",count);
  count++;

  printf("The src Mac is :");  //源mac地址
  for(i =0;i<6;i++)
  {
    printf("%1x", eptr->h_source[i]);
    if (i < 5)
    {
      printf(":");
    }
  }

  printf("The dest Mac is :");  //目的mac地址
  for(i =0;i<6;i++)
  {
    printf("%1x", eptr->h_dest[i]);
    if (i < 5)
    {
      printf(":");
    }
  }
  printf("\n================================\n");

  struct iphdr *p_iphdr;
  p_iphdr = (struct iphdr *)(packet + 14);  //ip数据包，，以太网帧的头长度为14个字节，这里+14就是ip帧结构了
  char *src = NULL;
  char *dest = NULL;
  saddr.s_addr = p_iphdr->saddr;     //32位源IP地址
  src = inet_ntoa(saddr);
  daddr.s_addr = p_iphdr->daddr;     //32位目的ip地址
  dest = inet_ntoa(saddr);
  printf("The src  ip is : %s\n",src);
  printf("The dest ip is : %s\n",dest);

  if (p_iphdr->protocol == 6)  //  Tcp的协议号为6
  {
    printf(" The Protocol: TCP\n");
    struct tcphdr *p_tcphdr;
    p_tcphdr = (struct tcphdr *)(packet + 14 + 20);
    printf("    Src port: %d\n    Dst port: %d\n\n", ntohs(p_tcphdr->source), ntohs(p_tcphdr->dest));
    data = (char *)(packet + 14 + 20 + 20);
    len = pkthdr->len - 54;
    print_mes(data, len);
    printf("\n\n");
    return;
  }
  else if (p_iphdr->protocol == 17)   //UDP协议
  {
    printf(" The Protocol: UDP\n\n");
    data = (char *)(packet + 14 + 20 + 20);
    len = pkthdr->len - 54;
    print_mes(data, len);
    printf("\n\n");
  }
  else if (p_iphdr->protocol == 1)   //ICMP协议
  {
    printf("    Protocol: ICMP\n\n");
    data = (char *)(packet + 14 + 20 + 20);
    len = pkthdr->len - 54;
    print_mes(data, len);
    printf("\n\n");
  }
  else            //其他协议
  {
    printf(" The  Protocol: unknow\n\n");
    data = (char *)(packet + 14 + 20 + 20);
    len = pkthdr->len - 54;
    print_mes(data, len);
    printf("\n\n");
  }
  return;
}

void print_mes(char *data, int len)
{
  int i, line_width = 16, len_rem;
  char *p = NULL;
  p = data;
  len_rem = len;
  i = 0;
  while (len != 0)
  {
    if (len_rem < line_width)
    {
      break;
    }
    printf("%05d  ", i);
    len_rem = len_rem - line_width;
    print(p);
    p = p + 16;
    i += 16;
  }
  return;
}

void print(char *line)
{
  char *p = line;
  int i;
  for (i = 0; i < 16; i++)
  {
    printf("%.2X ", (unsigned char)*p);
    p++;
  }
  p = line;
  printf(" ");
  for (i = 0; i < 16; i++)
  {
    if (!isprint(*p))
    {
      printf(".", *p);
    }
    else
    {
      printf("%c", *p);
    }
    p++;
  }
  printf("\n");
  return;
}


int main(int argc, char *argv[])
{
  char *dev, errbuf[PCAP_ERRBUF_SIZE];  //设备名
  bpf_u_int32 mask;
  bpf_u_int32 net;
  struct in_addr addr;
  char *net_c, *mask_c;      //网络号，子网掩码
  struct pcap_pkthdr header; //libpcap包头结构，包含捕获时间，捕获长度与数据包实际长度
  const u_char *packet;      //捕获到的实际数据包内容
  pcap_t *handle;            //libpcap设备描述符号

  struct tm *now_tm;
  time_t now;
  struct bpf_program fp;             //过滤器
  char filter_exp[] = "ip"; //实际的过滤规则
  dev = NULL;
  net_c = NULL;
  mask_c = NULL;
  net = 0;
  mask = 0;

  memset(errbuf, 0, PCAP_ERRBUF_SIZE);

  dev = pcap_lookupdev(errbuf); //获取设备名称
  if (NULL == dev)
  {
    perror("can not get device name\n");
    perror("errorbuf\n");
    return 1;
  }

  fprintf(stderr, "the device name is %s\n", dev); //输出设备名称


  /*用于获取网卡的网络号和子网掩码*/
  if (-1 == pcap_lookupnet(dev, &net, &mask, errbuf))
  {
    perror("can not get the interface message\n");
    perror("errbuf\n");
    return 1;
  }

  addr.s_addr = net;
  net_c = inet_ntoa(addr);

  if (NULL == net_c)
  {
    perror("can not convert ip addr\n");
    return 1;
  }
  addr.s_addr = mask;
  mask_c = inet_ntoa(addr);

  if (NULL == mask_c)
  {
    perror("can not convert ip addr\n");
    return 1;
  }
  printf("Device: %s\n", dev);
  printf("Net:    %s\n", net_c);
  printf("Mask:   %s\n\n", mask_c);

  printf("==================================================\n");

  // 打开设备文件，准备读取数据
  handle = pcap_open_live(dev, BUFSIZ, 1, 0, errbuf);

  if (handle == NULL)
  {
    fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
    return (2);
  }

  /*编译过滤规则*/
  if (pcap_compile(handle, &fp, filter_exp, 0, net) == -1)
  {
    fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
    return (2);
  }
  /*设置过滤规则*/
  if (pcap_setfilter(handle, &fp) == -1)
  {
    fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
    return (2);
  }

  pcap_loop(handle, -1, pcap_callback, NULL); //  抓包（类似抓包函数pcap_dispatch，指定參数cnt为1），返回一个无符号指针指向抓取到的数据

  /*关闭bpf过滤器*/
  pcap_freecode(&fp);
  pcap_close(handle);
  return;
}
