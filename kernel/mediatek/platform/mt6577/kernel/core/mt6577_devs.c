#include <generated/autoconf.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/ioport.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/android_pmem.h>
//FIX-ME: marked for early porting
//#include <linux/android_vmem.h>
#include <asm/setup.h>
#include <asm/mach/arch.h>
#include <linux/sysfs.h>
#include <asm/io.h>
#include <linux/spi/spi.h>
#include <linux/amba/bus.h>
#include <linux/amba/clcd.h>
#include <linux/version.h>
#include <linux/musbfsh.h>

#include "mach/memory.h"
#include "mach/irqs.h"
#include "mach/mt6577_reg_base.h"
#include "mach/mt6577_devs.h"
#include "mach/mt6577_boot.h"

/*NOTICE: the compile option should be defined if EFUSE is programed*/
#define CONFIG_MTK_USB_UNIQUE_SERIAL
#define SERIALNO_LEN 32
static char serial_number[SERIALNO_LEN];
#define USB_UNIQUE_HEX_SERIAL  1  //add by ljx

extern BOOTMODE get_boot_mode(void);
extern void adjust_kernel_cmd_line_setting_for_console(char*, char*);
struct {
	u32 base;
	u32 size;
} bl_fb = {0, 0};

static int use_bl_fb = 0;

/*=======================================================================*/
/* MT6577 USB GADGET                                                     */
/*=======================================================================*/
static u64 usb_dmamask = DMA_BIT_MASK(32);

struct platform_device mt_device_usb = {
	.name		  = "mt_usb",
	.id		  = -1,
	.dev = {                
		//.platform_data          = &usb_data_mt65xx,     
		.dma_mask               = &usb_dmamask,
		.coherent_dma_mask      = DMA_BIT_MASK(32), 
		//.release=musbfsh_hcd_release,    
		},
};

/*=======================================================================*/
/* MT6575 USB11 Host                      */
/*=======================================================================*/
#if defined(CONFIG_MTK_USBFSH) 
static u64 usb11_dmamask = DMA_BIT_MASK(32);
extern void musbfsh_hcd_release (struct device *dev);

static struct musbfsh_hdrc_config musbfsh_config_mt65xx = {    
	.multipoint     = false,    
	.dyn_fifo       = true,    
	.soft_con       = true,    
	.dma            = true,        
	.num_eps        = 6,    
	.dma_channels   = 4,
};
static struct musbfsh_hdrc_platform_data usb_data_mt65xx = {
	.mode           = 1,
	.config         = &musbfsh_config_mt65xx,
};
static struct platform_device mt_usb11_dev = {
	.name           = "musbfsh_hdrc",
	.id             = -1,
	.dev = {                
		.platform_data          = &usb_data_mt65xx,
		.dma_mask               = &usb11_dmamask,
		.coherent_dma_mask      = DMA_BIT_MASK(32),
		.release		= musbfsh_hcd_release,
	},
};
#endif

/*=======================================================================*/
/* MTK UART Ports                                                     */
/*=======================================================================*/
#if defined(CFG_DEV_UART1)
static struct resource mtk_resource_uart1[] = {
	{
		.start		= UART1_BASE,
		.end		= UART1_BASE + MTK_UART_SIZE - 1,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= MT6577_UART1_IRQ_ID,
		.flags		= IORESOURCE_IRQ,
	},
};
#endif

#if defined(CFG_DEV_UART2)
static struct resource mtk_resource_uart2[] = {
	{
		.start		= UART2_BASE,
		.end		= UART2_BASE + MTK_UART_SIZE - 1,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= MT6577_UART2_IRQ_ID,
		.flags		= IORESOURCE_IRQ,
	},
};
#endif

#if defined(CFG_DEV_UART3)
static struct resource mtk_resource_uart3[] = {
	{
		.start		= UART3_BASE,
		.end		= UART3_BASE + MTK_UART_SIZE - 1,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= MT6577_UART3_IRQ_ID,
		.flags		= IORESOURCE_IRQ,
	},
};
#endif

#if defined(CFG_DEV_UART4)
static struct resource mtk_resource_uart4[] = {
	{
		.start		= UART4_BASE,
		.end		= UART4_BASE + MTK_UART_SIZE - 1,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= MT6577_UART4_IRQ_ID,
		.flags		= IORESOURCE_IRQ,
	},
};
#endif

extern unsigned long max_pfn;
#define RESERVED_MEM_MODEM   		PHYS_OFFSET
#ifndef CONFIG_RESERVED_MEM_SIZE_FOR_PMEM
#define CONFIG_RESERVED_MEM_SIZE_FOR_PMEM 	0
#endif


#if defined(CONFIG_MTK_FB)
char temp_command_line[1024] = {0};
extern unsigned int DISP_GetVRamSizeBoot(char* cmdline);
#define RESERVED_MEM_SIZE_FOR_FB (DISP_GetVRamSizeBoot((char*)&temp_command_line))
extern void   mtkfb_set_lcm_inited(bool isLcmInited);
#else
#define RESERVED_MEM_SIZE_FOR_FB (0x400000)
#endif

/*
 * The memory size reserved for PMEM
 *
 * The size could be varied in different solutions. 
 * The size is set in mt65xx_fixup function.
 * - MT6577 in develop should be 0x3600000
 * - MT6577 SQC should be 0x0
 */
unsigned int RESERVED_MEM_SIZE_FOR_PMEM = 0;

#define TOTAL_RESERVED_MEM_SIZE (RESERVED_MEM_SIZE_FOR_PMEM + \
                                 RESERVED_MEM_SIZE_FOR_FB)

#define MAX_PFN        ((max_pfn << PAGE_SHIFT) + PHYS_OFFSET)

#define PMEM_MM_START  (MAX_PFN)
#define PMEM_MM_SIZE   (RESERVED_MEM_SIZE_FOR_PMEM)

#define FB_START       (PMEM_MM_START + PMEM_MM_SIZE)
#define FB_SIZE        (RESERVED_MEM_SIZE_FOR_FB)


static struct platform_device mtk_device_uart[] = {

    #if defined(CFG_DEV_UART1)
    {
    	.name			= "mtk-uart",
    	.id				= 0,
    	.num_resources	= ARRAY_SIZE(mtk_resource_uart1),
    	.resource		= mtk_resource_uart1,
    },
    #endif
    #if defined(CFG_DEV_UART2)
    {
    	.name			= "mtk-uart",
    	.id				= 1,
    	.num_resources	= ARRAY_SIZE(mtk_resource_uart2),
    	.resource		= mtk_resource_uart2,
    },
    #endif
    #if defined(CFG_DEV_UART3)
    {
    	.name			= "mtk-uart",
    	.id				= 2,
    	.num_resources	= ARRAY_SIZE(mtk_resource_uart3),
    	.resource		= mtk_resource_uart3,
    },
    #endif   

    #if defined(CFG_DEV_UART4)
    {
    	.name			= "mtk-uart",
    	.id				= 3,
    	.num_resources	= ARRAY_SIZE(mtk_resource_uart4),
    	.resource		= mtk_resource_uart4,
    },
    #endif
};



/*=======================================================================*/
/* MTK External MD Ctl                                                   */
/*=======================================================================*/
#if defined(MTK_DT_SUPPORT) && (!defined(EVDO_DT_SUPPORT))
static struct platform_device emd_chr_devs[] = {

	{
		.name	= "ext-md-ctl",
		.id	= 0,
	},
	{
		.name	= "ext-md-ctl",
		.id	= 1,
	},
	{
		.name	= "ext-md-ctl",
		.id	= 2,
	},
};
#endif
#if defined(CONFIG_FIQ_DEBUGGER)
extern void fiq_uart_fixup(int uart_port);
extern struct platform_device mt_fiq_debugger;
#endif

/* ========================================================================= */
/* implementation of serial number attribute                                 */
/* ========================================================================= */
#define to_sysinfo_attribute(x) container_of(x, struct sysinfo_attribute, attr)

struct sysinfo_attribute{
    struct attribute attr;
    ssize_t (*show)(char *buf);
    ssize_t (*store)(const char *buf, size_t count);
};

static struct kobject sn_kobj;

static ssize_t sn_show(char *buf){
    return snprintf(buf, 4096, "%s\n", serial_number);
}

struct sysinfo_attribute sn_attr = {
#if (LINUX_VERSION_CODE < KERNEL_VERSION(2,6,36))
    .attr = {"serial_number", THIS_MODULE, 0644},
#else
    .attr = {"serial_number", 0644},
#endif
    .show = sn_show,
    .store = NULL
};

static ssize_t sysinfo_show(struct kobject *kobj, struct attribute *attr, char *buf)
{
    struct sysinfo_attribute *sysinfo_attr = to_sysinfo_attribute(attr);
    ssize_t ret = -EIO;

    if(sysinfo_attr->show)
        ret = sysinfo_attr->show(buf);

    return ret;
}

static ssize_t sysinfo_store(struct kobject *kobj, struct attribute *attr, const char *buf, size_t count)
{
    struct sysinfo_attribute *sysinfo_attr = to_sysinfo_attribute(attr);
    ssize_t ret = -EIO;

    if(sysinfo_attr->store)
        ret = sysinfo_attr->store(buf, count);

    return ret;
}

static struct sysfs_ops sn_sysfs_ops = {
    .show = sysinfo_show,
    .store = sysinfo_store
};

static struct attribute *sn_attrs[] = {
    &sn_attr.attr,
    NULL
};

static struct kobj_type sn_ktype = {
    .sysfs_ops = &sn_sysfs_ops,
    .default_attrs = sn_attrs
};

static char udc_chr[32] = {"ABCDEFGHIJKLMNOPQRSTUVWSYZ456789"};
static char udc_chr1[16] = {"0123456789ABCDEF"};
int get_serial(uint64_t hwkey, uint32_t chipid, char ser[SERIALNO_LEN])
{
    uint16_t hashkey[4];
    int idx, ser_idx;
    uint32_t digit, id;
    uint64_t tmp = hwkey;

    memset(ser, 0x00, SERIALNO_LEN);

    /* split to 4 key with 16-bit width each */
    tmp = hwkey;
    for (idx = 0; idx < ARRAY_SIZE(hashkey); idx++) {
        hashkey[idx] = (uint16_t)(tmp & 0xffff);
        tmp >>= 16;
    }

    /* hash the key with chip id */
    id = chipid;
    for (idx = 0; idx < ARRAY_SIZE(hashkey); idx++) {
        digit = (id % 10);
        hashkey[idx] = (hashkey[idx] >> digit) | (hashkey[idx] << (16-digit));
        id = (id / 10);
    }

#if USB_UNIQUE_HEX_SERIAL
   ser_idx = 0;
    for (idx = 0; idx < ARRAY_SIZE(hashkey); idx++) {
        ser[ser_idx++] = (hashkey[idx] & 0x000f);
        ser[ser_idx++] = (hashkey[idx] & 0x00f0) >> 4;
        ser[ser_idx++] = (hashkey[idx] & 0x0f00) >> 8;
        ser[ser_idx++] = (hashkey[idx] & 0xf000) >> 12;
    }
    for (idx = 0; idx < ser_idx; idx++)
        ser[idx] = udc_chr1[(int)ser[idx]];
#else
    /* generate serail using hashkey */
    ser[0] = 'M';
    ser[1] = 'T';
    ser[2] = '-';
    ser_idx = 3;
    for (idx = 0; idx < ARRAY_SIZE(hashkey); idx++) {
        ser[ser_idx++] = (hashkey[idx] & 0x001f);
        ser[ser_idx++] = (hashkey[idx] & 0x00f8) >> 3;
        ser[ser_idx++] = (hashkey[idx] & 0x1f00) >> 8;
        ser[ser_idx++] = (hashkey[idx] & 0xf800) >> 11;
    }
    for (idx = 3; idx < ser_idx; idx++)
        ser[idx] = udc_chr[(int)ser[idx]];
#endif
    ser[ser_idx] = 0x00;
    return 0;
}

/*=======================================================================*/
/* MTK GPIO                                                           */
/*=======================================================================*/
struct platform_device gpio_dev =
{
    .name = "mt-gpio",
    .id   = -1,
};

#if defined(CONFIG_SERIAL_AMBA_PL011)
static struct amba_device uart1_device =
{
    .dev =
    {
        .coherent_dma_mask = ~0,
        .init_name = "dev:f1",
        .platform_data = NULL,
    },
    .res =
    {
        .start  = 0xE01F1000,
        .end = 0xE01F1000 + SZ_4K - 1,
        .flags  = IORESOURCE_MEM,
    },
    .dma_mask = ~0,
    .irq = MT6577_UART1_IRQ_ID,
};
#endif

/*=======================================================================*/
/* MT6577 SD Hosts                                                       */
/*=======================================================================*/
#if defined(CFG_DEV_MSDC0)
static struct resource mt6577_resource_sd0[] = {
    {
        .start  = MSDC_0_BASE,
        .end    = MSDC_0_BASE + 0x108,
        .flags  = IORESOURCE_MEM,
    },
    {
        .start  = MT6577_MSDC0_IRQ_ID,
        .flags  = IORESOURCE_IRQ,
    },
};
#endif

#if defined(CFG_DEV_MSDC1)
static struct resource mt6577_resource_sd1[] = {
    {
        .start  = MSDC_1_BASE,
        .end    = MSDC_1_BASE + 0x108,
        .flags  = IORESOURCE_MEM,
    },
    {
        .start  = MT6577_MSDC1_IRQ_ID,
        .flags  = IORESOURCE_IRQ,
    },
};
#endif

#if defined(CFG_DEV_MSDC2)
static struct resource mt6577_resource_sd2[] = {
    {
        .start  = MSDC_2_BASE,
        .end    = MSDC_2_BASE + 0x108,
        .flags  = IORESOURCE_MEM,
    },
    {
        .start  = MT6577_MSDC2_IRQ_ID,
        .flags  = IORESOURCE_IRQ,
    },
};
#endif

#if defined(CFG_DEV_MSDC3)
static struct resource mt6577_resource_sd3[] = {
    {
        .start  = MSDC_3_BASE,
        .end    = MSDC_3_BASE + 0x108,
        .flags  = IORESOURCE_MEM,
    },
    {
        .start  = MT6577_MSDC3_IRQ_ID,
        .flags  = IORESOURCE_IRQ,
    },
};
#endif

#if defined(CONFIG_MTK_FB)
static u64 mtkfb_dmamask = ~(u32)0;

static struct resource resource_fb[] = {
	{
		.start		= 0, /* Will be redefined later */
		.end		= 0,
		.flags		= IORESOURCE_MEM
	}
};

static struct platform_device mt6577_device_fb = {
    .name = "mtkfb",
    .id   = 0,
    .num_resources = ARRAY_SIZE(resource_fb),
    .resource      = resource_fb,
    .dev = {
        .dma_mask = &mtkfb_dmamask,
        .coherent_dma_mask = 0xffffffff,
    },
};
#endif

#ifdef MTK_HDMI_SUPPORT
static struct platform_device mtk_hdmi_dev = {
    .name = "hdmitx",
    .id   = 0,
};
#endif

#if defined (CONFIG_MTK_SPI)
static struct resource mt_spi_resources[] =
{
	[0]={
		.start = SPI_BASE,
		.end = SPI_BASE + 0x0028,
		.flags = IORESOURCE_MEM,
	},
	[1]={
		.start = MT6577_SPI_IRQ_ID,
		.flags = IORESOURCE_IRQ,
	},
};

static struct platform_device mt_spi_device = {
	.name = "mt-spi",
	.num_resources = ARRAY_SIZE(mt_spi_resources),
	.resource=mt_spi_resources
};

//static struct spi_board_info spi_board_devs[] __initdata = {
//	[0] = {
//        	.modalias="spidev",
//		.bus_num = 0,
//		.chip_select=0,
//		.mode = SPI_MODE_3,
//	},
//};
#endif

#if defined(CONFIG_MTK_ACCDET)
struct platform_device accdet_device = {
	.name	  ="Accdet_Driver",		
	.id		  = -1,	
	//.dev    ={
	//.release = accdet_dumy_release,
	//}
};
#endif
#if defined(MTK_TVOUT_SUPPORT)

static struct resource mt6577_TVOUT_resource[] = {
    [0] = {//TVC
        .start = TVC_BASE,
        .end   = TVC_BASE + 0x78,
        .flags = IORESOURCE_MEM,
    },
    [1] = {//TVR
        .start = TV_ROT_BASE,
        .end   = TV_ROT_BASE + 0x378,
        .flags = IORESOURCE_MEM,
    },
    [2] = {//TVE
        .start = TVE_BASE,
        .end   = TVE_BASE + 0x84,
        .flags = IORESOURCE_MEM,
    },
};

static u64 mt6577_TVOUT_dmamask = ~(u32)0;

static struct platform_device mt6577_TVOUT_dev = {
	.name		  = "TV-out",
	.id		  = 0,
	.num_resources	  = ARRAY_SIZE(mt6577_TVOUT_resource),
	.resource	  = mt6577_TVOUT_resource,
	.dev              = {
		.dma_mask = &mt6577_TVOUT_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	}
};
#endif
static struct platform_device mt6577_device_sd[] =
{
#if defined(CFG_DEV_MSDC0)
    {
        .name           = "mtk-sd",
        .id             = 0,
        .num_resources  = ARRAY_SIZE(mt6577_resource_sd0),
        .resource       = mt6577_resource_sd0,
        .dev = {
            .platform_data = &msdc0_hw,
        },
    },
#endif
#if defined(CFG_DEV_MSDC1)
    {
        .name           = "mtk-sd",
        .id             = 1,
        .num_resources  = ARRAY_SIZE(mt6577_resource_sd1),
        .resource       = mt6577_resource_sd1,
        .dev = {
            .platform_data = &msdc1_hw,
        },
    },
#endif
#if defined(CFG_DEV_MSDC2)
    {
        .name           = "mtk-sd",
        .id             = 2,
        .num_resources  = ARRAY_SIZE(mt6577_resource_sd2),
        .resource       = mt6577_resource_sd2,
        .dev = {
            .platform_data = &msdc2_hw,
        },
    },
#endif
#if defined(CFG_DEV_MSDC3)
    {
        .name           = "mtk-sd",
        .id             = 3,
        .num_resources  = ARRAY_SIZE(mt6577_resource_sd3),
        .resource       = mt6577_resource_sd3,
        .dev = {
            .platform_data = &msdc3_hw,
        },
    },
#endif
};

/*=======================================================================*/
/* MT6577 Keypad                                                         */
/*=======================================================================*/
#ifdef CONFIG_MTK_KEYPAD
static struct platform_device kpd_pdev = {
	.name	= "mtk-kpd",
	.id	= -1,
};
#endif

#ifdef CONFIG_RFKILL
/*=======================================================================*/
/* MT6573 RFKill module (BT and WLAN)                                             */
/*=======================================================================*/
/* MT66xx RFKill BT */
struct platform_device mt6577_device_rfkill = {
    .name   = "mt-rfkill",
    .id     = -1,
};
#endif

#if defined(CONFIG_KEYBOARD_HID)
/*=======================================================================*/
/* HID Keyboard  add by zhangsg                                                 */
/*=======================================================================*/
static struct platform_device mt_hid_dev = {
    .name = "hid-keyboard",
    .id   = -1,
};
#endif

/*=======================================================================*/
/* MT6577 Touch Panel                                                    */
/*=======================================================================*/
static struct platform_device mtk_tpd_dev = {
    .name = "mtk-tpd",
    .id   = -1,
};

/*=======================================================================*/
/* MT6577 ofn                                                           */
/*=======================================================================*/
#if defined(CUSTOM_KERNEL_OFN)
static struct platform_device ofn_driver =
{
    .name = "mtofn",
    .id   = -1,
};
#endif

/*=======================================================================*/
/* MT6577 CPUFreq                                                         */
/*=======================================================================*/
#ifdef CONFIG_CPU_FREQ
static struct platform_device cpufreq_pdev = {
    .name = "mtk-cpufreq",
    .id   = -1,
};
#endif

/*=======================================================================*/
/* MT6577 Thermal Controller module                                      */
/*=======================================================================*/
struct platform_device thermal_pdev = {
    .name = "mtk-thermal",
    .id   = -1,
};

/*=======================================================================*/
/* MT6577 USIF-DUMCHAR                                                          */
/*=======================================================================*/

static struct platform_device dummychar_device =
{
       .name           = "dummy_char",
        .id             = 0,
};

/*=======================================================================*/
/* MT6577 NAND                                                           */
/*=======================================================================*/
#if defined(CONFIG_MTK_MTD_NAND)
#define NFI_base    NFI_BASE//0x80032000
#define NFIECC_base NFIECC_BASE//0x80038000
static struct resource mt6577_resource_nand[] = {
	{
		.start		= NFI_base,
		.end		= NFI_base + 0x1A0,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= NFIECC_base,
		.end		= NFIECC_base + 0x150,
		.flags		= IORESOURCE_MEM,
	},
	{
		.start		= MT6577_NFI_IRQ_ID,
		.flags		= IORESOURCE_IRQ,
	},
	{
		.start		= MT6577_NFIECC_IRQ_ID,
		.flags		= IORESOURCE_IRQ,
	},
};

static struct platform_device mt6577_nand_dev = {
    .name = "mt6577-nand",
    .id   = 0,
   	.num_resources	= ARRAY_SIZE(mt6577_resource_nand),
   	.resource		= mt6577_resource_nand,
    .dev            = {
        .platform_data = &mt6577_nand_hw,
    },
};
#endif

/*=======================================================================*/
/* Audio                                                 */
/*=======================================================================*/
static u64        AudDrv_dmamask      = 0xffffffffUL;
static struct platform_device AudDrv_device = {
	.name  = "AudDrv_driver_device",
	.id    = 0,
	.dev   = {
		        .dma_mask = &AudDrv_dmamask,
		        .coherent_dma_mask =  0xffffffffUL
	         }
};

/*=======================================================================*/
/* MTK I2C                                                            */
/*=======================================================================*/

static struct i2c_board_info __initdata i2c_devs1[]={
	//{ I2C_BOARD_INFO("kd_camera_hw", 0xfe>>1), },
	//{ I2C_BOARD_INFO("FM50AF", 0x18), },
	//{ I2C_BOARD_INFO("dummy_eeprom", 0xA0>>1),},
	//{ I2C_BOARD_INFO("EEPROM_S24CS64A", 0xAA>>1),},
	///{ I2C_BOARD_INFO("MATV_I2C", 0x41), },
};

static struct resource mt_resource_i2c1[] = {
    {
        .start  = I2C0_BASE,
        .end    = I2C0_BASE + 0x70,
        .flags  = IORESOURCE_MEM,
    },
    {
        .start  = MT6577_I2C0_IRQ_ID,
        .flags  = IORESOURCE_IRQ,
    },
};

static struct resource mt_resource_i2c2[] = {
    {
        .start  = I2C1_BASE,
        .end    = I2C1_BASE + 0x70,
        .flags  = IORESOURCE_MEM,
    },
    {
        .start  = MT6577_I2C1_IRQ_ID,
        .flags  = IORESOURCE_IRQ,
    },
};

static struct resource mt_resource_i2c3[] = {
    {
        .start  = I2C2_BASE,
        .end    = I2C2_BASE + 0xC0,
        .flags  = IORESOURCE_MEM,
    },
    {
        .start  = MT6577_I2C_DUAL_IRQ_ID,
        .flags  = IORESOURCE_IRQ,
    },
};

static struct platform_device mt_device_i2c[] = {
    {
        .name           = "mt-i2c",
        .id             = 0,
        .num_resources  = ARRAY_SIZE(mt_resource_i2c1),
        .resource       = mt_resource_i2c1,
    },
    {
        .name           = "mt-i2c",
        .id             = 1,
        .num_resources  = ARRAY_SIZE(mt_resource_i2c2),
        .resource       = mt_resource_i2c2,
    },
    {
        .name           = "mt-i2c",
        .id             = 2,
        .num_resources  = ARRAY_SIZE(mt_resource_i2c3),
        .resource       = mt_resource_i2c3,
    },
};



static u64 mtk_mau_dmamask = ~(u32)0;

static struct platform_device mtk_mau_dev = {
	.name		  = "MTK_MAU",
	.id		  = 0,
	.dev              = {
		.dma_mask = &mtk_mau_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	}
};



/*=======================================================================*/
/* MT6577 MDP                                                            */
/*=======================================================================*/
#if 1 //defined(CONFIG_MDP_MT6577)
static struct resource mt6577_MDP_resource[] = {

    [0] = {//MID_BRZ
        .start = BRZ_BASE,
        .end   = BRZ_BASE + 0x34,
        .flags = IORESOURCE_MEM,
    },
    [1] = {//MID_CRZ             
        .start = CRZ_BASE,
        .end   = CRZ_BASE + 0xB8,
        .flags = IORESOURCE_MEM,
    },
    [2] = {//MID_PRZ0                                    
        .start = PRZ0_BASE,
        .end   = PRZ0_BASE + 0xB8,
        .flags = IORESOURCE_MEM,
    },
    [3] = {//MID_PRZ1                        
        .start = PRZ1_BASE,
        .end   = PRZ1_BASE + 0xB8,
        .flags = IORESOURCE_MEM,
    },
    [4] = {//MID_VRZ0            
        .start = VRZ0_BASE,
        .end   = VRZ0_BASE + 0x60,
        .flags = IORESOURCE_MEM,
    },
    [5] = {//MID_VRZ1            
        .start = VRZ1_BASE,
        .end   = VRZ1_BASE + 0x60,
        .flags = IORESOURCE_MEM,
    },
    [6] = {//MID_R_DMA0                      
        .start = R_DMA0_BASE,
        .end   = R_DMA0_BASE + 0x3D0,
        .flags = IORESOURCE_MEM,
    },
    [7] = {//MID_R_DMA1          
        .start = R_DMA1_BASE,
        .end   = R_DMA1_BASE + 0x3D0,
        .flags = IORESOURCE_MEM,
    },
    [8] = {//MID_OVL             
        .start = OVL_DMA_BASE,
        .end   = OVL_DMA_BASE + 0x14C,
        .flags = IORESOURCE_MEM,
    },
    [9] = {//MID_IPP             
        .start = IMGPROC_BASE,
        .end   = IMGPROC_BASE + 0x32C,
        .flags = IORESOURCE_MEM,
    },
    [10] = {//MID_MOUT            
        .start = MMSYS1_CONFIG_BASE,
        .end   = MMSYS1_CONFIG_BASE + 0x230,
        .flags = IORESOURCE_MEM,
    },
    [11] = {//MID_JPEG_DMA        
        .start = JPG_DMA_BASE,
        .end   = JPG_DMA_BASE + 0xB0,
        .flags = IORESOURCE_MEM,
    },
    [12] = {//MID_RGB_ROT0        
        .start = RGB_ROT0_BASE,
        .end   = RGB_ROT0_BASE + 0x37C,
        .flags = IORESOURCE_MEM,
    },
    [13] = {//MID_RGB_ROT1        
        .start = RGB_ROT1_BASE,
        .end   = RGB_ROT1_BASE + 0x37C,
        .flags = IORESOURCE_MEM,
    },
    [14] = {//MID_RGB_ROT2        
        .start = RGB_ROT2_BASE,
        .end   = RGB_ROT2_BASE + 0x37C,
        .flags = IORESOURCE_MEM,
    },
    [15] = {//MID_VDO_ROT0        
        .start = VDO_ROT0_BASE,
        .end   = VDO_ROT0_BASE + 0x37C,
        .flags = IORESOURCE_MEM,
    },
    [16] = {//MID_VDO_ROT1        
        .start = VDO_ROT1_BASE,
        .end   = VDO_ROT1_BASE + 0x37C,
        .flags = IORESOURCE_MEM,
    },

    #if 0
    [15] = {//ROTDMA IRQ line
        .start = MT6573_ROT_DMA_IRQ_LINE,
        .end   = MT6573_ROT_DMA_IRQ_LINE,
        .flags = IORESOURCE_IRQ,
    },
    [16] = {//RDMA IRQ line
        .start = MT6573_R_DMA_IRQ_LINE,
        .end   = MT6573_R_DMA_IRQ_LINE,
        .flags = IORESOURCE_IRQ,
    },
    [17] = {//RESZ IRQ line
        .start = MT6573_RESZ_IRQ_LINE,
        .end   = MT6573_RESZ_IRQ_LINE,
        .flags = IORESOURCE_IRQ,
    },
    [18] = {//CRZ IRQ line
        .start = MT6573_CRZ_IRQ_LINE,
        .end   = MT6573_CRZ_IRQ_LINE,
        .flags = IORESOURCE_IRQ,
    }
    #endif
};

static u64 mt6577_MDP_dmamask = ~(u32)0;

static struct platform_device mt6577_MDP_dev = {
	.name		    = "mt6575-MDP", /* FIXME. Sync to MHAL and init.rc */
	.id		        = 0,
	.num_resources	= ARRAY_SIZE(mt6577_MDP_resource),
	.resource	    = mt6577_MDP_resource,
	.dev            = {
		.dma_mask = &mt6577_MDP_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	}
};
#endif

/*=======================================================================*/
/* MT6573 GPS module                                                    */
/*=======================================================================*/
/* MT3326 GPS */
#ifdef CONFIG_MTK_GPS
struct platform_device mt3326_device_gps = {
	.name	       = "mt3326-gps",
	.id            = -1,
	.dev = {
        .platform_data = &mt3326_gps_hw,
    },	
};
#endif

/*=======================================================================*/
/* MT6573 PMEM                                                           */
/*=======================================================================*/
#if defined(CONFIG_ANDROID_PMEM)
static struct android_pmem_platform_data  pdata_multimedia = {
        .name = "pmem_multimedia",
        .no_allocator = 0,
        .cached = 1,
        .buffered = 1
};

static struct platform_device pmem_multimedia_device = {
        .name = "android_pmem",
        .id = 1,
        .dev = { .platform_data = &pdata_multimedia }
};
#endif

#if defined(CONFIG_ANDROID_VMEM)
static struct android_vmem_platform_data  pdata_vmultimedia = {
        .name = "vmem_multimedia",
        .no_allocator = 0,
        .cached = 1,
        .buffered = 1
};

static struct platform_device vmem_multimedia_device = {
        .name = "android_vmem",
        .id = -1,
        .dev = { .platform_data = &pdata_vmultimedia }
};
#endif

/*=======================================================================*/
/* MT6577 SYSRAM                                                         */
/*=======================================================================*/
static struct platform_device mt6577_SYSRAM_dev = {
	.name	= "camera-sysram", /* FIXME. Sync to driver, init.rc, MHAL */
	.id     = 0,
};

/*=======================================================================*/
/*=======================================================================*/
/* Commandline fitlter to choose the supported commands                  */
/*=======================================================================*/
static void cmdline_filter(struct tag *cmdline_tag, char *default_cmdline)
{
	const char *desired_cmds[] = {
	                                 "rdinit=",
                                     "nand_manf_id=",
                                     "nand_dev_id=",
                                     "uboot_ver=",
                                     "uboot_build_ver=",
                                     "lcm=",
									 "fps="
			             };

	int i;
	char *cs,*ce;

	cs = cmdline_tag->u.cmdline.cmdline;
	ce = cs;
	while((__u32)ce < (__u32)tag_next(cmdline_tag)) {

	    while(*cs == ' ' || *cs == '\0') {
	    	cs++;
	    	ce = cs;
	    }

	    if (*ce == ' ' || *ce == '\0') {
	    	for (i = 0; i < sizeof(desired_cmds)/sizeof(char *); i++){
	    	    if (memcmp(cs, desired_cmds[i], strlen(desired_cmds[i])) == 0) {
	    	        *ce = '\0';
         	        //Append to the default command line
        	        strcat(default_cmdline, " ");
	    	        strcat(default_cmdline, cs);
	    	    }
	    	}
	    	cs = ce + 1;
	    }
	    ce++;
	}
	if (strlen(default_cmdline) >= COMMAND_LINE_SIZE)
	{
		panic("Command line length is too long.\n\r");
	}
}
/*=======================================================================*/
/* Parse the framebuffer info						 */
/*=======================================================================*/
static int __init parse_tag_videofb_fixup(const struct tag *tags)
{
	bl_fb.base = tags->u.videolfb.lfb_base;
	bl_fb.size = tags->u.videolfb.lfb_size;
        use_bl_fb++;
	return 0;
}
void mt6577_fixup(struct machine_desc *desc, struct tag *tags, char **cmdline, struct meminfo *mi)
{
#ifndef CONFIG_MT6577_FPGA
    struct tag *cmdline_tag = NULL;
    struct tag *reserved_mem_bank_tag = NULL;
    struct tag *none_tag = NULL;

    int32_t max_limit_size = CONFIG_MAX_DRAM_SIZE_SUPPORT -
                             RESERVED_MEM_MODEM;
    int32_t avail_dram = 0;
    int32_t bl_mem_sz = 0;

	// xuecheng, set cmdline to temp_command_line, for display driver calculate reserved memory size
	struct tag *temp_tags = tags;
	for (; temp_tags->hdr.size; temp_tags = tag_next(temp_tags)) 
	{
		if(temp_tags->hdr.tag == ATAG_CMDLINE)
			cmdline_filter(temp_tags, (char*)&temp_command_line);
	}

    for (; tags->hdr.size; tags = tag_next(tags)) {
        if (tags->hdr.tag == ATAG_MEM) {
	    bl_mem_sz += tags->u.mem.size;

	    /*
             * Modify the memory tag to limit available memory to
             * CONFIG_MAX_DRAM_SIZE_SUPPORT
             */
            if (max_limit_size > 0) {
                if (max_limit_size >= tags->u.mem.size) {
                    max_limit_size -= tags->u.mem.size;
                    avail_dram += tags->u.mem.size;
                } else {
                    tags->u.mem.size = max_limit_size;
                    avail_dram += max_limit_size;
                    max_limit_size = 0;
                }

		// By Keene: 
		// remove this check to avoid calcuate pmem size before we know all dram size
		// Assuming the minimum size of memory bank is 256MB
                //if (tags->u.mem.size >= (TOTAL_RESERVED_MEM_SIZE)) {
                    reserved_mem_bank_tag = tags;
                //}
            } else {
                tags->u.mem.size = 0;
            }
        } else if (tags->hdr.tag == ATAG_CMDLINE) {
            cmdline_tag = tags;
        } else if (tags->hdr.tag == ATAG_BOOT) {
            g_boot_mode = tags->u.boot.bootmode;
        } else if (tags->hdr.tag == ATAG_VIDEOLFB) {
            parse_tag_videofb_fixup(tags);
        }
        else if(tags->hdr.tag == ATAG_META_COM)
        {
          g_meta_com_type = tags->u.meta_com.meta_com_type;
          g_meta_com_id = tags->u.meta_com.meta_com_id;
        }
    }
    /*
    * If the maximum memory size configured in kernel
    * is smaller than the actual size (passed from BL)
    * Still limit the maximum memory size but use the FB
    * initialized by BL
    */
    if (bl_mem_sz >= (CONFIG_MAX_DRAM_SIZE_SUPPORT - RESERVED_MEM_MODEM)) {
	use_bl_fb++;
    }

    /*
     * Setup PMEM size
     */
    /*
    if (avail_dram < 0x10000000)
        RESERVED_MEM_SIZE_FOR_PMEM = 0x1700000;
    else 
        RESERVED_MEM_SIZE_FOR_PMEM = 0x3600000;*/

     RESERVED_MEM_SIZE_FOR_PMEM = 0;


    /* Reserve memory in the last bank */
    if (reserved_mem_bank_tag)
        reserved_mem_bank_tag->u.mem.size -= ((__u32)TOTAL_RESERVED_MEM_SIZE);
    else // we should always have reserved memory
    	BUG();

    if(tags->hdr.tag == ATAG_NONE)
	none_tag = tags;

    if (cmdline_tag != NULL) {
#ifdef CONFIG_FIQ_DEBUGGER
        char *console_ptr;
        int uart_port;
#endif

        // This function may modify ttyMT3 to ttyMT0 if needed
        adjust_kernel_cmd_line_setting_for_console(cmdline_tag->u.cmdline.cmdline, *cmdline);
#ifdef CONFIG_FIQ_DEBUGGER
        if ((console_ptr=strstr(*cmdline, "ttyMT")) != 0)
        {
            uart_port = console_ptr[5] - '0';
            if (uart_port > 3)
                uart_port = -1;

            fiq_uart_fixup(uart_port);
        }
#endif
        cmdline_filter(cmdline_tag, *cmdline);
        /* Use the default cmdline */
        memcpy((void*)cmdline_tag,
               (void*)tag_next(cmdline_tag),
               /* ATAG_NONE actual size */
               (uint32_t)(none_tag) - (uint32_t)(tag_next(cmdline_tag)) + 8);
    }
#endif
}

//*==============================================
//auxadc device
//*==============================================
struct platform_device MT6577_auxadc_device = {
    .name   = "mt6577-auxadc",
    .id     = -1,
};

/*=======================================================================*/
/* MT6577 sensor module                                                  */
/*=======================================================================*/
struct platform_device sensor_gsensor = {
	.name	       = "gsensor",
	.id            = -1,
};

struct platform_device sensor_msensor = {
	.name	       = "msensor",
	.id            = -1,
};

struct platform_device sensor_orientation = {
	.name	       = "orientation",
	.id            = -1,
};

struct platform_device sensor_alsps = {
	.name	       = "als_ps",
	.id            = -1,
};

struct platform_device sensor_gyroscope = {
	.name	       = "gyroscope",
	.id            = -1,
};

struct platform_device sensor_barometer = {
	.name	       = "barometer",
	.id            = -1,
};
/* hwmon sensor */
struct platform_device hwmon_sensor = {
	.name	       = "hwmsensor",
	.id            = -1,
};
/*=======================================================================*/
/* MT6577 ISP                                                            */
/*=======================================================================*/
static struct resource mt_resource_isp[] = {
    [0] = { // ISP configuration
        .start = CAMINF_BASE,
        .end   = CAMINF_BASE + 0xD3C,
        .flags = IORESOURCE_MEM,
    },
    [1] = { // statistic result
        .start = ISPMEM_BASE,
        .end   = ISPMEM_BASE + 0x39C,
        .flags = IORESOURCE_MEM,
    },
    [2] = { // ISP IRQ
        .start = MT6577_CAM_IRQ_LINE,
        .flags = IORESOURCE_IRQ,
    }
};
static u64 mt_isp_dmamask = ~(u32) 0;
//
static struct platform_device mt_isp_dev = {
	.name		   = "camera-isp",
	.id		       = 0,
	.num_resources = ARRAY_SIZE(mt_resource_isp),
	.resource	   = mt_resource_isp,
	.dev           = {
		.dma_mask  = &mt_isp_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	}
};

/*=======================================================================*/
/* MT6577 EIS                                                            */
/*=======================================================================*/
static struct resource mt_resource_eis[] = {
    [0] = { // EIS configuration
        .start = EIS_BASE,
        .end   = EIS_BASE + 0x2C,
        .flags = IORESOURCE_MEM,
    }
};
static u64 mt_eis_dmamask = ~(u32) 0;
//
static struct platform_device mt_eis_dev = {
	.name		   = "camera-eis",
	.id		       = 0,
	.num_resources = ARRAY_SIZE(mt_resource_eis),
	.resource	   = mt_resource_eis,
	.dev           = {
		.dma_mask  = &mt_eis_dmamask,
		.coherent_dma_mask = 0xffffffffUL
	}
};

//
/*=======================================================================*/
/* Image sensor                                                        */
/*=======================================================================*/
static struct platform_device sensor_dev = {
	.name		  = "image_sensor",
	.id		  = -1,
};
//
/*=======================================================================*/
/* Lens actuator                                                        */
/*=======================================================================*/
static struct platform_device actuator_dev = {
	.name		  = "lens_actuator",
	.id		  = -1,
};
/*=======================================================================*/
/* MT6577 jogball                                                        */
/*=======================================================================*/
#ifdef CONFIG_MOUSE_PANASONIC_EVQWJN
static struct platform_device jbd_pdev = {
	.name = "mt6577-jb",
	.id = -1,
};
#endif

#if defined(CONFIG_BT_BRCM_SLEEP)
#define MT_WAKE_BT_IO				84
#define BT_WAKE_MT_IO				76
#define BT_WAKE_MT_IRQ				1
static struct resource bluesleep_resources[] = {
	{
		.name	= "gpio_host_wake",
		.start	= BT_WAKE_MT_IO,
		.end	= BT_WAKE_MT_IO,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "gpio_ext_wake",
		.start	= MT_WAKE_BT_IO,
		.end	= MT_WAKE_BT_IO,
		.flags	= IORESOURCE_IO,
	},
	{
		.name	= "host_wake",
		.start	= BT_WAKE_MT_IRQ,
		.end	= BT_WAKE_MT_IRQ,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device brcm_bluesleep_device = {
	.name = "bluesleep",
	.id		= -1,
	.num_resources	= ARRAY_SIZE(bluesleep_resources),
	.resource	= bluesleep_resources,
};
#endif //CONFIG_BT_BRCM_SLEEP
/*=======================================================================*/
/* MT6577 Resource Manager                                                         */
/*=======================================================================*/
static struct platform_device res_mgr_dev = {
	.name	= "camera-resmgr",
	.id     = -1,
};

static struct platform_device mt65xx_leds_device = {
	.name = "leds-mt65xx",
	.id = -1
};

/*=======================================================================*/
/* MT6577 Board Device Initialization                                    */
/*=======================================================================*/
__init int mt6577_board_init(void)
{
    int i = 0, retval = 0;

#if defined(MTK_DT_SUPPORT) && (!defined(EVDO_DT_SUPPORT))
    // Note: MUST init before USB and UART driver if this driver enable
    for (i = 0; i < ARRAY_SIZE(emd_chr_devs); i++){
        retval = platform_device_register(&emd_chr_devs[i]);
        if (retval != 0){
            printk("[emd] Regsiter emd_char_device %d failed\n", i);
            return retval;
        }
    }
#endif

#if defined(CONFIG_MTK_SERIAL)
    for (i = 0; i < ARRAY_SIZE(mtk_device_uart); i++){
        retval = platform_device_register(&mtk_device_uart[i]);
        if (retval != 0){
            return retval;
        }
    }
#endif

#if 0
#ifdef CONFIG_FIQ_DEBUGGER
	retval = platform_device_register(&mt_fiq_debugger);
	if (retval != 0){
		return retval;
	}
#endif
#endif

	{
		uint64_t key;
#if defined(CONFIG_MTK_USB_UNIQUE_SERIAL)
		if((g_boot_mode==META_BOOT)||(g_boot_mode==ATE_FACTORY_BOOT)||(g_boot_mode==ADVMETA_BOOT))
			key=0;
		else{
			key = (uint32_t)__raw_readl(EFUSE_CTR_BASE + 0x144);
			key = (key << 32) | ((uint32_t)__raw_readl(EFUSE_CTR_BASE + 0x140));
		}
#else
		key = 0;
#endif
		if (key != 0)
			get_serial(key, 6577, serial_number);
		else
			memcpy(serial_number, "0123456789ABCDEF", 16);

		retval = kobject_init_and_add(&sn_kobj, &sn_ktype, NULL, "sys_info");

		if (retval < 0)
			printk("[%s] fail to add kobject\n", "sys_info");
	}

#if defined(CONFIG_MTK_MTD_NAND)
    retval = platform_device_register(&mt6577_nand_dev);
    if (retval != 0) {
        printk(KERN_ERR "register nand device fail\n");
        return retval;
    }
#endif
//		printk(KERN_ALERT"[GPIO]%5d<%s> Register gpio devices\n", __LINE__, __func__);
	retval = platform_device_register(&gpio_dev);
	if (retval != 0){
		return retval;
	}

#if defined(CONFIG_KEYBOARD_HID)
	retval = platform_device_register(&mt_hid_dev);
	if (retval != 0){
		return retval;
	}	
#endif

#if defined(CONFIG_MTK_I2C)
//	i2c_register_board_info(0, i2c_devs0, ARRAY_SIZE(i2c_devs0));
	i2c_register_board_info(1, i2c_devs1, ARRAY_SIZE(i2c_devs1));
//	i2c_register_board_info(2, i2c_devs2, ARRAY_SIZE(i2c_devs2));
		for (i = 0; i < ARRAY_SIZE(mt_device_i2c); i++){
			retval = platform_device_register(&mt_device_i2c[i]);
			if (retval != 0){
				return retval;
			}
		}
#endif
#if defined(CONFIG_MTK_MMC)
    for (i = 0; i < ARRAY_SIZE(mt6577_device_sd); i++){
        retval = platform_device_register(&mt6577_device_sd[i]);
			if (retval != 0){
				printk("[%s]: failed to register mt6577_device_sd[%d]\n", __func__, i);
				return retval;
			}
			printk("[%s]: successed to register mt6577_device_sd[%d]\n", __func__, i);
		}
#endif

#if defined(CONFIG_MTK_SOUND)
    retval = platform_device_register(&AudDrv_device);
    printk("AudDrv_driver_device \n!");
    if (retval != 0){
       return retval;
    }
#endif

#if defined(CONFIG_MTK_FB)
    /* 
     * Bypass matching the frame buffer info. between boot loader and kernel 
     * if the limited memory size of the kernel is smaller than the 
     * memory size from bootloader
     */
    if (((bl_fb.base == FB_START) && (bl_fb.size == FB_SIZE)) || 
         (use_bl_fb == 2)) {
        printk("FB is initialized by BL(%d)\n", use_bl_fb);
        mtkfb_set_lcm_inited(1);
    } else if ((bl_fb.base == 0) && (bl_fb.size == 0)) {
        printk("FB is not initialized(%d)\n", use_bl_fb);
        mtkfb_set_lcm_inited(0);
    } else {
        printk(
"******************************************************************************\n"
"   WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING\n"
"******************************************************************************\n"
"\n"
"  The default FB base & size values are not matched between BL and kernel\n"
"    - BOOTLD: start 0x%08x, size %d\n"
"    - KERNEL: start 0x%08lx, size %d\n"
"\n"
"  If you see this warning message, please update your uboot.\n"
"\n"
"******************************************************************************\n"
"   WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING WARNING\n"
"******************************************************************************\n"
"\n",   bl_fb.base, bl_fb.size, FB_START, FB_SIZE);

        {
            int delay_sec = 5;
            
            while (delay_sec >= 0) {
                printk("\rcontinue after %d seconds ...", delay_sec--);
                mdelay(1000);
            }
            printk("\n");
        }
#if 0
        panic("The default base & size values are not matched "
              "between BL and kernel\n");
#endif
    }

    resource_fb[0].start = FB_START;
    resource_fb[0].end   = FB_START + FB_SIZE - 1;

    printk("FB start: 0x%x end: 0x%x\n", resource_fb[0].start,
                                         resource_fb[0].end);

    retval = platform_device_register(&mt6577_device_fb);
    if (retval != 0) {
         return retval;
    }
#endif

#if defined(CONFIG_MTK_LEDS)        
	retval = platform_device_register(&mt65xx_leds_device);	
	if (retval != 0)		
		return retval;
	printk("bei:device LEDS register\n");
#endif
#ifdef MTK_HDMI_SUPPORT
	retval = platform_device_register(&mtk_hdmi_dev);
	if (retval != 0){
		return retval;
	}

#endif

#if defined(CONFIG_MTK_SPI)
//    spi_register_board_info(spi_board_devs, ARRAY_SIZE(spi_board_devs));
    platform_device_register(&mt_spi_device);
#endif


    retval = platform_device_register(&mtk_mau_dev);
    printk("register MTK_MAU device\n");
    if (retval != 0) {
        return retval;
    }

#if defined(MTK_TVOUT_SUPPORT)
    retval = platform_device_register(&mt6577_TVOUT_dev);
	printk("register TV-out device\n");
    if (retval != 0) {
         return retval;
    }
#endif


#if defined(CONFIG_MTK_ACCDET)


    retval = platform_device_register(&accdet_device);
	printk("register accdet device\n");

	if (retval != 0) 
	{
		printk("platform_device_accdet_register error:(%d)\n", retval);	
		return retval;
	}
	else
	{
		printk("platform_device_accdet_register done!\n");
	}

#endif

#if 1 //defined(CONFIG_MDP_MT6577)
    //printk("[MDP]platform_device_register\n\r");
    retval = platform_device_register(&mt6577_MDP_dev);
    if(retval != 0){
        return retval;
    }
#endif

#if 1
  retval = platform_device_register(&MT6577_auxadc_device);
  if(retval != 0)
  {
     printk("****[mt6577_auxadc_driver] Unable to device register(%d)\n", retval);
	 return retval;
  }
#endif



#if defined(MTK_SENSOR_SUPPORT)

	retval = platform_device_register(&hwmon_sensor);
	printk("hwmon_sensor device!");
	if (retval != 0)
		return retval;

#if defined(CUSTOM_KERNEL_ACCELEROMETER)
	retval = platform_device_register(&sensor_gsensor);
		printk("sensor_gsensor device!");
	if (retval != 0)
		return retval;
#endif

#if defined(CUSTOM_KERNEL_MAGNETOMETER)
	retval = platform_device_register(&sensor_msensor);
		printk("sensor_msensor device!");
	if (retval != 0)
		return retval;
		
	retval = platform_device_register(&sensor_orientation);
		printk("sensor_osensor device!");
	if (retval != 0)
		return retval;
		
#endif

#if defined(CUSTOM_KERNEL_GYROSCOPE)
	retval = platform_device_register(&sensor_gyroscope);
		printk("sensor_gyroscope device!");
	if (retval != 0)
		return retval;
#endif

#if defined(CUSTOM_KERNEL_BAROMETER)
	retval = platform_device_register(&sensor_barometer);
		printk("sensor_barometer device!");
	if (retval != 0)
		return retval;
#endif

#if defined(CUSTOM_KERNEL_ALSPS)
	retval = platform_device_register(&sensor_alsps);
		printk("sensor_alsps device!");
	if (retval != 0)
		return retval;
#endif
#endif

#if defined(CONFIG_MTK_USBFSH)    
	printk("register musbfsh device\n");    
	retval = platform_device_register(&mt_usb11_dev);
	if (retval != 0){
		printk("register musbfsh device fail!\n"); 
		return retval;    
	}
#endif

#if defined(CONFIG_USB_MTK_HDRC)
	printk("mt_device_usb register\n");
	retval = platform_device_register(&mt_device_usb);
	if (retval != 0){
	printk("mt_device_usb register fail\n");
        return retval;
	}	
#endif

#ifdef CONFIG_MTK_KEYPAD
	retval = platform_device_register(&kpd_pdev);
	if (retval != 0) {
		return retval;
	}
#endif
#ifdef CONFIG_MOUSE_PANASONIC_EVQWJN
	retval = platform_device_register(&jbd_pdev);
	if (retval != 0) {
		return retval;
	}
#endif
#if defined(CONFIG_MTK_TOUCHPANEL)
    retval = platform_device_register(&mtk_tpd_dev);
    if (retval != 0) {
        return retval;
    }
#endif
#if defined(CONFIG_BT_BRCM_SLEEP)
//[WJ ADD]
	retval = platform_device_register(&brcm_bluesleep_device);
	if (retval != 0){
		return retval;
	}
#endif //CONFIG_BT_BRCM_SLEEP

#if defined(CUSTOM_KERNEL_OFN)
    retval = platform_device_register(&ofn_driver);
    if (retval != 0){
        return retval;
    }
#endif

#if (defined(CONFIG_MTK_MTD_NAND) ||defined(CONFIG_MTK_MMC))
retval = platform_device_register(&dummychar_device);
	if (retval != 0){
		return retval;
	}
#endif 


#if defined(CONFIG_ANDROID_PMEM)
    pdata_multimedia.start = PMEM_MM_START;;
    pdata_multimedia.size = PMEM_MM_SIZE;
    printk("PMEM start: 0x%lx size: 0x%lx\n", pdata_multimedia.start, pdata_multimedia.size);

    retval = platform_device_register(&pmem_multimedia_device);
    if (retval != 0){
       return retval;
    }
#endif

#if defined(CONFIG_ANDROID_VMEM)
    pdata_vmultimedia.start = PMEM_MM_START;;
    pdata_vmultimedia.size = PMEM_MM_SIZE;
    printk("VMEM start: 0x%lx size: 0x%lx\n", pdata_vmultimedia.start, pdata_vmultimedia.size);

    retval = platform_device_register(&vmem_multimedia_device);
    if (retval != 0){
	printk("vmem platform register failed\n");
       return retval;
    }
#endif

#ifdef CONFIG_CPU_FREQ
    retval = platform_device_register(&cpufreq_pdev);
    if (retval != 0) {
        return retval;
    }
#endif

#if 1
    retval = platform_device_register(&thermal_pdev);
    if (retval != 0) {
        return retval;
    }
#endif

//
//=======================================================================
// Image sensor
//=======================================================================
#if 1 ///defined(CONFIG_VIDEO_CAPTURE_DRIVERS)
    retval = platform_device_register(&sensor_dev);
    if (retval != 0){
    	return retval;
    }
#endif
//
//=======================================================================
// Lens motor
//=======================================================================
#if 1  //defined(CONFIG_ACTUATOR)
    retval = platform_device_register(&actuator_dev);
    if (retval != 0){
        return retval;
    }
#endif
//
#if 1 //defined(CONFIG_ISP_MT6577)
    retval = platform_device_register(&mt_isp_dev);
    if (retval != 0){
        return retval;
    }
#endif

#if 1
    retval = platform_device_register(&mt_eis_dev);
    if (retval != 0){
        return retval;
    }
#endif

#ifdef CONFIG_RFKILL
    retval = platform_device_register(&mt6577_device_rfkill);
    if (retval != 0){
        return retval;
    }
#endif

#if 1
	retval = platform_device_register(&mt6577_SYSRAM_dev);
	if (retval != 0){
		return retval;
	}
#endif

#if defined(CONFIG_MTK_GPS)
	retval = platform_device_register(&mt3326_device_gps);
	if (retval != 0){
		return retval;
	}	
#endif

	retval = platform_device_register(&res_mgr_dev);
	if (retval != 0){
		return retval;
	}

    return 0;
}

/*
 * is_pmem_range
 * Input
 *   base: buffer base physical address
 *   size: buffer len in byte
 * Return
 *   1: buffer is located in pmem address range
 *   0: buffer is out of pmem address range
 */
int is_pmem_range(unsigned long *base, unsigned long size)
{
        unsigned long start = (unsigned long)base;
        unsigned long end = start + size;

        //printk("[PMEM] start=0x%p,end=0x%p,size=%d\n", start, end, size);
        //printk("[PMEM] PMEM_MM_START=0x%p,PMEM_MM_SIZE=%d\n", PMEM_MM_START, PMEM_MM_SIZE);

        if (start < PMEM_MM_START)
                return 0;
        if (end >= PMEM_MM_START + PMEM_MM_SIZE)
                return 0;

        return 1;
}
EXPORT_SYMBOL(is_pmem_range);


unsigned int get_memory_size (void)
{
    printk ("total size :0x%x-0x%x-0x%x\n", 
            (unsigned int)(MAX_PFN + TOTAL_RESERVED_MEM_SIZE),
            TOTAL_RESERVED_MEM_SIZE, RESERVED_MEM_MODEM) ;

    return (MAX_PFN) - (RESERVED_MEM_MODEM);
}
EXPORT_SYMBOL(get_memory_size) ;


unsigned int get_max_DRAM_size(void)
{
	return  MAX_PFN + TOTAL_RESERVED_MEM_SIZE;
}
EXPORT_SYMBOL(get_max_DRAM_size);


unsigned int get_phys_offset(void)
{
	return PHYS_OFFSET;
}
EXPORT_SYMBOL(get_phys_offset);


#include <asm/sections.h>
void get_text_region (unsigned int *s, unsigned int *e)
{
    *s = (unsigned int)_text, *e=(unsigned int)_etext ;
}
EXPORT_SYMBOL(get_text_region) ;
