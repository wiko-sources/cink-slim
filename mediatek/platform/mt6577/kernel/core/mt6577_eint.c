#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/wakelock.h>
#include <linux/module.h>
#include <asm/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <cust_eint.h>

#include "mach/mt6577_reg_base.h"
#include "mach/eint.h"
#include "mach/irqs.h"
#include "mach/sync_write.h"

#define EINT_DEBUG   0
#if(EINT_DEBUG == 1)
#define dbgmsg printk
#else
#define dbgmsg(...)
#endif

/*
 * Define internal data structures.
 */

typedef struct 
{
    void (*eint_func[EINT_MAX_CHANNEL])(void);
    unsigned int eint_auto_umask[EINT_MAX_CHANNEL];
} eint_func;

typedef enum tagDEINT_GROUP{
    DEINT_GROUP0 = 0, DEINT_GROUP1, DEINT_GROUP2, DEINT_GROUP3,
    DEINT_GROUP4, DEINT_GROUP5, DEINT_GROUP6, DEINT_GROUP7,
    DEINT_GROUP_MAX
} deint_group;

/*
 * Define global variables.
 */

static const unsigned int EINT_IRQ = MT6577_EINT_IRQ_ID;
static const unsigned int EINT_DEB_CNT_MASK = 0x000007FF;
static const unsigned int EINT_DEB_PRESCALER_MASK = 0x00007000;
static const unsigned int EINT_DEB_PRESCALER_SHIFT = 12;
static const unsigned int EINT_DEB_EN_BIT = 0x00008000;
static const unsigned int EINT_POL_BIT = 0x00000800;
static const unsigned int MAX_MS_FOR_PRESCALER_0 = 64;
static const unsigned int MAX_MS_FOR_PRESCALER_1 = 127;
static const unsigned int MAX_MS_FOR_PRESCALER_2 = 255;
static const unsigned int MAX_MS_FOR_PRESCALER_3 = 511;
static const unsigned int MAX_MS_FOR_PRESCALER_4 = 1023;
static const unsigned int MAX_MS_FOR_PRESCALER_5 = 2047;
static const unsigned int MAX_MS_FOR_PRESCALER_6 = 4095;
static const unsigned int MAX_MS_FOR_PRESCALER_7 = 8000;
static const unsigned int EINT_PRESCALER_SHIFT = 12;
static eint_func EINT_FUNC;
struct wake_lock EINT_suspend_lock;
static unsigned int cur_eint_num;
static struct device_driver mt6577_eint_drv =
{
    .name = "eint",
    .bus = &platform_bus_type,
    .owner = THIS_MODULE,
};
#if 0
static eint_func DEINT_FUNC;
static const deint_group deint_group_id[DEINT_GROUP_MAX] = {
    DEINT_GROUP0, DEINT_GROUP1, DEINT_GROUP2, DEINT_GROUP3,
    DEINT_GROUP4, DEINT_GROUP5, DEINT_GROUP6, DEINT_GROUP7
};
#endif

/*
 * mt65xx_eint_mask: Mask the specified EINT number.
 * @eint_num: EINT number to mask
 */
void mt65xx_eint_mask(unsigned int eint_num)
{
    mt65xx_reg_sync_writel(1 << eint_num, EINT_MASK_SET);
}

/*
 * mt65xx_deint_mask: Mask the specified direct EINT number.
 * @eint_num: EINT number to mask
 */
void mt65xx_deint_mask(unsigned int eint_num)
{
    mt65xx_reg_sync_writel(1 << eint_num, EINT_DMASK_SET);
}

/*
 * mt65xx_eint_unmask: Unmask the specified EINT number.
 * @eint_num: EINT number to unmask
 */
void mt65xx_eint_unmask(unsigned int eint_num)
{
    mt65xx_reg_sync_writel(1 << eint_num, EINT_MASK_CLR);
}

/*
 * mt65xx_deint_unmask: Unmask the specified EINT number.
 * @eint_num: EINT number to unmask
 */
void mt65xx_deint_unmask(unsigned int eint_num)
{
    mt65xx_reg_sync_writel(1 << eint_num, EINT_DMASK_CLR);
}

/*
 * mt65xx_eint_set_hw_debounce: Set the hardware de-bounce time for the specified EINT number.
 * @eint_num: EINT number to acknowledge
 * @ms: the de-bounce time to set (in miliseconds)
 */
void mt65xx_eint_set_hw_debounce(unsigned int eint_num, unsigned int ms)
{
    unsigned int cnt, prescaler;
 
    if (ms == 0) {
        cnt = 1;
        prescaler = 0;
    } else if (ms < MAX_MS_FOR_PRESCALER_0) {
        /* calculate cnt value based on 32KHz clock cycles */
        cnt = ms << 5;
        prescaler = 0;
    } else if (ms < MAX_MS_FOR_PRESCALER_1) {
        /* calculate cnt value based on 16KHz clock cycles */
        cnt = ms << 4;
        prescaler = 1;
    } else if (ms < MAX_MS_FOR_PRESCALER_2) {
        /* calculate cnt value based on 8KHz clock cycles */
        cnt = ms << 3;
        prescaler = 2;
    } else if (ms < MAX_MS_FOR_PRESCALER_3) {
        /* calculate cnt value based on 4KHz clock cycles */
        cnt = ms << 2;
        prescaler = 3;
    } else if (ms < MAX_MS_FOR_PRESCALER_4) {
        /* calculate cnt value based on 2KHz clock cycles */
        cnt = ms << 1;
        prescaler = 4;
    } else if (ms < MAX_MS_FOR_PRESCALER_5) {
        /* calculate cnt value based on 1KHz clock cycles */
        cnt = ms;
        prescaler = 5;
    } else if (ms < MAX_MS_FOR_PRESCALER_6) {
        /* calculate cnt value based on 512Hz clock cycles */
        cnt = ms >> 1;
        prescaler = 6;
    } else if (ms < MAX_MS_FOR_PRESCALER_7) {
        /* calculate cnt value based on 256Hz clock cycles */
        cnt = ms >> 2;
        prescaler = 7;
    } else {
        ms = MAX_MS_FOR_PRESCALER_7;
        /* calculate cnt value based on 256Hz clock cycles */
        cnt = ms >> 2;
        prescaler = 7;
    }
    cnt &= EINT_DEB_CNT_MASK;

    /* setp 1: mask the EINT */
    writel(1 << eint_num, EINT_MASK_SET);

    /* step 2: disable debounce */
    mt65xx_reg_sync_writel(readl(EINT_CON(eint_num)) & ~EINT_DEB_EN_BIT, EINT_CON(eint_num));

    /* step 3: delay at least 5 32k cycles */
    udelay(157);

    /* step 4: set new debounce value */
    writel((readl(EINT_CON(eint_num)) & ~(EINT_DEB_PRESCALER_MASK | EINT_DEB_CNT_MASK)) | (prescaler << EINT_PRESCALER_SHIFT) | (cnt | EINT_DEB_EN_BIT), EINT_CON(eint_num));

    /* step 5: unmask the EINT */
    mt65xx_reg_sync_writel(1 << eint_num, EINT_MASK_CLR);
}

/*
 * mt65xx_eint_set_polarity: Set the polarity for the EINT number.
 * @eint_num: EINT number to set
 * @pol: polarity to set
 */
void mt65xx_eint_set_polarity(unsigned int eint_num, unsigned int pol)
{
    unsigned int count;
 
    if (pol == MT65XX_EINT_POL_NEG) {
        mt65xx_reg_sync_writel(readl(EINT_CON(eint_num)) & ~EINT_POL_BIT, EINT_CON(eint_num));
    } else {
        mt65xx_reg_sync_writel(readl(EINT_CON(eint_num)) | EINT_POL_BIT, EINT_CON(eint_num));
    }

    for (count = 0; count < 250; count++);
 
    mt65xx_reg_sync_writel(1 << eint_num, EINT_INTACK);
}

/*
 * mt65xx_eint_set_sens: Set the sensitivity for the EINT number.
 * @eint_num: EINT number to set
 * @sens: sensitivity to set
 * Always return 0.
 */
unsigned int mt65xx_eint_set_sens(unsigned int eint_num, unsigned int sens)
{
    if (sens == MT65xx_EDGE_SENSITIVE) {
        mt65xx_reg_sync_writel(1 << eint_num, EINT_SENS_CLR);
    } else if (sens == MT65xx_LEVEL_SENSITIVE) {
        mt65xx_reg_sync_writel(1 << eint_num, EINT_SENS_SET);
    }

    return 0;
}

/*
 * eint_do_tasklet: EINT tasklet function.
 * @unused: not use.
 */
void eint_do_tasklet(unsigned long unused)
{
    wake_lock_timeout(&EINT_suspend_lock, HZ / 2);
}

DECLARE_TASKLET(eint_tasklet, eint_do_tasklet, 0);

/*
 * mt65xx_eint_isr: EINT interrupt service routine.
 * @irq: EINT IRQ number
 * @dev_id:
 * Return IRQ returned code.
 */
static irqreturn_t mt65xx_eint_isr(int irq, void *dev_id)
{
    unsigned int index;
    unsigned int status;

    /* 
     * NoteXXX: Need to get the wake up for 0.5 seconds when an EINT intr tirggers.
     *          This is used to prevent system from suspend such that other drivers
     *          or applications can have enough time to obtain their own wake lock.
     *          (This information is gotten from the power management owner.)
     */
    tasklet_schedule(&eint_tasklet);

    status = readl(EINT_STA);

    dbgmsg("EINT Module - %s ISR Start\n", __func__);
    dbgmsg("EINT Module - EINT_STA = 0x%x\n", status);

    for (index = 0; index < EINT_MAX_CHANNEL; index++) {
        if (status & (1 << (index))) {
            mt65xx_eint_mask(index);
            if (EINT_FUNC.eint_func[index]) {
                EINT_FUNC.eint_func[index]();
            }

            mt65xx_reg_sync_writel(1 << index, EINT_INTACK);

#if(EINT_DEBUG == 1)
            status = readl(EINT_STA);
            printk("EINT Module - EINT_STA after ack = 0x%x\n", status);
#endif

            if (EINT_FUNC.eint_auto_umask[index]) {
                mt65xx_eint_unmask(index);
            }
        }
    }

    dbgmsg("EINT Module - %s ISR END\n", __func__);

    return IRQ_HANDLED;
}

#if 0
/*
 * mt65xx_deint_isr: Direct EINT interrupt service routine.
 * @irq: EINT IRQ number
 * @dev_id: Direct EINT group ID, 0 ~ 7
 * Return IRQ returned code.
 */
static irqreturn_t mt65xx_deint_isr(int irq, void *dev_id)
{
    unsigned int index;
    unsigned int status;
    const int iGroup = dev_id ? *(deint_group *)dev_id : -1;
    
    if (iGroup < 0) {
        printk(KERN_ERR "DEINT IRQ GROUP IS NOT VALID!!\n");
    }

    /* 
     * NoteXXX: Need to get the wake up for 0.5 seconds when an EINT intr tirggers.
     *          This is used to prevent system from suspend such that other drivers
     *          or applications can have enough time to obtain their own wake lock.
     *          (This information is gotten from the power management owner.)
     */
    /* FIXME: use the same tasklet?? */
    tasklet_schedule(&eint_tasklet);

    status = readl(EINT_STA);

    dbgmsg("DEINT Module - %s ISR Start\n", __func__);
    dbgmsg("DEINT Module - EINT_STA = 0x%x\n", status);

    for (index = iGroup * EINT_MAX_CHANNEL / DEINT_GROUP_MAX; index < (iGroup + 1) * EINT_MAX_CHANNEL / DEINT_GROUP_MAX ; index++) {
        if (status & (1 << (index))) {
            mt65xx_deint_mask(index);

            if (DEINT_FUNC.eint_func[index]) {
                DEINT_FUNC.eint_func[index]();
            }

            mt65xx_reg_sync_writel(1 << index, EINT_INTACK);

#if(EINT_DEBUG == 1)
            status = readl(EINT_STA);
            printk("EINT Module - EINT_STA after ack = 0x%x\n", status);
#endif

            if (DEINT_FUNC.eint_auto_umask[index]) {
                mt65xx_deint_unmask(index);
            }
        }
    }

    dbgmsg("DEINT Module - %s ISR END\n", __func__);

    return IRQ_HANDLED;
}
#endif

/*
 * mt65xx_eint_registration: register a EINT.
 * @eint_num: the EINT number to register
 * @is_deb_en: the indication flag of HW de-bounce time 
 * @pol: polarity value
 * @EINT_FUNC_PTR: the ISR callback function
 * @is_auto_unmask: the indication flag of auto unmasking after ISR callback is processed
 */
void mt65xx_eint_registration(unsigned int eint_num, unsigned int is_deb_en, unsigned int pol, void (EINT_FUNC_PTR)(void), unsigned int is_auto_umask)
{
    if (pol == MT65XX_EINT_POL_POS) { 
        mt65xx_reg_sync_writel(readl(EINT_CON(eint_num)) | EINT_POL_BIT, EINT_CON(eint_num));
    } else {
        mt65xx_reg_sync_writel(readl(EINT_CON(eint_num)) & ~EINT_POL_BIT, EINT_CON(eint_num));
    }

    if (eint_num < EINT_MAX_CHANNEL) {
        if (is_deb_en) {
            if (!(readl(EINT_CON(eint_num)) & EINT_DEB_EN_BIT)) {
                mt65xx_reg_sync_writel(readl(EINT_CON(eint_num)) | (EINT_DEB_CNT_MASK | EINT_DEB_EN_BIT), EINT_CON(eint_num));
            }
        } else {
            writel(readl(EINT_CON(eint_num)) & ~EINT_DEB_EN_BIT, EINT_CON(eint_num));
        }
    }

    EINT_FUNC.eint_func[eint_num] = EINT_FUNC_PTR;
    EINT_FUNC.eint_auto_umask[eint_num] = is_auto_umask;

    writel(1 << eint_num, EINT_INTACK);
    mt65xx_reg_sync_writel(1 << eint_num, EINT_MASK_CLR);
}

#if 0
/*
 * mt65xx_deint_registration: register a direct EINT.
 * @eint_num: the EINT number to register
 * @pol: polarity value
 * @EINT_FUNC_PTR: the ISR callback function
 * @is_auto_unmask: the indication flag of auto unmasking after ISR callback is processed
 */
void mt65xx_deint_registration(unsigned int eint_num, unsigned int pol, void (EINT_FUNC_PTR)(void), unsigned int is_auto_umask)
{
    if (pol == MT65XX_EINT_POL_POS) { 
        mt65xx_reg_sync_writel(readl(EINT_CON(eint_num)) | EINT_POL_BIT, EINT_CON(eint_num));
    } else {
        mt65xx_reg_sync_writel(readl(EINT_CON(eint_num)) & ~EINT_POL_BIT, EINT_CON(eint_num));
    }

    DEINT_FUNC.eint_func[eint_num] = EINT_FUNC_PTR;
    DEINT_FUNC.eint_auto_umask[eint_num] = is_auto_umask;

    writel(1 << eint_num, EINT_INTACK);
    mt65xx_reg_sync_writel(1 << eint_num, EINT_DMASK_CLR);
}
#endif

static ssize_t cur_eint_show(struct device_driver *driver, char *buf)
{
    return snprintf(buf, PAGE_SIZE, "%d\n", cur_eint_num);
}

static ssize_t cur_eint_store(struct device_driver *driver, const char *buf, size_t count)
{
    char *p = (char *)buf;
    unsigned int num;

    num = simple_strtoul(p, &p, 10);
    if (num >= EINT_MAX_CHANNEL) {
        printk("invalid EINT number\n");
    } else {
        cur_eint_num = num;
    }

    return count;
}

DRIVER_ATTR(current_eint, 0664, cur_eint_show, cur_eint_store);

static ssize_t cur_eint_sens_show(struct device_driver *driver, char *buf)
{
    int val, sens;

    val = readl(EINT_SENS);
    if (val & (1 << cur_eint_num)) {
        sens = 1;
    } else {
        sens = 0;
    }

    return snprintf(buf, PAGE_SIZE, "%d\n", sens);
}

static ssize_t cur_eint_sens_store(struct device_driver *driver, const char *buf, size_t count)
{
    char *p = (char *)buf;
    int sens;

    sens = simple_strtoul(p, &p, 10);
    if (sens == 1) {
        sens = MT65xx_LEVEL_SENSITIVE;
    } else if (sens == 0) {
        sens = MT65xx_EDGE_SENSITIVE;
    } else {
        printk("invalid sensitivity value\n");
        return count;
    }

    mt65xx_eint_mask(cur_eint_num);
    mt65xx_eint_set_sens(cur_eint_num, sens);
    mt65xx_eint_unmask(cur_eint_num);

    return count;
}

DRIVER_ATTR(current_eint_sens, 0644, cur_eint_sens_show, cur_eint_sens_store);

static ssize_t cur_eint_pol_show(struct device_driver *driver, char *buf)
{
    int val, pol;

    val = readl(EINT_CON(cur_eint_num));
    if (val & EINT_POL_BIT) {
        pol = 1;
    } else {
        pol = 0;
    }

    return snprintf(buf, PAGE_SIZE, "%d\n", pol);
}

static ssize_t cur_eint_pol_store(struct device_driver *driver, const char *buf, size_t count)
{
    char *p = (char *)buf;
    int pol;

    pol = simple_strtoul(p, &p, 10);
    if (pol == 1) {
        pol = MT65XX_EINT_POL_POS;
    } else if (pol == 0) {
        pol = MT65XX_EINT_POL_NEG;
    } else {
        printk("invalid polarity value\n");
        return count;
    }

    mt65xx_eint_mask(cur_eint_num);
    mt65xx_eint_set_polarity(cur_eint_num, pol);
    mt65xx_eint_unmask(cur_eint_num);

    return count;
}

DRIVER_ATTR(current_eint_pol, 0644, cur_eint_pol_show, cur_eint_pol_store);

static ssize_t cur_eint_deb_show(struct device_driver *driver, char *buf)
{
    int val;
    unsigned int cnt, prescaler, deb;

    val = readl(EINT_CON(cur_eint_num));
    cnt = val & EINT_DEB_CNT_MASK;
    prescaler = (val & EINT_DEB_PRESCALER_MASK) >> EINT_DEB_PRESCALER_SHIFT;
    switch (prescaler) {
    case 0:
        deb = cnt >> 5;
        break;

    case 1:
        deb = cnt >> 4;
        break;

    case 2:
        deb = cnt >> 3;
        break;

    case 3:
        deb = cnt >> 2;
        break;

    case 4:
        deb = cnt >> 1;
        break;

    case 5:
        deb = cnt;
        break;

    case 6:
        deb = cnt << 1;
        break;

    case 7:
        deb = cnt << 2;
        break;

    default:
        deb = 0;
        printk("invalid prescaler in the EIN_CON register\n");
        break;
    }

    return snprintf(buf, PAGE_SIZE, "%d\n", deb);
}

static ssize_t cur_eint_deb_store(struct device_driver *driver, const char *buf, size_t count)
{
    char *p = (char *)buf;
    unsigned int deb;

    deb = simple_strtoul(p, &p, 10);
    mt65xx_eint_set_hw_debounce(cur_eint_num, deb);

    return count;
}

DRIVER_ATTR(current_eint_deb, 0644, cur_eint_deb_show, cur_eint_deb_store);

static ssize_t cur_eint_deb_en_show(struct device_driver *driver, char *buf)
{
    int val, en;

    val = readl(EINT_CON(cur_eint_num));
    if (val & EINT_DEB_EN_BIT) {
        en = 1;
    } else {
        en = 0;
    }

    return snprintf(buf, PAGE_SIZE, "%d\n", en);
}

static ssize_t cur_eint_deb_en_store(struct device_driver *driver, const char *buf, size_t count)
{
    char *p = (char *)buf;
    unsigned int en;

    en = simple_strtoul(p, &p, 10);

    if (en != 1 && en != 0) {
        printk("invalid debounce-enable value\n");
    } else {
        mt65xx_eint_mask(cur_eint_num);
        if (en == 1) {
            writel(readl(EINT_CON(cur_eint_num)) | EINT_DEB_EN_BIT, EINT_CON(cur_eint_num));
        } else {
            writel(readl(EINT_CON(cur_eint_num)) & ~EINT_DEB_EN_BIT, EINT_CON(cur_eint_num));
        }
        mt65xx_eint_unmask(cur_eint_num);
    }

    return count;
}

DRIVER_ATTR(current_eint_deb_en, 0644, cur_eint_deb_en_show, cur_eint_deb_en_store);

/*
 * mt65xx_eint_init: initialize EINT driver.
 * Always return 0.
 */
typedef struct{
  char name[24];
  int  eint_num;
}MD_EINT_INFO;

#define MD_EINT_MAX 3
MD_EINT_INFO md_info[MD_EINT_MAX];

int get_td_eint_num(char *td_name, int len)
{
  unsigned int i;

  dbgmsg("[get_td_eint] name = %s\n", td_name);
  
  for(i=0; i<MD_EINT_MAX; i++)
  {
    if (!strncmp(td_name, md_info[i].name, len))
    {
      return md_info[i].eint_num;
    }
  }
  return -1;
}

//This function unmasks Direct External Interrupt
void DEINT_Unmask(unsigned int deint_no)
{
  writel(1<<deint_no, EINT_DMASK_CLR);
}

int mt65xx_eint_init(void)
{
    unsigned int i;
    int ret;
    unsigned int ap_domain = 0xFFFFFFFF;
#if defined(CUST_EINT_AST_DATA_INTR_NUM) || defined(CUST_EINT_AST_WAKEUP_INTR_NUM) || defined(CUST_EINT_AST_RFCONFLICT_INTR_NUM)
    unsigned int md_counter = 0;
#endif

    
#ifdef CUST_EINT_AST_DATA_INTR_NUM
    ap_domain &= ~(1<<CUST_EINT_AST_DATA_INTR_NUM);
    sprintf(md_info[md_counter].name, "AST_DATA_INTR");
    md_info[md_counter].eint_num = CUST_EINT_AST_DATA_INTR_NUM;
    mt65xx_eint_set_sens(CUST_EINT_AST_DATA_INTR_NUM, CUST_EINT_AST_DATA_INTR_SENSITIVE);
    mt65xx_eint_set_polarity(CUST_EINT_AST_DATA_INTR_NUM, CUST_EINT_AST_DATA_INTR_POLARITY);
    DEINT_Unmask(CUST_EINT_AST_DATA_INTR_NUM);
    dbgmsg("[EINT] name = %s\n", md_info[md_counter].name);
    md_counter++;
#endif
#ifdef CUST_EINT_AST_WAKEUP_INTR_NUM
    ap_domain &= ~(1<<CUST_EINT_AST_WAKEUP_INTR_NUM);
    sprintf(md_info[md_counter].name, "AST_WAKEUP_INTR");
    md_info[md_counter].eint_num = CUST_EINT_AST_WAKEUP_INTR_NUM;
    mt65xx_eint_set_sens(CUST_EINT_AST_WAKEUP_INTR_NUM, CUST_EINT_AST_WAKEUP_INTR_SENSITIVE);
    mt65xx_eint_set_polarity(CUST_EINT_AST_WAKEUP_INTR_NUM, CUST_EINT_AST_WAKEUP_INTR_POLARITY);
    DEINT_Unmask(CUST_EINT_AST_WAKEUP_INTR_NUM);
    dbgmsg("[EINT] name = %s\n", md_info[md_counter].name);
    md_counter++;
#endif
#ifdef CUST_EINT_AST_RFCONFLICT_INTR_NUM
    ap_domain &= ~(1<<CUST_EINT_AST_RFCONFLICT_INTR_NUM);
    sprintf(md_info[md_counter].name, "AST_RFCONFLICT_INTR");
    md_info[md_counter].eint_num = CUST_EINT_AST_RFCONFLICT_INTR_NUM;
    mt65xx_eint_set_sens(CUST_EINT_AST_RFCONFLICT_INTR_NUM, CUST_EINT_AST_RFCONFLICT_INTR_SENSITIVE);
    mt65xx_eint_set_polarity(CUST_EINT_AST_RFCONFLICT_INTR_NUM, CUST_EINT_AST_RFCONFLICT_INTR_POLARITY);
    DEINT_Unmask(CUST_EINT_AST_RFCONFLICT_INTR_NUM);
    dbgmsg("[EINT] name = %s\n", md_info[md_counter].name);
    md_counter++;
#endif

    wake_lock_init(&EINT_suspend_lock, WAKE_LOCK_SUSPEND, "EINT wakelock");

    for (i = 0; i< EINT_MAX_CHANNEL; i++) {
        EINT_FUNC.eint_func[i] = NULL;
        EINT_FUNC.eint_auto_umask[i] = 0;
    }

    
    /* assign to domain 0 for AP */
    dbgmsg("[EINT]ap_domain = %x\n", ap_domain);
    mt65xx_reg_sync_writel(ap_domain, EINT_D0EN);

    /* normal EINT IRQ registration */
    if (request_irq(EINT_IRQ, mt65xx_eint_isr, IRQF_TRIGGER_HIGH, "EINT", NULL)) {
        printk(KERN_ERR "EINT IRQ LINE NOT AVAILABLE!!\n");
    }

#if 0
    /* direct EINT IRQs registration */
    for (i = 0; i < DEINT_GROUP_MAX; i++) {
        if (request_irq(MT6577_EINT_DIRECT0_IRQ_ID + i, mt65xx_deint_isr, IRQF_TRIGGER_HIGH, "DEINT", (void *)&deint_group_id[i])) {
            printk(KERN_ERR "DEINT IRQ LINE NOT AVAILABLE!!\n");
        }
    }
#endif

    ret = driver_register(&mt6577_eint_drv);
    if (ret) {
        printk("fail to register mt6577_eint_drv\n");
    }

    ret = driver_create_file(&mt6577_eint_drv, &driver_attr_current_eint);
    ret |= driver_create_file(&mt6577_eint_drv, &driver_attr_current_eint_sens);
    ret |= driver_create_file(&mt6577_eint_drv, &driver_attr_current_eint_pol);
    ret |= driver_create_file(&mt6577_eint_drv, &driver_attr_current_eint_deb);
    ret |= driver_create_file(&mt6577_eint_drv, &driver_attr_current_eint_deb_en);
    if (ret) {
        printk("fail to create mt6577_eint sysfs files\n");
    }

    return 0;
}

arch_initcall(mt65xx_eint_init);

EXPORT_SYMBOL(mt65xx_eint_registration);
EXPORT_SYMBOL(mt65xx_eint_set_hw_debounce);
EXPORT_SYMBOL(mt65xx_eint_set_sens);
EXPORT_SYMBOL(mt65xx_eint_mask);
EXPORT_SYMBOL(mt65xx_eint_unmask);
EXPORT_SYMBOL(get_td_eint_num);
