# tcp backlog in Linux

当一个程序用`listen`将一个socket套接字置为`LISTEN`状态,它需要为这个socket指定一个`backlog`.
这个`backlog`经常被描述为将要进入的连接的队列的极限.

三次握手过程中,服务器会经历2种TCP状态

- TCP SYN_REVD
- ESTABELLISHED

对应也会维护两个队列
- 存放SYN的队列(半连接队列)
- 存放完成连接的队列(全连接队列)

当一个连接的状态是`SYN_REVD`时，它会被放在SYN队列,当它的状态变为`ESTABLISHED `时,它会被转移
到全连接队列.
后端的程序只从已完成的连接的队列中获取请求.

如果这两个队列很小的话,并发的性能一般会很差.

---------------------------
下面这个API是构建server socket时所调用的函数.
```c
int listen(int sockfd, int backlog);
```

```
backlog

Now it specifies the queue length for completely established sockets waiting to be accepted,
instead of the number of incomplete connection requests. 
The maximum length of the queue for incomplete sockets can be set using the tcp_max_syn_backlog sysctl. 
When syncookies are enabled there is no logical maximum length and this sysctl setting is ignored.
If the socket is of type AF_INET, and the backlog argument is greater than the constant SOMAXCONN(128 default), 
it is silently truncated to SOMAXCONN.

简单来说
backlog定义了完成了三次握手等待被`accpet`的socket的数量,而非没有完成三次握手的连接的数量(其可以被tcp_max_syn_backlog 调用更改).
当syncookies被启用(可以在一定程度上防范SYN FLOOD),没有逻辑上的半连接的队列的最大大小,tcp_max_syn_backlog sysctl被忽略.

当套接字是IPv4套接字并且`backlog`被设置为大于SOMAXCONN(默认为128),系统上起效的是SOMAXCONN(被默认截断)
即,backlog由系统和用户设置的两个值共同决定(Linux系统常规设置).
real_backlog = min(backlog, SOMAXCONN)

半连接队列长度 = min(backlog, SOMAXCONN，tcp_max_syn_backlog)。
即半连接队列长度不能超过全连接队列的长度

像NGINX/Tomcat这些Web服务器都可以设置backlog,当并发量很大额时候需要综合设置应用层和内核层的参数.
```

see:
listen实现:

https://github.com/torvalds/linux/blob/26bc672134241a080a83b2ab9aa8abede8d30e1c/net/socket.c#L1667