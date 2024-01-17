## 高并发内存池

>背景
>
>原型是Google的开源项目tcmalloc(Thread Caching Malloc)，即线程缓存的malloc，实现了高效的多线程内存管理，用于替代系统的内存分配相关的函数(malloc、free)。

#### 1.池化技术

程序先向系统申请过量的资源，然后自己管理，以备不时之需。

之所以要申请过量的资源，是因为每次申请该资源都有较大的开销，不如提前申请好，这样用时就会十分便捷，大大提高程序运行效率。

“池”技术：内存池、连接池、线程池、对象池等。

#### 2.内存池

程序预先从操作系统中申请一块足够大的内存，此后，当程序中需要申请内存的时候，不是直接向操作系统申请，而是直接从内存池中获取；当程序释放内存的时候，并不是真正将内存返回给操作系统，而是返回内存池。当程序退出(或特定时间)时，内存池才将之前申请的内存真正释放。

#### 3.内存池主要解决的问题

内存池主要解决的当然是**效率**问题，其次如果作为系统的内存分配器角度，还需要解决一下**内存碎片**问题。

### 入门：定长内存池

`concurrent` 并发

`ConcurentMemoryPool`

选择`Win32控制台应用程序`->`空项目`

自由链表：用链式结构来管理还回来的内存块。

![image-20240106230019477](C:\Users\iuk11\AppData\Roaming\Typora\typora-user-images\image-20240106230019477.png)

`void*`是一种“无类型指针”。可以指向任何类型的数据。

![image-20240107045939840](C:\Users\iuk11\AppData\Roaming\Typora\typora-user-images\image-20240107045939840.png)

```c++
*(void**)obj = _freeList;
_freeList = obj;
```

#### 完整代码解析

