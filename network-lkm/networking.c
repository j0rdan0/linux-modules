#include <linux/module.h>
#include <linux/init.h>

static int __init  networking_init(void) {
    pr_info("[*] loaded networking module\n");
    
    return 0;
}

static void __exit networking_exit(void) {
    pr_info("[*] unloaded networking module\n");
  
}

module_init(networking_init);
module_exit(networking_exit);


MODULE_LICENSE("GPL");