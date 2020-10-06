#include<stdio.h>
#include<WinSock2.h>
#include<WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define _CRT_SECURE_NO_WARNINGS
#define BUF_SIZE 100

int main()
{
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);

	sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);
	serverAddr.sin_port = htons(1234);

	sockaddr fromAddr;
	int addrLen = sizeof(fromAddr);
	while (1)
	{
		char buffer[BUF_SIZE] = { 0 };
		printf("Input a string: ");
		gets_s(buffer, BUF_SIZE);
		sendto(sock, buffer, strlen(buffer), 0, (sockaddr*)&serverAddr, sizeof(serverAddr));

		int strLen = recvfrom(sock, buffer, BUF_SIZE, 0, &fromAddr, &addrLen);
		buffer[strLen] = 0;
		printf("Message from server: %s\n", buffer);
	}
	closesocket(sock);
	WSACleanup();
	return 0;
}