#include<stdio.h>
#include<Winsock2.h>
#include<WS2tcpip.h>

#pragma comment (lib, "ws2_32.lib")  //����ws2_32.lib

int main()
{
	//��ʼ��DLL
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	
	//�����׽���
	SOCKET serverSock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	//���׽���
	sockaddr_in sockAddr;
	memset(&sockAddr, 0, sizeof(sockAddr));//ÿ���ֽڶ���0���
	sockAddr.sin_family = PF_INET;//ʹ��IPv��ַ
	inet_pton(AF_INET, "192.168.0.105", &sockAddr.sin_addr);
	sockAddr.sin_port = htons(1234);//�˿ڣ�htons()�����ǽ��˿ں��������ֽ���ת��Ϊ�����ֽ��������ֵ
	bind(serverSock, (SOCKADDR*)&sockAddr, sizeof(SOCKADDR));//�������׽��ְ��Լ���IP��ַ��˿ں�

	//�������״̬
	listen(serverSock, 20);
	//�������ӵ�һ������listen�Խ���connect���������󣬼�������

	//���տͻ�������
	SOCKADDR clientAddr;
	int nSize = sizeof(SOCKADDR);
	SOCKET clientSock = accept(serverSock, (SOCKADDR*)&clientAddr, &nSize);
	//accept()��Ҫ���ڷ���ˣ�һ��λ��listen����֮��Ĭ�ϻ��������̣�ֱ����һ���ͻ��������ӣ�
	//������

	//��ͻ��˷���������
	const char* str = "Hello World!";
	send(clientSock, str, strlen(str) + sizeof(char), NULL);

	//�ر��׽���
	closesocket(clientSock);
	closesocket(serverSock);

	//��ֹDLL��ʹ��
	WSACleanup();

	return 0;
}