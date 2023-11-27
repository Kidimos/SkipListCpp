## MySkiplist
* 初版SkipList主要是模仿学习代码随想录的skiplist项目
* 源代码地址：https://github.com/youngyangyang04/Skiplist-CPP
* 待优化：
  * delete的时候没有释放内存
  * 压力测试并不是全自动的
  * 跳表的key是int型，如果使用其他类型需要自定义比较函数，当然把这块抽象出来就更好
  * 如果再加上一致性协议，例如raft就构成了分布式存储，再启动一个http server就可以对外提供分布式存储服务了