Q_OBJECT

- 删除父指针会同时调用子类的析构函数

std::atomic<T> 模板类
unique_ptr::reset

分配内存 -> 调用构造函数初始
调用析构函数释放持有的资源 -> 释放内存

placement new 在已经分配好的内存上调用构造函数初始化这块内存

没有 placement delete 因为可以直接调用析构函数

thread this