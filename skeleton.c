#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h>



#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

int test_open(struct inode * i, struct file * f) {
    pr_info("***file opened\n");
    return 0;
}

ssize_t test_read(struct file * f, char __user * us, size_t s, loff_t * lf) {
    struct task_struct* proc;
    char* buff = kmalloc(4096,GFP_KERNEL);
    for_each_process(proc) {
        //printk(proc->comm);
        strcat(buff,proc->comm);
        strcat(buff," ");
    }
    printk(buff);

    if(copy_to_user(us,buff,4096)) {
        pr_info("***failed to write to user space\n");
    }
    pr_info("***file read %d\n",s);
    kfree(buff);
    return s;
}
ssize_t test_write(struct file * f, const char __user * us, size_t s, loff_t * lf) {
    char* buff = kmalloc(s,GFP_KERNEL);
    if(copy_from_user(buff,us,(unsigned long)s)) {
         pr_info("***failed to read from user space\n");
    }
    printk(buff);
    pr_info("***wrote to file\n");
    kfree(buff);
    return s;
}

static const struct file_operations test_fops = {
    .open = test_open,
    .read = test_read,
    .write = test_write,
};

static struct miscdevice test_device = { 
    .minor = MISC_DYNAMIC_MINOR,
    .name = "test_device",
    .mode = 0666,
    .fops = &test_fops, 
};


static int __init skeleton_init(void) {
    pr_info("[*] loaded debug module\n");
    msleep(1000);
    pr_info("*** after sleeping\n");
    
    int ret;
    ret = misc_register(&test_device);
    if (ret != 0) {
        pr_fmt("failed to register test device\n");
        return ret;   
    }
    char msg[1024];
    sprintf(msg,"registered device %s\n",test_device.name);

    printk(msg);
    
    return 0;
}

static void __exit skeleton_exit(void) {
    pr_info("[*] unloaded debug module\n");
    misc_deregister(&test_device);
    pr_fmt("deregistered device\n");   
  
}

module_init(skeleton_init);
module_exit(skeleton_exit);


MODULE_LICENSE("GPL");