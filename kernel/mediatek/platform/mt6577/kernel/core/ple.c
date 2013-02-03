#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/interrupt.h>

#if 0   /* obsolete defines for legacy interface */
#define SIZE_PER_PLE_BLOCK (4 * 16 * 1024)  /* 16K word */
#define PLE_BLOCK_MAX 256
#define MAX_SIZE_PER_PRELOAD (SIZE_PER_PLE_BLOCK * PLE_BLOCK_MAX)
#endif

#define LENGTH_MAX 0x3FFF
#define STRIDE_MAX 0xFF
#define NR_BLK_MAX 0xFF

DEFINE_SEMAPHORE(ple_sem);

static unsigned int ple_addr, ple_len, ple_stride, ple_nr_blk;

/*
 * ple_write: implement the write operation
 */
static ssize_t ple_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos)
{
    unsigned int addr, Rt;
    unsigned int *addr1 = (unsigned int *)buf, *addr2 = (unsigned int *)(buf + 4);

    if (get_user(addr, addr1)) {
        return -EFAULT;
    }
    if (get_user(Rt, addr2)) {
        return -EFAULT;
    }
    asm volatile (
        "mcrr p15, 0, %0, %1, c11\n"
        :
        : "r" (addr), "r" (Rt)
        : "cc");

    return count;
}

#if 0
/*
 * ple_addr_show: implement the sysfs show method
 */
static ssize_t ple_addr_show(struct device_driver *driver, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "addr = 0x%x\n", ple_addr);
}

/*
 * ple_addr_store: implement the sysfs store method
 */
static ssize_t ple_addr_store(struct device_driver *driver, const char *buf, size_t count)
{
    char *p = (char *)buf;

    ple_addr = simple_strtoul(p, &p, 16);
    ple_addr = ALIGN(ple_addr, 4);

    return count;
}

DRIVER_ATTR(addr, 0664, ple_addr_show, ple_addr_store);

/*
 * ple_len_show: implement the sysfs show method
 */
static ssize_t ple_len_show(struct device_driver *driver, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "len = 0x%x\n", ple_len);
}

/*
 * ple_len_store: implement the sysfs store method
 */
static ssize_t ple_len_store(struct device_driver *driver, const char *buf, size_t count)
{
    char *p = (char *)buf;
    unsigned val;

#if 0   /* obsolete code for legacy interface */
    ple_len = simple_strtoul(p, &p, 16);
    ple_len &= ~(4 - 1); 
    if (ple_len > MAX_SIZE_PER_PRELOAD) {
        ple_len = MAX_SIZE_PER_PRELOAD;
    }
#endif
    val = simple_strtoul(p, &p, 16);
    if (val <= LENGTH_MAX) {
        ple_len = val;
    }

    return count;
}

DRIVER_ATTR(len, 0664, ple_len_show, ple_len_store);

/*
 * ple_stride_show: implement the sysfs show method
 */
static ssize_t ple_stride_show(struct device_driver *driver, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "stride = 0x%x\n", ple_stride);
}

/*
 * ple_stride_store: implement the sysfs store method
 */
static ssize_t ple_stride_store(struct device_driver *driver, const char *buf, size_t count)
{
    char *p = (char *)buf;
    unsigned int val;

    val = simple_strtoul(p, &p, 16);
    if (val <= STRIDE_MAX) {
        ple_stride = val;
    }

    return count;
}

DRIVER_ATTR(stride, 0664, ple_stride_show, ple_stride_store);

/*
 * ple_nr_blk_show: implement the sysfs show method
 */
static ssize_t ple_nr_blk_show(struct device_driver *driver, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "number of blocks = 0x%x\n", ple_nr_blk);
}

/*
 * ple_nr_blk_store: implement the sysfs store method
 */
static ssize_t ple_nr_blk_store(struct device_driver *driver, const char *buf, size_t count)
{
    char *p = (char *)buf;
    unsigned int val;

    val = simple_strtoul(p, &p, 16);
    if (val <= NR_BLK_MAX) {
        ple_nr_blk = val;
    }

    return count;
}

DRIVER_ATTR(nr_blk, 0664, ple_nr_blk_show, ple_nr_blk_store);
#endif

/*
 * ple_show: implement the sysfs show method
 */
static ssize_t preload_show(struct device_driver *driver, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "addr = 0x%x\n len = 0x%x\n stride = 0x%x\n number of blocks = 0x%x\n",ple_addr, ple_len, ple_stride, ple_nr_blk); 
}

/*
 * ple_store: implement the sysfs store method
 */
static ssize_t preload_store(struct device_driver *driver, const char *buf, size_t count)
{
#if 0   /* obsolete code for legacy interface */
    unsigned int l, nr_blk, Rt;

    if (ple_len < SIZE_PER_PLE_BLOCK) {
        l = (ple_len >> 2) - 1;
        Rt = l << 18;
        asm volatile (
            "mcrr p15, 0, %0, %1, c11\n"
            :
            : "r" (ple_addr), "r" (Rt)
            : "cc");
    } else {
        l = (SIZE_PER_PLE_BLOCK >> 2) - 1;
        nr_blk = (ple_len / SIZE_PER_PLE_BLOCK) - 1;
        Rt = (l << 18) | (nr_blk << 2);
        asm volatile (
            "mcrr p15, 0, %0, %1, c11\n"
            :
            : "r" (ple_addr), "r" (Rt)
            : "cc");

        if ((ple_len % SIZE_PER_PLE_BLOCK) != 0) {
            l = ((ple_len % SIZE_PER_PLE_BLOCK) >> 2) - 1;
            Rt = l << 18;
            asm volatile (
                "mcrr p15, 0, %0, %1, c11\n"
                :
                : "r" (ple_addr + SIZE_PER_PLE_BLOCK * (nr_blk + 1)), "r" (Rt)
                : "cc");
        }
    }
#endif

    char *command;
    char *token [5];
    char *ptr;
    int  i = 0;
    unsigned int Rt;		
    if((strlen(buf)+1) > 128)
    {
      printk("EMI MPU: command overflow!");
      return -1;
    }
	
	if(in_interrupt()) {
		return -1;
	}

    command = kmalloc((size_t)128, GFP_KERNEL);    
    strcpy(command, buf);        
    ptr = (char *)buf;
	
    while(ptr != NULL) {
        ptr = strsep (&command, " ");
        token[i] = ptr;
//        printk("token[%d] = %s\n",  i,  token[i]);
        i++;
    }
	
    if(i != 5){
		printk("The number of parameter is wrong!!!\n");
		return -1;
    }
	
    down(&ple_sem);
	
    ple_addr = simple_strtoul(token[0], &token[0], 16);
	ple_addr = ALIGN(ple_addr, 4);
	
    ple_len = simple_strtoul(token[1], &token[1], 16);
	if (ple_len > LENGTH_MAX) {
        printk("error preload length !\n");
		goto preload_store_exit;
    }
	
	ple_stride = simple_strtoul(token[2], &token[2], 16);
    if (ple_stride > STRIDE_MAX) {
        printk("error preload stride !\n");
		goto preload_store_exit;
    }

	ple_nr_blk = simple_strtoul(token[3], &token[3], 16);
    if (ple_nr_blk > NR_BLK_MAX) {
        printk("error preload block !\n");
		goto preload_store_exit;
    }


    Rt = (ple_len << 18) | (ple_stride << 10) | (ple_nr_blk << 2);
    asm volatile (
        "mcrr p15, 0, %0, %1, c11\n"
        :
        : "r" (ple_addr), "r" (Rt)
        : "cc");

preload_store_exit:		
	up(&ple_sem);	
    return count;
}

DRIVER_ATTR(preload, 0664, preload_show, preload_store);

static struct device_driver ple_drv =
{
    .name = "ple",
    .bus = &platform_bus_type,
    .owner = THIS_MODULE,
};

static struct file_operations ple_fops =
{
    .write = ple_write,
};

static struct miscdevice ple_dev =
{
    .minor = MISC_DYNAMIC_MINOR,
    .name = "ple",
    .fops = &ple_fops,
};

/*
 * ple_init: module init function
 */
static int __init ple_init(void)
{
    int err;

    err = driver_register(&ple_drv);
    if (err) {
        printk(KERN_ALERT "Unable to create ple driver\n");
        goto ple_init_exit;
    }
#if 0
    err = driver_create_file(&ple_drv, &driver_attr_addr);
    err |= driver_create_file(&ple_drv, &driver_attr_len);
    err |= driver_create_file(&ple_drv, &driver_attr_stride);
    err |= driver_create_file(&ple_drv, &driver_attr_nr_blk);
#endif
    err |= driver_create_file(&ple_drv, &driver_attr_preload);
    if (err) {
        printk(KERN_ALERT "Unable to create ple sysfs files\n");
        goto ple_init_exit;
    }

    err = misc_register(&ple_dev);
    if (err) {
        printk(KERN_ALERT "Unable to register ple device! err = %d\n", err);
        goto ple_init_exit;
    }

ple_init_exit:
    return 0;
}

/*
 * ple_exit: module exit function
 */
static void __exit ple_exit(void)
{
}

module_init(ple_init);
module_exit(ple_exit);
