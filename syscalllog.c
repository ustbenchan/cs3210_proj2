#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/init.h>         /* Needed for the macros */
#include <linux/syscalls.h>		/* Needed for getting syscall reference */
#include <linux/time.h>			/* Needed for getting microseconds from Epoch */
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>
#include <asm/current.h>
#include <linux/unistd.h>
#include <linux/namei.h>


#define MODULE_VERS "0.01"
#define MODULE_NAME "syscalllog"

static unsigned long *sys_call_table;
static bool replaced = false;

asmlinkage long (*original_sys_open) (const char __user * filename, int
flags, int mode);

asmlinkage long our_fake_open_function(const char __user *filename, int
flags, int mode)
{
	//struct timespec tv = current_kernel_time();
	// try to get the current user id, timestamp and filename
	
	//printk(KERN_INFO "SyscallLog: Uid: %d open %s at time %ld\n",current->uid,filename,tv.tv_nsec);
	printk(KERN_INFO "SyscallLog: intercept open function\n");
	return original_sys_open(filename,flags,mode);
}


// init process
static int __init logger_init(void)
{
	
	int i=1024;
	unsigned long *sys_table;
	int flag = 0;
	sys_table = (unsigned long *)simple_strtoul("0xffffffff804fbb80",NULL,16);
	/*while(i) {
		if(sys_table[__NR_read] == (unsigned long)sys_read)	{
			sys_call_table=sys_table;
			flag=1;
			break;   
		}
		i--;
		sys_table++;

	}
	*/
	flag = 1;
	sys_call_table = sys_table;
	if(flag) {
		original_sys_open = (void *)sys_call_table[__NR_open];
		sys_call_table = (unsigned long *)our_fake_open_function;
		printk(KERN_INFO "SyscallLog: Syscall open found, replacing it...\n");
		replaced = true;
	}
	else {
		printk(KERN_INFO "SyscallLog: Syscall open not found, nothing to do...\n");
	}
	/*
	*/
	printk(KERN_INFO "SyscallLog: Everything loaded, good to go!\n");
	printk(KERN_INFO "SyscallLog: All logs will be recorded from now...\n");
	return 0;
}

// close process
static void __exit logger_exit(void)
{
	// unlink the file
	if (replaced) {
		sys_call_table[__NR_open] = (unsigned long *)original_sys_open;
	}
	printk(KERN_INFO "SyscallLog: Warning: You have turned off the logger.\n");
}

module_init(logger_init);
module_exit(logger_exit);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("System calls interposer");
