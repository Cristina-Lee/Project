#include<iostream>
#include<cassert>
#include<ctime>
#include<vector>

#include "MemoryPool.h"
#include "StackAlloc.h"

#define ELEMS 10000000	//插入元素个数
#define REPS 100	//重复次数

int main()
{
	clock_t clock_begin;
	
	//使用STL默认分配器
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

	//使用内存池
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
	
	//使用vector
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