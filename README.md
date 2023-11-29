## MySkiplist
* 初版SkipList主要是模仿学习代码随想录的skiplist项目
* 源代码地址：https://github.com/youngyangyang04/Skiplist-CPP

### 运行方法
* 主程序运行方法：
  * 远程主机配置工具链和cmake通过Clion运行
  * Cmake执行
    * ```
      mkdir build & cd build
      cmake ..
      make
      ./MySkipList
      ```
  * 通过makefile执行的方法
    * ```
      make            // complie demo main.cpp
      ./bin/main      // run
  * 压力测试运行方法：
    * ```
      sh stress_test_start.sh
      ```
### 补充点
* 待优化：
  * delete的时候没有释放内存
  * 压力测试并不是全自动的
  * 跳表的key是int型，如果使用其他类型需要自定义比较函数，当然把这块抽象出来就更好
  * 如果再加上一致性协议，例如raft就构成了分布式存储，再启动一个http server就可以对外提供分布式存储服务了