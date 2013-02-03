#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/wakelock.h>
#include <linux/module.h>
#include <asm/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/spinlock.h>

#include <linux/leds-mt65xx.h>

static struct device_driver mt6577_efuse_drv =
{
    .name = "mt6577_efuse",
    .bus = &platform_bus_type,
    .owner = THIS_MODULE,
};

static ssize_t mt6577_efuse_show(struct device_driver *driver, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "Efuse(0xC1019070) = %d\n", *(volatile unsigned int*)(0xF1019070));
}

static ssize_t mt6577_efuse_store(struct device_driver *driver, const char *buf, size_t count)
{
    return count;
}

DRIVER_ATTR(mt6577_efuse, 0644, mt6577_efuse_show, mt6577_efuse_store);

int __init mt6577_efuse_init(void)
{
    int ret;

    ret = driver_register(&mt6577_efuse_drv);
    if (ret) {
        printk("fail to create mt6577 EFUSE driver\n");
    }

    ret = driver_create_file(&mt6577_efuse_drv, &driver_attr_mt6577_efuse);
    if (ret) {
        printk("fail to create mt6577 EFUSE sysfs files\n");
    }

    return 0;
}

arch_initcall(mt6577_efuse_init);
