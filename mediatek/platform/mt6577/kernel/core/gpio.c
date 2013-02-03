/******************************************************************************
 * mt_gpio.c - MTKLinux GPIO Device Driver
 * 
 * Copyright 2008-2009 MediaTek Co.,Ltd.
 * 
 * DESCRIPTION:
 *     This file provid the other drivers GPIO relative functions
 *
 ******************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/autoconf.h>
#include <linux/platform_device.h>

#include <linux/types.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/atomic.h>
#include <linux/miscdevice.h>

#include <linux/mtgpio.h>
#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_typedefs.h>
#include <mach/mt_gpio.h>

/******************************************************************************
 MACRO Definition
******************************************************************************/
#define GPIO_DEVICE "mt-gpio"
#define VERSION     GPIO_DEVICE
/*---------------------------------------------------------------------------*/
#define GPIO_WR32(addr, data)   __raw_writel(data, addr)
#define GPIO_RD32(addr)         __raw_readl(addr)
#define GPIO_SET_BITS(BIT,REG)   ((*(volatile u32*)(REG)) = (u32)(BIT))
#define GPIO_CLR_BITS(BIT,REG)   ((*(volatile u32*)(REG)) &= ~((u32)(BIT)))
/*---------------------------------------------------------------------------*/
#define MAX_GPIO_REG_BITS      16
#define MAX_GPIO_MODE_PER_REG  5
#define GPIO_MODE_BITS         3 
/*---------------------------------------------------------------------------*/
#define GPIOTAG                "[GPIO] "
#define GPIOLOG(fmt, arg...)   printk(GPIOTAG fmt, ##arg)
#define GPIOMSG(fmt, arg...)   printk(fmt, ##arg)
#define GPIOERR(fmt, arg...)   printk(KERN_ERR GPIOTAG "%5d: "fmt, __LINE__, ##arg)
#define GPIOFUC(fmt, arg...)   //printk(GPIOTAG "%s\n", __FUNCTION__)
#define GIO_INVALID_OBJ(ptr)   ((ptr) != gpio_obj)
#define GPIO_RETERR(res, fmt, args...)                                               \
    do {                                                                             \
        printk(KERN_ERR GPIOTAG "%s:%04d: " fmt"\n", __FUNCTION__, __LINE__, ##args);\
        return res;                                                                  \
    } while(0)

/*---------------------------------------------------------------------------*/

#if !defined(ENABLE_GPIO_SUPPORT)
	s32 mt_set_gpio_dir(u32 pin, u32 dir)				{return RSUCCESS;}
	s32 mt_get_gpio_dir(u32 pin)						{return GPIO_DIR_UNSUPPORTED;}	
	s32 mt_set_gpio_pull_enable(u32 pin, u32 enable)	{return RSUCCESS;}
	s32 mt_get_gpio_pull_enable(u32 pin)				{return GPIO_PULL_EN_UNSUPPORTED;}
	s32 mt_set_gpio_pull_select(u32 pin, u32 select)	{return RSUCCESS;}
	s32 mt_get_gpio_pull_select(u32 pin)				{return GPIO_PULL_UNSUPPORTED;}
	s32 mt_set_gpio_inversion(u32 pin, u32 enable)		{return RSUCCESS;}
	s32 mt_get_gpio_inversion(u32 pin)					{return GPIO_DATA_INV_UNSUPPORTED;}
	s32 mt_set_gpio_out(u32 pin, u32 output)			{return RSUCCESS;}
	s32 mt_get_gpio_out(u32 pin)						{return GPIO_OUT_UNSUPPORTED;}
	s32 mt_get_gpio_in(u32 pin) 						{return GPIO_IN_UNSUPPORTED;}
	s32 mt_set_gpio_mode(u32 pin, u32 mode) 			{return RSUCCESS;}
	s32 mt_get_gpio_mode(u32 pin)						{return GPIO_MODE_UNSUPPORTED;}
	s32 mt_set_clock_output(u32 num, u32 src, u32 div)	  {return RSUCCESS;}
	s32 mt_get_clock_output(u32 num, u32 *src, u32 *div)  {return CLK_SRC_UNSUPPORTED;}
	/*---------------------------------------------------------------------------*/
#else 
	
#if (defined(CONFIG_MT6575T_FPGA)  || defined(CONFIG_MT6577_FPGA))

#define MT_GPIO_DIR_REG 0xF100C008
#define MT_GPIO_OUT_REG 0xF100C000
#define MT_GPIO_IN_REG  0xF100C010

static spinlock_t mt_gpio_spinlock = __SPIN_LOCK_UNLOCKED(die.lock);
static void mt_gpio_set_bit(u32 nr, u32 reg)
{
	u32 value;
	value = readl(reg);
	value |= 1L << nr;
	writel(value, reg);
}

static void mt_gpio_clear_bit(u32 nr, u32 reg)
{
	u32 value;
	value = readl(reg);
	value &= ~(1L << nr);
	writel(value, reg);
}

static u32 mt_gpio_get_bit(u32 nr , u32 reg)
{
	u32 value;
	value = readl(reg);
	value &= (1L << nr);
	return value ? 1 : 0 ;
}

s32 mt_set_gpio_dir(u32 pin, u32 dir)
{
	s32 ret = RSUCCESS;
	unsigned long flags = 0;
	spin_lock_irqsave(&mt_gpio_spinlock, flags);
	if (dir == GPIO_DIR_IN || dir == GPIO_DIR_DEFAULT)
		mt_gpio_clear_bit(pin, MT_GPIO_DIR_REG);
	else if (dir == GPIO_DIR_OUT)
		mt_gpio_set_bit(pin, MT_GPIO_DIR_REG);
	else
		ret = GPIO_DIR_UNSUPPORTED;
	spin_unlock_irqrestore(&mt_gpio_spinlock, flags);
	return ret;
}
s32 mt_get_gpio_dir(u32 pin)
{
	s32 value;
	unsigned long flags = 0;
	spin_lock_irqsave(&mt_gpio_spinlock, flags);
	value = mt_gpio_get_bit(pin, MT_GPIO_DIR_REG);
	spin_unlock_irqrestore(&mt_gpio_spinlock, flags);
	return value;
}  
s32 mt_set_gpio_pull_enable(u32 pin, u32 enable)    {return RSUCCESS;}
s32 mt_get_gpio_pull_enable(u32 pin)                {return GPIO_PULL_EN_UNSUPPORTED;}
s32 mt_set_gpio_pull_select(u32 pin, u32 select)    {return RSUCCESS;}
s32 mt_get_gpio_pull_select(u32 pin)                {return GPIO_PULL_UNSUPPORTED;}
s32 mt_set_gpio_inversion(u32 pin, u32 enable)      {return RSUCCESS;}
s32 mt_get_gpio_inversion(u32 pin)                  {return GPIO_DATA_INV_UNSUPPORTED;}

s32 mt_set_gpio_out(u32 pin, u32 output)
{
	s32 ret = RSUCCESS;
	unsigned long flags = 0;
	spin_lock_irqsave(&mt_gpio_spinlock, flags);
	if (output == GPIO_OUT_ZERO || output == GPIO_OUT_DEFAULT || output == GPIO_DATA_OUT_DEFAULT) {
		mt_gpio_clear_bit(pin, MT_GPIO_OUT_REG);
	} else if (output == GPIO_OUT_ONE) {
		mt_gpio_set_bit(pin, MT_GPIO_OUT_REG);
	} else {
		ret = GPIO_OUT_UNSUPPORTED;
	}
	spin_unlock_irqrestore(&mt_gpio_spinlock, flags);
	return ret;
}
s32 mt_get_gpio_out(u32 pin)
{
	s32 value;
	unsigned long flags = 0;
	spin_lock_irqsave(&mt_gpio_spinlock, flags);
	value = mt_gpio_get_bit(pin, MT_GPIO_OUT_REG);
	spin_unlock_irqrestore(&mt_gpio_spinlock, flags);
	
	return value;
}
s32 mt_get_gpio_in(u32 pin) {
	s32 value;
	unsigned long flags = 0;
	spin_lock_irqsave(&mt_gpio_spinlock, flags);
	value = mt_gpio_get_bit(pin, MT_GPIO_IN_REG);
	spin_unlock_irqrestore(&mt_gpio_spinlock, flags);
	return value;
}
s32 mt_set_gpio_mode(u32 pin, u32 mode)             {return RSUCCESS;}
s32 mt_get_gpio_mode(u32 pin)                       {return GPIO_MODE_UNSUPPORTED;}
s32 mt_set_clock_output(u32 num, u32 src, u32 div)    {return RSUCCESS;}
s32 mt_get_clock_output(u32 num, u32 *src, u32 *div)  {return CLK_SRC_UNSUPPORTED;}

static ssize_t mt_gpio_dump_regs(char *buf, ssize_t bufLen)
{
    int idx = 0, len = 0;
	
	GPIOMSG("PIN: [DIR] [DOUT] [DIN]\n");
    for (idx = 0; idx < 8; idx++) {        
        len += snprintf(buf+len, bufLen-len, "%d: %d %d %d\n",
               idx, mt_get_gpio_dir(idx), mt_get_gpio_out(idx),mt_get_gpio_in(idx)); 
    }
    return len;
}

/*---------------------------------------------------------------------------*/
static ssize_t mt_gpio_show_pin(struct device* dev, 
                               struct device_attribute *attr, char *buf)
{    
    return mt_gpio_dump_regs(buf, PAGE_SIZE);
}
/*---------------------------------------------------------------------------*/
static ssize_t mt_gpio_store_pin(struct device* dev, struct device_attribute *attr,  
                                 const char *buf, size_t count)
{
    int pin, ret;
    int dout, dir,din;
	
    if(!strncmp(buf, "-h", 2)){
        GPIOMSG("PIN: [DIR] [DOUT] [DIN]\n");
    }else if(!strncmp(buf, "-w", 2)) {
        buf += 2;
        if (!strncmp(buf, "dout", 4) && (2 == sscanf(buf+4, "%d %d", &pin, &dout)))
            GPIOMSG("set dout(%3d, %d)=%d\n", pin, dout, mt_set_gpio_out(pin, dout));
        else if(!strncmp(buf, "dir", 3) &&  (2 == sscanf(buf+3, "%d %d", &pin, &dir)))
            GPIOMSG("set dir (%3d, %d)=%d\n", pin, dir, mt_set_gpio_dir(pin, dir));
        else 
            GPIOMSG("invalid format: '%s'", buf);
    } 
    return count;    
}
/*---------------------------------------------------------------------------*/
static DEVICE_ATTR(pin,      0664, mt_gpio_show_pin,   mt_gpio_store_pin);
/*---------------------------------------------------------------------------*/
static struct device_attribute *gpio_attr_list[] = {
    &dev_attr_pin,
};
/*---------------------------------------------------------------------------*/
static int mt_gpio_create_attr(struct device *dev) 
{
    int idx, err = 0;
    int num = (int)(sizeof(gpio_attr_list)/sizeof(gpio_attr_list[0]));
    if (!dev)
        return -EINVAL;

    for (idx = 0; idx < num; idx++) {
        if ((err = device_create_file(dev, gpio_attr_list[idx])))
            break;
    }
    
    return err;
}
/*---------------------------------------------------------------------------*/
static int mt_gpio_delete_attr(struct device *dev)
{
    int idx ,err = 0;
    int num = (int)(sizeof(gpio_attr_list)/sizeof(gpio_attr_list[0]));
    
    if (!dev)
        return -EINVAL;

    for (idx = 0; idx < num; idx++) 
        device_remove_file(dev, gpio_attr_list[idx]);

    return err;
}

/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
static struct miscdevice mt_gpio_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "mtgpio",
};
/*---------------------------------------------------------------------------*/
static int mt_gpio_probe(struct platform_device *dev)
{
    int err;
    struct miscdevice *misc = &mt_gpio_device;
    
	printk(KERN_ALERT"[GPIO]%5d,<%s> FPGA gpio devices probe\n", __LINE__, __func__);
    
 
    if ((err = misc_register(misc)))
        GPIOERR("register gpio\n");    

    if ((err = mt_gpio_create_attr(misc->this_device)))
        GPIOERR("create attribute\n");

    return err;
}
/*---------------------------------------------------------------------------*/
static int mt_gpio_remove(struct platform_device *dev)
{
    int err;    
    struct miscdevice *misc = &mt_gpio_device;
	
    if ((err = mt_gpio_delete_attr(misc->this_device)))
        GPIOERR("delete attr\n");

    if ((err = misc_deregister(misc)))
        GPIOERR("deregister gpio\n");
    
    return err;
}
/*---------------------------------------------------------------------------*/
static void mt_gpio_shutdown(struct platform_device *dev)
{
    GPIOLOG("shutdown\n");
}
/*---------------------------------------------------------------------------*/
static int mt_gpio_suspend(struct platform_device *dev, pm_message_t state)
{
    //GPIOLOG("suspend: %d!\n", state.event);
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
static int mt_gpio_resume(struct platform_device *dev)
{
    //printk("GPIO Resume !\n");
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
static struct platform_driver gpio_driver = 
{
    .probe          = mt_gpio_probe,
    .remove         = mt_gpio_remove,
    .shutdown       = mt_gpio_shutdown,
    .suspend        = mt_gpio_suspend,
    .resume         = mt_gpio_resume,
    .driver         = {
            .name = GPIO_DEVICE,
        },    
};
/*---------------------------------------------------------------------------*/
static int __init mt_gpio_init(void)
{
    int ret = 0;
    GPIOLOG("version: %s\n", VERSION);
    
    ret = platform_driver_register(&gpio_driver);
    return ret;
}
/*---------------------------------------------------------------------------*/
static void __exit mt_gpio_exit(void)
{
    platform_driver_unregister(&gpio_driver);
    return;
}
/*---------------------------------------------------------------------------*/
module_init(mt_gpio_init);
module_exit(mt_gpio_exit);
MODULE_AUTHOR("Ranran <ranran.lu@mediatek.com>");
MODULE_DESCRIPTION("MT General Purpose Driver (GPIO) $Revision$");
MODULE_LICENSE("GPL");

#else

/******************************************************************************
Enumeration/Structure
******************************************************************************/
#define CLK_NUM 6
static u32 clkout_reg_addr[CLK_NUM] = {
    (0xF0001A00),
    (0xF0001A04),
    (0xF0001A08),
    (0xF0001A0C),
    (0xF0001A10),
    (0xF0001A14)
};
struct mt_gpio_obj {
    atomic_t        ref;
    dev_t           devno;
    struct class    *cls;
    struct device   *dev;
    struct cdev     chrdev;
    spinlock_t      lock;
    GPIO_REGS       *reg;
    struct miscdevice *misc;
};
static struct mt_gpio_obj gpio_dat = {
    .ref  = ATOMIC_INIT(0),
    .cls  = NULL,
    .dev  = NULL,
    .lock = __SPIN_LOCK_UNLOCKED(die.lock),
    .reg  = (GPIO_REGS*)(GPIO_BASE),
    .misc = NULL,
};
static struct mt_gpio_obj *gpio_obj = &gpio_dat;

/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_dir(u32 pin, u32 dir)
{
    u32 pos;
    u32 bit;
    struct mt_gpio_obj *obj = gpio_obj;
	unsigned long flags = 0;

    if (!obj)
        GPIO_RETERR(-EACCES,"");

    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;

    if (dir >= GPIO_DIR_MAX)
        return -EINVAL;
    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;
    spin_lock_irqsave(&obj->lock, flags);
    if (dir == GPIO_DIR_IN)
        GPIO_SET_BITS((1L << bit), &obj->reg->dir[pos].rst);
    else
        GPIO_SET_BITS((1L << bit), &obj->reg->dir[pos].set);
	spin_unlock_irqrestore(&obj->lock, flags);
    return RSUCCESS;
    
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_set_gpio_dir);
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_dir(u32 pin)
{    
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        GPIO_RETERR(-EACCES,"");
    
    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;
    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;
    
    reg = GPIO_RD32(&obj->reg->dir[pos].val);
    return (((reg & (1L << bit)) != 0)? 1: 0);        
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_get_gpio_dir);
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_pull_enable(u32 pin, u32 enable)
{
    u32 pos;
    u32 bit;
    struct mt_gpio_obj *obj = gpio_obj;
	unsigned long flags = 0;

    if (!obj)
        GPIO_RETERR(-EACCES,"");
    
    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;

    if (enable >= GPIO_PULL_EN_MAX)
        return -EINVAL;
    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;
	spin_lock_irqsave(&obj->lock, flags);

    if (enable == GPIO_PULL_DISABLE)
        GPIO_SET_BITS((1L << bit), &obj->reg->pullen[pos].rst);
    else
        GPIO_SET_BITS((1L << bit), &obj->reg->pullen[pos].set);
	spin_unlock_irqrestore(&obj->lock, flags);
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_set_gpio_pull_enable);
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_pull_enable(u32 pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        GPIO_RETERR(-EACCES,"");
    
    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;
    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIO_RD32(&obj->reg->pullen[pos].val);
    return (((reg & (1L << bit)) != 0)? 1: 0);        
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_get_gpio_pull_enable);
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_pull_select(u32 pin, u32 select)
{
    u32 pos;
    u32 bit;
    struct mt_gpio_obj *obj = gpio_obj;
	unsigned long flags = 0;

    if (!obj)
        GPIO_RETERR(-EACCES,"");

    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;
    
    if (select >= GPIO_PULL_MAX)
        return -EINVAL;

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;
    spin_lock_irqsave(&obj->lock, flags);
    if (select == GPIO_PULL_DOWN)
        GPIO_SET_BITS((1L << bit), &obj->reg->pullsel[pos].rst);
    else
        GPIO_SET_BITS((1L << bit), &obj->reg->pullsel[pos].set);
	spin_unlock_irqrestore(&obj->lock, flags);
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_set_gpio_pull_select);
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_pull_select(u32 pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        GPIO_RETERR(-EACCES,"");

    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;
    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIO_RD32(&obj->reg->pullsel[pos].val);
    return (((reg & (1L << bit)) != 0)? 1: 0);        
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_get_gpio_pull_select);
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_inversion(u32 pin, u32 enable)
{
    u32 pos;
    u32 bit;
    struct mt_gpio_obj *obj = gpio_obj;
	unsigned long flags = 0;

    if (!obj)
        GPIO_RETERR(-EACCES,"");

    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;

    if (enable >= GPIO_DATA_INV_MAX)
        return -EINVAL;

    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;
    spin_lock_irqsave(&obj->lock, flags);
    if (enable == GPIO_DATA_UNINV)
        GPIO_SET_BITS((1L << bit), &obj->reg->dinv[pos].rst);
    else
        GPIO_SET_BITS((1L << bit), &obj->reg->dinv[pos].set);
	spin_unlock_irqrestore(&obj->lock, flags);
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_set_gpio_inversion);
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_inversion(u32 pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        GPIO_RETERR(-EACCES,"");

    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;
    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIO_RD32(&obj->reg->dinv[pos].val);
    return (((reg & (1L << bit)) != 0)? 1: 0);        
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_get_gpio_inversion);
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_out(u32 pin, u32 output)
{
    u32 pos;
    u32 bit;
    struct mt_gpio_obj *obj = gpio_obj;
	unsigned long flags = 0;

    if (!obj)
        GPIO_RETERR(-EACCES,"");

    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;

    if (output >= GPIO_OUT_MAX)
        return -EINVAL;
    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;
    spin_lock_irqsave(&obj->lock, flags);
    if (output == GPIO_OUT_ZERO)
        GPIO_SET_BITS((1L << bit), &obj->reg->dout[pos].rst);
    else
        GPIO_SET_BITS((1L << bit), &obj->reg->dout[pos].set);
	spin_unlock_irqrestore(&obj->lock, flags);
    return RSUCCESS;
}
EXPORT_SYMBOL(mt_set_gpio_out);
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_out(u32 pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        GPIO_RETERR(-EACCES,"");

    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;
    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIO_RD32(&obj->reg->dout[pos].val);
    return (((reg & (1L << bit)) != 0)? 1: 0);        
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_get_gpio_out);
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_in(u32 pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        GPIO_RETERR(-EACCES,"");

    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;
    
    pos = pin / MAX_GPIO_REG_BITS;
    bit = pin % MAX_GPIO_REG_BITS;

    reg = GPIO_RD32(&obj->reg->din[pos].val);
    return (((reg & (1L << bit)) != 0)? 1: 0);        
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_get_gpio_in);
/*---------------------------------------------------------------------------*/
s32 mt_set_gpio_mode(u32 pin, u32 mode)
{
    u32 pos;
    u32 bit;
    u32 reg;
    u32 mask = (1L << GPIO_MODE_BITS) - 1;    
    struct mt_gpio_obj *obj = gpio_obj;
	unsigned long flags = 0;

    if (!obj)
        GPIO_RETERR(-EACCES,"");

    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;

    if (mode >= GPIO_MODE_MAX)
        return -EINVAL;
    
    pos = pin / MAX_GPIO_MODE_PER_REG;
    bit = pin % MAX_GPIO_MODE_PER_REG;

   
    spin_lock_irqsave(&obj->lock, flags);
    reg = GPIO_RD32(&obj->reg->mode[pos].val);

    reg &= ~(mask << (GPIO_MODE_BITS*bit));
    reg |= (mode << (GPIO_MODE_BITS*bit));
    
    GPIO_WR32(&obj->reg->mode[pos].val, reg);
    spin_unlock_irqrestore(&obj->lock, flags);

    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_set_gpio_mode);
/*---------------------------------------------------------------------------*/
s32 mt_get_gpio_mode(u32 pin)
{
    u32 pos;
    u32 bit;
    u32 reg;
    u32 mask = (1L << GPIO_MODE_BITS) - 1;    
    struct mt_gpio_obj *obj = gpio_obj;

    if (!obj)
        GPIO_RETERR(-EACCES,"");

    if (pin >= MAX_GPIO_PIN)
        return -EINVAL;
    
    pos = pin / MAX_GPIO_MODE_PER_REG;
    bit = pin % MAX_GPIO_MODE_PER_REG;

    reg = GPIO_RD32(&obj->reg->mode[pos].val);
    
    return ((reg >> (GPIO_MODE_BITS*bit)) & mask);
}
/*---------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_get_gpio_mode);

s32 mt_set_clock_output(u32 num, u32 src, u32 div)
{
    u32 pin_reg;
    u32 reg_value = 0;
		struct mt_gpio_obj *obj = gpio_obj;
		unsigned long flags = 0;

    if (num >= CLK_MAX )
        return -EINVAL;
    if (src >= CLK_SRC_MAX)
        return -EINVAL;
	if ((div > 16) || (div <= 0))
        return -EINVAL;

    pin_reg = clkout_reg_addr[num];
   
    reg_value = div - 1;
    reg_value |= (src << 4); 
	spin_lock_irqsave(&obj->lock, flags);
	GPIO_WR32(pin_reg,reg_value);
	spin_unlock_irqrestore(&obj->lock, flags);
    return RSUCCESS;
}
EXPORT_SYMBOL(mt_set_clock_output);

s32 mt_get_clock_output(u32 num, u32 * src, u32 *div)
{
    u32 reg_value;
    u32 pin_reg;
  
    if (num >= CLK_MAX)
        return -EINVAL;

    pin_reg = clkout_reg_addr[num];
	reg_value = GPIO_RD32(pin_reg);
	*src = reg_value >> 4;
        printk("src==%d\n", *src);
	*div = (reg_value & 0x0f) + 1;    
	printk("div==%d\n", *div);
	return RSUCCESS;
}
EXPORT_SYMBOL(mt_get_clock_output);


/*---------------MD convert gpio-name to gpio-number--------*/
typedef struct md_gpio_info {
	char m_gpio_name[34];
	int m_gpio_num;
}MD_GPIO_INFO;

#define GPIO_NAME_MAX 12
MD_GPIO_INFO gpio_info[GPIO_NAME_MAX];

int mt_get_md_gpio(char * gpio_name, int len)
{
	unsigned int i;

	for (i = 0; i < GPIO_NAME_MAX; i++)
	{
		if (!strncmp (gpio_name, gpio_info[i].m_gpio_name, len))
		{
			GPIOMSG("gpio_info[i].m_gpio_num==%d\n", gpio_info[i].m_gpio_num);
			return (gpio_info[i].m_gpio_num);
		}
	}
	GPIOERR("There is no this gpio name!!!\n");
	return -1;
}
EXPORT_SYMBOL(mt_get_md_gpio);
/*---------------MD convert gpio-name to gpio-number--------*/

/*****************************************************************************/
/* sysfs operation                                                           */
/*****************************************************************************/
void mt_gpio_self_test(void)
{
    int i, val;
    for (i = 0; i < GPIO_MAX; i++)
    {
        s32 res,old;
        GPIOMSG("GPIO-%3d test\n", i);
        /*direction test*/
        old = mt_get_gpio_dir(i);
        if (old == 0 || old == 1) {
            GPIOLOG(" dir old = %d\n", old);
        } else {
            GPIOERR(" test dir fail: %d\n", old);
            break;
        }
        if ((res = mt_set_gpio_dir(i, GPIO_DIR_OUT)) != RSUCCESS) {
            GPIOERR(" set dir out fail: %d\n", res);
            break;
        } else if ((res = mt_get_gpio_dir(i)) != GPIO_DIR_OUT) {
            GPIOERR(" get dir out fail: %d\n", res);
            break;
        } else {
            /*output test*/
            s32 out = mt_get_gpio_out(i);
            if (out != 0 && out != 1) {
                GPIOERR(" get out fail = %d\n", old);
                break;
            } 
            for (val = 0; val < GPIO_OUT_MAX; val++) {
                if ((res = mt_set_gpio_out(i,0)) != RSUCCESS) {
                    GPIOERR(" set out[%d] fail: %d\n", val, res);
                    break;
                } else if ((res = mt_get_gpio_out(i)) != 0) {
                    GPIOERR(" get out[%d] fail: %d\n", val, res);
                    break;
                }
            }
            if ((res = mt_set_gpio_out(i,out)) != RSUCCESS)
            {
                GPIOERR(" restore out fail: %d\n", res);
                break;
            }
        }
            
        if ((res = mt_set_gpio_dir(i, GPIO_DIR_IN)) != RSUCCESS) {
            GPIOERR(" set dir in fail: %d\n", res);
            break;
        } else if ((res = mt_get_gpio_dir(i)) != GPIO_DIR_IN) {
            GPIOERR(" get dir in fail: %d\n", res);
            break;
        } else {
            GPIOLOG(" input data = %d\n", res);
        }
        
        if ((res = mt_set_gpio_dir(i, old)) != RSUCCESS) {
            GPIOERR(" restore dir fail: %d\n", res);
            break;
        }
        for (val = 0; val < GPIO_PULL_EN_MAX; val++) {
            if ((res = mt_set_gpio_pull_enable(i,val)) != RSUCCESS) {
                GPIOERR(" set pullen[%d] fail: %d\n", val, res);
                break;
            } else if ((res = mt_get_gpio_pull_enable(i)) != val) {
                GPIOERR(" get pullen[%d] fail: %d\n", val, res);
                break;
            }
        }        
        if ((res = mt_set_gpio_pull_enable(i, old)) != RSUCCESS) {
            GPIOERR(" restore pullen fail: %d\n", res);
            break;
        }

        /*pull select test*/
        old = mt_get_gpio_pull_select(i);
        if (old == 0 || old == 1)
            GPIOLOG(" pullsel old = %d\n", old);
        else {
            GPIOERR(" pullsel fail: %d\n", old);
            break;
        }
        for (val = 0; val < GPIO_PULL_MAX; val++) {
            if ((res = mt_set_gpio_pull_select(i,val)) != RSUCCESS) {
                GPIOERR(" set pullsel[%d] fail: %d\n", val, res);
                break;
            } else if ((res = mt_get_gpio_pull_select(i)) != val) {
                GPIOERR(" get pullsel[%d] fail: %d\n", val, res);
                break;
            }
        } 
        if ((res = mt_set_gpio_pull_select(i, old)) != RSUCCESS)
        {
            GPIOERR(" restore pullsel fail: %d\n", res);
            break;
        }     

        /*data inversion*/
        old = mt_get_gpio_inversion(i);
        if (old == 0 || old == 1)
            GPIOLOG(" inv old = %d\n", old);
        else {
            GPIOERR(" inv fail: %d\n", old);
            break;
        }
        for (val = 0; val < GPIO_DATA_INV_MAX; val++) {
            if ((res = mt_set_gpio_inversion(i,val)) != RSUCCESS) {
                GPIOERR(" set inv[%d] fail: %d\n", val, res);
                break;
            } else if ((res = mt_get_gpio_inversion(i)) != val) {
                GPIOERR(" get inv[%d] fail: %d\n", val, res);
                break;
            }
        } 
        if ((res = mt_set_gpio_inversion(i, old)) != RSUCCESS) {
            GPIOERR(" restore inv fail: %d\n", res);
            break;
        }     

        /*mode control*/
        old = mt_get_gpio_mode(i);
        if ((old >= GPIO_MODE_00) && (val < GPIO_MODE_MAX))
        {
            GPIOLOG(" mode old = %d\n", old);
        }
        else
        {
            GPIOERR(" get mode fail: %d\n", old);
            break;
        }
        for (val = 0; val < GPIO_MODE_MAX; val++) {
            if ((res = mt_set_gpio_mode(i, val)) != RSUCCESS) {
                GPIOERR("set mode[%d] fail: %d\n", val, res);
                break;
            } else if ((res = mt_get_gpio_mode(i)) != val) {
                GPIOERR("get mode[%d] fail: %d\n", val, res);
                break;
            }            
        }        
        if ((res = mt_set_gpio_mode(i,old)) != RSUCCESS) {
            GPIOERR(" restore mode fail: %d\n", res);
            break;
        }   
        
    }
    GPIOLOG("GPIO test done\n");
}
/*----------------------------------------------------------------------------*/
void mt_gpio_load(GPIO_REGS *regs) 
{
    GPIO_REGS *pReg = (GPIO_REGS*)(GPIO_BASE);
    int idx;
    
    if (!regs)
        GPIOERR("%s: null pointer\n", __func__);
    memset(regs, 0x00, sizeof(*regs));
    for (idx = 0; idx < sizeof(pReg->dir)/sizeof(pReg->dir[0]); idx++)
        regs->dir[idx].val = __raw_readl(&pReg->dir[idx]);
    for (idx = 0; idx < sizeof(pReg->pullen)/sizeof(pReg->pullen[0]); idx++)
        regs->pullen[idx].val = __raw_readl(&pReg->pullen[idx]);
    for (idx = 0; idx < sizeof(pReg->pullsel)/sizeof(pReg->pullsel[0]); idx++)
        regs->pullsel[idx].val =__raw_readl(&pReg->pullsel[idx]);
    for (idx = 0; idx < sizeof(pReg->dinv)/sizeof(pReg->dinv[0]); idx++)
        regs->dinv[idx].val =__raw_readl(&pReg->dinv[idx]);
    for (idx = 0; idx < sizeof(pReg->dout)/sizeof(pReg->dout[0]); idx++)
        regs->dout[idx].val = __raw_readl(&pReg->dout[idx]);
    for (idx = 0; idx < sizeof(pReg->mode)/sizeof(pReg->mode[0]); idx++)
        regs->mode[idx].val = __raw_readl(&pReg->mode[idx]);
    for (idx = 0; idx < sizeof(pReg->din)/sizeof(pReg->din[0]); idx++)
        regs->din[idx].val = __raw_readl(&pReg->din[idx]);
}
/*----------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_gpio_load);
/*----------------------------------------------------------------------------*/
void mt_gpio_dump(GPIO_REGS *regs) 
{
    GPIO_REGS *cur = kzalloc(sizeof(*cur), GFP_KERNEL);    
    int idx;

    GPIOMSG("%s\n", __func__);
    if (cur == NULL) {
        GPIOERR("null pointer\n");
        return;
    } else if (regs == NULL) { /*if arg is null, load & dump; otherwise, dump only*/
        regs = cur;
        mt_gpio_load(regs);
        GPIOMSG("dump current: %p\n", regs);
    } else {
        GPIOMSG("dump %p ...\n", regs);    
    }

    GPIOMSG("---# dir #-----------------------------------------------------------------\n");
    for (idx = 0; idx < sizeof(regs->dir)/sizeof(regs->dir[0]); idx++) {
        GPIOMSG("0x%04X ", regs->dir[idx].val);
        if (7 == (idx % 8)) GPIOMSG("\n");
    }
    GPIOMSG("\n---# pullen #--------------------------------------------------------------\n");        
    for (idx = 0; idx < sizeof(regs->pullen)/sizeof(regs->pullen[0]); idx++) {
        GPIOMSG("0x%04X ", regs->pullen[idx].val);    
        if (7 == (idx % 8)) GPIOMSG("\n");
    }
    GPIOMSG("\n---# pullsel #-------------------------------------------------------------\n");   
    for (idx = 0; idx < sizeof(regs->pullsel)/sizeof(regs->pullsel[0]); idx++) {
        GPIOMSG("0x%04X ", regs->pullsel[idx].val);     
        if (7 == (idx % 8)) GPIOMSG("\n");
    }
    GPIOMSG("\n---# dinv #----------------------------------------------------------------\n");   
    for (idx = 0; idx < sizeof(regs->dinv)/sizeof(regs->dinv[0]); idx++) {
        GPIOMSG("0x%04X ", regs->dinv[idx].val);     
        if (7 == (idx % 8)) GPIOMSG("\n");
    }
    GPIOMSG("\n---# dout #----------------------------------------------------------------\n");   
    for (idx = 0; idx < sizeof(regs->dout)/sizeof(regs->dout[0]); idx++) {
        GPIOMSG("0x%04X ", regs->dout[idx].val);     
        if (7 == (idx % 8)) GPIOMSG("\n");
    }
    GPIOMSG("\n---# din  #----------------------------------------------------------------\n");   
    for (idx = 0; idx < sizeof(regs->din)/sizeof(regs->din[0]); idx++) {
        GPIOMSG("0x%04X ", regs->din[idx].val);     
        if (7 == (idx % 8)) GPIOMSG("\n");
    }
    GPIOMSG("\n---# mode #----------------------------------------------------------------\n");   
    for (idx = 0; idx < sizeof(regs->mode)/sizeof(regs->mode[0]); idx++) {
        GPIOMSG("0x%04X ", regs->mode[idx].val);     
        if (7 == (idx % 8)) GPIOMSG("\n");
    }    
    GPIOMSG("\n---------------------------------------------------------------------------\n");    
}
/*----------------------------------------------------------------------------*/
EXPORT_SYMBOL(mt_gpio_dump);
void gpio_dump_regs(void)
{
    int idx = 0;	
    int num, bit; 
    int mode, pullsel, din, dout, pullen, dir, dinv;
    u32 mask = (1L << GPIO_MODE_BITS) - 1;     
    GPIO_REGS *reg = (GPIO_REGS*)GPIO_BASE;   
    for (idx = 0; idx < MAX_GPIO_PIN; idx++) {
        num = idx / MAX_GPIO_REG_BITS;
        bit = idx % MAX_GPIO_REG_BITS;
        pullsel = (reg->pullsel[num].val & (1L << bit)) ? (1) : (0);
        din     = (reg->din[num].val & (1L << bit)) ? (1) : (0);
        dout    = (reg->dout[num].val & (1L << bit)) ? (1) : (0);
        pullen  = (reg->pullen[num].val & (1L << bit)) ? (1) : (0);
        dir     = (reg->dir[num].val & (1L << bit)) ? (1) : (0);
        dinv    = (reg->dinv[num].val & (1L << bit)) ? (1) : (0);
        num = idx / MAX_GPIO_MODE_PER_REG;        
        bit = idx % MAX_GPIO_MODE_PER_REG;
        mode = (reg->mode[num].val >> (GPIO_MODE_BITS*bit)) & mask;
        printk("idx = %3d: mode = %d, pullsel = %d, din = %d, dout = %d, pullen = %d, dir = %d, dinv = %d\n", idx, mode, pullsel, din, dout, pullen, dir, dinv); 
    }
}
EXPORT_SYMBOL(gpio_dump_regs);
/*----------------------------------------------------------------------------*/
static ssize_t mt_gpio_dump_regs(char *buf, ssize_t bufLen, GPIO_REGS *cur)
{
    int idx = 0, len = 0;
    int num, bit; 
    int mode, pullsel, din, dout, pullen, dir, dinv;
    u32 mask = (1L << GPIO_MODE_BITS) - 1;        
    for (idx = 0; idx < MAX_GPIO_PIN; idx++) {
        num = idx / MAX_GPIO_REG_BITS;
        bit = idx % MAX_GPIO_REG_BITS;
        pullsel = (cur->pullsel[num].val & (1L << bit)) ? (1) : (0);
        din     = (cur->din[num].val & (1L << bit)) ? (1) : (0);
        dout    = (cur->dout[num].val & (1L << bit)) ? (1) : (0);
        pullen  = (cur->pullen[num].val & (1L << bit)) ? (1) : (0);
        dir     = (cur->dir[num].val & (1L << bit)) ? (1) : (0);
        dinv    = (cur->dinv[num].val & (1L << bit)) ? (1) : (0);
        num = idx / MAX_GPIO_MODE_PER_REG;        
        bit = idx % MAX_GPIO_MODE_PER_REG;
        mode = (cur->mode[num].val >> (GPIO_MODE_BITS*bit)) & mask;
        len += snprintf(buf+len, bufLen-len, "%3d: %d %d %d %d %d %d %d\n",
               idx, mode, pullsel, din, dout, pullen, dir, dinv); 
    }
    return len;
}
/*---------------------------------------------------------------------------*/
static void mt_gpio_read_pin(GPIO_CFG* cfg, int method)
{
    if (method == 0) {
        GPIO_REGS *cur = (GPIO_REGS*)GPIO_BASE;    
        u32 mask = (1L << GPIO_MODE_BITS) - 1;        
        int num, bit; 
        num = cfg->no / MAX_GPIO_REG_BITS;
        bit = cfg->no % MAX_GPIO_REG_BITS;
        cfg->pullsel= (cur->pullsel[num].val & (1L << bit)) ? (1) : (0);
        cfg->din    = (cur->din[num].val & (1L << bit)) ? (1) : (0);
        cfg->dout   = (cur->dout[num].val & (1L << bit)) ? (1) : (0);
        cfg->pullen = (cur->pullen[num].val & (1L << bit)) ? (1) : (0);
        cfg->dir    = (cur->dir[num].val & (1L << bit)) ? (1) : (0);
        cfg->dinv   = (cur->dinv[num].val & (1L << bit)) ? (1) : (0);
        num = cfg->no / MAX_GPIO_MODE_PER_REG;        
        bit = cfg->no % MAX_GPIO_MODE_PER_REG;
        cfg->mode   = (cur->mode[num].val >> (GPIO_MODE_BITS*bit)) & mask;
    } else if (method == 1) {
        cfg->pullsel= mt_get_gpio_pull_select(cfg->no);
        cfg->din    = mt_get_gpio_in(cfg->no);
        cfg->dout   = mt_get_gpio_out(cfg->no);
        cfg->pullen = mt_get_gpio_pull_enable(cfg->no);
        cfg->dir    = mt_get_gpio_dir(cfg->no);
        cfg->dinv   = mt_get_gpio_inversion(cfg->no);
        cfg->mode   = mt_get_gpio_mode(cfg->no);
    }
}
/*---------------------------------------------------------------------------*/
static ssize_t mt_gpio_dump_addr(struct device* dev)
{
    int idx;
    struct mt_gpio_obj *obj = (struct mt_gpio_obj*)dev_get_drvdata(dev);
    GPIO_REGS *reg = obj->reg;

    GPIOMSG("# direction\n");
    for (idx = 0; idx < sizeof(reg->dir)/sizeof(reg->dir[0]); idx++)
        GPIOMSG("val[%2d] %p\nset[%2d] %p\nrst[%2d] %p\n", idx, &reg->dir[idx].val, idx, &reg->dir[idx].set, idx, &reg->dir[idx].rst);
    GPIOMSG("# pull enable\n");
    for (idx = 0; idx < sizeof(reg->pullen)/sizeof(reg->pullen[0]); idx++)
        GPIOMSG("val[%2d] %p\nset[%2d] %p\nrst[%2d] %p\n", idx, &reg->pullen[idx].val, idx, &reg->pullen[idx].set, idx, &reg->pullen[idx].rst);
    GPIOMSG("# pull select\n");
    for (idx = 0; idx < sizeof(reg->pullsel)/sizeof(reg->pullsel[0]); idx++)
        GPIOMSG("val[%2d] %p\nset[%2d] %p\nrst[%2d] %p\n", idx, &reg->pullsel[idx].val, idx, &reg->pullsel[idx].set, idx, &reg->pullsel[idx].rst);
    GPIOMSG("# data inversion\n");
    for (idx = 0; idx < sizeof(reg->dinv)/sizeof(reg->dinv[0]); idx++)
        GPIOMSG("val[%2d] %p\nset[%2d] %p\nrst[%2d] %p\n", idx, &reg->dinv[idx].val, idx, &reg->dinv[idx].set, idx, &reg->dinv[idx].rst);
    GPIOMSG("# data output\n");
    for (idx = 0; idx < sizeof(reg->dout)/sizeof(reg->dout[0]); idx++)
        GPIOMSG("val[%2d] %p\nset[%2d] %p\nrst[%2d] %p\n", idx, &reg->dout[idx].val, idx, &reg->dout[idx].set, idx, &reg->dout[idx].rst);
    GPIOMSG("# data input\n");
    for (idx = 0; idx < sizeof(reg->din)/sizeof(reg->din[0]); idx++)
        GPIOMSG("val[%2d] %p\n", idx, &reg->din[idx].val);
    GPIOMSG("# mode\n");
    for (idx = 0; idx < sizeof(reg->mode)/sizeof(reg->mode[0]); idx++)
        GPIOMSG("val[%2d] %p\nset[%2d] %p\nrst[%2d] %p\n", idx, &reg->mode[idx].val, idx, &reg->mode[idx].set, idx, &reg->mode[idx].rst);
    return 0;    
}
/*---------------------------------------------------------------------------*/
static ssize_t mt_gpio_compare(struct device *dev)
{
    int idx;
    struct mt_gpio_obj *obj = (struct mt_gpio_obj*)dev_get_drvdata(dev);
    GPIO_REGS *reg = obj->reg;
    GPIO_REGS *cur = kzalloc(sizeof(*cur), GFP_KERNEL);    

    if (!cur)
        return 0;
    
    mt_gpio_load(cur);
    for (idx = 0; idx < sizeof(reg->dir)/sizeof(reg->dir[0]); idx++)
        if (reg->dir[idx].val != cur->dir[idx].val)
            GPIOERR("mismatch dir[%2d]: %x <> %x\n", idx, reg->dir[idx].val, cur->dir[idx].val);
    for (idx = 0; idx < sizeof(reg->pullen)/sizeof(reg->pullen[0]); idx++)
        if (reg->pullen[idx].val != cur->pullen[idx].val)
            GPIOERR("mismatch pullen[%2d]: %x <> %x\n", idx, reg->pullen[idx].val, cur->pullen[idx].val);
    for (idx = 0; idx < sizeof(reg->pullsel)/sizeof(reg->pullsel[0]); idx++)
        if (reg->pullsel[idx].val != cur->pullsel[idx].val)
            GPIOERR("mismatch pullsel[%2d]: %x <> %x\n", idx, reg->pullsel[idx].val, cur->pullsel[idx].val);
    for (idx = 0; idx < sizeof(reg->dinv)/sizeof(reg->dinv[0]); idx++)
        if (reg->dinv[idx].val != cur->dinv[idx].val)
            GPIOERR("mismatch dinv[%2d]: %x <> %x\n", idx, reg->dinv[idx].val, cur->dinv[idx].val);
    for (idx = 0; idx < sizeof(reg->dout)/sizeof(reg->dout[0]); idx++)
        if (reg->dout[idx].val != cur->dout[idx].val)
            GPIOERR("mismatch dout[%2d]: %x <> %x\n", idx, reg->dout[idx].val, cur->dout[idx].val);
    for (idx = 0; idx < sizeof(reg->din)/sizeof(reg->din[0]); idx++)
        if (reg->din[idx].val != cur->din[idx].val)
            GPIOERR("mismatch din[%2d]: %x <> %x\n", idx, reg->din[idx].val, cur->din[idx].val);
    for (idx = 0; idx < sizeof(reg->mode)/sizeof(reg->mode[0]); idx++)
        if (reg->mode[idx].val != cur->mode[idx].val)
            GPIOERR("mismatch mode[%2d]: %x <> %x\n", idx, reg->mode[idx].val, cur->mode[idx].val); 

    kfree(cur);
    return 0;
}
/*---------------------------------------------------------------------------*/
static ssize_t mt_gpio_show_pin(struct device* dev, 
                                struct device_attribute *attr, char *buf)
{
    GPIO_REGS *reg = (GPIO_REGS*)GPIO_BASE;
    return mt_gpio_dump_regs(buf, PAGE_SIZE, reg);;
}
/*---------------------------------------------------------------------------*/
static ssize_t mt_gpio_store_pin(struct device* dev, struct device_attribute *attr,  
                                 const char *buf, size_t count)
{
    int pin, ret;
    int mode, pullsel, dout, pullen, dir, dinv, din;
    u32 num,src,div;
    struct mt_gpio_obj *obj = (struct mt_gpio_obj*)dev_get_drvdata(dev);    
    if (!strncmp(buf, "-h", 2)) {
        GPIOMSG("PIN: [MODE] [PSEL] [DIN] [DOUT] [PEN] [DIR] [DINV]\n");
    } else if (!strncmp(buf, "-r0", 3) && (1 == sscanf(buf+3, "%d", &pin))) {
        GPIO_CFG cfg = {.no = pin};
        mt_gpio_read_pin(&cfg, 0);
        GPIOMSG("%3d: %d %d %d %d %d %d %d\n", cfg.no, cfg.mode, cfg.pullsel, 
                cfg.din, cfg.dout, cfg.pullen, cfg.dir, cfg.dinv);
    } else if (!strncmp(buf, "-r1", 3) && (1 == sscanf(buf+3, "%d", &pin))) {
        GPIO_CFG cfg = {.no = pin};
        mt_gpio_read_pin(&cfg, 1);
        GPIOMSG("%3d: %d %d %d %d %d %d %d\n", cfg.no, cfg.mode, cfg.pullsel, 
                cfg.din, cfg.dout, cfg.pullen, cfg.dir, cfg.dinv);
    } else if (!strncmp(buf, "-w", 2)) {
        buf += 2;
        if (!strncmp(buf, "mode", 4) && (2 == sscanf(buf+4, "%d %d", &pin, &mode)))
            GPIOMSG("set mode(%3d, %d)=%d\n", pin, mode, mt_set_gpio_mode(pin, mode));
        else if (!strncmp(buf, "psel", 4) && (2 == sscanf(buf+4, "%d %d", &pin, &pullsel)))
            GPIOMSG("set psel(%3d, %d)=%d\n", pin, pullsel, mt_set_gpio_pull_select(pin, pullsel));
        else if (!strncmp(buf, "dout", 4) && (2 == sscanf(buf+4, "%d %d", &pin, &dout)))
            GPIOMSG("set dout(%3d, %d)=%d\n", pin, dout, mt_set_gpio_out(pin, dout));
        else if (!strncmp(buf, "pen", 3) &&  (2 == sscanf(buf+3, "%d %d", &pin, &pullen)))
            GPIOMSG("set pen (%3d, %d)=%d\n", pin, pullen, mt_set_gpio_pull_enable(pin, pullen));
        else if (!strncmp(buf, "dir", 3) &&  (2 == sscanf(buf+3, "%d %d", &pin, &dir)))
            GPIOMSG("set dir (%3d, %d)=%d\n", pin, dir, mt_set_gpio_dir(pin, dir));
        else if (!strncmp(buf, "dinv", 4) && (2 == sscanf(buf+4, "%d %d", &pin, &dinv)))
            GPIOMSG("set dinv(%3d, %d)=%d\n", pin, dinv, mt_set_gpio_inversion(pin, dinv));      
        else if (8 == sscanf(buf, "=%d:%d %d %d %d %d %d %d", &pin, &mode, &pullsel, &din, &dout, &pullen, &dir, &dinv)) {
            GPIOMSG("set mode(%3d, %d)=%d\n", pin, mode, mt_set_gpio_mode(pin, mode));
            GPIOMSG("set psel(%3d, %d)=%d\n", pin, pullsel, mt_set_gpio_pull_select(pin, pullsel));
            GPIOMSG("set dout(%3d, %d)=%d\n", pin, dout, mt_set_gpio_out(pin, dout));
            GPIOMSG("set pen (%3d, %d)=%d\n", pin, pullen, mt_set_gpio_pull_enable(pin, pullen));
            GPIOMSG("set dir (%3d, %d)=%d\n", pin, dir, mt_set_gpio_dir(pin, dir));
            GPIOMSG("set dinv(%3d, %d)=%d\n", pin, dinv, mt_set_gpio_inversion(pin, dinv));      
        } else 
            GPIOMSG("invalid format: '%s'", buf);
    } else if (!strncmp(buf, "-t", 2)) {
        mt_gpio_self_test();
    } else if (!strncmp(buf, "-c", 2)) {
        mt_gpio_compare(dev);
    } else if (!strncmp(buf, "-da", 3)) {
        mt_gpio_dump_addr(dev);
    } else if (!strncmp(buf, "-d", 2)) {
        mt_gpio_dump(obj->reg);
    } else if (!strncmp(buf, "-m", 2)) {
        buf += 2;
        if (!strncmp(buf, "s", 1) && (3 == sscanf(buf+1, "%d %d %d", &num, &src, &div)))
            GPIOMSG("set num(%d, %d, %d)=%d\n", num, src, div, mt_set_clock_output(num, src,div));
	else if(!strncmp(buf, "g", 1) && (1 == sscanf(buf+1, "%d", &num))){
	    ret = mt_get_clock_output(num, &src,&div);
            GPIOMSG("get num(%d, %d, %d)=%d\n", num, src, div,ret);
	}else 
            GPIOMSG("invalid format: '%s'", buf);
    }
    return count;    
}
/*---------------------------------------------------------------------------*/
static DEVICE_ATTR(pin,      0664, mt_gpio_show_pin,   mt_gpio_store_pin);
/*---------------------------------------------------------------------------*/
static struct device_attribute *gpio_attr_list[] = {
    &dev_attr_pin,
};
/*---------------------------------------------------------------------------*/
static int mt_gpio_create_attr(struct device *dev) 
{
    int idx, err = 0;
    int num = (int)(sizeof(gpio_attr_list)/sizeof(gpio_attr_list[0]));
    if (!dev)
        return -EINVAL;

    for (idx = 0; idx < num; idx++) {
        if ((err = device_create_file(dev, gpio_attr_list[idx])))
            break;
    }
    
    return err;
}
/*---------------------------------------------------------------------------*/
static int mt_gpio_delete_attr(struct device *dev)
{
    int idx ,err = 0;
    int num = (int)(sizeof(gpio_attr_list)/sizeof(gpio_attr_list[0]));
    
    if (!dev)
        return -EINVAL;

    for (idx = 0; idx < num; idx++) 
        device_remove_file(dev, gpio_attr_list[idx]);

    return err;
}
/*****************************************************************************/
/* File operation                                                            */
/*****************************************************************************/
static int mt_gpio_open(struct inode *inode, struct file *file)
{
    struct mt_gpio_obj *obj = gpio_obj;
    GPIOFUC();

    if (!obj) {
        GPIOERR("NULL pointer");
        return -EFAULT;
    }

    atomic_inc(&obj->ref);
    file->private_data = obj;
    return nonseekable_open(inode, file);
}
/*---------------------------------------------------------------------------*/
static int mt_gpio_release(struct inode *inode, struct file *file)
{
    struct mt_gpio_obj *obj = gpio_obj;

    GPIOFUC();

    if (!obj) {
        GPIOERR("NULL pointer");
        return -EFAULT;
    }

    atomic_dec(&obj->ref);
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
static long mt_gpio_ioctl(struct file *file, 
                             unsigned int cmd, unsigned long arg)
{
    struct mt_gpio_obj *obj = gpio_obj;
    long res;
    u32 pin;

    GPIOFUC();

    if (!obj) {
        GPIOERR("NULL pointer");
        return -EFAULT;
    }

    switch(cmd) 
    {
        case GPIO_IOCQMODE:
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_get_gpio_mode(pin);
            break;
        }
        case GPIO_IOCTMODE0:
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_mode(pin, GPIO_MODE_00);
            break;
        }
        case GPIO_IOCTMODE1:      
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_mode(pin, GPIO_MODE_01);
            break;
        }
        case GPIO_IOCTMODE2:      
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_mode(pin, GPIO_MODE_02);
            break;
        }
        case GPIO_IOCTMODE3:      
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_mode(pin, GPIO_MODE_03);
            break;
        }
        case GPIO_IOCQDIR:        
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_get_gpio_dir(pin);
            break;
        }
        case GPIO_IOCSDIRIN:      
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_dir(pin, GPIO_DIR_IN);
            break;
        }
        case GPIO_IOCSDIROUT:     
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_dir(pin, GPIO_DIR_OUT);
            break;
        }
        case GPIO_IOCQPULLEN:       
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_get_gpio_pull_enable(pin);
            break;
        }
        case GPIO_IOCSPULLENABLE:
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_pull_enable(pin, TRUE);
            break;
        }
        case GPIO_IOCSPULLDISABLE:
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_pull_enable(pin, FALSE);
            break;
        }
        case GPIO_IOCQPULL:
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_get_gpio_pull_select(pin);
            break;
        }
        case GPIO_IOCSPULLDOWN:
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_pull_select(pin, GPIO_PULL_DOWN);
            break;
        }
        case GPIO_IOCSPULLUP:
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_pull_select(pin, GPIO_PULL_UP);
            break;
        }
        case GPIO_IOCQINV:        
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_get_gpio_inversion(pin);
            break;
        }
        case GPIO_IOCSINVENABLE:
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_inversion(pin, TRUE);
            break;
        }
        case GPIO_IOCSINVDISABLE: 
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_inversion(pin, FALSE);
            break;
        }
        case GPIO_IOCQDATAIN:     
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EFAULT) : mt_get_gpio_in(pin);
            break;
        }
        case GPIO_IOCQDATAOUT:    
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_get_gpio_out(pin);
            break;
        }
        case GPIO_IOCSDATALOW:    
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_out(pin, GPIO_OUT_ZERO);
            break;
        }
        case GPIO_IOCSDATAHIGH:   
        {
            pin = (u32)arg;
            res = GIO_INVALID_OBJ(obj) ? (-EACCES) : mt_set_gpio_out(pin, GPIO_OUT_ONE);
            break;
        }
        default:
        {
            res = -EPERM;
            break;
        }
    }

    if (res == -EACCES)
        GPIOERR(" cmd = 0x%8X, invalid pointer\n", cmd);
    else if (res < 0)
        GPIOERR(" cmd = 0x%8X, err = %ld\n", cmd, res);
    return res;
}
/*---------------------------------------------------------------------------*/
static struct file_operations mt_gpio_fops = 
{
    .owner=        THIS_MODULE,
    .unlocked_ioctl=	mt_gpio_ioctl,
    .open=         mt_gpio_open,    
    .release=      mt_gpio_release,
};
/*----------------------------------------------------------------------------*/
static struct miscdevice mt_gpio_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = "mtgpio",
    .fops = &mt_gpio_fops,
};
/*---------------------------------------------------------------------------*/
static int mt_gpio_probe(struct platform_device *dev)
{
    int err;
	/*---------------MD convert gpio-name to gpio-number--------*/
	unsigned int var_max;
	/*---------------MD convert gpio-name to gpio-number--------*/
    struct miscdevice *misc = &mt_gpio_device;
    
	printk(KERN_ALERT"[GPIO]%5d,<%s> gpio devices probe\n", __LINE__, __func__);
    GPIOLOG("Registering GPIO device\n");
    
    if (!gpio_obj)
        GPIO_RETERR(-EACCES, "");
    gpio_obj->misc = misc;
    
    if ((err = misc_register(misc)))
        GPIOERR("register gpio\n");    

    if ((err = mt_gpio_create_attr(misc->this_device)))
        GPIOERR("create attribute\n");

    dev_set_drvdata(misc->this_device, gpio_obj);

	var_max = 0;
	/*---------------MD convert gpio-name to gpio-number--------*/
#ifdef GPIO_AST_CS_PIN
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_AST_HIF_CS");
	gpio_info[var_max].m_gpio_num = GPIO_AST_CS_PIN;
	var_max++;
#endif
#ifdef GPIO_AST_CS_PIN_LPCE
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_AST_HIF_CS_ID");
	gpio_info[var_max].m_gpio_num = GPIO_AST_CS_PIN_LPCE;
	var_max++;
#endif
#ifdef GPIO_AST_RST_PIN
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_AST_Reset");
	gpio_info[var_max].m_gpio_num = GPIO_AST_RST_PIN;
	var_max++;
#endif
#ifdef GPIO_AST_CLK32K_PIN
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_AST_CLK_32K");
	gpio_info[var_max].m_gpio_num = GPIO_AST_CLK32K_PIN;
	var_max++;
#endif
#ifdef GPIO_AST_CLK32K_PIN_CLK
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_AST_CLK_32K_CLKM");
	gpio_info[var_max].m_gpio_num = GPIO_AST_CLK32K_PIN_CLK;
	var_max++;
#endif
#ifdef GPIO_AST_WAKEUP_PIN
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_AST_Wakeup");
	gpio_info[var_max].m_gpio_num = GPIO_AST_WAKEUP_PIN;
	var_max++;
#endif
#ifdef GPIO_AST_INTR_PIN
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_AST_INT");
	gpio_info[var_max].m_gpio_num = GPIO_AST_INTR_PIN;
	var_max++;
#endif
#ifdef GPIO_AST_WAKEUP_INTR_PIN
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_AST_WAKEUP_INT");
	gpio_info[var_max].m_gpio_num = GPIO_AST_WAKEUP_INTR_PIN;
	var_max++;
#endif
#ifdef GPIO_AST_AFC_SWITCH_PIN
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_AST_AFC_Switch");
	gpio_info[var_max].m_gpio_num = GPIO_AST_AFC_SWITCH_PIN;
	var_max++;
#endif
#ifdef GPIO_FDD_BAND_SUPPORT_DETECT_1ST_PIN
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_FDD_Band_Support_Detection_1");
	gpio_info[var_max].m_gpio_num = GPIO_FDD_BAND_SUPPORT_DETECT_1ST_PIN;
	var_max++;
#endif
#ifdef GPIO_FDD_BAND_SUPPORT_DETECT_2ND_PIN
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_FDD_Band_Support_Detection_2");
	gpio_info[var_max].m_gpio_num = GPIO_FDD_BAND_SUPPORT_DETECT_2ND_PIN;
	var_max++;
#endif
#ifdef GPIO_FDD_BAND_SUPPORT_DETECT_3RD_PIN
	sprintf(gpio_info[var_max].m_gpio_name, "GPIO_FDD_Band_Support_Detection_3");
	gpio_info[var_max].m_gpio_num = GPIO_FDD_BAND_SUPPORT_DETECT_3RD_PIN;
	var_max++;
#endif
/*---------------MD convert gpio-name to gpio-number--------*/
    return err;
}
/*---------------------------------------------------------------------------*/
static int mt_gpio_remove(struct platform_device *dev)
{
    struct mt_gpio_obj *obj = platform_get_drvdata(dev);
    int err;
    
    if ((err = mt_gpio_delete_attr(obj->misc->this_device)))
        GPIOERR("delete attr\n");

    if ((err = misc_deregister(obj->misc)))
        GPIOERR("deregister gpio\n");
    
    return err;
}
/*---------------------------------------------------------------------------*/
static void mt_gpio_shutdown(struct platform_device *dev)
{
    GPIOLOG("shutdown\n");
}
/*---------------------------------------------------------------------------*/
static int mt_gpio_suspend(struct platform_device *dev, pm_message_t state)
{
   //GPIOLOG("suspend: %d!\n", state.event);
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
static int mt_gpio_resume(struct platform_device *dev)
{
    //printk("GPIO Resume !\n");
    return RSUCCESS;
}
/*---------------------------------------------------------------------------*/
static struct platform_driver gpio_driver = 
{
    .probe          = mt_gpio_probe,
    .remove         = mt_gpio_remove,
    .shutdown       = mt_gpio_shutdown,
    .suspend        = mt_gpio_suspend,
    .resume         = mt_gpio_resume,
    .driver         = {
            .name = GPIO_DEVICE,
        },    
};
/*---------------------------------------------------------------------------*/
static int __init mt_gpio_init(void)
{
    int ret = 0;
    GPIOLOG("version: %s\n", VERSION);
    
    ret = platform_driver_register(&gpio_driver);
    return ret;
}
/*---------------------------------------------------------------------------*/
static void __exit mt_gpio_exit(void)
{
    platform_driver_unregister(&gpio_driver);
    return;
}
/*---------------------------------------------------------------------------*/
module_init(mt_gpio_init);
module_exit(mt_gpio_exit);
MODULE_AUTHOR("Ranran <ranran.lu@mediatek.com>");
MODULE_DESCRIPTION("MT General Purpose Driver (GPIO) $Revision$");
MODULE_LICENSE("GPL");
/*---------------------------------------------------------------------------*/

#endif //(CONFIG_MT6575T_FPGA  || CONFIG_MT6577_FPGA)
#endif
