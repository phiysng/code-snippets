### Is it okay to convert a pointer from a derived class to its base class? ¶ Δ

Yes.

An object of a derived class is a kind of the base class. Therefore the conversion from a derived class pointer to a base class pointer is perfectly safe, and happens all the time. For example, if I am pointing at a car, I am in fact pointing at a vehicle, so converting a Car* to a Vehicle* is perfectly safe and normal:

    void f(Vehicle* v);
    void g(Car* c) { f(c); }  // Perfectly safe; no cast

(Note: this FAQ has to do with public inheritance; private and protected inheritance are different.)
- C++的public继承是is-a的关系,私有和保护继承与上面的不同

### What’s the difference between public, private, and protected?

- A member (either data member or member function) declared in a private section of a class can only 
	be accessed by member functions and friends of that class

- A member (either data member or member function) declared in a protected section of a class can only 
	be accessed by member functions and friends of that class, and by member functions and friends of derived classes

- A member (either data member or member function) declared in a public section of a class can be accessed by anyone

- 只有公共成员能在类外被直接
- 保护和私有成员只能被成员函数/友元访问,保护级别还能被子类和子类的友元访问

### Why can’t my derived class access private things from my base class? ¶ Δ

- To protect you from future changes to the base class.

- Derived classes do not get access to private members of a base class. This effectively “seals off” the derived class from any changes made to the private members of the base class.

- 子类不能访问父类的私有成员

# private-inheritance

What are the access rules with private and protected inheritance?  

Take these classes as examples:
```c++
    class B                    { /*...*/ };
    class D_priv : private   B { /*...*/ };
    class D_prot : protected B { /*...*/ };
    class D_publ : public    B { /*...*/ };
    class UserClass            { B b; /*...*/ };
```
None of the derived classes can access anything that is private in B. In D_priv, the public and protected parts of B are private. In D_prot, the public and protected parts of B are protected. In D_publ, the public parts of B are public and the protected parts of B are protected (D_publ is-a-kind-of-a B). class UserClass can access only the public parts of B, which “seals off” UserClass from B.

- 在子类中不能访问父类的私有成员
- 私有继承中父类的公有和保护成员都成为子类的私有成员(可以在这一个子类的成员函数/友元中访问)
- 保护继承中父类的公有和保护成员都成为子类的保护成员(可以被子类/当前继承了父类的子类/友元中访问)
- 在公有继承中,父类中的公有成员依然是公有成员,保护成员依然是保护成员(对于这两个使用上面的规则)
- 包含的时候,从一个B的实例中只能访问`b`的公共的部分

- 可以使一个B中的公有成员在保护/私有继承的子类中依然公有,使用`using B::f`这种语句
To make a public member of B public in D_priv or D_prot, state the name of the member with a B:: prefix. E.g., to make member B::f(int,float) public in D_prot, you would say:
```c++
    class D_prot : protected B {
    public:
      using B::f;  // Note: Not using B::f(int,float)
    };
```


### When my base class’s constructor calls a virtual function on its this object, why doesn’t my derived class’s override of that virtual function get invoked?  
- ref https://stackoverflow.com/questions/12092933/calling-virtual-function-from-destructor
- 尽量不要在`ctor/dtor`调用虚函数,此时虚函数表现的和非虚函数一样,会调用当前指针所表示的平凡版本
- 不表现出动态绑定的特性是因为此时子类可能还未初始化/子类部分已经被析构掉了

Because that would be very dangerous, and C++ is protecting you from that danger.

The rest of this FAQ gives a rationale for why C++ needs to protect you from that danger, but before we start that, be advised that you can get the effect as if dynamic binding worked on the this object even during a constructor via The Dynamic Binding During Initialization Idiom.

You can call a virtual function in a constructor, but be careful. It may not do what you expect. In a constructor, the virtual call mechanism is disabled because overriding from derived classes hasn’t yet happened. Objects are constructed from the base up, “base before derived”.

Consider:

        #include<string>
        #include<iostream>
        using namespace std;
        class B {
        public:
            B(const string& ss) { cout << "B constructor\n"; f(ss); }
            virtual void f(const string&) { cout << "B::f\n";}
        };
        class D : public B {
        public:
            D(const string & ss) :B(ss) { cout << "D constructor\n";}
            void f(const string& ss) { cout << "D::f\n"; s = ss; }
        private:
            string s;
        };
        int main()
        {
            D d("Hello");
        }

the program compiles and produce

        B constructor
        B::f
        D constructor

Note not D::f. Consider what would happen if the rule were different so that D::f() was called from B::B(): Because the constructor D::D() hadn’t yet been run, D::f() would try to assign its argument to an uninitialized string s. The result would most likely be an immediate crash. So fortunately the C++ language doesn’t let this happen: it makes sure any call to this->f() that occurs while control is flowing through B’s constructor will end up invoking B::f(), not the override D::f().

Destruction is done “derived class before base class”, so virtual functions behave as in constructors: Only the local definitions are used – and no calls are made to overriding functions to avoid touching the (now destroyed) derived class part of the object.

For more details see D&E 13.2.4.2 or TC++PL3 15.4.3.

It has been suggested that this rule is an implementation artifact. It is not so. In fact, it would be noticeably easier to implement the unsafe rule of calling virtual functions from constructors exactly as from other functions. However, that would imply that no virtual function could be written to rely on invariants established by base classes. That would be a terrible mess.

### What’s the meaning of, Warning: Derived::f(char) hides Base::f(double)? ¶ Δ
- 函数声明的覆盖 包括虚函数与非虚函数
It means you’re going to die.

- 如果基类声明一个函数,子类也声明一个同名函数(只是参数类型/常量性不同),那么基类声明的函数会被隐藏而非重载/重写(即使基类的`f(double)`是虚函数)
Here’s the mess you’re in: if Base declares a member function f(double x), and Derived declares a member function f(char c) (same name but different parameter types and/or constness), then the Base f(double x) is “hidden” rather than “overloaded” or “overridden” (even if the Base f(double x) is virtual).
```c++
    class Base {
    public:
      void f(double x);  // Doesn't matter whether or not this is virtual
    };
    class Derived : public Base {
    public:
      void f(char c);  // Doesn't matter whether or not this is virtual
    };
    int main()
    {
      Derived* d = new Derived();
      Base* b = d;
      b->f(65.3);  // Okay: passes 65.3 to f(double x)
      d->f(65.3);  // Bizarre: converts 65.3 to a char ('A' if ASCII) and passes it to f(char c); does NOT call f(double x)!!
      delete d;
      return 0;
    }
```
Here’s how you get out of the mess: Derived must have a using declaration of the hidden member function. For example,
```c++
    class Base {
    public:
      void f(double x);
    };
    class Derived : public Base {
    public:
      using Base::f;  // This un-hides Base::f(double x)
      void f(char c);
    };
```
If the using syntax isn’t supported by your compiler, redefine the hidden Base member function(s), even if they are non-virtual. Normally this re-definition merely calls the hidden Base member function using the :: syntax. E.g.,
```c++
    class Derived : public Base {
    public:
      void f(double x) { Base::f(x); }  // The redefinition merely calls Base::f(double x)
      void f(char c);
    };
```
Note: the hiding problem also occurs if class Base declares a method f(char).

Note: warnings are not part of the standard, so your compiler may or may not give the above warning.

在你将一个基类指针指向一个子类的对象的时候,基类的函数不会被隐藏,因为编译器无法假定它会指向某一个具体的子类
Note: nothing gets hidden when you have a base-pointer. Think about it: what a derived class does or does not do is irrelevant when the compiler is dealing with a base-pointer. The compiler might not even know that the particular derived class exists. Even if it knows of the existence of some particular derived class, it cannot assume that a specific base-pointer necessarily points at an object of that particular derived class. Hiding takes place when you have a derived pointer, not when you have a base pointer.

### Why doesn’t overloading work for derived classes? 为什么函数重载在子类不起作用?
That question (in many variations) are usually prompted by an example like this:

        #include<iostream>
        using namespace std;
        class B {
        public:
            int f(int i) { cout << "f(int): "; return i+1; }
            // ...
        };
        class D : public B {
        public:
            double f(double d) { cout << "f(double): "; return d+1.3; }
            // ...
        };
        int main()
        {
            D* pd = new D;
            cout << pd->f(2) << '\n';
            cout << pd->f(2.3) << '\n';
            delete pd;
        }

which will produce:

        f(double): 3.3
        f(double): 3.6

rather than the

        f(int): 3
        f(double): 3.6

that some people (wrongly) guessed.

换句话说,根本就不存在`D`和`B`之间的重载决议,重载决议每次只在一个作用域起作用:编译器在`D`类的作用域找到了`double f(double)`,然后就调用这个函数,
因为在D中就找到了一个匹配,编译器不会再去`B`的作用域寻找.C++中不存在`跨作用域的函数重载`,子类的作用域也不例外.
```
下面是我自己的理解:
	按照[上一条](https://isocpp.org/wiki/faq/strange-inheritance#hiding-rule)的理解,子类的重名的函数会覆盖父类的函数(不管是否是虚函数)
	此时如果在`D`中没有合适的匹配,编译器会报错

	在另一种情况下,子类没有覆盖父类的名字,此时在子类中找不到这个名字的话,编译器会去父类的作用域去寻找这个名字
```
In other words, there is no overload resolution between D and B. Overload resolution conceptually happens in one scope at a time: 
The compiler looks into the scope of D, finds the single function double f(double), and calls it. Because it found a match, it never 
bothers looking further into the (enclosing) scope of B. In C++, there is no overloading across scopes – derived class scopes are 
not an exception to this general rule. (See D&E or TC++PL4 for details).

But what if I want to create an overload set of all my f() functions from my base and derived class? That’s easily done using a using-declaration, which asks to bring the functions into the scope:

        class D : public B {
        public:
            using B::f; // make every f from B available
            double f(double d) { cout << "f(double): "; return d+1.3; }
            // ...
        };

Given that modification, the output will be:

        f(int): 3
        f(double): 3.6

That is, overload resolution was applied to B’s f() and D’s f() to select the most appropriate f() to call.

### C++可见性


小时候，我一度以为这样的代码是不合法的。
```c++
// 在foo类的成员函数中访问foo的私有成员
class foo { int a; public: int foobar(foo * f) { return this->a + f->a; } };
```
因为我担心在 foo::foobar 中不能访问 f 的私有成员变量 a。

后来我明白了，所谓私有，是针对类的，而不是具体的对象。
```c++
class foo { protected: int a; }; class foobar : public foo { public: int bar(foo * f) { return this->a + f->a; } };
```
这次，在 foobar::bar 里，访问 this 的 a 成员允许，但 f 的 a 成员却被禁止了。

因为 foo::a 对 foobar 是 protected 的，foobar 的成员函数可以访问自己的 a ，但是对于 foo 指针，就禁止了。
--------------
我的想法:
    这里`bar`函数是`foobar`的成员函数,而不是foo的成员函数,因此不能使用`f->a`的方式获取其保护/私有成员
------------
想了一下，解决方案是。
```c++
class foo { protected: int a; static int get_a(foo *self) { return self->a; } }; class foobar : public foo { public: int bar(foo * f) { return this->a + get_a(f); } };
```
很坏味道。不过也不太所谓了。

