#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/printk.h>
#include <linux/cdev.h>
#include <linux/string.h>
#include <asm-generic/uaccess.h>

#define S_N 0
#define N_D 2
#define DRIVER_NAME "Hello world driver"

static dev_t mydevno;

static char msg[] = "This is the message in msg.";

int myopen(struct inode* inodep, struct file *fp){
    printk("Driver" DRIVER_NAME " opened. \n");
    return 0;
}

int myread(struct file *fp, char __user * buf, size_t count, loff_t *position){
    int num;
    int ret;
    if (count < strlen(msg)){
        num = count;
    } else {
        num = strlen(msg);
    }
    ret = copy_to_user(buf, msg, num);
    if(ret){
        printk("Fail to copy data from the kernal space to the user space. \n");
        return -1;
    }
    return num;
}

int myclose(struct inode* inodep, struct file *fp){
    printk("Driver " DRIVER_NAME " closed. \n");
    return 0;
}

struct file_operations myfops = {
    owner: THIS_MODULE,
    open: myopen,
    read: myread,
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
MODULE_DESCRIPTION("这个一份重新照抄的驱动代码，用于查错");
