#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>

static int procinfo_show(struct seq_file *m, void *v)
{
    struct task_struct *task;
    
    for_each_process(task) {
        seq_printf(m, "PID: %d, Name: %s, State: %ld\n",
                   task->pid, task->comm, task->stats);
    }
    
    return 0;
}

static int procinfo_open(struct inode *inode, struct file *file)
{
    return single_open(file, procinfo_show, NULL);
}

static const struct proc_ops procinfo_fops = {
    .proc_open = procinfo_open,
    .proc_read = seq_read,
    .proc_lseek = seq_lseek,
    .proc_release = single_release,
};

static int __init procinfo_init(void)
{
    proc_create("procinfo", 0, NULL, &procinfo_fops);
    return 0;
}

static void __exit procinfo_exit(void)
{
    remove_proc_entry("procinfo", NULL);
}

module_init(procinfo_init);
module_exit(procinfo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Process Information Kernel Module");
