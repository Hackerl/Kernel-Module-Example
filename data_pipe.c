#include "data_pipe.h"
#include <linux/proc_fs.h>
#include <linux/stat.h>

int open_data_pipe(struct inode *node, struct file *f);
ssize_t read_data_pipe(struct file *f, char __user *buffer, size_t count, loff_t *pos);
int release_data_pipe(struct inode *node, struct file *f);

static const struct file_operations data_pipe_fops = {
        .owner		= THIS_MODULE,
        .open		= open_data_pipe,
        .read		= read_data_pipe,
        .release	= release_data_pipe,
        .llseek		= no_llseek
};

int init_data_pipe(const char *name) {
    struct proc_dir_entry *parent_dir = proc_mkdir(name, NULL);

    if (!parent_dir)
        return -1;

    if (!proc_create_data("data_pipe", S_IRUSR, parent_dir,
            &data_pipe_fops, NULL)) {
        remove_proc_subtree(name, NULL);
        return -1;
    }

    return 0;
}

int open_data_pipe(struct inode *node, struct file *f) {
    return 0;
}

ssize_t read_data_pipe(struct file *f, char __user *buffer, size_t count, loff_t *pos) {
    return 0;
}

int release_data_pipe(struct inode *node, struct file *f) {
    return 0;
}

int cleanup_data_pipe(const char* name) {
    remove_proc_subtree(name, NULL);
    return 0;
}
