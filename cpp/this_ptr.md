你可以不那么严格地认为：class MyClass
```c++
{
    void my_call(int arg1, float arg2) {}
};
```大致上相当于：
```struct MyClass
{
};

void MyClass_my_call(MyClass* this, int arg1, float arg2);
```

Python语言的类成员函数会要求显式的写出`self`,类似于`this`指针的一个概念

实际上，C语言编制的库，很多都是这样的。当然，实际上method call并不完全是这样实现的，比如this参数会被特别对待。不过这种细节问题对你暂时不重要。

