/*连续申请和连续释放*/
#include "MemoryBlock.h"
#include "MemoryPool.h"
//#include "MemoryPool.cpp"    //导致Link2005
#include<iostream>
#include<ctime>
using namespace std;

const int MAXLENGTH = 100000;	//测试个数
int* pArr[MAXLENGTH];//指针数组，存放指向int型的指针
int* cArr[MAXLENGTH];

int main()
{
	MemoryPool pool(6, 2048, 1024);
	//int* pArr[MAXLENGTH];//指针数组，存放指向int型的指针
	//int* cArr[MAXLENGTH];
	//堆栈溢出问题，
	
	cout << "/*测试1：连续申请和连续释放*/" << endl;
	clock_t clock_begin = clock();
	for (int i = 0; i < MAXLENGTH; i++)
		pArr[i] = static_cast<int*>(pool.Alloc());
	for (int i = 0; i < MAXLENGTH; i++)
	{
		pool.Free(pArr[i]);
		pArr[i] = nullptr;
	}
	clock_t clock_end = clock();
	cout <<"使用MemoryPool: "<< "程序运行了" << clock_end - clock_begin << "个系统时钟" << endl;
	
	clock_begin = clock();
	for (int i = 0; i < MAXLENGTH; i++)
		cArr[i] = new int;
	for (int i = 0; i < MAXLENGTH; i++)
	{
		delete cArr[i];
		cArr[i] = nullptr;
	}
	clock_end = clock();
	cout <<"使用默认函数： "<< "程序运行了" << clock_end - clock_begin << "个系统时钟" << endl;

	cout << endl;
	cout << "/*测试2：反复申请和释放小块内存*/" << endl;
	clock_begin = clock();
	for (int i = 0; i < MAXLENGTH; i++)
	{
		pArr[i] = static_cast<int*>(pool.Alloc());
		pool.Free(pArr[i]);
	}
	clock_end = clock();
	cout << "使用MemoryPool: " << "程序运行了" << clock_end - clock_begin << "个系统时钟" << endl;

	clock_begin = clock();		
	for (int i = 0; i < MAXLENGTH; i++)
	{
		cArr[i] = new int;
		delete cArr[i];
		cArr[i] = nullptr;
	}
	clock_end = clock();
	cout << "使用默认函数： " << "程序运行了" << clock_end - clock_begin << "个系统时钟" << endl;

	return 0;
}
