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
  char cmd_input; 
  fd = open("/Led1_Control_By_Tekon_2024", O_WRONLY); 
  if (fd<0){
    // 尝试open()驱动文件时出现了异常
    perror("坏了，哥咱尽力了，但是可能没有找到对应的文件，出现了异常QAQ \n");
    exit(EXIT_FAILURE);
  }
  do {
    printf("按下 \'1\' 开灯, \'0\'关灯, \'2\'退出LED控制程序");
    scanf(" %c", &cmd_input); //接收外界的键盘输入("1"打开，"0"关闭)
    printf("键盘先生输入了 %c \n", &cmd_input);
    if (cmd_input == '2') break; // 输入"2"终止LED控制程序
    write(fd,&cmd_input,1); // 正式'写入'硬件
  } while(1);
  close(fd);
  exit(EXIT_SUCCESS);
}