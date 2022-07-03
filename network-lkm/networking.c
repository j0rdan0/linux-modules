#include <linux/module.h>
#include <linux/init.h>
#include <linux/netdevice.h>

static int __init  networking_init(void) {

    pr_info("[*] loaded networking module\n");
    struct net_device* dev;
    read_lock(&dev_base_lock);
    dev = first_net_device(&init_net);
    while(dev) {
        pr_info("dev name: %s with MTU: %u on IRQ: %d\n",dev->name,dev->mtu,dev->irq);
        dev = next_net_device(dev);
    }
    read_unlock(&dev_base_lock);
    
    return 0;
}

static void __exit networking_exit(void) {
    pr_info("[*] unloaded networking module\n");
  
}

module_init(networking_init);
module_exit(networking_exit);


MODULE_LICENSE("GPL");