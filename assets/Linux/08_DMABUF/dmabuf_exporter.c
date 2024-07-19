#include <linux/dma-buf.h>
#include <linux/module.h>

static struct sg_table *exporter_map_dma_buf(struct dma_buf_attachment *attachment,
                                             enum dma_data_direction dir)
{
    return NULL;
}

static void exporter_unmap_dma_buf(struct dma_buf_attachment *attachment,
                                   struct sg_table *table,
                                   enum dma_data_direction dir)
{
}

static void exporter_release(struct dma_buf *dmabuf)
{
}

static int exporter_mmap(struct dma_buf *dmabuf, struct vm_area_struct *vma)
{
    return -ENODEV;
}

static int exporter_begin_cpu_access(struct dma_buf *dmabuf, enum dma_data_direction dir)
{
    return 0;
}

static int exporter_end_cpu_access(struct dma_buf *dmabuf, enum dma_data_direction dir)
{
    return 0;
}

static const struct dma_buf_ops exp_dmabuf_ops = {
    .map_dma_buf = exporter_map_dma_buf,
    .unmap_dma_buf = exporter_unmap_dma_buf,
    .release = exporter_release,
    .mmap = exporter_mmap,
    .begin_cpu_access = exporter_begin_cpu_access,
    .end_cpu_access = exporter_end_cpu_access,
};

static int __init exporter_init(void)
{
    DEFINE_DMA_BUF_EXPORT_INFO(exp_info);
    struct dma_buf *dmabuf;

    exp_info.ops = &exp_dmabuf_ops;
    exp_info.size = PAGE_SIZE;
    exp_info.flags = O_CLOEXEC;
    exp_info.priv = "null";

    dmabuf = dma_buf_export(&exp_info);

    return 0;
}

module_init(exporter_init);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple DMA-Buf exporter example");
MODULE_IMPORT_NS(DMA_BUF);
