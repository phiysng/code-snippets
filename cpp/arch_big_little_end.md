## 判断处理器是大端还是小端

今天笔试遇到了这个问题,以前看书的时候遇到过这个问题,但是当时没想起来,这里记录一下

-----------
可以使用结构体来选择内存的一部分,这里我们将`int64_t`的8个字节分成两个部分,前4个字节为`0x0`,后四个字节为`0xFFFFFFFF`,然后我们打印前面的四个字节(即 a),看结果怎么样就可以获得处理器是大端机还是小端机了。
```c
#include<stdio.h>
#include<stdint.h>

union EndIdentifier{
int32_t a;
int64_t b;
};

int main(int argc, char *argv[])
{
		union EndIdentifier a;
		a.b = 0x00000000FFFFFFFF;

		printf("%d\n",a.a);
		printf("%d\n",0xFFFFFFFF);
		
		return 0;
}
```
输出如下
```bash
-1
-1
```
-----------------------------

由此可见 ,这里数据的低位放在了低地址处,所以是小端机(这里是`x86-64`,最典型的小端机)
如果是大端机，`a.a`应该为`0`.