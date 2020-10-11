#pragma comment(lib, "ws2_32.lib")

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<process.h>

#define DEF_PACKET_SIZE 32 //默认ping发送的数据包大小，32个字节
#define BUFFER_SIZE 4096  //缓冲区大小
#define ICMP_ECHO 8	//回显请求
#define ICMP_ECHOREPLY 0	//回显应答

char sendbuf[BUFFER_SIZE];//发送缓冲区
char recvbuf[BUFFER_SIZE];//接收缓冲区
struct sockaddr_in destAddr;//ICMP报文目的地址
struct sockaddr_in fromAddr;//
SOCKET sockfd;//套接字
int packetsize;//ICMP报文大小 8+32(默认)，ICMP固定格式8字节
int packet_no;//用于icmp_seq
int pid;//记录当前进程ID，函数_getpid()
int SEND_COUNT;//Echo数据报个数，默认为4

//计算校验和
unsigned short CheckSum(unsigned short* buffer, int size);
//初始化所需的变量
void InitPing();
//显示用户帮助信息
void UserHelp();
//获取用户提交的参数
void GetArguments(int argc, char** argv);
//填充ICMP报文字段,构造ICMP回显请求报文
void PackICMP(int pack_no);
//解读ICMP报文信息,buffer为接收ICMP报文的缓冲区,length为接收到的字节数
void DecodeICMP(char* buffer, int length);
//ping操作处理
void PingTest();

typedef struct _iphdr
{
	unsigned char ip_hl : 4;//首部长度
	unsigned char ip_verson : 4;//版本
	unsigned char ip_tos;//服务类型
	unsigned short ip_totallen;//IP数据报总长度
	unsigned short ip_id;//计数器，每产生一个数据报就加1，赋值给标识字段
	unsigned short ip_offset;//片偏移
	unsigned char ip_ttl;//Time To Live，生存时间，即数据报在网络中可通过的路由器数的最大值
	unsigned char ip_proto;//协议，指明分组的数据部分应该交给哪个运输层协议
	unsigned short ip_checksum;//首部校验和，只校验分组的首部，而不校验数据部分
	unsigned long ip_src;//源地址，标识发送方的IP地址
	unsigned long ip_dest;//目的地址，标识接收方的IP地址
}IPHeader;

typedef struct _icmp
{
	unsigned char icmp_type;//8位类型
	unsigned char icmp_code;//8位代码。ICMP报文类型由类型字段和代码字段共同决定
	unsigned short icmp_checksum;//校验和字段，覆盖整个ICMP报文
	unsigned short icmp_id;//标识符，发送端可任意设定；可置成发送进程的ID，区别于其他进程
	unsigned short icmp_seq;//序列号从0开始，每发送一次新的回显请求就加1。ping程序打印出返回的每个分组的序号，可查看分组是否丢失，重复或失序。
	unsigned long icmp_data;//携带数据
	//unsigned long timestamp;//时间戳
}ICMP;

//计算校验和，二进制反码求和
unsigned short CheckSum(unsigned short* buffer, int size)
{
	unsigned long cksum = 0;
	while (size > 1)
	{
		cksum += *buffer++;
		size -= sizeof(unsigned short);
	}
	if (size)
	{
		cksum += *(unsigned char*)buffer;
	}
	cksum = (cksum >> 16) + (cksum & 0xffff);
	cksum += (cksum >> 16);
	return (unsigned short)(~cksum);
}

void InitPing()
{
	//packetsize = DEF_PACKET_SIZE;
	packet_no = 0;
	pid = _getpid();
	SEND_COUNT = 4; //
}

void UserHelp()
{

}

void GetArguments(int argc, char** argv)
{

}

void PackICMP(int packet_no)
{
	
	//memset(sendbuf, 0, BUFFER_SIZE);
	ICMP* icmp = NULL;
	icmp = (ICMP*)sendbuf;
	icmp->icmp_type = ICMP_ECHO;//ICMP报文类型设置为回显请求
	icmp->icmp_code = 0;//8 0 回显请求
	icmp->icmp_checksum = 0;
	icmp->icmp_seq = packet_no;
	icmp->icmp_id = pid;//设置为当前进程ID
	icmp->icmp_data = GetTickCount();//返回操作系统启动到当前经过的毫秒数,ping程序通过在ICMP报文中存放发送请求的时间值来计算往返时间
	icmp->icmp_checksum = CheckSum((unsigned short*)icmp, packetsize);
}
//
void DecodeICMP(char* buffer, int length)
{
	IPHeader* ip = (IPHeader*)buffer;
	ICMP* icmp;
	double rtt;//往返时间
	int iphdrlen = ip->ip_hl * 4;//IP首部长度
	icmp = (ICMP*)(buffer + iphdrlen);

	//如果收到的不是ICMP回显应答报文，则输出错误信息
	if (icmp->icmp_type != ICMP_ECHOREPLY)
	{
		printf(" No ICMP Echo Reply received!  Error type: %d\n", icmp->icmp_type);
		return;
	}
	//如果接收到的ICMP不是当前进程的
	if (icmp->icmp_id != pid)
	{
		printf("someone else's pakcet!\n");
		return;
	}
	length = length - iphdrlen - sizeof(ICMP);
	rtt = GetTickCount() - icmp->icmp_data;//计算往返时间
	//打印回显应答信息
	printf("Reply form %s: bytes=%d  time=%.0fms  TTL=%d  icmp_seq=%u\n",
		inet_ntoa(fromAddr.sin_addr),
		length,
		rtt,
		ip->ip_ttl,
		icmp->icmp_seq);
}

void SendPacket()
{
	packetsize = sizeof(ICMP)+DEF_PACKET_SIZE;

	PackICMP(packet_no++);
	SOCKET writeret = sendto(sockfd, sendbuf, packetsize, 0, (struct sockaddr*)&destAddr, sizeof(destAddr));
	//如果发送不成功
	if (writeret == SOCKET_ERROR)
	{	//如果是由于超时不成功
		if (WSAGetLastError() == WSAETIMEDOUT)
		{
			printf("Request time out.\n");
			return;
		}
		//其他原因
		printf("sendto() failed. Error: %d\n", WSAGetLastError());
	}
}

void RecvPacket()
{
	int fromlen = sizeof(fromAddr);
	int recvret;//recvfrom返回接收到的字节数，错误则返回-1
	int success = 0;
	do
	{
		recvret = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*) & fromAddr, &fromlen);
		//printf("recvret = %d\n", recvret);
		//如果接收不成功
		if (recvret == SOCKET_ERROR)
		{
			//如果是由于超时不成功
			if (WSAGetLastError() == WSAETIMEDOUT)
			{
				printf("Request timed out.\n");
				return;
			}

			printf("recvfrom() failed. Error: %d\n", WSAGetLastError());
			return;
		}
		//解读ICMP报文
		success = 1;
		DecodeICMP(recvbuf, recvret);

	} while (success != 1);

	
}

void PingTest()
{
	struct hostent* host;
	struct protoent* protocol;
	WSADATA wsaData;//store the Windows Sockets data after being called by function WSAStartup 
	int timeout = 1000;//毫秒
	const char* par_host = "www.baidu.com";

	if (WSAStartup(0x1010, &wsaData) != 0)
	{//WSAStartup must be the first called Windows Sockets function
		printf("wsastartup error\n");
		exit(1);
	}
	if ((protocol = getprotobyname("icmp")) == NULL)
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

	memset(&destAddr, 0, sizeof(destAddr));//每个字节填充0
	destAddr.sin_family = AF_INET;//IPv4
	if (host = gethostbyname(par_host))
	{//gethostbyname函数可以利用字符串格式的域名获得IP地址,并且将地址信息装入hostent域名结构体
		//失败时返回NULL指针
		memcpy((char*)&destAddr.sin_addr, host->h_addr, host->h_length);//IP地址
		if (host = gethostbyaddr(host->h_addr, 4, PF_INET))//利用IP地址获得域名
			par_host = host->h_name;//获取正式主机名
	}
	else if (destAddr.sin_addr.s_addr = inet_addr(par_host) == INADDR_NONE)
	{
		printf("Unkown host %s\n", par_host);
		//exit(1);
		return;
	}

	//pid = _getpid();//用来获取当前进程的进程ID
	printf("Pinging %s [%s]: with %d bytes of data: \n\n", par_host, inet_ntoa(destAddr.sin_addr), DEF_PACKET_SIZE);
	for (int i = 0; i < SEND_COUNT; i++)
	{
		SendPacket();
		RecvPacket();
		Sleep(1000);
	}
	closesocket(sockfd);
	WSACleanup();
}

int main()
{
	InitPing();
	PingTest();
	return 0;
}