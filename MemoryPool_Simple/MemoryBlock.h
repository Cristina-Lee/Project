#pragma once

#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

#define  USHORT unsigned short
#define  ULONG unsigned long

struct MemoryBlock
{
	USHORT nSize;		//内存块中所有内存分配单元总大小
	USHORT nFree;		//内存块中自由分配单元的个数
	USHORT nFirst;		//下一个可供分配的单元的编号
	MemoryBlock* pNext;		//指向下一个内存块
	char aData[1];//地址偏移地位

	static void* operator new(size_t, const USHORT _nSum, const USHORT _nUnitSize)
	{//nSum为内存块中内存分配单元个数,取值nInitSize或nGrowSize
		return ::operator  new(sizeof(MemoryBlock) + _nSum * _nUnitSize);//申请一片连续的内存空间，包括头结构和多个内存分配单元
	}
	static void operator delete(void* p, size_t)
	{
		::operator delete(p);	//删除内存块
	}
	MemoryBlock(const USHORT _nSum= 1, const USHORT _nUnitSize = 0):
		nSize(_nSum*_nUnitSize),
		nFree(_nSum-1),
		nFirst(1),	//构造完毕后0号单元立刻会被分配
		pNext(nullptr)
	{
		aData[0] = '0';//????aData的初始化问题
		char* pData = aData;	//pData初始指向0号单元
		for (USHORT i = 1; i < _nSum; i++)
		{
			*reinterpret_cast<USHORT*>(pData) = i;
			pData += _nUnitSize;
		}

	}
	~MemoryBlock() {}
};


#endif