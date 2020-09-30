#include "MemoryPool.h"

const USHORT MEMPOOL_ALIGNMENT = 2;

MemoryPool::MemoryPool(const USHORT _nUnitSize, const USHORT _nInitSize, const USHORT _nGrowSize)
{
	pBlock = nullptr;	//MemoryPool创建时内存块链表为空
	nInitSize = _nInitSize;
	nGrowSize = _nGrowSize;

	if (_nUnitSize > 4)
		nUnitSize = (_nUnitSize + (MEMPOOL_ALIGNMENT - 1)) & ~(MEMPOOL_ALIGNMENT - 1);
	//nUnitSize取值 大于_nUnitSize的最小的MEMPOOL_ALIGNMENT的倍数
	else if (_nUnitSize <= 2)
		nUnitSize = 2;
	else
		nUnitSize = 4;
}

MemoryPool::~MemoryPool()
{//销毁整个内存池
	MemoryBlock* pMyBlock = pBlock;
	while (pMyBlock)
	{
		pBlock = pMyBlock->pNext;
		delete pMyBlock;
		pMyBlock = pBlock;
	}
}

void* MemoryPool::Alloc()
{
	if (!pBlock)	//内存池 第一次申请一个内存块
	{
		MemoryBlock* pNewBlock = new(nInitSize, nUnitSize)MemoryBlock(nInitSize, nUnitSize);
		if (!pNewBlock)
			return nullptr;
		pBlock = pNewBlock;
		
		return (void*)pNewBlock->aData;
	}
	MemoryBlock* pMyBlock = pBlock;
	while (pMyBlock && !pMyBlock->nFree)
		pMyBlock = pMyBlock->pNext;//内存块链表不为空时遍历，找到有自由分配单元的内存块
	if (pMyBlock)
	{
		char* pFree = pMyBlock->aData + (pMyBlock->nFirst * nUnitSize);
		pMyBlock->nFirst = *((USHORT*)pFree);//下一个自由分配单元编号，pFree指向的分配单元的头两个字节
		pMyBlock->nFree--;
		return (void*)pFree;
	}
	else//内存链表为空或者所有内存块都没有自由分配单元
	{
		if (!nGrowSize)
			return nullptr;
		pMyBlock = new (nGrowSize, nUnitSize)MemoryBlock(nGrowSize, nUnitSize);
		if (!pMyBlock)
			return nullptr;
		pMyBlock->pNext = pBlock;//头插法
		pBlock = pMyBlock;
		return (void*)(pMyBlock->aData);
	}
}

void MemoryPool::Free(void* pFree)
{//回收内存，pFree指向待回收的内存分配单元
	MemoryBlock* pMyBlock = pBlock;
	MemoryBlock* pPreMyBlock = pBlock;
	while (((ULONG)pMyBlock->aData > (ULONG)pFree) || ((ULONG)pFree >= ((ULONG)pMyBlock->aData + pMyBlock->nSize)))
	{
		pPreMyBlock = pMyBlock;//pMyBlock的前一个内存块
		pMyBlock = pMyBlock->pNext;
	}
	if (!pMyBlock)
		return; //不属于此内存池范围，不可返回给此内存池

	//确定了pFree在某个内存块地址范围中
	//将此回收单元放在自由分配单元链表的头部
	pMyBlock->nFree++;//自由分配单元数加1
	*((USHORT*)pFree) = pMyBlock->nFirst;
	pMyBlock->nFirst = (USHORT)(((ULONG)pFree - (ULONG)(pMyBlock->aData)) / nUnitSize);

	//若此内存块全部空闲,将此内存块移出MemoryPool
	if (pMyBlock->nFree * nUnitSize == pMyBlock->nSize)
	{
		if (pMyBlock == pBlock)
		{
			pBlock = pBlock->pNext;
			delete pPreMyBlock;
			pPreMyBlock = nullptr;
			pMyBlock = nullptr;
		}
		else
		{
			pPreMyBlock->pNext = pMyBlock->pNext;
			delete pMyBlock;
			pMyBlock = nullptr;
		}
	}
	else
	{//内存块中还有非自由分配单元，则将其移至MemoryPool维护的内存块链表的头部
		if (pMyBlock != pBlock)
		{
			pPreMyBlock->pNext = pMyBlock->pNext;
			pMyBlock->pNext = pBlock;
			pBlock = pMyBlock;
		}		
	}
	pFree = nullptr;
}
