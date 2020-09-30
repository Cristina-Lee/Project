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

//T为存储的对象类型，默认使用std::allocator作为对象的分配器
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
	{//释放栈中元素的所有内存
		Node* curr = head_;
		while (curr!=nullptr)
		{
			Node* tmp = curr->prev;//引发了异常：读取访问权限冲突；curr是nullptr
			allocator_.destroy(curr);
			allocator_.deallocate(curr, 1);
			curr = tmp;
		}
		head_ = nullptr;
	}
	void push(T element)
	{
		Node* newNode = allocator_.allocate(1);//为一个节点分配内存
		allocator_.construct(newNode, Node());//调用节点的构造函数

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