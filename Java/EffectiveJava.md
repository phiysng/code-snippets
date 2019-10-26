## 第十一章 并发

### 78 同步访问共享的可变数据

读/写一个变量是原子的,除非这个变量的类型是long/double

Java内存模型:一个线程所作的变化何时以及如何变成对其他线程可见

`synchronized`关键字可以保证在同一时刻,只有一个线程可以执行某
一个方法,或者一个代码块
两层含义
- 互斥,一个对象被一个线程修改时,可以阻止另一个线程观察到对象内部不一致的状态
- 进入同步方法/代码块的每个线程,都能够看到由同一个锁保护的之前所有的修改结果

Java语言规范保证线程在读取原子数据的时候,不会看到任意的数值,但他并不保证一个线程写入的值对另一个线程是可见的(由于处理器Cache等原因)

```
为了在线程之间进行可靠的通信,也为了互斥访问,同步是必要的.
```

不要使用`Thread.stop`方法

在线程A中终止线程B,建议的做法是用B去轮询一个`bool`值,这个值一开始为`false`,可以在线程A中修改为`true`,以表示让线程B终止自己.

#### v1

```java
// Broken! - How long would you expect this program to run?  (Page 312)
public class StopThread {
    private static boolean stopRequested;

    public static void main(String[] args)
            throws InterruptedException {
        Thread backgroundThread = new Thread(() -> {
            int i = 0;
            while (!stopRequested)
                i++;
        });
        backgroundThread.start();

        TimeUnit.SECONDS.sleep(1);
        stopRequested = true;
    }
}
```
可能遇到Java虚拟机的优化策略
将
```java
while (!stopRequested)
    i++;
```
优化为
```java
if(!stopRequested)
    while(true)
        i++;
```
这种优化(被称为提升,hoisting)在单线程下是没有问题的,但是在多线程的情况下,`stopRequested`可能会被其他的线程修改,这种优化策略就出现了问题
这种情形被称为活性失败(具体参见Java并发编程实战)
(编译原理QAQ)

volatile 内存可见性 优化 指令的重排列

#### v2 使用同步来保持变量的可见性
```java
public class StopThread {
    private static boolean stopRequested;

    private static synchronized void requestStop() {
        stopRequested = true;
    }

    private static synchronized boolean stopRequested() {
        return stopRequested;
    }

    public static void main(String[] args)
            throws InterruptedException {
        Thread backgroundThread = new Thread(() -> {
            int i = 0;
            while (!stopRequested())
                i++;
        });
        backgroundThread.start();

        TimeUnit.SECONDS.sleep(1);
        requestStop();
    }
}  
```

**除非读写方法都被同步,否则无法保证同步能起作用(只同步写方法还不够,个人理解:其他线程读的数据依然可能是旧的数据)**
这个类里被同步方法的动作即使没有同步也是原子的,即这些方法的同步只是为了它的通信效果,而非互斥访问。

#### v3 volatile保证可见性
```java
public class StopThread {
    private static volatile boolean stopRequested;

    public static void main(String[] args)
            throws InterruptedException {
        Thread backgroundThread = new Thread(() -> {
            int i = 0;
            while (!stopRequested)
                i++;
        });
        backgroundThread.start();

        TimeUnit.SECONDS.sleep(1);
        stopRequested = true;
    }
}
```
volatile不执行互斥访问,但是保证任何一个线程在读取该域的时候都能看到最近刚刚被写入的值.


### 80 executor task和 stream优先于线程

