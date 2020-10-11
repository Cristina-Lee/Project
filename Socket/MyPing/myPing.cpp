#pragma comment(lib, "ws2_32.lib")

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<WinSock2.h>
#include<ws2tcpip.h>
#include<process.h>

#define DEF_PACKET_SIZE 32 //Ĭ��ping���͵����ݰ���С��32���ֽ�
#define BUFFER_SIZE 4096  //��������С
#define ICMP_ECHO 8	//��������
#define ICMP_ECHOREPLY 0	//����Ӧ��

char sendbuf[BUFFER_SIZE];//���ͻ�����
char recvbuf[BUFFER_SIZE];//���ջ�����
struct sockaddr_in destAddr;//ICMP����Ŀ�ĵ�ַ
struct sockaddr_in fromAddr;//
SOCKET sockfd;//�׽���
int packetsize;//ICMP���Ĵ�С 8+32(Ĭ��)��ICMP�̶���ʽ8�ֽ�
int packet_no;//����icmp_seq
int pid;//��¼��ǰ����ID������_getpid()
int SEND_COUNT;//Echo���ݱ�������Ĭ��Ϊ4

//����У���
unsigned short CheckSum(unsigned short* buffer, int size);
//��ʼ������ı���
void InitPing();
//��ʾ�û�������Ϣ
void UserHelp();
//��ȡ�û��ύ�Ĳ���
void GetArguments(int argc, char** argv);
//���ICMP�����ֶ�,����ICMP����������
void PackICMP(int pack_no);
//���ICMP������Ϣ,bufferΪ����ICMP���ĵĻ�����,lengthΪ���յ����ֽ���
void DecodeICMP(char* buffer, int length);
//ping��������
void PingTest();

typedef struct _iphdr
{
	unsigned char ip_hl : 4;//�ײ�����
	unsigned char ip_verson : 4;//�汾
	unsigned char ip_tos;//��������
	unsigned short ip_totallen;//IP���ݱ��ܳ���
	unsigned short ip_id;//��������ÿ����һ�����ݱ��ͼ�1����ֵ����ʶ�ֶ�
	unsigned short ip_offset;//Ƭƫ��
	unsigned char ip_ttl;//Time To Live������ʱ�䣬�����ݱ��������п�ͨ����·�����������ֵ
	unsigned char ip_proto;//Э�飬ָ����������ݲ���Ӧ�ý����ĸ������Э��
	unsigned short ip_checksum;//�ײ�У��ͣ�ֻУ�������ײ�������У�����ݲ���
	unsigned long ip_src;//Դ��ַ����ʶ���ͷ���IP��ַ
	unsigned long ip_dest;//Ŀ�ĵ�ַ����ʶ���շ���IP��ַ
}IPHeader;

typedef struct _icmp
{
	unsigned char icmp_type;//8λ����
	unsigned char icmp_code;//8λ���롣ICMP���������������ֶκʹ����ֶι�ͬ����
	unsigned short icmp_checksum;//У����ֶΣ���������ICMP����
	unsigned short icmp_id;//��ʶ�������Ͷ˿������趨�����óɷ��ͽ��̵�ID����������������
	unsigned short icmp_seq;//���кŴ�0��ʼ��ÿ����һ���µĻ�������ͼ�1��ping�����ӡ�����ص�ÿ���������ţ��ɲ鿴�����Ƿ�ʧ���ظ���ʧ��
	unsigned long icmp_data;//Я������
	//unsigned long timestamp;//ʱ���
}ICMP;

//����У��ͣ������Ʒ������
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
	icmp->icmp_type = ICMP_ECHO;//ICMP������������Ϊ��������
	icmp->icmp_code = 0;//8 0 ��������
	icmp->icmp_checksum = 0;
	icmp->icmp_seq = packet_no;
	icmp->icmp_id = pid;//����Ϊ��ǰ����ID
	icmp->icmp_data = GetTickCount();//���ز���ϵͳ��������ǰ�����ĺ�����,ping����ͨ����ICMP�����д�ŷ��������ʱ��ֵ����������ʱ��
	icmp->icmp_checksum = CheckSum((unsigned short*)icmp, packetsize);
}
//
void DecodeICMP(char* buffer, int length)
{
	IPHeader* ip = (IPHeader*)buffer;
	ICMP* icmp;
	double rtt;//����ʱ��
	int iphdrlen = ip->ip_hl * 4;//IP�ײ�����
	icmp = (ICMP*)(buffer + iphdrlen);

	//����յ��Ĳ���ICMP����Ӧ���ģ������������Ϣ
	if (icmp->icmp_type != ICMP_ECHOREPLY)
	{
		printf(" No ICMP Echo Reply received!  Error type: %d\n", icmp->icmp_type);
		return;
	}
	//������յ���ICMP���ǵ�ǰ���̵�
	if (icmp->icmp_id != pid)
	{
		printf("someone else's pakcet!\n");
		return;
	}
	length = length - iphdrlen - sizeof(ICMP);
	rtt = GetTickCount() - icmp->icmp_data;//��������ʱ��
	//��ӡ����Ӧ����Ϣ
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
	//������Ͳ��ɹ�
	if (writeret == SOCKET_ERROR)
	{	//��������ڳ�ʱ���ɹ�
		if (WSAGetLastError() == WSAETIMEDOUT)
		{
			printf("Request time out.\n");
			return;
		}
		//����ԭ��
		printf("sendto() failed. Error: %d\n", WSAGetLastError());
	}
}

void RecvPacket()
{
	int fromlen = sizeof(fromAddr);
	int recvret;//recvfrom���ؽ��յ����ֽ����������򷵻�-1
	int success = 0;
	do
	{
		recvret = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr*) & fromAddr, &fromlen);
		//printf("recvret = %d\n", recvret);
		//������ղ��ɹ�
		if (recvret == SOCKET_ERROR)
		{
			//��������ڳ�ʱ���ɹ�
			if (WSAGetLastError() == WSAETIMEDOUT)
			{
				printf("Request timed out.\n");
				return;
			}

			printf("recvfrom() failed. Error: %d\n", WSAGetLastError());
			return;
		}
		//���ICMP����
		success = 1;
		DecodeICMP(recvbuf, recvret);

	} while (success != 1);

	
}

void PingTest()
{
	struct hostent* host;
	struct protoent* protocol;
	WSADATA wsaData;//store the Windows Sockets data after being called by function WSAStartup 
	int timeout = 1000;//����
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

	memset(&destAddr, 0, sizeof(destAddr));//ÿ���ֽ����0
	destAddr.sin_family = AF_INET;//IPv4
	if (host = gethostbyname(par_host))
	{//gethostbyname�������������ַ�����ʽ���������IP��ַ,���ҽ���ַ��Ϣװ��hostent�����ṹ��
		//ʧ��ʱ����NULLָ��
		memcpy((char*)&destAddr.sin_addr, host->h_addr, host->h_length);//IP��ַ
		if (host = gethostbyaddr(host->h_addr, 4, PF_INET))//����IP��ַ�������
			par_host = host->h_name;//��ȡ��ʽ������
	}
	else if (destAddr.sin_addr.s_addr = inet_addr(par_host) == INADDR_NONE)
	{
		printf("Unkown host %s\n", par_host);
		//exit(1);
		return;
	}

	//pid = _getpid();//������ȡ��ǰ���̵Ľ���ID
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