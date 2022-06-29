#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/slab.h>


#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

int test_open(struct inode * i, struct file * f) {
    pr_info("***file opened\n");
    return 0;
}

ssize_t test_read(struct file * f, char __user * us, size_t s, loff_t * lf) {
    pr_info("***file read %d\n",s);
    return s;
}

static const struct file_operations test_fops = {
    .open = test_open,
    .read = test_read,
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