#include<stdio.h>
#include<stdlib.h>
#include<Winsock2.h>
#include<WS2tcpip.h>

//ws2_32.lib��Ӧws2_32.dll���ṩ�˶��������API��֧�֣���ʹ�����е�API����Ӧ�ý�ws2_32.lib���빤�̣�������Ҫ��̬����ws2_32.dll
#pragma comment(lib, "ws2_32.lib")  

int main()
{
	//��ʼ��DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);

	//�����׽���
	SOCKET sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);//��һ���������ӣ��ɹ��򷵻�һ���ļ����,֮���������������Ӷ�ͨ������ļ����

	//���������������
	sockaddr_in sockAddr;//sockaddr_in��internet�������׽��ֵĵ�ַ��ʽ
	memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sin_family = PF_INET;
	//sockAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//inet_addr()�����ǽ�һ��IP�ַ���ת��Ϊһ�������ֽ��������ֵ
	inet_pton(AF_INET, "192.168.0.105", &sockAddr.sin_addr);
	sockAddr.sin_port = htons(1234);
	connect(sock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));
	//connect�������ڿͻ��˽���TCP���ӣ�connect���������ӵ�һ������
	//��һ������sockfd��ָ�����ݷ��͵��׽��֣���������﷢�͵�����
	//�ڶ�������server_addr:ָ�����ݷ��͵�Ŀ�ĵأ�Ҳ���Ƿ������˵ĵ�ַ��

	//���շ��������ص�����
	char szBuffer[MAXBYTE] = { 0 };
	recv(sock, szBuffer, MAXBYTE, NULL);//�����ǿͻ����Ƿ�����Ӧ�ó�����recv������TCP���ӵ���һ�˽�������

	//������յ�������
	printf("Message from server: %s\n", szBuffer);
	
	//�ر��׽���
	closesocket(sock);

	//��ֹʹ��DLL
	WSACleanup();
	
	system("pause");

	return 0;


}