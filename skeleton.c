#include <linux/module.h>
#include <linux/init.h>

static int __init skeleton_init(void) {
    printk("[*]loaded debug module\n");
    return 0;
}

static void __exit skeleton_exit(void) {
    printk("[*]unloaded debug module\n");
}

module_init(skeleton_init);
module_exit(skeleton_exit);


MODULE_LICENSE("GPL");