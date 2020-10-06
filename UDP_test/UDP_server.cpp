//UDP��ֻ�д����׽��ֵĹ��̺����ݽ����Ĺ���
//UDP���ͺ���sendto()������ҪĿ���ַ��Ϣ
//UDP���պ���recvfrom()������Ҫ���Ͷ˵�ַ��Ϣ

#include<stdio.h>
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 100

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//�����׽���
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//���׽���
	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);//�Զ���ȡIP��ַ
	serverAddr.sin_port = htons(1234);
	bind(sock, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR));

	//���տͻ�������
	SOCKADDR clientAddr;//
	int nSize = sizeof(SOCKADDR);
	char buffer[BUF_SIZE];//������
	while (1)
	{
		int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &clientAddr, &nSize);
		buffer[strLen] = 0;
		printf("Message from client: %s\n", buffer);

		sendto(sock, buffer, strLen, 0, &clientAddr, nSize);
	}

	closesocket(sock);
	WSACleanup();
	return 0;
}