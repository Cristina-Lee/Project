#pragma once
#ifndef STACKALLOC_H
#define STACKALLOC_H

#include<memory>

template<typename T>
struct StackNode_
{
	T data;
	StackNode_* prev;
};

//TΪ�洢�Ķ������ͣ�Ĭ��ʹ��std::allocator��Ϊ����ķ�����
template<typename T, typename Alloc=std::allocator<T> >
class StackAlloc
{
public:
	typedef StackNode_<T> Node;
	typedef typename Alloc::template rebind<Node>::other allocator;

	StackAlloc() { head_ = nullptr; }
	~StackAlloc() { clear(); }

	bool empty() { return (head_ == nullptr); }
	void clear()
	{//�ͷ�ջ��Ԫ�ص������ڴ�
		Node* curr = head_;
		while (curr!=nullptr)
		{
			Node* tmp = curr->prev;//�������쳣����ȡ����Ȩ�޳�ͻ��curr��nullptr
			allocator_.destroy(curr);
			allocator_.deallocate(curr, 1);
			curr = tmp;
		}
		head_ = nullptr;
	}
	void push(T element)
	{
		Node* newNode = allocator_.allocate(1);//Ϊһ���ڵ�����ڴ�
		allocator_.construct(newNode, Node());//���ýڵ�Ĺ��캯��

		newNode->data = element;
		newNode->prev = head_;
		head_ = newNode;
	}
	T pop()
	{
		T result = head_->data;
		Node* tmp = head_->prev;
		allocator_.destroy(head_);
		allocator_.deallocate(head_, 1);
		head_ = tmp;
		return result;
	}
	T top() { return (head_->data); }
private:
	allocator allocator_;
	Node* head_;
};

#endif