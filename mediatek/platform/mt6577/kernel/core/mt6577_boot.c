#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/spinlock.h>
#include <linux/uaccess.h>
#include <linux/mm.h>
#include <linux/kfifo.h>

#include <linux/firmware.h>
#include <linux/syscalls.h>
#include <linux/uaccess.h>
#include <linux/platform_device.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>

#include <mach/mt6577_boot.h>
#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_typedefs.h>
#include <mach/sbchk_base.h>
#include <linux/version.h>


#define MOD "BOOT"

/* hardware version register */
#define APHW_CODE           (VER_BASE)
#define APHW_SUBCODE        (VER_BASE + 0x04)
#define APHW_VER            (VER_BASE + 0x08)
#define APSW_VER            (VER_BASE + 0x0C)

/*****************************************************************************
 * manage chip version
 *****************************************************************************/
struct chip_code {
    unsigned short  hw_code;
    unsigned short  hw_subcode;
    unsigned short  hw_ver;
    unsigned short  sw_ver;
};
struct chip_info {
    CHIP_VER            ver;
    const char*         str;
    struct chip_code    code;
};

struct chip_desp {
    struct chip_code reg;   //!<: chip code read from register
    struct chip_info inf;   //!<: chip information
}; 

static struct chip_desp g_chip_desp = {
    {0x0000, 0x000, 0x0000, 0x0000},
    {0x0000, NULL, {0x0000, 0x0000, 0x0000, 0x0000}}
};

/*get chip string*/
static char* get_chip_str(CHIP_VER ver)
{
    if (CHIP_E1 == ver || CHIP_6575_E1 == ver) 
        return "6575E1";
    else if (CHIP_E2 == ver || CHIP_6575_E2 == ver)
        return "6575E2";
    else if (CHIP_E3 == ver || CHIP_6575_E3 == ver)
        return "6575E3";
    else if (CHIP_6577_E1 == ver)
        return "6577E1";
    return "";
}
/*load chip description*/
static int load_chip_desp(struct chip_desp *desp)
{
    desp->reg.hw_code    = DRV_Reg32(APHW_CODE);
    desp->reg.hw_subcode = DRV_Reg32(APHW_SUBCODE);
    desp->reg.hw_ver     = DRV_Reg32(APHW_VER);
    desp->reg.sw_ver     = DRV_Reg32(APSW_VER);

    if (0x8A00 == desp->reg.hw_subcode)
    {
        if (0xE100 == desp->reg.sw_ver)
        {
            memcpy(&desp->inf.code, &desp->reg, sizeof(desp->reg));
            desp->inf.str = "6575E1";
            desp->inf.ver = CHIP_6575_E1;
        }
        else if (0xE201 == desp->reg.sw_ver)
        {
            memcpy(&desp->inf.code, &desp->reg, sizeof(desp->reg));
            if (0x40000000 == (DRV_Reg32(0xF1019100) & 0x40000000))
            {
                desp->inf.str = "6575E3";
                desp->inf.ver = CHIP_6575_E3;
            }
            else
            {
                desp->inf.str = "6575E2";
                desp->inf.ver = CHIP_6575_E2;
            }
        }
        else
            return EINVAL;
    }
    else if (0x8B00 == desp->reg.hw_subcode)
    {
        desp->inf.code.hw_code      = 0x6577;
        desp->inf.code.hw_subcode   = 0x8B00;
        desp->inf.code.hw_ver       = 0xCA00;
        desp->inf.code.sw_ver       = 0xE100;
        desp->inf.str = "6577E1";
        desp->inf.ver = CHIP_6577_E1;
    }
    else
        return EINVAL;
    return 0;    
}
static int get_reg_chip_code(void)
{
    return g_chip_desp.reg.hw_code;
}
static int get_reg_chip_hw_ver(void)
{
    return g_chip_desp.reg.hw_ver;
}
static int get_reg_chip_sw_ver(void)
{
    return g_chip_desp.reg.sw_ver;
}
static int get_reg_chip_hw_subcode(void)
{
    return g_chip_desp.reg.hw_subcode;
}

/* return chip code */
unsigned int get_chip_code(void)
{   
    return g_chip_desp.inf.code.hw_code;
}
EXPORT_SYMBOL(get_chip_code);

/* return hardware version */
unsigned int get_chip_hw_ver(void)
{
    return g_chip_desp.inf.code.hw_ver;
}
EXPORT_SYMBOL(get_chip_hw_ver);

/* return software version */
unsigned int get_chip_sw_ver(void)
{   
    return g_chip_desp.inf.code.sw_ver;
}
EXPORT_SYMBOL(get_chip_sw_ver);

/* return hardware subcode */
unsigned int get_chip_hw_subcode(void)
{
    return g_chip_desp.inf.code.hw_subcode;
}
EXPORT_SYMBOL(get_chip_hw_subcode);

CHIP_VER get_chip_eco_ver(void)
{ 
    if (g_chip_desp.inf.ver >= CHIP_6575_E2)
        return CHIP_6575_E2;
    return g_chip_desp.inf.ver;
}
EXPORT_SYMBOL(get_chip_eco_ver);

CHIP_VER get_chip_ver(void)
{ 
    return g_chip_desp.inf.ver;
}
EXPORT_SYMBOL(get_chip_ver);

/*****************************************************************************
 * sysfs interface
 *****************************************************************************/
BOOTMODE g_boot_mode = UNKNOWN_BOOT;
META_COM_TYPE g_meta_com_type = META_UNKNOWN_COM;
unsigned int g_meta_com_id = 0;

static struct device_driver meta_com_type_info =
{
    .name = "meta_com_type_info",
    .bus = &platform_bus_type,
    .owner = THIS_MODULE,
};

static struct device_driver meta_com_id_info =
{
    .name = "meta_com_id_info",
    .bus = &platform_bus_type,
    .owner = THIS_MODULE,
};

static ssize_t (*md_show)(char*) = NULL;
static ssize_t (*md_store)(const char*,size_t) = NULL;


void boot_register_md_func(ssize_t (*show)(char*), ssize_t (*store)(const char*,size_t))
{
    md_show = show;
    md_store = store;
}

static ssize_t boot_show(struct kobject *kobj, struct attribute *a, char *buf)
{
    if (!strncmp(a->name, MD_SYSFS_ATTR, strlen(MD_SYSFS_ATTR)) && md_show) 
    {
        return md_show(buf);
    }
    else if (!strncmp(a->name, INFO_SYSFS_ATTR, strlen(INFO_SYSFS_ATTR)))
    {
        return sprintf(buf, "%x%x%x%X%s %X%X%X%X%s\n", 
                        get_reg_chip_code(), get_reg_chip_hw_subcode(), 
                        get_reg_chip_hw_ver(), get_reg_chip_sw_ver(),
                        get_chip_str(get_chip_eco_ver()),  
                        get_chip_code(),   get_chip_hw_subcode(), 
                        get_chip_hw_ver(), get_chip_sw_ver(), 
                        get_chip_str(get_chip_ver()));
    }
    else
    {
        return sprintf(buf, "%d\n", g_boot_mode);
    }
}

static ssize_t boot_store(struct kobject *kobj, struct attribute *a, const char *buf, size_t count)
{
    sbchk_base();

    if (!strncmp(a->name, MD_SYSFS_ATTR, strlen(MD_SYSFS_ATTR)) && md_store) 
    {
        return md_store(buf, count);
    }
    else if (!strncmp(a->name, BOOT_SYSFS_ATTR, strlen(BOOT_SYSFS_ATTR)))   
    {   /* for implementing android <-> fm */
        int mode = 0;
        if (1 != sscanf(buf, "0x%x", &mode))
        {
            printk("BOOT: error!!\n");
        }
        else if ((mode & 0xFFFF0000) == 0x19710000)
        {
            mode &= (~0xFFFF0000);
            printk("BOOT: %x -> %x\n", g_boot_mode, mode);            
            if ( ((g_boot_mode == NORMAL_BOOT) && (mode == FACTORY_BOOT)) ||
                 ((g_boot_mode == FACTORY_BOOT) && (mode == NORMAL_BOOT)) )
            {
                g_boot_mode = mode;                
            }
        } 
    }    
    return count;
}


/* boot object */
static struct kobject boot_kobj;
static struct sysfs_ops boot_sysfs_ops = {
    .show = boot_show,
    .store = boot_store
};

/* boot attribute */
struct attribute boot_attr = {BOOT_SYSFS_ATTR, 0664};
struct attribute md_attr = {MD_SYSFS_ATTR, 0664};
struct attribute info_attr = {INFO_SYSFS_ATTR, 0644};
static struct attribute *boot_attrs[] = {
    &boot_attr,
    &md_attr,
    &info_attr,
    NULL
};

/* boot type */
static struct kobj_type boot_ktype = {
    .sysfs_ops = &boot_sysfs_ops,
    .default_attrs = boot_attrs
};

/* boot device node */
static dev_t boot_dev_num;
static struct cdev boot_cdev;
static struct file_operations boot_fops = {
    .owner = THIS_MODULE,
    .open = NULL,
    .release = NULL,
    .write = NULL,
    .read = NULL,
    .unlocked_ioctl = NULL
};

/* boot device class */
static struct class *boot_class;
static struct device *boot_device;


/* return boot mode */
BOOTMODE get_boot_mode(void)
{
    return g_boot_mode;
}


/*****************************************************************************
 * meta mode
 *****************************************************************************/
/* for convenience, simply check is meta mode or not */
bool is_meta_mode(void)
{   
    if(g_boot_mode == META_BOOT)
    {   
        return true;
    }
    else
    {   
        return false;
    }
}

bool is_advanced_meta_mode(void)
{
    if (g_boot_mode == ADVMETA_BOOT)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool com_is_enable(void)  // usb android will check whether is com port enabled default. in normal boot it is default enabled. 
{	if(g_boot_mode == NORMAL_BOOT)
	{	return false;
	}
	else
	{	return true;
	}
}

static int boot_mode_proc(char *page, char **start, off_t off,int count, int *eof, void *data)
{
    char *p = page;
    int len = 0; 

    p += sprintf(p, "\n\rBOOT MODE : " );
    switch(g_boot_mode)
    {
        case NORMAL_BOOT :
            p += sprintf(p, "NORMAL BOOT\n");
            break;
        case META_BOOT :
            p += sprintf(p, "META BOOT\n");
            break;
        case ADVMETA_BOOT :
            p += sprintf(p, "Advanced META BOOT\n");
            break;   
        case ATE_FACTORY_BOOT :
            p += sprintf(p, "ATE_FACTORY BOOT\n");
            break;
        case ALARM_BOOT :
            p += sprintf(p, "ALARM BOOT\n");
            break;
        default :
            p += sprintf(p, "UNKNOWN BOOT\n");
            break;
    }  
    *start = page + off;
    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len  : count;     
}

static ssize_t meta_com_type_show(struct device_driver *driver, char *buf)
{
  return sprintf(buf, "%d\n", g_meta_com_type);
}

static ssize_t meta_com_type_store(struct device_driver *driver, const char *buf, size_t count)
{
  /*Do nothing*/
  return count;
}

DRIVER_ATTR(meta_com_type_info, 0644, meta_com_type_show, meta_com_type_store);


static ssize_t meta_com_id_show(struct device_driver *driver, char *buf)
{
  return sprintf(buf, "%d\n", g_meta_com_id);
}

static ssize_t meta_com_id_store(struct device_driver *driver, const char *buf, size_t count)
{
  /*Do nothing*/
  return count;
}

DRIVER_ATTR(meta_com_id_info, 0644, meta_com_id_show, meta_com_id_store);


static int __init boot_mod_init(void)
{
    int ret;

    /* allocate device major number */
    if (alloc_chrdev_region(&boot_dev_num, 0, 1, BOOT_DEV_NAME) < 0) {
        printk("[%s] fail to register chrdev\n",MOD);
        return -1;
    }

    /* add character driver */
    cdev_init(&boot_cdev, &boot_fops);
    ret = cdev_add(&boot_cdev, boot_dev_num, 1);
    if (ret < 0) {
        printk("[%s] fail to add cdev\n",MOD);
        return ret;
    }

    /* create class (device model) */
    boot_class = class_create(THIS_MODULE, BOOT_DEV_NAME);
    if (IS_ERR(boot_class)) {
        printk("[%s] fail to create class\n",MOD);
        return (int)boot_class;
    }

    boot_device = device_create(boot_class, NULL, boot_dev_num, NULL, BOOT_DEV_NAME);
    if (IS_ERR(boot_device)) {
        printk("[%s] fail to create device\n",MOD);
        return (int)boot_device;
    }

    /* add kobject */
    ret = kobject_init_and_add(&boot_kobj, &boot_ktype, &(boot_device->kobj), BOOT_SYSFS);
    if (ret < 0) {
        printk("[%s] fail to add kobject\n",MOD);
        return ret;
    }
    
    /* read chip version */
    ret = load_chip_desp(&g_chip_desp);
    if (ret < 0) {
        printk("[%s] fail to load chip verion", MOD);
        return ret;
    }
    printk("[%s] APHW_CODE = 0x%x\n",MOD,get_chip_code());
    printk("[%s] APHW_VER  = 0x%x\n",MOD,get_chip_eco_ver());
    
    /* create proc entry at /proc/boot_mode */
    create_proc_read_entry("boot_mode", S_IRUGO, NULL, boot_mode_proc, NULL);

    if(g_boot_mode == META_BOOT || g_boot_mode == ADVMETA_BOOT)
    {
      /* register driver and create sysfs files */
      ret = driver_register(&meta_com_type_info);
      if (ret) 
      {
        printk("fail to register META COM TYPE driver\n");
      }
      ret = driver_create_file(&meta_com_type_info, &driver_attr_meta_com_type_info);
      if (ret) 
      {
        printk("[BOOT INIT] Fail to create META COM TPYE sysfs file\n");
      }

      ret = driver_register(&meta_com_id_info);
      if (ret) 
      {
        printk("fail to register META COM ID driver\n");
      }
      ret = driver_create_file(&meta_com_id_info, &driver_attr_meta_com_id_info);
      if (ret) 
      {
        printk("[BOOT INIT] Fail to create META COM ID sysfs file\n");
      }
    }    
    
    return 0;
}

static void __exit boot_mod_exit(void)
{
    cdev_del(&boot_cdev);
}

module_init(boot_mod_init);
module_exit(boot_mod_exit);
MODULE_DESCRIPTION("Mediatek Boot Information Querying Driver");
MODULE_LICENSE("Proprietary");
EXPORT_SYMBOL(is_meta_mode);
EXPORT_SYMBOL(is_advanced_meta_mode);
EXPORT_SYMBOL(get_boot_mode);
EXPORT_SYMBOL(boot_register_md_func);

