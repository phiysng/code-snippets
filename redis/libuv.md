使用`callback(回调)`机制来完成异步IO,通常带有参数`uv_stream_t`
或`uv_req_t`,这个数据表示这次回调绑定的数据

C语言没有原生的closure支持,如果有的话,closure应是callback机制的
最佳解决方案

所以在C语言中模拟闭包的方式是用一个C Function并携带一个`void *ud`
此`ud`即原本应该在闭包中绑定的数据块

TODO: 支持闭包的现代程序语言

在libuv中`uv_stream_t`大致等同于这个`ud`