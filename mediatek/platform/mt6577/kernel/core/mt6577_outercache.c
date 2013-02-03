#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/platform_device.h>

#include <asm/cacheflush.h>

static struct device_driver outercache_drv =
{
    .name = "outercache",
    .bus = &platform_bus_type,
    .owner = THIS_MODULE,
};

static ssize_t outer_sync_show(struct device_driver *driver, char *buf)
{
    return 0;
}

static ssize_t outer_sync_store(struct device_driver *driver, const char *buf, size_t count)
{
    outer_sync();

    return count;
}

DRIVER_ATTR(outer_sync, 0664, outer_sync_show, outer_sync_store);

int __init outercache_init(void)
{
    int ret;

    ret = driver_register(&outercache_drv);
    if (ret) {
        printk("fail to create outercache driver\n");
    }
    ret = driver_create_file(&outercache_drv, &driver_attr_outer_sync);
    if (ret) {
        printk("fail to create outercache sysfs files\n");
    }

    return 0;
}

arch_initcall(outercache_init);
