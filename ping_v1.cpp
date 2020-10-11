#pragma comment(lib, "ws2_32.lib")

#include<stdio.h>
#include<windows.h>
//#include<WinSock2.h>
#include<process.h>

#define SEND_SIZE 32	//在默认情况下Windows的ping发送的数据包大小为32byte，可自定义大小，但不得超过65500byte
#define PACKET_SIZE 4096
#define ICMP_ECHO 8//回显请求
#define ICMP_ECHOREPLY 0//回显应答
//#define _WINSOCK_DEPRECATED_NO_WARNINGS 
struct icmp
{
	unsigned char icmp_type;//message type,8bit
	unsigned char icmp_code;//
	unsigned short icmp_cksum;//checksum, 16bit
	unsigned short icmp_id;//identification, 16bit
	unsigned short icmp_seq;//sequence，16bit, the initial value is zero,each time a new echo request is sent, one is added.
	unsigned long icmp_data;//
};

struct ip
{
	unsigned char ip_hl : 4;//header length, 4bit
	unsigned char ip_v : 4;//version, 4bit
	unsigned char ip_tos;//type of service, 8bit
	unsigned short ip_len;//total length, 16bit
	unsigned short ip_id;//identification, 16bit
	unsigned short ip_off;//fragment offset field, 13bit
	unsigned char ip_ttl;//time to live, 8bit
	unsigned char ip_p;//protocol, 8bit
	unsigned short ip_sum;//checksum, 16bit
	unsigned long ip_src;//source address
	unsigned long ip_dst;//destination address

};
char sendpacket[PACKET_SIZE];
char recvpacket[PACKET_SIZE];
struct sockaddr_in dest_addr;//internet环境下套接字的地址形式
struct sockaddr_in from_addr;
SOCKET sockfd;//套接字
int pid;

unsigned short cal_chksum(unsigned short* addr, int len);//计算校验和
int pack(int pack_no);
int unpack(char* buf, int len);
void send_packet(void);
void recv_packet(void);

int main(int argc, char* argv[])
{
	struct hostent* host;//host entry,域名和网络地址结构体
	struct protoent* protocol;//offer official protocal name, Alias list, protoco number 
	WSADATA wsaData;//store the Windows Sockets data after being called by function WSAStartup 
	int timeout = 1000;//毫秒
	int SEND_COUNT = 4;//Echo数据报数，默认为4
	int i;
	const char* par_host = "www.baidu.com";

	if (WSAStartup(0x1010, &wsaData) != 0)
	{//WSAStartup must be the first called Windows Sockets function
		printf("wsastartup error\n");
		exit(1);
	}
	if ((protocol=getprotobyname("icmp")) == NULL)
	{
		printf("getprotobyname error\n");
		exit(1);
	}
	if ((sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP)) < 0)
	{//socket()创建失败返回-1,解决：需要以管理员身份运行；common7/IDE/devenv.exe
		//使用原始套接字可以读写ICMP，ping程序就是使用原始套接字发送ICMP回显请求
		//建立原始套接口后，可以通过它向网络中写自己的IP数据包，为了防止非法用户破坏网络，规定只有超级用户才有创建原始套接口的权限
		printf("socket error\n");
		exit(1);
	}
	//在发送和接收数据的过程中，有时由于网络状况等原因，发收不能预期进行，而设置收发时限
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
		fprintf(stderr, "failed to set recv timeout: %d\n", WSAGetLastError());
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
		fprintf(stderr, "failed to set send timeout: %d\n", WSAGetLastError());
	
	memset(&dest_addr, 0, sizeof(dest_addr));//每个字节填充0
	dest_addr.sin_family = AF_INET;//IPv4
	if (host = gethostbyname(par_host))
	{//gethostbyname函数可以利用字符串格式的域名获得IP地址,并且将地址信息装入hostent域名结构体
		//失败时返回NULL指针
		memcpy((char*)&dest_addr.sin_addr, host->h_addr, host->h_length);
		if (host = gethostbyaddr(host->h_addr, 4, PF_INET))//利用IP地址获得域名
			par_host = host->h_name;//获取正式主机名
	}
	else if (dest_addr.sin_addr.s_addr = inet_addr(par_host) == INADDR_NONE)
	{
		printf("Unkown host %s\n", par_host);
		exit(1);
	}

	pid = _getpid();//用来获取当前进程的进程ID
	printf("Pinging %s [%s]: with %d bytes of data: \n\n", par_host, inet_ntoa(dest_addr.sin_addr), SEND_SIZE);
	for (i = 0; i < SEND_COUNT; i++)
	{
		send_packet();
		recv_packet();
		Sleep(1000);
	}

}
/*
unsigned short cal_chksum(unsigned short* data, int len)
{
	int sum = 0;
	int odd = (len & 0x01);
	while (len & 0xfffe)
	{
		sum += *(unsigned short*)data;
		data += 2;
		len -= 2;
	}
	if (odd)
	{
		unsigned short tmp = ((*data) << 8) & 0xff00;
		sum += tmp;
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return ~sum;
}
*/
//二进制反码求和
unsigned short cal_chksum(unsigned short* addr, int len)
{
	unsigned long cksum = 0;
	while (len > 1)
	{
		cksum += *addr++;
		len -= sizeof(unsigned short);
	}
	if (len)
		cksum += *(UCHAR*)addr;
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (USHORT)(~cksum);
}

//构造ICMP回显请求报文
int pack(int pack_no)
{
	int packsize;
	struct icmp* icmp;
	packsize = 8 + SEND_SIZE;//ICMP头结构8byte
	icmp = (struct icmp*)sendpacket;//发送缓冲区
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_cksum = 0;
	icmp->icmp_seq = pack_no;
	icmp->icmp_id = pid;//
	icmp->icmp_data = GetTickCount();//返回从操作系统启动到当前经过的毫秒数
	icmp->icmp_cksum = cal_chksum((unsigned short*)icmp, packsize);
	return packsize;
}
int unpack(char* buf, int len)
{
	struct ip* ip;
	struct icmp* icmp;
	double rtt;//
	int iphdrlen;

	ip = (struct ip*)buf;
	iphdrlen = ip->ip_hl * 4;//IP首部长度以4B为单位
	icmp = (struct icmp*)(buf + iphdrlen);
	if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid))//过滤回显应答报文
	{
		len = len - iphdrlen - 8;
		rtt = GetTickCount() - icmp->icmp_data;
		printf("Reply from %s: bytes=%d time=%.0fms TTL=%d icmp_seq=%u\n",
			inet_ntoa(from_addr.sin_addr),
			len,
			rtt,
			ip->ip_ttl,
			icmp->icmp_seq);
		return 1;
	}
	return 0;
}

void send_packet()
{
	int packetsize;
	static int pack_no = 0;
	packetsize = pack(pack_no++);
	if (sendto(sockfd, sendpacket, packetsize, 0, (struct sockaddr*) & dest_addr, sizeof(dest_addr)) < 0)
		printf("Destination host unreachable.\n");
}

void recv_packet()
{
	int n, fromlen;
	int success=0;//????

	fromlen = sizeof(from_addr);
	do
	{//recvfrom返回接收到的字节数，错误则返回-1，
		if ((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr*) & from_addr, &fromlen)) >= 0)
			success = unpack(recvpacket, n);
		else if (WSAGetLastError() == WSAETIMEDOUT)
		{
			printf("Request timed out.\n");//为什么总是超时？
			return;
		}
	} while (!success);

}