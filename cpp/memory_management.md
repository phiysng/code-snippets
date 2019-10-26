# 内存管理
ref https://hexilee.me/2019/07/18/modern-memory-management/
## 左值引用

### 可变引用

### 不可变引用

`const`引用不能赋给`non-const`引用
Pros
- Cpp 的引用固然在减少拷贝、控制可变性上做得很不错
Cons
- 引用可能比对象本身活得更长
- 可能同时持有对同一对象的多个可变引用，不能静态检查到潜在的数据竞争

## 拷贝

## 移动
Cons

- 虽然 move 了，但后面可能还会不小心用到。当然这种情况现代编辑器和编译器一般都会给个 warning。
- 虽然 move 了，但之前的引用还在被使用，这种情况编辑器和编译器很难发觉。


## 智能指针

unique_ptr => Box
std::shared_ptr => std::sync::Arc
std::weak_ptr => std::sync::Weak

std::rc::Rc 是 Arc 的单线程版本

Rust 的 Box 相对 Cpp 的std::unique_ptr更优，因为 Rust 可以在编译期保证

- 不能对已移交所有权的变量取引用（已移交所有权的变量无绑定对象）
- 在其任意引用的生命期内对象不能被移动
