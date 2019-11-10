# backlog是怎样在Linux网络协议栈上起作用的

当一个应用用`listen`系统调用将一个套接字置于`LISTEN`状态,你需要指定这个套接字的`backlog`.

`backlog`一般解释为将要连接的队列的极限(the limit for the queue of incoming connections.)

因为TCP协议的三次握手,一个将要完成的连接在到达`ESTABLISHED `状态并可以被返回给`accept`之前会经过一个中间状态`SYN RECEIVED`.
这意味着一个`TCP/IP`协议栈有两种选择来为一个`LISTEN`状态的套接字实现`backlog queue`.

- 一种实现使用一个队列,它的大小被`listen()`系统调用的`backlog`参数决定.当一个`SYN`报文到达,
系统返回一个`SYN/ACK`报文并将这个连接添加进队列中.当客户端对应的ACK到达,这个连接的状态转变为`ESTABLISHED`
并且可以被交付给应用.这意味着这个队列可以包含两种不同状态的连接:`SYN RECEIVED`和`ESTABLISHED`.
只有完成状态的连接可以被返回给调用了`accpet()`的应用.
- 这个实现使用两个队列,一个`SYN queue`或者说`incomplete connection queue`,和一个`accept queue`(或者说`complete connection queue`).处于` SYN RECEIVED`状态的连接被添加进`SYN queue`,当三路握手完毕,连接的状态转变为`ESTABLISHED`,
此时将连接移动到`accept queue`.此时`accept()`系统调用就只是简单的消费`accept queue`的连接.在这种情况下,`backlog`参数
决定了`accept queue`的大小.

历史上,BSD衍生的TCP实现使用第一种思路,这种选择意味着当达到最大`backlog`的时候,当客户端再发送`SYN`请求,服务器
将不再响应`SYN ACK`报文,通常的TCP实现会单纯的丢弃`SYN`报文(而不是相应一个`RST`报文),因而客户端会超时重传(retry).
这是`TCP/IP详解 卷三 section 14.5 listen`描述的情形.

注意,Stevens 实际上解释道BSD实现实际上使用两个独立的队列,   但是他们表现的像一个由`backlog`固定最大大小的队列,逻辑上
来说,BSD表现的如下所述:
```

The queue limit applies to the sum of […] the number of entries on the incomplete connection queue […] 
and […] the number of entries on the completed connection queue […].

```

在Linux上,事情就不太一样了,正如`listen()`系统调用的`man page`所述,
```
The behavior of the backlog argument on TCP sockets changed with Linux 2.2. 
Now it specifies the queue length for completely established sockets waiting to be accepted, 
instead of the number of incomplete connection requests. 
The maximum length of the queue for incomplete sockets can be set 
using /proc/sys/net/ipv4/tcp_max_syn_backlog.

即backlog为完成的连接数极限,而未完成的连接的极限大小由*tcp_max_syn_backlog*决定.
```

这意味着当前版本的Linux内核使用第二种实现思路,使用两个不同的队列:一个SYN队列,大小由一个系统级别的参数决定,一个
accept队列由应用指定.

现在的问题就是当`accept queue`已满且一个套接字完成了三次握手需要移到`accept queue`,会发生什么?
这种情况被 `net/ipv4/tcp_minisocks.c`下的`tcp_check_req`处理.相关的代码如下:
```c
 child = inet_csk(sk)->icsk_af_ops->syn_recv_sock(sk, skb, req, NULL);
        if (child == NULL)
                goto listen_overflow;
```

对于IPv4,第一行代码会实际上调用`net/ipv4/tcp_ipv4.c`中的`tcp_v4_syn_recv_sock `,包含如下代码:
```c
 if (sk_acceptq_is_full(sk))
                goto exit_overflow;
```

我们看到这里检查了`accept queue`.`exit_overflow`标签下面做了一些清理动作,更新了`/proc/net/netstat`下的
`ListenOverflows `和`ListenDrops `状态,然后返回NULL.这会触发`tcp_check_req`中的`listen_overflow `.

```c
listen_overflow:
        if (!sysctl_tcp_abort_on_overflow) {
                inet_rsk(req)->acked = 1;
                return NULL;
        }
```

这意味着除非` /proc/sys/net/ipv4/tcp_abort_on_overflow`被设置为1(这种情况下上面的代码后面的代码会发送一个`RST`报文),协议栈实现实际上什么也不做.

总结一下,如果`accept queue`已满且新的套接字完成了三次握手,协议栈会单纯的丢弃这个报文(第二个SYN ACK),一开始
这好像有点难以理解,但是请牢记`SYN RECEIVED`状态拥有一个关联的定时器,如果ACK报文没有被服务器收到(或者被服务器丢弃,如现在的
情形),TCP/IP协议栈会重传`SYN/ACK 报文`(重传的次数由/proc/sys/net/ipv4/tcp_synack_retries指定,使用 
exponential backoff algorithm)。

可以从下述的报文流中看到一个客户端尝试去`connect()`一个已经到达其最大backlog的服务端会发生什么.
```
  0.000  127.0.0.1 -> 127.0.0.1  TCP 74 53302 > 9999 [SYN] Seq=0 Len=0
  0.000  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
  0.000  127.0.0.1 -> 127.0.0.1  TCP 66 53302 > 9999 [ACK] Seq=1 Ack=1 Len=0
  0.000  127.0.0.1 -> 127.0.0.1  TCP 71 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
  0.207  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
  0.623  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
  1.199  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
  1.199  127.0.0.1 -> 127.0.0.1  TCP 66 [TCP Dup ACK 6#1] 53302 > 9999 [ACK] Seq=6 Ack=1 Len=0
  1.455  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
  3.123  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
  3.399  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
  3.399  127.0.0.1 -> 127.0.0.1  TCP 66 [TCP Dup ACK 10#1] 53302 > 9999 [ACK] Seq=6 Ack=1 Len=0
  6.459  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
  7.599  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
  7.599  127.0.0.1 -> 127.0.0.1  TCP 66 [TCP Dup ACK 13#1] 53302 > 9999 [ACK] Seq=6 Ack=1 Len=0
 13.131  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
 15.599  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
 15.599  127.0.0.1 -> 127.0.0.1  TCP 66 [TCP Dup ACK 16#1] 53302 > 9999 [ACK] Seq=6 Ack=1 Len=0
 26.491  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
 31.599  127.0.0.1 -> 127.0.0.1  TCP 74 9999 > 53302 [SYN, ACK] Seq=0 Ack=1 Len=0
 31.599  127.0.0.1 -> 127.0.0.1  TCP 66 [TCP Dup ACK 19#1] 53302 > 9999 [ACK] Seq=6 Ack=1 Len=0
 53.179  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
106.491  127.0.0.1 -> 127.0.0.1  TCP 71 [TCP Retransmission] 53302 > 9999 [PSH, ACK] Seq=1 Ack=1 Len=5
106.491  127.0.0.1 -> 127.0.0.1  TCP 54 9999 > 53302 [RST] Seq=1 Len=0

```
