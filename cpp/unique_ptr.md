# unique_ptr

- 当类的声明中没有显式的声明一个拷贝构造函数的时候,编译器会隐式的声明一个
- 但是如果类声明了一个移动构造函数/移动赋值运算符,隐式的声明会被定义为`delete`

转移所有权
```c++

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <numeric>

using namespace std;

void transfer_ownership(unique_ptr<string> ptr)
{
    cout << *ptr << endl;
}

int main(int argc, const char** argv)
{
    auto ptr = make_unique<string>(string("ptr hold"));
    transfer_ownership(move(ptr));
    assert(nullptr == ptr);
    return 0;
}

```


## 将`unique_ptr`作为参数传递给函数

- 值传递
- 左值引用
- 右值引用

- 与一般的C++代码相同,左值/右值引用可以同时出现
- 值传递与左值引用不能同时出现
	- 函数调用2有歧义

- 值传递与右值引用不能同时出现()
	- 1有歧义 move()后既可以作为值传递,也可以作为引用传递
	- 两个调用都只能处理右值的情况 语句2没有合适的匹配


```
void transfer_ownership(unique_ptr<string> ptr); // 1
void transfer_ownership(unique_ptr<string>& ptr); // 2
void transfer_ownership(unique_ptr<string>&& ptr); // 3

auto ptr = make_unique<string>(string("ptr hold"));
transfer_ownership(move(ptr)); // 1
transfer_ownership(ptr); // 2
```

--------------------------------
一般来说如果调用语句伴随着所有权的转移,建议尽量使用按值传递的做法

