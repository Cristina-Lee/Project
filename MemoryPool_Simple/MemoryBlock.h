#pragma once

#ifndef MEMORYBLOCK_H
#define MEMORYBLOCK_H

#define  USHORT unsigned short
#define  ULONG unsigned long

struct MemoryBlock
{
	USHORT nSize;		//�ڴ���������ڴ���䵥Ԫ�ܴ�С
	USHORT nFree;		//�ڴ�������ɷ��䵥Ԫ�ĸ���
	USHORT nFirst;		//��һ���ɹ�����ĵ�Ԫ�ı��
	MemoryBlock* pNext;		//ָ����һ���ڴ��
	char aData[1];//��ַƫ�Ƶ�λ

	static void* operator new(size_t, const USHORT _nSum, const USHORT _nUnitSize)
	{//nSumΪ�ڴ�����ڴ���䵥Ԫ����,ȡֵnInitSize��nGrowSize
		return ::operator  new(sizeof(MemoryBlock) + _nSum * _nUnitSize);//����һƬ�������ڴ�ռ䣬����ͷ�ṹ�Ͷ���ڴ���䵥Ԫ
	}
	static void operator delete(void* p, size_t)
	{
		::operator delete(p);	//ɾ���ڴ��
	}
	MemoryBlock(const USHORT _nSum= 1, const USHORT _nUnitSize = 0):
		nSize(_nSum*_nUnitSize),
		nFree(_nSum-1),
		nFirst(1),	//������Ϻ�0�ŵ�Ԫ���̻ᱻ����
		pNext(nullptr)
	{
		aData[0] = '0';//????aData�ĳ�ʼ������
		char* pData = aData;	//pData��ʼָ��0�ŵ�Ԫ
		for (USHORT i = 1; i < _nSum; i++)
		{
			*reinterpret_cast<USHORT*>(pData) = i;
			pData += _nUnitSize;
		}

	}
	~MemoryBlock() {}
};


#endif