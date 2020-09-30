#include<iostream>
#include<cassert>
#include<ctime>
#include<vector>

#include "MemoryPool.h"
#include "StackAlloc.h"

#define ELEMS 10000000	//����Ԫ�ظ���
#define REPS 100	//�ظ�����

int main()
{
	clock_t clock_begin;
	
	//ʹ��STLĬ�Ϸ�����
	StackAlloc<int, std::allocator<int> > stackDefault;
	clock_begin = clock();
	for (int j = 0; j < REPS; j++)
	{
		assert(stackDefault.empty());
		for (int i = 0; i < ELEMS; i++)
			stackDefault.push(i);
		for (int i = 0; i < ELEMS; i++)
			stackDefault.pop();
	}
	clock_t clock_end = clock();
	std::cout << "Default Allocator Time: ";
	std::cout << (((double)clock_end - clock_begin) / CLOCKS_PER_SEC) << std::endl;

	//ʹ���ڴ��
	StackAlloc<int, MemoryPool<int> > stackPool;
	clock_begin = clock();
	for (int j = 0; j < REPS; j++)
	{
		assert(stackPool.empty());
		for (int i = 0; i < ELEMS; i++)
			stackPool.push(i);
		for (int i = 0; i < ELEMS; i++)
			stackPool.pop();
	}
	clock_end = clock();
	std::cout << "MemoryPool Allocator Time: ";
	std::cout << (((double)clock_end - clock_begin) / CLOCKS_PER_SEC) << std::endl;
	
	//ʹ��vector
	std::vector<int> stackVector;
	clock_begin = clock();
	for (int j = 0; j < REPS; j++)
	{
		assert(stackVector.empty());
		for (int i = 0; i < ELEMS; i++)
			stackVector.push_back(i);
		for (int i = 0; i < ELEMS; i++)
			stackVector.pop_back();
	}
	clock_end = clock();
	std::cout << "MemoryPool Allocator Time: ";
	std::cout << (((double)clock_end - clock_begin) / CLOCKS_PER_SEC) << std::endl;
	
	return 0;
}