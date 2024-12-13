##### StudentController

一个简易的学生管理系统



## 项目介绍

- 此项目实现了一个基础管理系统模型
- 并提供了多个可用库提供高扩展性的开发组件



#### 项目扩展推荐组件

- Networker: TCP网络组件(内置HTTP协议库) [可以实现数据的 **网络同步(sync)** 或 **异地容灾** ]
- Buffer: 智能字符串组件 [ **自动扩容**，支持多种读写操作]
- Convert: 数据类型转换组件 [用于字符串与数字等转换]



## 构建

#### mingw(推荐):

```shell
mkdir build
cd build
cmake ..
mingw32-make
```



## 基础设施:
 #### hashTable: 哈希表
  - 哈希冲突解决方案: 为冲突的哈希节点实现单向链表
  - 后续考虑引入分表机制减少单表压力
 #### memPool: 内存池
  - 防止内存泄漏,对内存分配池统一管理和错误控制,以及提供内存利用率和减少内存频繁被释放
  - 特性函数: mem_hook(支持回调函数直接使用Pool)
  - 后续考虑引入GC垃圾回收机制
 #### database: 自研数据库驱动
  - 文件数据库(后续考虑引入索引表提升性能)
 - convert: 内部数据类型转换器(服务其他组件)
 #### console: 控制台组件
  - 与用户态的数据交互
 #### error: 内置错误控制机制
  - 特性: 能够自动捕获信号实现自动保存数据
 #### buffer: 智能字符串(自动扩容,多种IO流函数)


## 其他驱动:
 #### encoder: 加密器
 #### worker: 网络交互库
  - http: http协议(包含 [请求库] / [响应库])
  - httper: HTTP请求库



### worker使用 (仅限Windows系统)

> 注: 控制器目前并没有使用到Worker的代码

#### Httper模式(请求http):

```c
worker_init();
Worker* worker = httper_create("http://test.com/hello", 80, message); //参数三
if(worker == NULL){
    printf("创建httper-worker失败,请检查是否为正常Url!\n");
    exit(0);
}
worker_connect(worker, 30);
```

#### Worker模式

```c
worker_init();
int error;
Worker* worker = worker_create("127.0.0.1", 8888, &error); //参数三
if(worker == NULL){
    printf("创建worker失败,错误代码: %d!\n", error);
    exit(0);
}
worker -> onMessage = messager; //响应的回调函数
worker_connect(worker, 30);
```



#### 其他说明

> Q: Httper与Worker的区别

A: Httper是基于Worker的HTTP协议处理器，其中实现了对http/https域名的解析，对HTTP数据包的组装和响应包的解析

> Q: 数据库文件怎么改路径?

A: 数据文件读取逻辑在 `includes/database.h` 的定义中,可自行更改名称

