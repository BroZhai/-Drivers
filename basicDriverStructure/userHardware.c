// 这里去编写一个用户(开发板上的)程序，尝试去打开我们刚刚写的驱动
// 实际上这个程序就是在"模拟硬件"，这个硬件将会从我们的 mknod 生成的 '硬件设备'中 去与 设备驱动文件 展开通信
// (以上的理论共计花费了4天时间才他奶奶的搞明白，课讲的过于抽象和含糊了xwx)

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
//以上的库都和 UNIX的 open() read() close()等 '原生操作'有关 [POSIX, file descriptors]

#include <stdio.h> // perror() printf() 等方法的库
#include <stdlib.h> // EXIT_SUCCESS / FAILURE 等常量定义的库

void main() {
  int fd;
  char buf[1024]; // 创建"用户接收缓冲区"
  fd = open("/userHard", O_RDONLY); // 这里就将会是我们打开的"mknod"生成的文件地址 (去你的 我直接存根目录了;|)
  if (fd<0){
    // 尝试open()驱动文件时出现了异常
    perror("坏了，哥咱尽力了，但是可能没有找到对应的文件，出现了异常QAQ \n");
    exit(EXIT_FAILURE);
  }
  // 成功的打开了文件，开始读取内容
  int num = read(fd, buf, sizeof(buf)-1); // 这里-1的意义: 预留"最后一位"成0 当终止符
  buf[num] = 0; //设置读取内容后 的最后一位为0
  printf("嘿！我们收到了来自/userHard 从 directcopy.ko 驱动文件中 读取 的消息:\"%s\" \n", buf); //特意加了俩 " 作为修饰
  close(fd);
  exit(EXIT_SUCCESS);
}