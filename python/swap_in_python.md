# python的swap语法糖

今天在写Leetcode的算法题的时候,遇到了这样一个问题:

我们都知道python有这样一个语法糖

```python
a , b = b ,a
```

这也是官方所推荐的swap做法.

但是今天我写了一个更复杂的swap,就出现了一些问题,程序的运行结果很奇怪

```python
nums = [5,4,3,2,1]

# v1
nums[i], nums[nums[i] - 1] = nums[nums[i] - 1], nums[i]

# v2
nums[nums[i] - 1], nums[i] = nums[i], nums[nums[i] - 1]
```

#### v1 分析
赋值表达式的右边分别计算了`nums[nums[i] - 1]`和`nums[i]`的值,这一步没有问题

然后``nums[nums[i] - 1]`的值赋给了左边的`nums[i]`,这一步就是问题所在,

因为接下来的`nums[nums[i] - 1]`中的`nums[i] - 1`,即索引的值的计算依赖`nums[i]`,

这就意味着我的程序在接下来赋的值可能并不是我想要的值(比如我想交换`nums[1]` `nums[3]`的值,

但是此时因为nums[3]的`3`这个值依赖于`nums[1]`,我更改了`nums[1]`之后,我计算出的位置是

`nums[5]`而非`nums[3]`).

#### v2 分析

第二个`swap`交换了一下顺序,这样不会先更改`nums[i]`的值,这样计算出的索引依旧是原来的那一个,此时我们

顺利完成`nums[nums[i] - 1]`的赋值,然后我们再更新`nums[i]`的值就不会出问题了

#### 总结
语法糖也不能乱用,最好还是对于其原理有一定的了解,否则运行不及预期也有些无从下手修正.

出现如题这种索引依赖的值会在**swap**中更新这种比较**tricky**的情形,最简单的方式还是直接缓存`index`.

ref:
https://stackoverflow.com/questions/14836228/is-there-a-standardized-method-to-swap-two-variables-in-python/14836456#14836456


