// 在重新整理完'驱动文件'相关的知识后，我们现在来写一个LED控制程序玩玩
// 主要用到了write: 向板子写入信息

#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <asm-generic/uaccess.h>

// 控制板子上的LED用到了以下的两个"新库"
#include <linyx/gpio.h>
#include <mach/gpio.h>

#define S_N 0
#define N_D 2
#define DRIVER_NAME "LED_Driver"
#define LED1 S5PV210_GPJ2(0) //定义咱开发板上的LED指示灯
#define LED_ON 0 // 这里有点'反直觉' (板子定义的0是高电压)
#define LED_OFF 1 // 1 低电压(关)

static dev_t mydevno;

static char msg[] = "This is the message in msg.";

int myopen(struct inode* inodep, struct file *fp){
    printk("Driver " DRIVER_NAME " opened. \n");
    return 0;
}

int myclose(struct inode* inodep, struct file *fp){
    printk("Driver " DRIVER_NAME " closed. \n");
    return 0;
}

int mywrite(struct file *fp, char __user * buf, size_t count, loff_t * position){
    /*fp: 指向的用户空间"文件对象"
     buf: 指向用户空间缓冲区的指针
     count: 限定"写入信息的多少"
     position: 内核缓存中'写入指针'的位置 (从哪里准备开始写)

     (整体的实现逻辑就是一个简单的二元判断开关)
    */ 
   // 首先的逻辑便是从板子上读取到 "板子灯" 的状态 
   char userLed_status;
   int copyStatus;
   // copy_from_user(&本地接收的对象[地址空间], 用户的缓存, 读入多少[字节整数]);
   copyStatus = copy_from_user(&userLed_status, buf，1);

   if(copyStatus!=0){ //默认可以直接写成if(copyStatus)，C里面只要不是0一律视为true :p
     printk("尝试获取LED的状态时发生了错误OAO!!!");
   } 

   if(userLed_status==1){
    // 读到的LED处于关闭状态
    gpio_set_value(LED1, LED_ON); // 这个方法就是厂家定义的控制方法，课上直接照抄的
   }else{
    // 读到的LED处于打开状态
    gpio_set_value(LED1, LED_OFF);
   }

}

struct file_operations myfops = {
    owner: THIS_MODULE,
    open: myopen,
    write: mywrite,
    release: myclose,
};

struct cdev mydev;

static int __init helloworld_init(void) {
    int ret;
    unsigned int major; //怎么多了个unsinged?
    unsigned int minor;

    // register a major number
    ret = alloc_chrdev_region(&mydevno, S_N, N_D, DRIVER_NAME);
    if (ret < 0){
        printk("alloc_chrdev_region failed. Driver" DRIVER_NAME "cannot get major number. \n");
        return ret;
    }

    major = MAJOR(mydevno);
    minor = MINOR(mydevno);

    printk("Driver " DRIVER_NAME "initialized (major number = %d, minor number = %d). \n", major, minor);
    printk("Driver " DRIVER_NAME "initialized (major number = %d, minor number = %d). \n", major, minor);

    //register a char device
    cdev_init(&mydev, &myfops);
    mydev.owner = THIS_MODULE;
    ret = cdev_add(&mydev, mydevno, N_D);
    if (ret) {
        printk("Driver " DRIVER_NAME "register fail. \n");
        return ret;
    }
    return 0;

}

static void __exit helloworld_exit(void) {
    cdev_del(&mydev);
    unregister_chrdev_region(mydevno, N_D);
    printk("Driver" DRIVER_NAME "unloaded. \n");
}

module_init(helloworld_init);
module_exit(helloworld_exit);

MODULE_LICENSE("MIT");
MODULE_AUTHOR("TechNiko_Pancake");
MODULE_DESCRIPTION("直接照着代码小改的一个LED控制驱动");
