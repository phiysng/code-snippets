## 所有的函数形参均为左值
https://stackoverflow.com/questions/38926753/r-value-parameters-in-a-function

```c++
#include <string>

void foo(std::string&& str) {
  // Accept a rvalue of str
}

void bar(std::string&& str) {
  // foo(str);          // Does not compile. Compiler says cannot bind lvalue into rvalue.
  foo(std::move(str));  // do compile.
}

int main(int argc, char *argv[]) {
  bar(std::string("c++_rvalue"));
  return 0;
}
```

