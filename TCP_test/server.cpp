#include<stdio.h>
#include<Winsock2.h>
#include<WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")  //加载ws2_32.lib

int main()
{
	//初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	//创建套接字
	SOCKET serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//绑定套接字
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));//每个字节都用0填充
	sockAddr.sin_family = PF_INET;//使用IPv地址
	inet_pton(AF_INET, "192.168.0.105", &sockAddr.sin_addr);
	sockAddr.sin_port = htons(1234);//端口，htons()作用是将端口号由主机字节序转换为网络字节序的整数值
	bind(serverSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));//服务器套接字绑定自己的IP地址与端口号

	//进入监听状态
	listen(serverSock, 20);
	//被动连接的一方调用listen以接收connect的连接请求，即服务器

	//接收客户端请求
	SOCKADDR clientAddr;
	int nSize = sizeof(SOCKADDR);
	SOCKET clientSock = accept(serverSock, (SOCKADDR*)&clientAddr, &nSize);
	//accept()主要用于服务端，一般位于listen函数之后，默认会阻塞进程，直到有一个客户请求连接，
	//建立好

	//向客户端发送是数据
	const char* str = "Hello World!";
	send(clientSock, str, strlen(str) + sizeof(char), NULL);

	//关闭套接字
	closesocket(clientSock);
	closesocket(serverSock);

	//终止DLL的使用
	WSACleanup();

	return 0;
}