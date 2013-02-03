/******************************************************************************
 * camera_eis.c - MT6577 Linux EIS Device Driver
 *
 * Copyright 2008-2009 MediaTek Co.,Ltd.
 *
 * DESCRIPTION:
 *     
 *
 ******************************************************************************/

#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <asm/io.h>
//#include <asm/tcm.h>
#include <linux/proc_fs.h>  //proc file use
//
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/uaccess.h>
#include <asm/atomic.h>
#include <linux/sched.h>
#include <linux/mm.h>
//
#include <mach/mt6577_reg_base.h>
#include <mach/camera_eis.h>
#include <mach/mt6577_clock_manager.h>

//

/*******************************************************************************
*
********************************************************************************/
#define EIS_TAG                 "[EIS kernel] "
#define EIS_LOG(fmt, arg...)    printk(EIS_TAG fmt, ##arg)
#define EIS_ERR(fmt, arg...)    printk(EIS_TAG "Err: %5d:, "fmt, __LINE__, ##arg)
//
#define EIS_DEV_NAME            "camera-eis"

/*******************************************************************************
*
********************************************************************************/
// Register definition

/*******************************************************************************
*
********************************************************************************/
static spinlock_t eis_lock;
static bool bClockState = FALSE;

/*******************************************************************************
*
********************************************************************************/
static int mt_eis_dump_reg(__user mt_eis_reg_t *preg_io)
{
    int ret = 0;
    mt_eis_reg_t reg;
    int i;

    spin_lock_irq(&eis_lock);
    
    for (i = 0; i < 11; i++) {
        reg.val[i] = ioread32(EIS_BASE + 4 * i);
    }

    reg.val[11] = CRZ_BASE + 0x10;            // CRZ source size
    reg.val[12] = ioread32(CRZ_BASE + 0x10);
    reg.val[13] = CRZ_BASE + 0x14;            // CRZ target size
    reg.val[14] = ioread32(CRZ_BASE + 0x14);
    reg.val[15] = 0x12345678;
    reg.val[16] = 0;
    reg.val[17] = 0x12345678;
    reg.val[18] = 0;
    reg.val[19] = 0x12345678;
    reg.val[20] = 0; 

    if (copy_to_user(preg_io, &reg, sizeof(mt_eis_reg_t)) != 0) {
        EIS_ERR("copy_to_user failed \n");
        ret = -1;
    }

    spin_unlock_irq(&eis_lock);
        
    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static int mt_eis_clk_onoff(__user unsigned long arg)
{
    spin_lock_irq(&eis_lock);        

    if (arg == 1)
    {
        if (bClockState == FALSE)
        {
            enable_clock(MT65XX_PDN_MM_EIS , "EIS");
            bClockState = TRUE;
        }
    }
    else
    {
        if (bClockState == TRUE)
        {    
            disable_clock(MT65XX_PDN_MM_EIS , "EIS");
            bClockState = FALSE;            
        }
    }
    
    spin_unlock_irq(&eis_lock);

    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static long mt_eis_ioctl(struct file *file,
                        unsigned int cmd, unsigned long arg)
{
    long ret = 0;

    switch (cmd) {

    case MT_EIS_IOC_T_DUMP_REG:
        ret = mt_eis_dump_reg((__user mt_eis_reg_t *) arg);
        break;

    case MT_EIS_IOC_T_CLK_ONOFF:
        ret = mt_eis_clk_onoff(arg);
        break;
        
    default:
        EIS_ERR("[mt_eis_ioctl] unknown cmd \n");
        ret = -EPERM;
        break;
    }

    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static int mt_eis_open(struct inode *inode, struct file *file)
{
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static int mt_eis_release(struct inode *inode, struct file *file)
{
    spin_lock_irq(&eis_lock);        

    if (bClockState == TRUE)
    {    
        disable_clock(MT65XX_PDN_MM_EIS , "EIS");        
        bClockState = FALSE;            
    }

    spin_unlock_irq(&eis_lock);
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static int mt_eis_mmap(struct file *file, struct vm_area_struct *vma)
{
    //EIS_LOG("[mt_eis_mmap] \n");

    vma->vm_page_prot = pgprot_noncached(vma->vm_page_prot);            
    if (remap_pfn_range(vma, vma->vm_start, vma->vm_pgoff, 
        vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
        return -EAGAIN;
    }

    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static dev_t dev_eis;
static struct cdev *pcdev_eis = NULL;
static struct class *pclass_eis = NULL;
static const struct file_operations mt_eis_fops = {
    .owner   = THIS_MODULE,
    .open    = mt_eis_open,
    .release = mt_eis_release,
    .mmap    = mt_eis_mmap,
    .unlocked_ioctl   = mt_eis_ioctl
};
//static int eis_irq_num;

/*******************************************************************************
*
********************************************************************************/
inline static void mt_eis_unregister_char_driver(void)
{
    EIS_LOG("[mt_eis_unregister_char_driver] \n");

    if (pcdev_eis != NULL) {
        cdev_del(pcdev_eis);
        pcdev_eis = NULL;
    }

    unregister_chrdev_region(dev_eis, 1);
}

/*******************************************************************************
*
********************************************************************************/
inline static int mt_eis_register_char_driver(void)
{
    int ret = 0;

    EIS_LOG("[mt_eis_register_char_driver] \n");

    if ( (ret = alloc_chrdev_region(&dev_eis, 0, 1, EIS_DEV_NAME)) < 0 ) {
        EIS_ERR("alloc_chrdev_region failed, %d \n", ret);
        return ret;
    }
    //Allocate driver
    pcdev_eis = cdev_alloc();
    if (pcdev_eis == NULL) {
        EIS_ERR("cdev_alloc failed\n");
        ret = -ENOMEM;
        goto mt_eis_register_char_driver_exit;
    }
    //Attatch file operation.
    cdev_init(pcdev_eis, &mt_eis_fops);

    pcdev_eis->owner = THIS_MODULE;
    //Add to system
    if ( (ret = cdev_add(pcdev_eis, dev_eis, 1)) < 0) {
        EIS_ERR("Attatch file operation failed, %d \n", ret);
        goto mt_eis_register_char_driver_exit;
    }

mt_eis_register_char_driver_exit:

    if (ret < 0) {
        mt_eis_unregister_char_driver();
    }

    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static int mt_eis_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct resource *pres = NULL;
    int i;

    EIS_LOG("[mt_eis_probe] enter \n");
    
    // Check platform_device parameters
    if (pdev == NULL) {
        dev_err(&pdev->dev, "pdev is NULL \n");
        return -ENXIO;
    }
    // Register char driver
    if ( (ret = mt_eis_register_char_driver()) ) {
        dev_err(&pdev->dev, "register char failed \n");
        return ret;
    }
    // Mapping CAM_REGISTERS
    for (i = 0; i < 1; i++) {
        pres = platform_get_resource(pdev, IORESOURCE_MEM, i);
        if (pres == NULL) {
            dev_err(&pdev->dev, "platform_get_resource failed \n");
            ret = -ENOMEM;
            goto mt_eis_probe_exit;
        }
        pres = request_mem_region(pres->start, pres->end - pres->start + 1, pdev->name);
        if (pres == NULL) {
            dev_err(&pdev->dev, "request_mem_region failed \n");
            ret = -ENOMEM;
            goto mt_eis_probe_exit;
        }
    }

    // Create class register
    pclass_eis = class_create(THIS_MODULE, "eisdrv");
    if (IS_ERR(pclass_eis)) {
        ret = PTR_ERR(pclass_eis);
        EIS_ERR("Unable to create class, err = %d\n", ret);
        return ret;
    }
    // FIXME: error handling
    device_create(pclass_eis, NULL, dev_eis, NULL, EIS_DEV_NAME);
    // Initialize critical section
    spin_lock_init(&eis_lock);

mt_eis_probe_exit:

    if (ret < 0) {
        mt_eis_unregister_char_driver();
    }

    EIS_LOG("[mt_eis_probe] exit \n");

    return ret;
}

/*******************************************************************************
* Called when the device is being detached from the driver
********************************************************************************/
static int mt_eis_remove(struct platform_device *pdev)
{
    struct resource *pres;
    int i;
    //int irq_num;

    EIS_LOG("[mt_eis_remove] \n");
    
    // unregister char driver.
    mt_eis_unregister_char_driver();
    // unmaping EIS registers
    for (i = 0; i < 2; i++) {
        pres = platform_get_resource(pdev, IORESOURCE_MEM, 0);
        release_mem_region(pres->start, (pres->end - pres->start + 1));
    }

    device_destroy(pclass_eis, dev_eis);

    class_destroy(pclass_eis);
    pclass_eis = NULL;

    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static int mt_eis_suspend(struct platform_device *pdev, pm_message_t mesg)
{
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static int mt_eis_resume(struct platform_device *pdev)
{
    return 0;
}

/*******************************************************************************
*
********************************************************************************/
static struct platform_driver eis_driver = {
    .probe   = mt_eis_probe,
    .remove  = mt_eis_remove,
    .suspend = mt_eis_suspend,
    .resume  = mt_eis_resume,
    .driver  = {
        .name  =  "camera-eis",
        .owner = THIS_MODULE,
    }
};

/*******************************************************************************
*
********************************************************************************/
static int
mt_eis_dump_reg_to_proc(
    char *page, char **start, off_t off, int count, int *eof, void *data
)
{
    char *p = page;
    int len = 0;
    u32 u4Index = 0 ;

    //EIS_LOG("[mt_eis_dump_reg_to_proc] \n");

    p += sprintf(p, "\n\r MT6577 EIS Register \n\r");
    p += sprintf(p, "======General Setting ====\n\r");
    for (; u4Index <= 0x28; u4Index += 4) {
        p += sprintf(p,"+0x%0x 0x%0x\n\r", u4Index, ioread32(EIS_BASE + u4Index));
    }

    *start = page + off;

    len = p - page;
    if (len > off) {
        len -= off;
    }
    else {
        len = 0;
    }

    return len < count ? len : count;
}

/*******************************************************************************
*
********************************************************************************/
static int  mt_eis_reg_debug( 
    struct file *file, const char *buffer, unsigned long count, void *data
)
{
    char regBuf[64]; 
    u32 u4CopyBufSize = (count < (sizeof(regBuf) - 1)) ? (count) : (sizeof(regBuf) - 1); 
    u32 u4Addr = 0; 
    u32 u4Data = 0; 

    if (copy_from_user(regBuf, buffer, u4CopyBufSize))
        return -EFAULT;

    if (sscanf(regBuf, "%x %x",  &u4Addr, &u4Data) == 2) {
            iowrite32(u4Data, EIS_BASE + u4Addr); 
            EIS_LOG("write addr = 0x%08x, data = 0x%08x\n", u4Addr, ioread32(EIS_BASE + u4Addr));             
    }
    else if (sscanf(regBuf, "%x", &u4Addr) == 1) {    
            EIS_LOG("read addr = 0x%08x, data = 0x%08x\n", u4Addr, ioread32(EIS_BASE + u4Addr)); 
    }

    return count; 
}

/*******************************************************************************
*
********************************************************************************/
static int __init mt_eis_init(void)
{
    int ret = 0;
    struct proc_dir_entry *prEntry;

    //EIS_LOG("[mt_eis_init] \n");

    if ((ret = platform_driver_register(&eis_driver)) < 0) {
        EIS_ERR("platform_driver_register fail \n");
        return ret;
    }

    prEntry = create_proc_entry("driver/eis_reg", 0, NULL); 
    if (prEntry) {
        prEntry->read_proc = mt_eis_dump_reg_to_proc; 
        prEntry->write_proc = mt_eis_reg_debug; 
    }
    else {
        EIS_ERR("add /proc/driver/eis_reg entry fail \n");  
    }

    return ret;
}

/*******************************************************************************
*
********************************************************************************/
static void __exit mt_eis_exit(void)
{
    //EIS_LOG("[mt_eis_exit] \n");
    platform_driver_unregister(&eis_driver);
}

/*******************************************************************************
*
********************************************************************************/
module_init(mt_eis_init);
module_exit(mt_eis_exit);
MODULE_DESCRIPTION("CAMERA EIS driver");
MODULE_AUTHOR("ME3");
MODULE_LICENSE("GPL");

