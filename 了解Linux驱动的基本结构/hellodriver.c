// 好耶，终于来开始学习Linux驱动的编写了
// 咱来看看是这个"配置"囊个回事?
// (Tips: 在Linux驱动文件时，我们所处的环境是"内核环境",所以一般"用户环境的库"我们就用不了力)

#include <linux/types.h> 
#include <linux/string.h> //my_open()中的"strlen"用到了这个库
// header files暂时不全，下节课继续补充

#define startNumber 3; // 定好"次设备号"起始号
#define deviceCounts 2; // 定好"次设备数量"
#define deviceName "I'm HelloDriver"; // 定义好"设备名" (给人看的)

static char Kmsg[] = "这里是内核里面定义好的消息，准备通过copy_to_user()传给用户"; // my_read()函数就会读取这里的东西

static dev_t devMasterNum; // 定好一块"存储空间"，类型为dev_t，用其"地址"存储 请求成功返回的"主设备号"
// 有关上面'static'关键字的补充: 这里在全局变量上 的static指 该变量"仅在本文件内生效" (Java 的 private)

// 以上三个"重define"参数 还有 "主设备号存储区" 正是 alloc_chrdev_region()所需要的

static struct cdev dev; // 创建一个 字驱动表"存储结构体"(提供cdev.init()的"驱动表对象"的存储)

static int my_open(struct inode *inode, struct file *fp){
  // 具体实现"打开文件"的方法
  printk("文件(设备)" deviceName "已被打开\n");
  return 0;
}

static int my_read(struct file *fp, char *buf, size_t count, loff_t * position){
  // 具体"读取文件"的方法
  // 注意这里的 *buf ，它只是一个user level的指针，指向了 用户空间的"接收buffer"
  
  int numRead; // 定义"要读字节"的值(如果用户定义了的话)
  int copyReturn; // 定义"copy_to_user()"方法的操作结果 (返回值)

  if(count < strlen(Kmsg)){ //如果用户"限定了读取长度"
    numRead = count; // 将用户定义的"读取长度"赋值给上面的numRead，一会copy_to_user()用
  }else if(count > strlen(Kmsg)){
    numRead = strlen(Kmsg); // 用户设定的"读取长度"超过了内核定义内容的长度，直接设最大的长度给 内核内容的"字长"
  }
 copyReturn = copy_to_user(buf, Kmsg, numRead);
 // copy_to_user()函数: 将内核空间里面的数据 给到 用户空间的"接收缓存"中 (因为环境不通，要跨级传数据只能这样做)
 // 语法: copy_to_user(用户空间缓存，内核中的内容，要读取的字节数);
 
 // copy_to_user()会返回一个整数表示操作是否成功，"0"表示成功，其他值表示异常(就这个函数特例草!)
 if(copyReturn){ // 这里的逻辑是: 非"0"(copy2user'异常')就进去 [对应上面的'特例']
    printk("阿偶，copy_to_return似乎发生了错误QAQ...");
    return 9; //终止程序 (我们假设baka号码为异常XD)
 }
 // 否则程序即为正常结束，返回我们成功读取的字节数
 return numRead;
}

static int my_close(struct inode *inode, struct file *fp){
  // 具体"关闭文件"的方法
  printk("文件(设备)" deviceName "已被关闭\n");
  return 0;
}


static struct file_operations fops = { // 定义该字驱动表的"可操作项"，每一个项又要额外去写其对应的方法，不同的操作项间用","隔开
  // Tips: 这里的"可操作项"在源码里面非常之多，我们只需要"挑想要的"去实现就好了
  // (对于这个模块，我们只需用到 打开 读取 和 关闭 这些操作)
  owner: THIS_MODULE, // 声明是 哪个模块 将拥有这些 "操作项"
  open: my_open,
  read: my_read,
  release: my_close,

  //Tips: 这里其实就相当于 SFT表 和 inode表(具体操作到的"对象") 之间的映射
};

int __init hellodriver_init(void){
   // 声明"初始化函数"，仅在启动该模块时"调用一次"(之后就被清掉了), 此方法存在于Stack中
   // 而如果static应用在"此方法"中的某个变量上，则该"本地变量"则会变成"全局变量"

  int returnValue;
  returnValue = alloc_chrdev_region(&devnoStorage, startNumber, deviceCounts, deviceName);
  // 有关alloc_chrdev_region(设备号的"存放空间"(指针)，次设备号"起始点"，共有多少个不同的次设备号，该模块"人读的"名字)
  // 当模块成功申请到设备号时，会将其"申请成功的主设备号"进行返回[通常都>0] (上面的returnValue就是在接收主设备号的)
  // 简单来说，功能就是"定义存储次设备号，申请成功后传回主设备号"
  
  if (returnValue < 0){ //设备号申请异常
    printk("设备" deviceName "不能取得(主)设备号 \n"); // printk即是"printf"的平替，但只能在内核环境中用
    return returnValue; // 返回终止程序
  }

  major = MAJOR(devnoStorage); //从存储设备号的"空间"用 MAJOR()方法 取得其存储其存储的"主设备号"
  printk("设备" deviceName "初始化完成, 取得的主设备号为%d \n", major);

  // 接下来，我们来看看如何建立(注册)一个 字驱动表
  cdev.init(&dev, &fops); // 这里就相当于将这个"驱动文件"的 FDT表(操作请求) -> 系统SFT表(系统对应的操作方法) 做映射
  // cdev.init(存储字驱动表的"结构体对象"，该字驱动的"操作项结构体" [不同的可用操作方法, SFT表 -> inode表])

  // 一旦上面的init完成，之前声明的dev就成了一个 可用的"字驱动表"，接下来就是对这个驱动表做一些"补充设置"
  dev.owner = THIS_MODULE;
  
  // 一旦有了字驱动表，我们就可以往表里"add()设备"了
  int createResult; 
  createResult = cdev.add(&dev, &devnoStorage, deviceCounts);
  // cdev.add(创建的字驱动"表", 设备的"主设备号", 共"多少个设备");
  // 如果设备在表中创建成功的话，会返回一个"非0"的值
  if (createResult == 0) {
    printk("字驱动设备" deviceName "创建失败...\n");
    return createResult;
  }
  
  return 0; //所有的设备都被正确的加到了驱动表中，return 0 结束"初始化"代码段

}
int __exit hellodriver_init(void){
   // 模块在"卸载时"被调用 
   cdev_del($dev); // 调用cdev_del()方法删除 对应的"字驱动表" (删驱动表)
   unregister_chrdev_region(devnoStorage, deviceCounts); // 释放该字驱动表"所占的空间" (删内部驱动设备)
   printk("文件(设备)" deviceName "已卸载...\n");
}

module_init(hellodriver_init); // 指定模块的初始化函数 (上面定义好的)
module_exit(hellodriver_exit); // 用于指定模块(退出（卸载）函数)

MODULE_LICENSE("MIT"); //定义该模块采用的许可证协议
MODULE_AUTHOR("TechNiko_Pancake"); // 我是谁?
MODULE_DESCRIPTION("这里是有关该驱动文件的'描述'"); // 有关该模块的描述

// 驱动设备文件就是一个个"文件模块"，在内核需要时加载，不需要时卸载
// 在C编程的时候，注意分清declaration(仅'声明'类型) 和 defination(声明类型 + 分空间)的区别
// extern就是一个"仅declarion"关键字" (no program memory allocated)
