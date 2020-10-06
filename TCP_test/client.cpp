#include<stdio.h>
#include<stdlib.h>
#include<Winsock2.h>
#include<WS2tcpip.h>

//ws2_32.lib对应ws2_32.dll，提供了对网络相关API的支持，若使用其中的API，则应该将ws2_32.lib加入工程，否则需要动态载入ws2_32.dll
#pragma comment(lib, "ws2_32.lib")  

int main()
{
	//初始化DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//创建套接字
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//打开一个网络连接，成功则返回一个文件句柄,之后操作这个网络连接都通过这个文件句柄

	//向服务器发起请求
	sockaddr_in sockAddr;//sockaddr_in是internet环境下套接字的地址形式
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	//sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//inet_addr()作用是将一个IP字符串转化为一个网络字节序的整数值
	inet_pton(AF_INET, "192.168.0.105", &sockAddr.sin_addr);
	sockAddr.sin_port = htons(1234);
	connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	//connect函数用于客户端建立TCP连接，connect由主动连接的一方调用
	//第一个参数sockfd：指定数据发送的套接字，解决从哪里发送的问题
	//第二个参数server_addr:指定数据发送的目的地，也就是服务器端的地址。

	//接收服务器传回的数据
	char szBuffer[MAXBYTE] = { 0 };
	recv(sock, szBuffer, MAXBYTE, NULL);//不论是客户还是服务器应用程序都用recv函数从TCP连接的另一端接收数据

	//输出接收到的数据
	printf("Message from server: %s\n", szBuffer);
	
	//关闭套接字
	closesocket(sock);

	//终止使用DLL
	WSACleanup();
	
	system("pause");

	return 0;


}