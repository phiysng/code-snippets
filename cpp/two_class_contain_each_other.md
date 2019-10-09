# 如何定义两个类,每一个类都把另一个类作为自己的一个成员

## 错误示范
```c++
// bar.h
#ifndef BAR_H
#define BAR_H
#include "foo.h"
class bar {
public:
  foo getFoo();
protected:
  foo f;
};
#endif
//foo.h
#ifndef FOO_H
#define FOO_H
#include "bar.h"
class foo {
public:
  bar getBar();
protected:
  bar b;
};
#endif

//main.cpp
#include "foo.h"
#include "bar.h"

int
main (int argc, char **argv)
{
  foo myFoo;
  bar myBar;
}
```
编译报错,foo/bar都包含不完整的类型
```bash
$ g++ main.cpp
In file included from foo.h:3,
                 from main.cpp:1:
bar.h:6: error: ‘foo’ does not name a type
bar.h:8: error: ‘foo’ does not name a type
```

## 解决方案
- 用指针来指向需要包含的对象,使用前向声明来表示包含的那个类已存在
- 指针的大小是确定的,因此类的大小也就可以确定
- 避免了`A需要B需要A`这种循环引用的问题
```c++
#ifndef BAR_H
#define BAR_H

class foo; // Say foo exists without defining it.

class bar {
public:
  foo* getFoo();
protected:
  foo* f;
};
#endif 

#ifndef FOO_H
#define FOO_H

class bar; // Say bar exists without defining it.

class foo {
public:
  bar* getBar();
protected:
  bar* f;
};
#endif 
```

参考:
- https://stackoverflow.com/questions/4964482/how-to-create-two-classes-in-c-which-use-each-other-as-data