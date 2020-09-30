/*��������������ͷ�*/
#include "MemoryBlock.h"
#include "MemoryPool.h"
//#include "MemoryPool.cpp"    //����Link2005
#include<iostream>
#include<ctime>
using namespace std;

const int MAXLENGTH = 100000;	//���Ը���
int* pArr[MAXLENGTH];//ָ�����飬���ָ��int�͵�ָ��
int* cArr[MAXLENGTH];

int main()
{
	MemoryPool pool(6, 2048, 1024);
	//int* pArr[MAXLENGTH];//ָ�����飬���ָ��int�͵�ָ��
	//int* cArr[MAXLENGTH];
	//��ջ������⣬
	
	cout << "/*����1����������������ͷ�*/" << endl;
	clock_t clock_begin = clock();
	for (int i = 0; i < MAXLENGTH; i++)
		pArr[i] = static_cast<int*>(pool.Alloc());
	for (int i = 0; i < MAXLENGTH; i++)
	{
		pool.Free(pArr[i]);
		pArr[i] = nullptr;
	}
	clock_t clock_end = clock();
	cout <<"ʹ��MemoryPool: "<< "����������" << clock_end - clock_begin << "��ϵͳʱ��" << endl;
	
	clock_begin = clock();
	for (int i = 0; i < MAXLENGTH; i++)
		cArr[i] = new int;
	for (int i = 0; i < MAXLENGTH; i++)
	{
		delete cArr[i];
		cArr[i] = nullptr;
	}
	clock_end = clock();
	cout <<"ʹ��Ĭ�Ϻ����� "<< "����������" << clock_end - clock_begin << "��ϵͳʱ��" << endl;

	cout << endl;
	cout << "/*����2������������ͷ�С���ڴ�*/" << endl;
	clock_begin = clock();
	for (int i = 0; i < MAXLENGTH; i++)
	{
		pArr[i] = static_cast<int*>(pool.Alloc());
		pool.Free(pArr[i]);
	}
	clock_end = clock();
	cout << "ʹ��MemoryPool: " << "����������" << clock_end - clock_begin << "��ϵͳʱ��" << endl;

	clock_begin = clock();		
	for (int i = 0; i < MAXLENGTH; i++)
	{
		cArr[i] = new int;
		delete cArr[i];
		cArr[i] = nullptr;
	}
	clock_end = clock();
	cout << "ʹ��Ĭ�Ϻ����� " << "����������" << clock_end - clock_begin << "��ϵͳʱ��" << endl;

	return 0;
}
