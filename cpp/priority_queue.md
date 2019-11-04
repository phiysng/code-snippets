# C++中的优先队列

ref:https://stackoverflow.com/questions/50635728/lambda-function-doesnt-work-in-priority-queue-in-c-when-using


正确的代码应该是像这样的:
模板容器需要一个变量,而lambda的类型只有编译器知道(lambda实际上内部被实现为一个
包含了重载的`()`运算符的函数以及其捕获的变量),而在pq的构造函数中则接受一个变量作为参数.
```c++
auto cmp=[](...){return true;};
priority_queue<pair<int,int>,vector<pair<int,int>>, decltype(cmp)> pq(cmp);

```

只使用`decltype(cmp)`获得类型不能了解到任何的本地变量,变量的`attachment`只在实际创建`cmp`
的是偶出现.

-----------------------------------

`priority_queue`的默认构造函数是(C++17)
```c++
explicit priority_queue(const Compare& x = Compare(), Container&& y = Container());
```

可以看出,这里涉及了类型`Compare`的默认构造,__C++ 20__ 之前lambda不是可默认构造的.

无状态的lambda的默认构造没有进 __C++ 17__ ,现在出现在了 __C++ 20__ 草案中. 

技术上来说, C++17的编译器甚至应该应该拒绝编译`[]`版本,但是好像一些编译器已经开始了C++20
的初步支持.

我注意到clang++6.0.0 事实上允许`[&]`的lambda的默认构造,当实际上这个lambda没有捕获任何的
变量的时候,但是按照C++标准这种情形应该被`rejected`.这种情形可能会随着C++20的定稿而改变.

另一种方法:使用`function`
ref: https://unofficial-competition-guide.readthedocs.io/en/latest/priority-queue-stl.html
```c++
bool compare_function(const pair<int, int>& p1, const pair<int, int>& p2)
{
    return p1.first < p2.first;
}
int main()
{
    priority_queue< pair<int, int>,
                    vector< pair<int, int> >,
                    function< bool(pair<int, int>, pair<int, int>) >
                  > pq2(&compare_function);
    return 0;
}
```
-----------------------------------------
https://stackoverflow.com/questions/5807735/c-priority-queue-with-lambda-comparator-error

对于闭包(closure),默认构造函数和拷贝赋值运算符被`delete`，并且拷贝构造函数和析构函数被隐式的定义. from [expr.prim.lambda]/19

lambda函数的类永远都没有一个默认的构造函数,即使它什么变量都没有捕获.因此`priority_queue`只能
通过拷贝一个`lambda`对象的方式(通过调用拷贝构造函数)创建它所需要的对象.



