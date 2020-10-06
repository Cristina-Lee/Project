//UDP中只有创建套接字的过程和数据交换的过程
//UDP发送函数sendto()函数需要目标地址信息
//UDP接收函数recvfrom()函数需要发送端地址信息

#include<stdio.h>
#include<WinSock2.h>
#pragma comment(lib, "ws2_32.lib")

#define BUF_SIZE 100

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//创建套接字
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	//绑定套接字
	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);//自动获取IP地址
	serverAddr.sin_port = htons(1234);
	bind(sock, (SOCKADDR*)&serverAddr, sizeof(SOCKADDR));

	//接收客户端请求
	SOCKADDR clientAddr;//
	int nSize = sizeof(SOCKADDR);
	char buffer[BUF_SIZE];//缓冲区
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