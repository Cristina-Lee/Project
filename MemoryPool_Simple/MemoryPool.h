#pragma once
#ifndef MEMORYPOOL_H 
#define MEMORYPOOL_H

#include "MemoryBlock.h"

class MemoryPool
{
private:
	MemoryBlock* pBlock;	//内存池中内存块链表的表头
	USHORT nUnitSize;	//内存块中的内存分配单元的大小
	USHORT nInitSize;	//第一次申请的内存块中的内存分配单元的个数
	USHORT nGrowSize;		//再次申请的内存块的内存分配单元的个数

public:
	MemoryPool(const USHORT _nUnitSize, const USHORT _nInitsize=1024, const USHORT _nGrowSize=256);
	//destructor
	~MemoryPool();
	
	//self-defined memory  allocation function
	void* Alloc();

	//release memory
	void Free(void* pFree);
	
};

#endif
