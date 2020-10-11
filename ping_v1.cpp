#pragma comment(lib, "ws2_32.lib")

#include<stdio.h>
#include<windows.h>
//#include<WinSock2.h>
#include<process.h>

#define SEND_SIZE 32	//��Ĭ�������Windows��ping���͵����ݰ���СΪ32byte�����Զ����С�������ó���65500byte
#define PACKET_SIZE 4096
#define ICMP_ECHO 8//��������
#define ICMP_ECHOREPLY 0//����Ӧ��
//#define _WINSOCK_DEPRECATED_NO_WARNINGS 
struct icmp
{
	unsigned char icmp_type;//message type,8bit
	unsigned char icmp_code;//
	unsigned short icmp_cksum;//checksum, 16bit
	unsigned short icmp_id;//identification, 16bit
	unsigned short icmp_seq;//sequence��16bit, the initial value is zero,each time a new echo request is sent, one is added.
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
struct sockaddr_in dest_addr;//internet�������׽��ֵĵ�ַ��ʽ
struct sockaddr_in from_addr;
SOCKET sockfd;//�׽���
int pid;

unsigned short cal_chksum(unsigned short* addr, int len);//����У���
int pack(int pack_no);
int unpack(char* buf, int len);
void send_packet(void);
void recv_packet(void);

int main(int argc, char* argv[])
{
	struct hostent* host;//host entry,�����������ַ�ṹ��
	struct protoent* protocol;//offer official protocal name, Alias list, protoco number 
	WSADATA wsaData;//store the Windows Sockets data after being called by function WSAStartup 
	int timeout = 1000;//����
	int SEND_COUNT = 4;//Echo���ݱ�����Ĭ��Ϊ4
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
	{//socket()����ʧ�ܷ���-1,�������Ҫ�Թ���Ա������У�common7/IDE/devenv.exe
		//ʹ��ԭʼ�׽��ֿ��Զ�дICMP��ping�������ʹ��ԭʼ�׽��ַ���ICMP��������
		//����ԭʼ�׽ӿں󣬿���ͨ������������д�Լ���IP���ݰ���Ϊ�˷�ֹ�Ƿ��û��ƻ����磬�涨ֻ�г����û����д���ԭʼ�׽ӿڵ�Ȩ��
		printf("socket error\n");
		exit(1);
	}
	//�ڷ��ͺͽ������ݵĹ����У���ʱ��������״����ԭ�򣬷��ղ���Ԥ�ڽ��У��������շ�ʱ��
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
		fprintf(stderr, "failed to set recv timeout: %d\n", WSAGetLastError());
	if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, (char*)&timeout, sizeof(timeout)) < 0)
		fprintf(stderr, "failed to set send timeout: %d\n", WSAGetLastError());
	
	memset(&dest_addr, 0, sizeof(dest_addr));//ÿ���ֽ����0
	dest_addr.sin_family = AF_INET;//IPv4
	if (host = gethostbyname(par_host))
	{//gethostbyname�������������ַ�����ʽ���������IP��ַ,���ҽ���ַ��Ϣװ��hostent�����ṹ��
		//ʧ��ʱ����NULLָ��
		memcpy((char*)&dest_addr.sin_addr, host->h_addr, host->h_length);
		if (host = gethostbyaddr(host->h_addr, 4, PF_INET))//����IP��ַ�������
			par_host = host->h_name;//��ȡ��ʽ������
	}
	else if (dest_addr.sin_addr.s_addr = inet_addr(par_host) == INADDR_NONE)
	{
		printf("Unkown host %s\n", par_host);
		exit(1);
	}

	pid = _getpid();//������ȡ��ǰ���̵Ľ���ID
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
//�����Ʒ������
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

//����ICMP����������
int pack(int pack_no)
{
	int packsize;
	struct icmp* icmp;
	packsize = 8 + SEND_SIZE;//ICMPͷ�ṹ8byte
	icmp = (struct icmp*)sendpacket;//���ͻ�����
	icmp->icmp_type = ICMP_ECHO;
	icmp->icmp_code = 0;
	icmp->icmp_cksum = 0;
	icmp->icmp_seq = pack_no;
	icmp->icmp_id = pid;//
	icmp->icmp_data = GetTickCount();//���شӲ���ϵͳ��������ǰ�����ĺ�����
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
	iphdrlen = ip->ip_hl * 4;//IP�ײ�������4BΪ��λ
	icmp = (struct icmp*)(buf + iphdrlen);
	if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == pid))//���˻���Ӧ����
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
	{//recvfrom���ؽ��յ����ֽ����������򷵻�-1��
		if ((n = recvfrom(sockfd, recvpacket, sizeof(recvpacket), 0, (struct sockaddr*) & from_addr, &fromlen)) >= 0)
			success = unpack(recvpacket, n);
		else if (WSAGetLastError() == WSAETIMEDOUT)
		{
			printf("Request timed out.\n");//Ϊʲô���ǳ�ʱ��
			return;
		}
	} while (!success);

}