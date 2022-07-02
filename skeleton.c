
#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/uaccess.h>
#include <linux/sched/signal.h>
#include <linux/preempt.h>




int test_open(struct inode * i, struct file * f) {
    pr_info("***file opened\n");
    return 0;
}

ssize_t test_read(struct file * f, char __user * us, size_t s, loff_t * lf) {
    struct task_struct* proc;
    char* buff = kmalloc(4096,GFP_KERNEL);
    for_each_process(proc) {
        strcat(buff,proc->comm);
        strcat(buff," ");
    }

    if(copy_to_user(us,buff,4096)) {
        pr_info("***failed to write to user space\n");
    }
    pr_info("***file read %d\n",s);
    kfree(buff);
    return s; // need to return 0 to have cat not loop ?
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
    printk("test_write at: 0x%px\n",test_write);
    // get_nr_threads(p) - get threads no of task
    if ( likely(in_task())) {
         printk("current process: %s at 0x%px has stack at 0x%px at has %d threads \n",current->comm,current,current->stack,get_nr_threads(current));
    }
   else {
    printk("running in interrupt context\n");
   }

    struct task_struct* t,*p;

    // iterate over each thread
    int t_no = 0;
   do_each_thread(p,t){
        task_lock(t);
        printk("TGID: %d PID: %d Comm: %s\n",t->tgid,t->pid,t->comm);
        t_no++;
        task_unlock(t);
   } while_each_thread(p,t);

    printk("%d threads running\n",t_no);

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