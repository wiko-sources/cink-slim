#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/mm.h>
#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/irq.h>
#include <linux/sched.h>
#include <linux/list.h>
#ifdef CONFIG_MTK_AEE_FEATURE
#include <linux/aee.h>
#endif  

#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_emi_mpu.h>
#include <mach/mt6577_device_apc.h>
#include <mach/mtk_mau.h>
#include <mach/sync_write.h>
#include <mach/irqs.h>
#include <mach/dma.h>

#define NR_REGION_ABORT 8
#define MAX_EMI_MPU_STORE_CMD_LEN 128
#define ABORT_EMI 0x40000008

struct dma_addr{
    u32 start_address;
    u32 size;
    u8 end;
    struct dma_addr *next; 
};
 
int msdc_get_dma_status(int host_id);
struct dma_addr*  msdc_get_dma_address(int host_id);
u8 USB_DMA_status(u8 *pbDMAen, u8 *pbDMAdir);
u32 USB_DMA_address(u32 *len, u8 bchannel);
u8 NFI_DMA_status(void);
u32 NFI_DMA_address(void);

struct mst_tbl_entry
{
    u32 master;
    u32 id_mask;
    u32 id_val;
    char *name;
};

struct emi_mpu_notifier_block
{
    struct list_head list;
    emi_mpu_notifier notifier; 
};

static const struct mst_tbl_entry mst_tbl[] =
{
    /* apmcu */
    { .master = MST_ID_APMCU, .id_mask = 0x7, .id_val = 0x0, .name = "APMCU" },

    /* peripheral */
    { .master = MST_ID_AHB0, .id_mask = 0x1F, .id_val = 0x2, .name = "AHB0(USB1.1,USB2.0,NFI,MSDC0" },
    { .master = MST_ID_AHB1, .id_mask = 0x1F, .id_val = 0xA, .name = "AHB1(THERM,PWM,MSDC3,MSDC2" },
    { .master = MST_ID_SDXC, .id_mask = 0x1F, .id_val = 0x12, .name = "SDXC(MSDC1)" },
    { .master = MST_ID_DMA, .id_mask = 0x1F, .id_val = 0x1A, .name = "DMA" },

    /* Modem (ARM2EMI) */
    { .master = MST_ID_ARM_I, .id_mask = 0x1F, .id_val = 0x3, .name = "MD:ARM-I" },
    { .master = MST_ID_ARM_RW, .id_mask = 0x1F, .id_val = 0xB, .name = "MD:ARM-RW" },
    { .master = MST_ID_ARM_DMA, .id_mask = 0x1F, .id_val = 0x13, .name = "MD:ARM-DMA" },

    /* Modem (FCORE2EMI) */
    { .master = MST_ID_FCORE_I, .id_mask = 0x7F, .id_val = 0x4, .name = "FCORE-I" },
    { .master = MST_ID_FCORE_D0, .id_mask = 0x7F, .id_val = 0x2C, .name = "FCORE-D0" },
    { .master = MST_ID_FCORE_D1, .id_mask = 0x7F, .id_val = 0x54, .name = "FCORE-D1" },
    { .master = MST_ID_DSP_DMA, .id_mask = 0x7F, .id_val = 0x1C, .name = "DSP-DMA" },

    /* Modem (MD2EMI) */
    { .master = MST_ID_MDPERI_PFC, .id_mask = 0x7F, .id_val = 0x5, .name = "MDPERI-PFC" },
    { .master = MST_ID_MDPERI_AHB2AXI, .id_mask = 0x7F, .id_val = 0x15, .name = "MDPERI-AHB2AXI" },
    { .master = MST_ID_MDPERI_P_DMA, .id_mask = 0x7F, .id_val = 0x25, .name = "MDPERI-P_DMA" },
    { .master = MST_ID_DEBUG, .id_mask = 0x7F, .id_val = 0xD, .name = "MD:Debug" },
    { .master = MST_ID_MD2G_AHB0, .id_mask = 0x7F, .id_val = 0x1D, .name = "MD2G-AHB0" },
    { .master = MST_ID_MD2G_AHB1, .id_mask = 0x7F, .id_val = 0x5D, .name = "MD2G-AHB1" },
    { .master = MST_ID_HSPA, .id_mask = 0x2D, .id_val = 0x2D, .name = "HSPA" },

    /* multimedia */
    { .master = MST_ID_LARB0, .id_mask = 0x6FF, .id_val = 0x60E, .name = "LARB0" },
    { .master = MST_ID_LARB1, .id_mask = 0x6FF, .id_val = 0x416, .name = "LARB1" },
    { .master = MST_ID_LARB2, .id_mask = 0x6FF, .id_val = 0x21E, .name = "LARB2" },
    { .master = MST_ID_LARB3_PA, .id_mask = 0x6FF, .id_val = 0x16, .name = "LARB3-PA" },
    { .master = MST_ID_3D0, .id_mask = 0x86, .id_val = 0x86, .name = "3D-0" },
    { .master = MST_ID_3D1, .id_mask = 0x106, .id_val = 0x106, .name = "3D-1" },
    { .master = MST_ID_LARB3_VA, .id_mask = 0x186, .id_val = 0x186, .name = "LARB3-VA" },
    { .master = MST_ID_LARB3_TLB, .id_mask = 0x1C6, .id_val = 0x1C6, .name = "LARB3-TLB" },
};
struct list_head emi_mpu_notifier_list[NR_MST];
static const char *UNKNOWN_MASTER = "unknown";
static spinlock_t emi_mpu_lock;

static int __match_id(u32 id, int tbl_idx)
{
    if ((id & mst_tbl[tbl_idx].id_mask) == mst_tbl[tbl_idx].id_val) {
        return 1;
    } else {
        return 0;
    }
}

static u32 __id2mst(u32 id)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(mst_tbl); i++) {
        if (__match_id(id, i)) {
            return mst_tbl[i].master;
        }
    }
    return MST_INVALID;
}

static char *__id2name(u32 id)
{
    int i;

    for (i = 0; i < ARRAY_SIZE(mst_tbl); i++) {
        if (__match_id(id, i)) {
            return mst_tbl[i].name;
        }
    }
    return (char *)UNKNOWN_MASTER;
}

static void __clear_emi_mpu_vio(void)
{
    u32 dbg_s, dbg_t;

    /* clear violation status */
    writel(0x000003FF, EMI_MPUP);
    writel(0x000003FF, EMI_MPUQ);
    writel(0x000003FF, EMI_MPUR);

    /* clear debug info */
    mt65xx_reg_sync_writel(0x80000000 , EMI_MPUS);
    dbg_s = readl(EMI_MPUS);
    dbg_t = readl(EMI_MPUT);
    if (dbg_s || dbg_t) {
        printk(KERN_CRIT "Fail to clear EMI MPU violation\n");
        printk(KERN_CRIT "EMI_MPUS = %x, EMI_MPUT = %x", dbg_s, dbg_t);
    }
}

static irqreturn_t mpu_violation_irq(int irq, void *dev_id)
{  
    u32 dbg_s, dbg_t, dbg_pqr;
    u32 master_ID, domain_ID, wr_vio;
    s32 region;
    int i, status;
    u8 usb_DMA_en, usb_DMA_dir;
    u32 usb_DMA_len, usb_DMA_addr;
    struct list_head *p;
    struct emi_mpu_notifier_block *block;
    struct dma_addr *msdc_dma_address, *p_msdc_dma_address;


    if (readl(AP_DEVAPC0_DXS_VIO_STA) != 0x2 ||(readl(AP_DEVAPC1_D0_VIO_STA) & ABORT_EMI) == 0) {
        printk(KERN_INFO "Not EMI MPU violation.\n");
        return IRQ_NONE;
    }

    dbg_s = readl(EMI_MPUS);
    dbg_t = readl(EMI_MPUT);

    master_ID = dbg_s & 0x000007FF;
    domain_ID = (dbg_s >> 12) & 0x00000003;
    wr_vio = (dbg_s >> 28) & 0x00000003;
    region = (dbg_s >> 16) & 0xFF;

    for (i = 0 ; i < NR_REGION_ABORT; i++) {
        if ((region >> i) & 1) {
            break;
        }
    }
    region = (i >= NR_REGION_ABORT)? -1: i;

    switch (domain_ID) {
    case 0:
        dbg_pqr = readl(EMI_MPUP);
        break;
    case 1:
        dbg_pqr = readl(EMI_MPUQ);
        break;
    case 2:
        dbg_pqr = readl(EMI_MPUR);
        break;
    default:
        dbg_pqr = 0;
        break;
    }

    printk(KERN_CRIT "EMI MPU violation.\n");
    printk(KERN_CRIT "[EMI MPU] Debug info start ----------------------------------------\n");

    printk(KERN_CRIT "EMI_MPUS = %x, EMI_MPUT = %x.\n", dbg_s, dbg_t);
    printk(KERN_CRIT "Current process is \"%s \" (pid: %i).\n", current->comm, current->pid);
    printk(KERN_CRIT "Violation address is 0x%x.\n", dbg_t);
    printk(KERN_CRIT "Violation master ID is 0x%x (%s).\n", master_ID, __id2name(master_ID));
    printk(KERN_CRIT "Violation domain ID is 0x%x.\n", domain_ID);
    printk(KERN_CRIT "%s violation.\n", (wr_vio == 1)? "Write": "Read");
    printk(KERN_CRIT "Region is %d\n\r", region);
    if (dbg_pqr & OOR_VIO) {
        printk(KERN_CRIT "Out of range violation.\n");
    }
    
    if(master_ID == 0x2 || master_ID == 0xA) {

#ifdef CONFIG_MTK_MTD_NAND
        /****************************************************/
        /*                NFI DMA Status                    */
        /****************************************************/
        printk(KERN_CRIT "NFI DMA running status:\n");
        status = NFI_DMA_status();
        if(status == 0)
            printk("Disable Now\n");
        else if (status == 1)
            printk("Write Data from NAND to DRAM \n");
        else if (status == 3)
            printk("Write Data from DRAM to NAND \n");
        printk(KERN_CRIT "NFI DMA Addr: %x\n", NFI_DMA_address());
#endif

        /****************************************************/
        /*                USB DMA Status                    */
        /****************************************************/
        printk(KERN_CRIT "USB DMA running status:\n");
        status = USB_DMA_status(&usb_DMA_en, &usb_DMA_dir);
        for(i = 0; i < 8; i++) {

            if(((usb_DMA_en >> i) & 0x1) == 1)
                printk("USB CH%d is enabled\n", i);
            else
                printk("USB CH%d is disabled\n", i);

            if(((usb_DMA_dir >> i) & 0x1) == 0)
                printk("Write data from USB to DRAM\n");
            else
                printk("Write data from DRAM to USB\n");
    
            usb_DMA_addr = USB_DMA_address(&usb_DMA_len, i);
            printk("USB CH%d DMA Addr = 0x%x, Len = 0x%x\n", i, usb_DMA_addr, usb_DMA_len);
            printk("**************************************\n");
        }
    
 
        /****************************************************/
        /*                MSDC DMA Status                    */
        /****************************************************/
        printk(KERN_CRIT "MSDC DMA running status:\n");
        for(i = 0; i < 4; i++) {

            status = msdc_get_dma_status(i);
            printk("MSDC%d: ", i);
            if(status == 0){
                printk("DMA mode is disabled Now\n");
            }else if (status == 1){
                printk("Write data from SD to DRAM within DMA mode\n");
            }else if (status == 2){
                printk("Write data from DRAM to SD within DMA mode\n");
            }else if (status == -1){
            	printk("No data transaction or the device is not present until now\n");
                continue;
            }
        
            msdc_dma_address =  msdc_get_dma_address(i); 
        
            if(msdc_dma_address){
                p_msdc_dma_address = msdc_dma_address;
                printk("MSDC%d: \n", i);
                while (p_msdc_dma_address != NULL){
                    printk("   addr=0x%x, size=%d\n", p_msdc_dma_address->start_address, p_msdc_dma_address->size);
                    if(p_msdc_dma_address->end)
                       break;
                    p_msdc_dma_address = p_msdc_dma_address->next;
               }
            }else {
                printk("MSDC%d: BD count=0\n", i);
            }
            printk("**************************************\n");
        }
    }

    printk(KERN_CRIT "MAU status:\n");
    MAU_get_port_with_m4u(dbg_t, dbg_t);

    printk(KERN_CRIT "[EMI MPU] Debug info end------------------------------------------\n");

#ifdef CONFIG_MTK_AEE_FEATURE
    aee_kernel_exception("EMI MPU", "EMI MPU violation.\nEMP_MPUS = 0x%x, EMI_MPUT = 0x%x, EMI_MPU(PQR).\n", dbg_s, dbg_t, dbg_pqr);
#endif

    __clear_emi_mpu_vio();

    writel(readl(AP_DEVAPC1_D0_VIO_STA) | ABORT_EMI , AP_DEVAPC1_D0_VIO_STA);
    while ((readl(AP_DEVAPC1_D0_VIO_STA) & ABORT_EMI) != 0);

    writel(0x2, AP_DEVAPC0_DXS_VIO_STA);
    while ((readl(AP_DEVAPC0_DXS_VIO_STA) & 0x2) != 0);

    list_for_each(p, &(emi_mpu_notifier_list[__id2mst(master_ID)])) {
        block = list_entry(p, struct emi_mpu_notifier_block, list);
        block->notifier(dbg_t, wr_vio);
    }

    return IRQ_HANDLED;
}

/*
 * emi_mpu_set_region_protection: protect a region.
 * @start: start address of the region
 * @end: end address of the region
 * @region: EMI MPU region id
 * @access_permission: EMI MPU access permission
 * Return 0 for success, otherwise negative status code.
 */
int emi_mpu_set_region_protection(unsigned int start, unsigned int end, int region, unsigned int access_permission)
{
    int ret = 0;
    unsigned int tmp;
    
    if((end != 0) || (start !=0)) {
        start = start >> 20;
        end = end >> 20;

        if (end <= start) {
            return -EINVAL;
        }
    }
	

    spin_lock(&emi_mpu_lock);

    switch (region) {
    case 0:
        writel((start << 16) | end, EMI_MPUA); 
		tmp = readl(EMI_MPUI) & 0xFFFF0000;
        writel(tmp | access_permission, EMI_MPUI);
        break; 

    case 1:
        writel((start << 16) | end, EMI_MPUB);
		tmp = readl(EMI_MPUI) & 0x0000FFFF;
        writel(tmp | (access_permission << 16), EMI_MPUI);
        break;

    case 2:
        writel((start << 16) | end, EMI_MPUC);
		tmp = readl(EMI_MPUJ) & 0xFFFF0000;
        writel(tmp | access_permission, EMI_MPUJ);
        break;

    case 3:
        writel((start << 16) | end, EMI_MPUD);
		tmp = readl(EMI_MPUJ) & 0x0000FFFF;
        writel(tmp | (access_permission << 16), EMI_MPUJ);
        break;        

    case 4:
        writel((start << 16) | end, EMI_MPUE);
		tmp = readl(EMI_MPUK) & 0xFFFF0000;
        writel(tmp | access_permission, EMI_MPUK);
        break;  

    case 5:
        writel((start << 16) | end, EMI_MPUF);
		tmp = readl(EMI_MPUK) & 0x0000FFFF;
        writel(tmp | (access_permission << 16), EMI_MPUK);
        break;

    case 6:
        writel((start << 16) | end, EMI_MPUG);
		tmp = readl(EMI_MPUL) & 0xFFFF0000;
        writel(tmp | access_permission, EMI_MPUL);
        break;

    case 7:
        writel((start << 16) | end, EMI_MPUH);
		tmp = readl(EMI_MPUL) & 0x0000FFFF;
        writel(tmp | (access_permission << 16), EMI_MPUL);
        break;

    default:
        ret = -EINVAL;
        break;
    }

    spin_unlock(&emi_mpu_lock);

    return ret;
}

/*
 * emi_mpu_notifier_register: register a notifier.
 * master: MST_ID_xxx
 * notifier: the callback function
 * Return 0 for success, otherwise negative error code.
 */
int emi_mpu_notifier_register(int master, emi_mpu_notifier notifier)
{
    struct emi_mpu_notifier_block *block;
    static int emi_mpu_notifier_init = 0;
    int i;

    if (master >= MST_INVALID) {
        return -EINVAL;
    }

    block = kmalloc(sizeof(struct emi_mpu_notifier_block), GFP_KERNEL);
    if (!block) {
        return -ENOMEM;
    }

    if (!emi_mpu_notifier_init) {
        for (i = 0; i < NR_MST; i++) {
            INIT_LIST_HEAD(&(emi_mpu_notifier_list[i]));
        }
        emi_mpu_notifier_init = 1;
    }

    block->notifier = notifier;
    list_add(&(block->list), &(emi_mpu_notifier_list[master]));

    return 0;
}

static ssize_t emi_mpu_show(struct device_driver *driver, char *buf)
{
    char *ptr = buf;
    unsigned int start, end;
    unsigned int reg_value;
    unsigned int d0, d1, d2;
    static const char *permission[6] = 
    { 
        "No protect",
        "Only R/W for secure access",
        "Only R/W for secure access, and non-secure read access",
        "Only R/W for secure access, and non-secure write access",
        "Only R for secure/non-secure",
        "Both R/W are forbidden" 
    };

    reg_value = readl(EMI_MPUA);
    start = ((reg_value >> 16) << 20);
    end = ((reg_value & 0xFFFF) << 20);
    ptr += sprintf(ptr, "Region 0 --> 0x%x to 0x%x\n", start, end);

    reg_value = readl(EMI_MPUB);
    start = ((reg_value >> 16) << 20);
    end = ((reg_value & 0xFFFF) << 20);
    ptr += sprintf(ptr, "Region 1 --> 0x%x to 0x%x\n", start, end);

    reg_value = readl(EMI_MPUC);
    start = ((reg_value >> 16) << 20);
    end = ((reg_value & 0xFFFF) << 20);
    ptr += sprintf(ptr, "Region 2 --> 0x%x to 0x%x\n", start, end);

    reg_value = readl(EMI_MPUD);
    start = ((reg_value >> 16) << 20);
    end = ((reg_value & 0xFFFF) << 20);
    ptr += sprintf(ptr, "Region 3 --> 0x%x to 0x%x\n", start, end);

    reg_value = readl(EMI_MPUE);
    start = ((reg_value >> 16) << 20);
    end = ((reg_value & 0xFFFF) << 20);
    ptr += sprintf(ptr, "Region 4 --> 0x%x to 0x%x\n", start, end);

    reg_value = readl(EMI_MPUF);
    start = ((reg_value >> 16) << 20);
    end = ((reg_value & 0xFFFF) << 20);
    ptr += sprintf(ptr, "Region 5 --> 0x%x to 0x%x\n", start, end);

    reg_value = readl(EMI_MPUG);
    start = ((reg_value >> 16) << 20);
    end = ((reg_value & 0xFFFF) << 20);
    ptr += sprintf(ptr, "Region 6 --> 0x%x to 0x%x\n", start, end);

    reg_value = readl(EMI_MPUH);
    start = ((reg_value >> 16) << 20);
    end = ((reg_value & 0xFFFF) << 20);
    ptr += sprintf(ptr, "Region 7 --> 0x%x to 0x%x\n", start, end);
    
    ptr += sprintf (ptr, "\n");

    reg_value = readl(EMI_MPUI);
    d0 = (reg_value & 0x7);
    d1 = (reg_value >> 3) & 0x7;
    d2 = (reg_value >> 6) & 0x7;
    ptr += sprintf(ptr, "Region 0 --> d0 = %s, d1 = %s, d2 = %s\n", permission[d0],  permission[d1],  permission[d2]);

    d0 = ((reg_value>>16) & 0x7);
    d1 = ((reg_value>>16) >> 3) & 0x7;
    d2 = ((reg_value>>16) >> 6) & 0x7;
    ptr += sprintf(ptr, "Region 1 --> d0 = %s, d1 = %s, d2 = %s\n", permission[d0],  permission[d1],  permission[d2]);

    reg_value = readl(EMI_MPUJ);
    d0 = (reg_value & 0x7);
    d1 = (reg_value >> 3) & 0x7;
    d2 = (reg_value >> 6) & 0x7;
    ptr += sprintf(ptr, "Region 2 --> d0 = %s, d1 = %s, d2 = %s\n", permission[d0],  permission[d1],  permission[d2]);

    d0 = ((reg_value>>16) & 0x7);
    d1 = ((reg_value>>16) >> 3) & 0x7;
    d2 = ((reg_value>>16) >> 6) & 0x7;
    ptr += sprintf(ptr, "Region 3 --> d0 = %s, d1 = %s, d2 = %s\n", permission[d0],  permission[d1],  permission[d2]);

    reg_value = readl(EMI_MPUK);
    d0 = (reg_value & 0x7);
    d1 = (reg_value >> 3) & 0x7;
    d2 = (reg_value >> 6) & 0x7;
    ptr += sprintf(ptr, "Region 4 --> d0 = %s, d1 = %s, d2 = %s\n", permission[d0],  permission[d1],  permission[d2]);

    d0 = ((reg_value>>16) & 0x7);
    d1 = ((reg_value>>16) >> 3) & 0x7;
    d2 = ((reg_value>>16) >> 6) & 0x7;
    ptr += sprintf(ptr, "Region 5 --> d0 = %s, d1 = %s, d2 = %s\n", permission[d0],  permission[d1],  permission[d2]);

    reg_value = readl(EMI_MPUL);
    d0 = (reg_value & 0x7);
    d1 = (reg_value >> 3) & 0x7;
    d2 = (reg_value >> 6) & 0x7;
    ptr += sprintf(ptr, "Region 6 --> d0 = %s, d1 = %s, d2 = %s\n", permission[d0],  permission[d1],  permission[d2]);

    d0 = ((reg_value>>16) & 0x7);
    d1 = ((reg_value>>16) >> 3) & 0x7;
    d2 = ((reg_value>>16) >> 6) & 0x7;
    ptr += sprintf(ptr, "Region 7 --> d0 = %s, d1 = %s, d2 = %s\n", permission[d0],  permission[d1],  permission[d2]);

    return strlen(buf);
}

static ssize_t emi_mpu_store(struct device_driver *driver, const char *buf, size_t count)
{
    int i;    
    unsigned int start_addr;
    unsigned int end_addr; 
    unsigned int region; 
    unsigned int access_permission;
    char *command;
    char *ptr;
    char *token [5];

    if ((strlen(buf) + 1) > MAX_EMI_MPU_STORE_CMD_LEN) {
        printk(KERN_CRIT "emi_mpu_store command overflow.");
        return count;
    }
    printk(KERN_CRIT "emi_mpu_store: %s\n", buf);

    command = kmalloc((size_t)MAX_EMI_MPU_STORE_CMD_LEN, GFP_KERNEL);
    if (!command) {
        return count;
    }
    strcpy(command, buf);
    ptr = (char *)buf;

    if (!strncmp(buf, EN_MPU_STR, strlen(EN_MPU_STR))) {
        i = 0;
        while (ptr != NULL) {
            ptr = strsep(&command, " ");
            token[i] = ptr;
            printk(KERN_DEBUG "token[%d] = %s\n", i, token[i]);
            i++;
        }
        for (i = 0; i < 5; i++) {
            printk(KERN_DEBUG "token[%d] = %s\n", i, token[i]);
        }

        start_addr = simple_strtoul(token[1], &token[1], 16);
        end_addr = simple_strtoul(token[2], &token[2], 16);
        region = simple_strtoul(token[3], &token[3], 16);
        access_permission = simple_strtoul(token[4], &token[4], 16);
        emi_mpu_set_region_protection(start_addr, end_addr, region, access_permission);
        printk(KERN_CRIT "Set EMI_MPU: start: 0x%x, end: 0x%x, region: %d, permission: 0x%x.\n", start_addr, end_addr, region, access_permission);
    } else if (!strncmp(buf, DIS_MPU_STR, strlen(DIS_MPU_STR))) {
        i = 0;
        while (ptr != NULL) {
            ptr = strsep (&command, " ");
            token[i] = ptr;
            printk(KERN_DEBUG "token[%d] = %s\n", i, token[i]);
            i++;
        }
        for (i = 0;i < 5; i++) {
            printk(KERN_DEBUG "token[%d] = %s\n", i, token[i]);
        }

        start_addr = simple_strtoul(token[1], &token[1], 16);
        end_addr = simple_strtoul(token[2], &token[2], 16);
        region = simple_strtoul(token[3], &token[3], 16);
        emi_mpu_set_region_protection(0x0, 0x0, region, SET_ACCESS_PERMISSON(NO_PRETECTION, NO_PRETECTION, NO_PRETECTION));
        printk("set EMI MPU: start: 0x%x, end: 0x%x, region: %d, permission: 0x%x\n", 0, 0, region, SET_ACCESS_PERMISSON(NO_PRETECTION, NO_PRETECTION, NO_PRETECTION));
    } else {
        printk(KERN_CRIT "Unkown emi_mpu command.\n");
    }

    kfree(command);

    return count;
}

DRIVER_ATTR(mpu_config, 0644, emi_mpu_show, emi_mpu_store);

static struct device_driver emi_mpu_ctrl =
{
    .name = "emi_mpu_ctrl",
    .bus = &platform_bus_type,
    .owner = THIS_MODULE,
};

static int __init emi_mpu_mod_init(void)
{
    int ret;

    printk(KERN_INFO "Initialize EMI MPU.\n");
  
    spin_lock_init(&emi_mpu_lock);

    __clear_emi_mpu_vio();

    writel(readl(AP_DEVAPC0_APC_CON) & (0xFFFFFFFF ^ (1<<2)), AP_DEVAPC0_APC_CON);
    writel(readl(AP_DEVAPC1_APC_CON) & (0xFFFFFFFF ^ (1<<2)), AP_DEVAPC1_APC_CON);
    writel(readl(MM_DEVAPC0_APC_CON) & (0xFFFFFFFF ^ (1<<2)), MM_DEVAPC0_APC_CON);
    writel(readl(MM_DEVAPC1_APC_CON) & (0xFFFFFFFF ^ (1<<2)), MM_DEVAPC1_APC_CON);
    writel(0x0000007F, AP_DEVAPC0_DXS_VIO_STA);
    writel(0x00FF00FD, AP_DEVAPC0_DXS_VIO_MASK);
    writel(readl(AP_DEVAPC1_D0_VIO_STA) | ABORT_EMI , AP_DEVAPC1_D0_VIO_STA);
    writel(readl(AP_DEVAPC1_D0_VIO_MASK) & ~ABORT_EMI , AP_DEVAPC1_D0_VIO_MASK);
    writel(0x2, AP_DEVAPC0_DXS_VIO_STA);    
  
    /* 
     * NoteXXX: Interrupts of vilation (including SPC in SMI, or EMI MPU) 
     *          are triggered by the device APC.
     *          Need to share the interrupt with the SPC driver. 
     */
    ret = request_irq(MT6577_APARM_DOMAIN_IRQ_ID, (irq_handler_t)mpu_violation_irq, IRQF_TRIGGER_LOW | IRQF_SHARED, "mt6577_emi_mpu", &emi_mpu_ctrl); 
    if (ret != 0) {
        printk(KERN_CRIT "Fail to request EMI_MPU interrupt. Error = %d.\n", ret);
        return ret;
    }
  
#if !defined(USER_BUILD_KERNEL)
    /* register driver and create sysfs files */
    ret = driver_register(&emi_mpu_ctrl);
    if (ret) {
        printk(KERN_CRIT "Fail to register EMI_MPU driver.\n");
    }
    ret = driver_create_file(&emi_mpu_ctrl, &driver_attr_mpu_config);
    if (ret) {
        printk(KERN_CRIT "Fail to create EMI_MPU sysfs file.\n");
    }
#endif

    /* if using 6GB DRAM, memory access at address 6GB~8GB is not allowed */
    if ((*(volatile u32 *)0xF0003000 == 0x0002302E) ||  /* 6GB LPDDR */ 
        (*(volatile u32 *)0xF0003000 == 0x0002311A)) {    /* 6GB LPDDR2 */
        emi_mpu_set_region_protection(0x30000000, 0x40000000, 4, SET_ACCESS_PERMISSON(FORBIDDEN, FORBIDDEN, FORBIDDEN));
        printk(KERN_CRIT "Set protection for 6GB DRAM.\n");
    } 

    return 0;
}

static void __exit emi_mpu_mod_exit(void)
{
}

void start_mm_mau_protect(unsigned int mau_entry_start, unsigned int mau_entry_end, unsigned int entry_id)
{  
    /* MAU Entry configuration */
    MTK_MAU_CONFIG mauConfig = {0};

    mauConfig.EntryID = entry_id;
    mauConfig.Enable = true;
    mauConfig.Mode = 0;
    mauConfig.InvalidMasterLARB0 = 0xffffffff;
    mauConfig.InvalidMasterLARB1 = 0xffffffff;
    mauConfig.InvalidMasterLARB2 = 0xffffffff;
    mauConfig.InvalidMasterLARB3 = 0xffffffff;
    mauConfig.ReadEn = true;
    mauConfig.WriteEn = true;
    mauConfig.StartAddr = mau_entry_start;
    mauConfig.EndAddr = mau_entry_end;     
    MAU_Config(&mauConfig);
}

module_init(emi_mpu_mod_init);
module_exit(emi_mpu_mod_exit);

EXPORT_SYMBOL(emi_mpu_set_region_protection);
EXPORT_SYMBOL(start_mm_mau_protect);
