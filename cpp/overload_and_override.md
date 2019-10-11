https://stackoverflow.com/questions/9568852/overloading-by-return-type

C++不能通过返回值重载函数

MSDN关于重载的文档
https://docs.microsoft.com/en-us/cpp/cpp/function-overloading?view=vs-2019

Google Abseil博客
https://abseil.io/tips/148

from Stroustrup himself[1]:
可以自底向上的解析一个表达式而不用去考虑表达式整体

The reason that C++ doesn't allow overload resolution based on a return type is that I wanted overload resolution to be bottom up. For example, you can determine the meaning of a subexpression a+b without considering the complete expression that a+b is part of. Overload resolution can be subtle so even though I knew how to use return types as part of resolution (Ada showed how), I decided not to. As a result of this decision, a+b means the same in a+b+c as in a+b+d.