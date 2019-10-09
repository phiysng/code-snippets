------------------------------
### 一个传统但并不100%正确的 DCLP 实现
```c++

class Singleton {
public:
    static Singleton* GetInstance()
    {
        if (instance == nullptr) {
            std::lock_guard<std::mutex> lock(mtx);
            if (instance == nullptr) {
                instance = new Singleton();				// <-- key point
            }
        }

        return instance;
    }

private:
    // omit explicit static field initialization.
    static std::mutex mtx;
    static Singleton* instance;
};
```

### 原因
```c++
instance = new Singleton();
//等价于三个过程

tmp = operator new(sizeof(Singleton));  // step 1: allocate memory via operator new
new(tmp) Singleton;                     // step 2: placement-new for construction
instance = tmp;                         // step 3: assign addr to instance
// 步骤1肯定先完成 步骤2和3的顺序则可以由编译器自行决定
// 在先进行3再执行2的构造函数初始化的时候,多线程环境下某一个线程可能会拿到 instance
// 但是此时instance只是分配了内存，还未初始化,此时使用此对象的行为是未定义的

```

- 多线程环境下，DCLP 要能正常工作的一个前提是：如果某个线程 看到 instance 非空，那么 instance 对应的对象必须是构造完毕的；==i.e. 执行 step 3 之前，step 1 和 step 2 必须是已经完成的。

- 不幸的是，因为代码优化的需求，导致两个后果：

    - 编译器生成代码时会存在乱序
    - 现代多核CPU不光每个核存在执行指令乱序，同一段代码不同核同一时刻的执行的顺序都可能是不同的
- 这就会导致上面的例子中，可能出现：

    - 线程 A 先更新了 instance 地址到新内存
    - 线程 B 进入函数，检查发现 instance 不为空；但是此时对象尚未构造完毕，使用对象就跪了

### C++11版本
```c++
class Singleton {
public:
    static Singleton* GetInstance()
    {
        auto tmp = instance.load(std::memory_order_acquire);    // <-- read-acquire operation
        if (tmp == nullptr) {
            std::lock_guard<std::mutex> lock(mtx);
            tmp = instance.load(std::memory_order_relaxed);     // <-- relaxed is enough
            if (tmp == nullptr) {
                tmp = new Singleton();
                instance.store(tmp, std::memory_order_release); // <-- write-release operation
            }
        }

        return tmp;
    }

private:
    static std::mutex mtx;
    static std::atomic<Singleton*> instance;
};
```