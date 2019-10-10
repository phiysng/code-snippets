# Deleting derived classes in std::unique_ptr<Base> containers
https://stackoverflow.com/questions/24060497/deleting-derived-classes-in-stdunique-ptrbase-containers

- 析构函数标记为虚函数
- 自定义`unique_ptr`的`deleter`
```c++
std::unique_ptr<B, void (*)(B *)> pb
    = std::unique_ptr<D, void (*)(B *)>(new D,
        [](B *p){ delete static_cast<D *>(p); });
```