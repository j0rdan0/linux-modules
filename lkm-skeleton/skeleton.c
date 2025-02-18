#include <linux/module.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/sched.h>
#include <linux/preempt.h>
#include <linux/sched/signal.h>
#include <linux/gfp.h>
#include <asm/pgtable.h>
#include <linux/vmalloc.h>
#include <asm/io.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/umh.h>
#include <linux/file.h>
#include <linux/rcupdate.h> 
#include <linux/fdtable.h>
#include <linux/xarray.h>
#include <linux/pagemap.h>
#include <linux/highmem.h>
#define PID 10822
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/wait.h>
#include <linux/types.h>
#include <linux/uaccess.h>


int major_no;
int reg_err;

static ssize_t test_read(struct file* filp,char __user* buff,size_t len,loff_t* offset) {
	char* msg = "from kernel space";
	pr_info("writing to userspace");
	return copy_to_user(buff,msg,strlen(msg));
}
static ssize_t test_write(struct file* filp,const char __user* buff,size_t len,loff_t* offset) {
	char* msg = kmalloc(len+1,GFP_KERNEL);
	ssize_t wrote;
	wrote = len - copy_from_user(msg,buff,len);
	if(len >0)
	{
		msg[wrote] = '\0';
		pr_info("read from userpasce: %s\n",msg);
	}
	kfree(msg);
	return wrote;
}
static int test_open(struct inode* inode, struct file* filp) {
	pr_info("opened device with inode: %ul\n",filp->f_inode->i_ino);
	return 0;

}
static int test_release(struct inode* inode,struct file* filp) {
	pr_info("closed device with inode: %ul\n",inode->i_ino);
	return 0;
}


const static struct file_operations fops = {
	.read = test_read,
	.write = test_write,
	.open = test_open,
	.release = test_release

};

static int register_dummy_dev(void) {
	major_no = register_chrdev(0,"test-dev",&fops);
	if(major_no < 0) {
		pr_warn("failed creating char device\n");
		return -1;
	}
	pr_info("created char device with major no: %d\n",major_no);
	pr_info("finish creation with: mknod /dev/test-dev c %d 0\n",major_no);
	return 0;
}


static void get_size(void) {
	struct wait_queue_head t;
	struct fdtable fdtab;
	pr_info("sizoef struct wait %d , sizeof fdtable:%d\n",sizeof(t),sizeof(fdtab));

}

struct proc_dir_entry* d_test;
static int test_fs_show(struct seq_file* m,void* v) {
	seq_puts(m,"testing procfs usage");
	seq_putc(m,'\n');
	return 0;
}

static int test_proc_init(void) {
	d_test = proc_create_single("testing",0,NULL,test_fs_show);
	return 0;
}
static int test_proc_exit(void) {
	proc_remove(d_test);
	return 0;
}

static void get_process_file(void) {
	struct pid* pid = find_get_pid(PID);
	void *entry;
	unsigned long index;
	struct task_struct* sshd = get_pid_task(pid,PIDTYPE_PID);
	struct fdtable* fdt = rcu_dereference(sshd->files->fdt);
	struct file** f = rcu_dereference(fdt->fd);
	struct xarray *pages;
	pr_info("max fds for sshd: %d\n",fdt->max_fds);
	struct inode* inode = f[5]->f_inode;
	struct address_space* mapping = inode->i_mapping;
	pages =  &mapping->i_pages;

	if (mapping_empty(mapping)) {
			pr_info("mapping for file empty");
			return;
	}
	else {
		pr_info("mapping of file not empty");
		char* data_temp;
		struct page* temp = read_cache_page(mapping,0,0,data_temp);
		if(temp != NULL) {
			lock_page(temp);
			pr_info("got page:");
			data_temp=kmap(temp);
			print_hex_dump(KERN_INFO,"",DUMP_PREFIX_OFFSET,16,1,data_temp,64,true);
			kunmap(temp);
			unlock_page(temp);
			put_page(temp);
		}
		else {
			pr_warn("failed getting page for mapping");
		}
	}
	struct page* cache_page = pagecache_get_page(mapping,0,0x00000100,GFP_KERNEL);
	if(cache_page == NULL) {
		pr_warn("failed getting cached page");
	}
	else {
		pr_info("got cached page at: 0x%p\n",cache_page);
	}
	xa_for_each(pages, index, entry) {
        struct page *page;
	char* data;
        // Check if entry is a page
        if (xa_is_value(entry)) {
            pr_info("Index %lu: xa_value entry\n", index);
            continue;
        }

        page = entry;
        if (page) {
		  data = kmap_local_page(page);

            pr_info("Index %lu: Page found, flags: 0x%lx\n", index, page->flags);
if (data) {
                // Print the first 64 bytes of the page as a hex dump
                pr_info("Index %lu: Page data (first 64 bytes):\n", index);
                print_hex_dump(KERN_INFO, "  ", DUMP_PREFIX_OFFSET, 16, 1, data, 64, true);

                // Unmap the page
                kunmap_local(data);
            } else {
                pr_warn("Failed to map page at index %lu\n", index);
            }
        }
	if ( index == 10) {
		break;
	}
    }

}
static void run_usermode_app(void) {
	char path[] = "/bin/dd";
	static char* envp[] = { 
		"HOME=/",
		"TERM=linux",
		"PATH=/sbin:/usr/sbin:/bin:/usr/bin",
		NULL
	};
	char** argv = kmalloc(sizeof(char* [5]),GFP_KERNEL);
	argv[0] = "/bin/dd";
	argv[1] = "status=none";
	argv[2] = "if=/etc/passwd";
	argv[3] = "of=/tmp/passwd";
	argv[4] = NULL;
	int ret = call_usermodehelper(path,argv,envp,UMH_KILLABLE);
	pr_info("returned with code: %d\n",ret);
	kfree(argv);	
}

static void add_char_dev(void) {
	struct cdev* test_cdev = cdev_alloc();
	dev_t d;
	alloc_chrdev_region(&d,1, 20, "test_cdev");

	struct file_operations f_ops;
	cdev_init(test_cdev,&f_ops);
	int ret = cdev_add(test_cdev,d,1);
	if ( ret < 0) {
		pr_warn("failed to add char device to system");
	}
	else {
		pr_info("added char device to system with major no: %d\n",d);

		cdev_del(test_cdev);
		pr_info("remove char device from system");
	}

}


// to test that kmalloc allocates contigous memory, while vmalloc does not
static void kmalloc_test(void) {

	void* buff = kmalloc(PAGE_SIZE*3,GFP_KERNEL);

	pr_info("allocated mem at with kmalloc: %pK, PA: 0x%lx\n",buff,virt_to_phys(buff));
	pr_info("second page of buffer allocated with kmalloc at PA: 0x%lx\n",virt_to_phys(buff+PAGE_SIZE));
	pr_info("third page of buffer allocated with kmalloc at PA: 0x%lx\n",virt_to_phys(buff+(PAGE_SIZE*2)));

	void* buff_v = vmalloc(PAGE_SIZE*3);

	pr_info("allocated mem at with vmalloc: %pK\n",buff_v);
	struct page* p = vmalloc_to_page(buff_v);
	pr_info("got page at: %pK with PA: 0x%llx\n",p,(unsigned long long)page_to_phys(p));
	p = vmalloc_to_page(buff_v+PAGE_SIZE);

	pr_info("got page 2 at: %pK with PA: 0x%llx\n",p,(unsigned long long)page_to_phys(p));

	p = vmalloc_to_page(buff_v+(PAGE_SIZE*2));

	
	pr_info("got page 3 at: %pK with PA: 0x%llx\n",p,(unsigned long long)page_to_phys(p));

	kfree(buff);
	vfree(buff_v);
}


struct file* f = NULL;
static int get_file2(char* filename) {
	 f = filp_open(filename,O_RDONLY,0);
	if (IS_ERR(f)) {
		pr_warn("failed openinig file %s\n",filename);
		return -1;

	}
	else {
		pr_info("opened file %s\n",filename);
		int size = (f->f_inode->i_size)*8; // converting to bits from bytes size
		pr_info("size %d\n",size);
		char* buff = kmalloc(size,GFP_KERNEL);
		loff_t pos = 0;
		kernel_read(f,buff,size,&pos);
		pr_info("%s\n",buff);

	}
	return 0;

}

static int __init skeleton_init(void) {
    pr_info("[*] loaded debug module\n");
    char* filename = "/etc/passwd";
    get_file2(filename);
    reg_err = register_dummy_dev();
    return 0;
    }

static void __exit skeleton_exit(void) {
	if(f != NULL) {
		filp_close(f,0);
	}
	if(reg_err == 0) {
		unregister_chrdev(major_no,"test-dev");
	}
    pr_info("[*] unloaded debug module\n");

  
}

module_init(skeleton_init);
module_exit(skeleton_exit);


MODULE_LICENSE("GPL");
MODULE_INFO(info,"testing shit in kernel mode, cuz why not");
MODULE_ALIAS("skel");
MODULE_DESCRIPTION("trying kernel development");
