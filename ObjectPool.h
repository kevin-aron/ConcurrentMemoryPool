#pragma once
#include<iostream>
#include<vector>
#include<time.h>
using std::cout;
using std::endl;

#ifdef _WIN32
	#include<Windows.h>
#else
//
#endif

inline static void* SystemAlloc(size_t kpage)
{
#ifdef _WIN32
	void* ptr = VirtualAlloc(0, kpage << 13, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
#else
	//Linux下brk mmap等
#endif
	if (ptr == nullptr)
	{
		throw std::bad_alloc();
	}
	return ptr;
}

template<class T>
class ObjectPool 
{
public:
	T* New()
	{
		T* obj = nullptr;
		if (_freeList != nullptr) 
		{
			void* next = *((void**)_freeList);
			obj = (T*)_freeList;
			_freeList = next;
			return obj;
		}
		else 
		{
			if (_remainBytes < sizeof(T)) 
			{
				//_memory = (char*)molloc(128 * 1024);
				_remainBytes = 128 * 1024;
				_memory = (char*)SystemAlloc(_remainBytes >> 13);
				if (_memory == nullptr) 
				{
					throw std::bad_alloc();
				}
			}
			obj = (T*)_memory;
			size_t objSize = sizeof(T) < sizeof(void*) ? sizeof(void*) : sizeof(T);
			_memory += objSize;
			_remainBytes -= objSize;
		}
		//定位T，显示的调用T的构造函数初始化
		new(obj)T;
		return obj;
	}
	void Delete(T* obj) 
	{
		obj->~T(); //显示调用析构函数清理对象
		*(void**)obj = _freeList;
		_freeList = obj;
	}
private:
	char* _memory = nullptr; 
	size_t _remainBytes = 0;
	void* _freeList = nullptr;
};

struct TreeNode
{
	int _val;
	TreeNode* _left;
	TreeNode* _right;
	TreeNode() 
		:_val(0)
		,_left(nullptr)
		,_right(nullptr)
	{}
};

void TestObjectPool()
{
	const size_t Rounds = 5;
	const size_t N = 100000;

	std::vector<TreeNode*> v1;
	v1.reserve(N);

	size_t begin1 = clock();
	for (size_t j = 0; j < Rounds; ++j) 
	{
		for (int i = 0; i < N; ++i) 
		{
			v1.push_back(new TreeNode);
		}
		for (int i = 0; i < N; ++i)
		{
			delete v1[i];
		}
		v1.clear();
	}
	size_t end1 = clock();

	std::vector<TreeNode*> v2;
	v2.reserve(N);

	ObjectPool<TreeNode> TNPool;
	size_t begin2 = clock();
	for (size_t j = 0; j < Rounds; ++j)
	{
		for (int i = 0; i < N; ++i)
		{
			v2.push_back(TNPool.New());
		}
		for (int i = 0; i < N; ++i)
		{
			TNPool.Delete(v2[i]);
		}
		v2.clear();
	}
	size_t end2 = clock();

	cout << "new cost time:" << end1 - begin1 << endl;
	cout << "object pool cost time:" << end2 - begin2 << endl;

}