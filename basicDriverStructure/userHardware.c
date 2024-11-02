// 这里去编写一个用户(开发板上的)程序，尝试去打开我们刚刚写的驱动

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
  fd = open("dev/hellodriver", O_RDONLY);
  if (fd<0){
    // 尝试open()驱动文件时出现了异常
    perror("诶嘿，你猜我为什么显示了?");
    exit(EXIT_FAILURE);
  }
  // 成功的打开了文件，开始读取内容
  int num = read(fd, buf, sizeof(buf)-1); // 这里-1的意义: 预留"最后一位"成0 当终止符
  buf[num] = 0; //设置读取内容后 的最后一位为0
  printf("嘿！我们收到了来自/dev/hellodriver 的消息:\"%s\" \n", buf); //特意加了俩 " 作为修饰
  close(fd);
  exit(EXIT_SUCCESS);
}