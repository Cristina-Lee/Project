#pragma once
#ifndef MEMORYPOOL_H 
#define MEMORYPOOL_H

#include "MemoryBlock.h"

class MemoryPool
{
private:
	MemoryBlock* pBlock;	//�ڴ�����ڴ������ı�ͷ
	USHORT nUnitSize;	//�ڴ���е��ڴ���䵥Ԫ�Ĵ�С
	USHORT nInitSize;	//��һ��������ڴ���е��ڴ���䵥Ԫ�ĸ���
	USHORT nGrowSize;		//�ٴ�������ڴ����ڴ���䵥Ԫ�ĸ���

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
