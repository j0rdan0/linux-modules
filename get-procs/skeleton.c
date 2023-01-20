#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>

static int __init skeleton_init(void) {
    struct task_struct* curr,*t;
    int tasks = 0,threads = 0,k_threads = 0;
    int running = 0, sleeping =0;
    if(likely(in_task())) {
	    for_each_process(curr) {
			    switch(task_state_to_char(curr)) {
					    case 'R':
					    running++;
					    break;
					    case 'S':
					    sleeping++;
					    break;
					    case 'I':
					    sleeping++;
					    break;
					    } 
		if(!curr->mm) {
			k_threads++;
		}
		    for_each_thread(curr,t) {
					    			    
			    if (curr->tgid != t->pid) {
				    threads++;
				}
			    else if(curr->tgid == t->pid) {
				    tasks++;
			   }
		    }
	    }

    }	
    pr_info("tasks: [%d] threads: [%d] kernel threads: [%d] running: [%d] sleeping: [%d]\n",tasks-k_threads,threads,k_threads,running, sleeping);
	
	
    return 0;
}

static void __exit skeleton_exit(void) {
  
}

module_init(skeleton_init);
module_exit(skeleton_exit);


MODULE_LICENSE("GPL");
