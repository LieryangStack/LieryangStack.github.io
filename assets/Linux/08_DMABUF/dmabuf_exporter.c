#include <linux/dma-buf.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

/* 字符设备的主设备号和次设备号 */
static dev_t dev_number;
static struct cdev c_dev;
static struct class *cl;

/**
 * @brief: DMA缓冲区映射回调函数
 */
static struct sg_table *exporter_map_dma_buf(struct dma_buf_attachment *attachment,
                                             enum dma_data_direction dir)
{
    return NULL;
}

/**
 * @brief: DMA缓冲区解除映射回调函数
 */
static void exporter_unmap_dma_buf(struct dma_buf_attachment *attachment,
                                   struct sg_table *table,
                                   enum dma_data_direction dir)
{
}

/**
 * @brief: 释放DMA缓冲区的回调函数
 */
static void exporter_release(struct dma_buf *dmabuf)
{
}

/**
 * @brief: 将DMA缓冲区映射到用户空间的回调函数
 */
static int exporter_mmap(struct dma_buf *dmabuf, struct vm_area_struct *vma)
{
    return -ENODEV;
}

/**
 * @brief: 开始CPU访问DMA缓冲区的回调函数
 */
static int exporter_begin_cpu_access(struct dma_buf *dmabuf, enum dma_data_direction dir)
{
    return 0;
}

/**
 * @brief: 结束CPU访问DMA缓冲区的回调函数
 */
static int exporter_end_cpu_access(struct dma_buf *dmabuf, enum dma_data_direction dir)
{
    return 0;
}

/* 定义DMA缓冲区操作的回调函数集合 */
static const struct dma_buf_ops exp_dmabuf_ops = {
    .map_dma_buf = exporter_map_dma_buf,
    .unmap_dma_buf = exporter_unmap_dma_buf,
    .release = exporter_release,
    .mmap = exporter_mmap,
    .begin_cpu_access = exporter_begin_cpu_access,
    .end_cpu_access = exporter_end_cpu_access,
};

/* 设备文件操作函数 */
static int my_open(struct inode *i, struct file *f)
{
    return 0;
}

static int my_close(struct inode *i, struct file *f)
{
    return 0;
}

static long my_ioctl(struct file *f, unsigned int cmd, unsigned long arg)
{
    return 0;
}

static struct file_operations pugs_fops = {
    .owner = THIS_MODULE,
    .open = my_open,
    .release = my_close,
    .unlocked_ioctl = my_ioctl,
};

/**
 * @brief: 模块初始化函数
 */
static int __init exporter_init(void)
{
    /* 用于定义并初始化 exp_info 变量 */
    DEFINE_DMA_BUF_EXPORT_INFO(exp_info);

    exp_info.ops = &exp_dmabuf_ops;       // 设置操作回调函数集合
    exp_info.size = PAGE_SIZE;            // 设置DMA缓冲区大小
    exp_info.flags = O_CLOEXEC;           // 设置文件描述符标志
    exp_info.priv = "null";               // 设置私有数据字段

    struct dma_buf *dmabuf;

    /* 将exp_info中定义的DMA-BUF导出到内核中，会根据exp_info创建一个新的DMA-BUF对象 */
    dmabuf = dma_buf_export(&exp_info);

    /* 注册字符设备 */
    if (alloc_chrdev_region(&dev_number, 0, 1, "dma_buf_device") < 0)
    {
        return -1;
    }

    if ((cl = class_create("chardev")) == NULL)
    {
        unregister_chrdev_region(dev_number, 1);
        return -1;
    }

    if (device_create(cl, NULL, dev_number, NULL, "dma_buf_device") == NULL)
    {
        class_destroy(cl);
        unregister_chrdev_region(dev_number, 1);
        return -1;
    }

    cdev_init(&c_dev, &pugs_fops);

    if (cdev_add(&c_dev, dev_number, 1) == -1)
    {
        device_destroy(cl, dev_number);
        class_destroy(cl);
        unregister_chrdev_region(dev_number, 1);
        return -1;
    }

    return 0;
}

/**
 * @brief: 模块退出函数
 */
static void __exit exporter_exit(void)
{
    cdev_del(&c_dev);
    device_destroy(cl, dev_number);
    class_destroy(cl);
    unregister_chrdev_region(dev_number, 1);
}

module_init(exporter_init);
module_exit(exporter_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple DMA-Buf exporter example with char device");
MODULE_IMPORT_NS(DMA_BUF);
