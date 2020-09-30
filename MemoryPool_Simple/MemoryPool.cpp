#include "MemoryPool.h"

const USHORT MEMPOOL_ALIGNMENT = 2;

MemoryPool::MemoryPool(const USHORT _nUnitSize, const USHORT _nInitSize, const USHORT _nGrowSize)
{
	pBlock = nullptr;	//MemoryPool����ʱ�ڴ������Ϊ��
	nInitSize = _nInitSize;
	nGrowSize = _nGrowSize;

	if (_nUnitSize > 4)
		nUnitSize = (_nUnitSize + (MEMPOOL_ALIGNMENT - 1)) & ~(MEMPOOL_ALIGNMENT - 1);
	//nUnitSizeȡֵ ����_nUnitSize����С��MEMPOOL_ALIGNMENT�ı���
	else if (_nUnitSize <= 2)
		nUnitSize = 2;
	else
		nUnitSize = 4;
}

MemoryPool::~MemoryPool()
{//���������ڴ��
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
	if (!pBlock)	//�ڴ�� ��һ������һ���ڴ��
	{
		MemoryBlock* pNewBlock = new(nInitSize, nUnitSize)MemoryBlock(nInitSize, nUnitSize);
		if (!pNewBlock)
			return nullptr;
		pBlock = pNewBlock;
		
		return (void*)pNewBlock->aData;
	}
	MemoryBlock* pMyBlock = pBlock;
	while (pMyBlock && !pMyBlock->nFree)
		pMyBlock = pMyBlock->pNext;//�ڴ������Ϊ��ʱ�������ҵ������ɷ��䵥Ԫ���ڴ��
	if (pMyBlock)
	{
		char* pFree = pMyBlock->aData + (pMyBlock->nFirst * nUnitSize);
		pMyBlock->nFirst = *((USHORT*)pFree);//��һ�����ɷ��䵥Ԫ��ţ�pFreeָ��ķ��䵥Ԫ��ͷ�����ֽ�
		pMyBlock->nFree--;
		return (void*)pFree;
	}
	else//�ڴ�����Ϊ�ջ��������ڴ�鶼û�����ɷ��䵥Ԫ
	{
		if (!nGrowSize)
			return nullptr;
		pMyBlock = new (nGrowSize, nUnitSize)MemoryBlock(nGrowSize, nUnitSize);
		if (!pMyBlock)
			return nullptr;
		pMyBlock->pNext = pBlock;//ͷ�巨
		pBlock = pMyBlock;
		return (void*)(pMyBlock->aData);
	}
}

void MemoryPool::Free(void* pFree)
{//�����ڴ棬pFreeָ������յ��ڴ���䵥Ԫ
	MemoryBlock* pMyBlock = pBlock;
	MemoryBlock* pPreMyBlock = pBlock;
	while (((ULONG)pMyBlock->aData > (ULONG)pFree) || ((ULONG)pFree >= ((ULONG)pMyBlock->aData + pMyBlock->nSize)))
	{
		pPreMyBlock = pMyBlock;//pMyBlock��ǰһ���ڴ��
		pMyBlock = pMyBlock->pNext;
	}
	if (!pMyBlock)
		return; //�����ڴ��ڴ�ط�Χ�����ɷ��ظ����ڴ��

	//ȷ����pFree��ĳ���ڴ���ַ��Χ��
	//���˻��յ�Ԫ�������ɷ��䵥Ԫ�����ͷ��
	pMyBlock->nFree++;//���ɷ��䵥Ԫ����1
	*((USHORT*)pFree) = pMyBlock->nFirst;
	pMyBlock->nFirst = (USHORT)(((ULONG)pFree - (ULONG)(pMyBlock->aData)) / nUnitSize);

	//�����ڴ��ȫ������,�����ڴ���Ƴ�MemoryPool
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
	{//�ڴ���л��з����ɷ��䵥Ԫ����������MemoryPoolά�����ڴ�������ͷ��
		if (pMyBlock != pBlock)
		{
			pPreMyBlock->pNext = pMyBlock->pNext;
			pMyBlock->pNext = pBlock;
			pBlock = pMyBlock;
		}		
	}
	pFree = nullptr;
}
