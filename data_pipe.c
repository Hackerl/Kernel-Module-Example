#include "data_pipe.h"
#include <linux/proc_fs.h>
#include <linux/stat.h>
#include <linux/circ_buf.h>
#include <linux/slab.h>
#include <linux/smp.h>
#include <linux/uaccess.h>

int open_data_pipe(struct inode *node, struct file *f);
ssize_t read_data_pipe(struct file *f, char __user *buffer, size_t count, loff_t *pos);
ssize_t write_data_pipe(struct file *f, const char __user *buffer, size_t count, loff_t *pos);

struct ring_buffer_ctx {
    unsigned long size;
    struct circ_buf ring;
    spinlock_t producer_lock;
    spinlock_t consumer_lock;
};

static struct ring_buffer_ctx data_pipe_buffer_ctx = {};

static const struct file_operations data_pipe_fops = {
        .owner		= THIS_MODULE,
        .open		= open_data_pipe,
        .read		= read_data_pipe,
        .write      = write_data_pipe,
        .llseek		= no_llseek
};

int init_data_pipe(const char *name) {
    struct proc_dir_entry *parent_dir = NULL;

    spin_lock_init(&data_pipe_buffer_ctx.producer_lock);
    spin_lock_init(&data_pipe_buffer_ctx.consumer_lock);

    data_pipe_buffer_ctx.ring.head = 0;
    data_pipe_buffer_ctx.ring.tail = 0;

    data_pipe_buffer_ctx.size = 1024;
    data_pipe_buffer_ctx.ring.buf = kzalloc(1024, GFP_KERNEL);

    parent_dir = proc_mkdir(name, NULL);

    if (!parent_dir)
        return -1;

    if (!proc_create("data_pipe", S_IRUSR, parent_dir,
            &data_pipe_fops)) {
        remove_proc_subtree(name, NULL);
        return -1;
    }

    return 0;
}

int open_data_pipe(struct inode *node, struct file *f) {
    nonseekable_open(node, f);
    return 0;
}

ssize_t read_data_pipe(struct file *f, char __user *buffer, size_t count, loff_t *pos) {
    ssize_t ret = 0;
    void *cache_buffer = NULL;

    cache_buffer = kzalloc(count, GFP_KERNEL);

    ret = pop_data(cache_buffer, count);

    if (ret > 0) {
        ret = copy_to_user(buffer, cache_buffer, count) == 0 ? ret : 0;
    }

    if (cache_buffer) {
        kfree(cache_buffer);
        cache_buffer = NULL;
    }

    return ret;
}

ssize_t write_data_pipe(struct file *f, const char __user *buffer, size_t count, loff_t *pos) {
    ssize_t ret = 0;
    void *cache_buffer = NULL;

    cache_buffer = kzalloc(count, GFP_KERNEL);

    if (copy_from_user(cache_buffer, buffer, count) == 0) {
        ret = push_data(cache_buffer, count);
    }

    if (cache_buffer) {
        kfree(cache_buffer);
        cache_buffer = NULL;
    }

    return ret;
}

int cleanup_data_pipe(const char *name) {
    remove_proc_subtree(name, NULL);

    if (data_pipe_buffer_ctx.ring.buf) {
        kfree(data_pipe_buffer_ctx.ring.buf);
        data_pipe_buffer_ctx.ring.buf = NULL;
    }

    return 0;
}

unsigned long push_data(void *buffer, unsigned long length) {
    unsigned long ret = 0;
    unsigned long size, head, tail = 0;

    spin_lock(&data_pipe_buffer_ctx.producer_lock);

    size = data_pipe_buffer_ctx.size;
    head = data_pipe_buffer_ctx.ring.head;
    tail = READ_ONCE(data_pipe_buffer_ctx.ring.tail);

    if (CIRC_SPACE(head, tail, size) >= length) {
        ret = length;
        memcpy(data_pipe_buffer_ctx.ring.buf + head, buffer, length);
        smp_store_release(&data_pipe_buffer_ctx.ring.head, (head + length) & (size - 1));
    }

    spin_unlock(&data_pipe_buffer_ctx.producer_lock);

    return ret;
}

unsigned long pop_data(void *buffer, unsigned long length) {
    unsigned long ret = 0;
    unsigned long size, head, tail = 0;

    spin_lock(&data_pipe_buffer_ctx.consumer_lock);

    size = data_pipe_buffer_ctx.size;
    head = smp_load_acquire(&data_pipe_buffer_ctx.ring.head);
    tail = data_pipe_buffer_ctx.ring.tail;

    if (CIRC_CNT(head, tail, size) >= length) {
        ret = length;
        memcpy(buffer, data_pipe_buffer_ctx.ring.buf + tail, length);
        smp_store_release(&data_pipe_buffer_ctx.ring.tail, (tail + length) & (size - 1));
    }

    spin_unlock(&data_pipe_buffer_ctx.consumer_lock);

    return ret;
}