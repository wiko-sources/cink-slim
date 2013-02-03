#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/semaphore.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/xlog.h>

#include "mach/mt6577_reg_base.h"
#include "mach/mt6577_emi_bwl.h"
#include "mach/sync_write.h"

DEFINE_SEMAPHORE(emi_bwl_sem);

static struct platform_driver mem_bw_ctrl = {
    .driver     = {
        .name = "mem_bw_ctrl",
        .owner = THIS_MODULE,
    },
};

/* define EMI bandwiwth limiter control table */
static struct emi_bwl_ctrl ctrl_tbl[NR_CON_SCE];

/* current concurrency scenario */
static int cur_con_sce = 0x0FFFFFFF;

/* define concurrency scenario strings */
static const char *con_sce_str[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) #con_sce,
#include "mach/mt6577_con_sce.h"
#undef X_CON_SCE
};

/* define EMI bandwidth allocation tables */
/****************** For DDR1 ******************/
static const unsigned int emi_arba_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arba,
#include "mach/mt6577_con_sce.h"
#undef X_CON_SCE
};

static const unsigned int emi_arbc_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbc,
#include "mach/mt6577_con_sce.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbd_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbd,
#include "mach/mt6577_con_sce.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbe_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbe,
#include "mach/mt6577_con_sce.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbf_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbf,
#include "mach/mt6577_con_sce.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbg_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbg,
#include "mach/mt6577_con_sce.h"
#undef X_CON_SCE
};

/****************** For DDR2 ******************/

static const unsigned int emi_arba_ddr2_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arba,
#include "mach/mt6577_con_sce_ddr2.h"
#undef X_CON_SCE
};

static const unsigned int emi_arbc_ddr2_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbc,
#include "mach/mt6577_con_sce_ddr2.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbd_ddr2_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbd,
#include "mach/mt6577_con_sce_ddr2.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbe_ddr2_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbe,
#include "mach/mt6577_con_sce_ddr2.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbf_ddr2_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbf,
#include "mach/mt6577_con_sce_ddr2.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbg_ddr2_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbg,
#include "mach/mt6577_con_sce_ddr2.h"
#undef X_CON_SCE
};
/**********************************************/

/****************** For DDR3 ******************/

static const unsigned int emi_arba_ddr3_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arba,
#include "mach/mt6577_con_sce_ddr3.h"
#undef X_CON_SCE
};

static const unsigned int emi_arbc_ddr3_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbc,
#include "mach/mt6577_con_sce_ddr3.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbd_ddr3_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbd,
#include "mach/mt6577_con_sce_ddr3.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbe_ddr3_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbe,
#include "mach/mt6577_con_sce_ddr3.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbf_ddr3_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbf,
#include "mach/mt6577_con_sce_ddr3.h"
#undef X_CON_SCE
};
static const unsigned int emi_arbg_ddr3_val[] =
{
#define X_CON_SCE(con_sce, arba, arbc, arbd, arbe, arbf, arbg) arbg,
#include "mach/mt6577_con_sce_ddr3.h"
#undef X_CON_SCE
};
/**********************************************/

static int get_ddr_type(void)
{
    return (*(volatile unsigned int *)(DRAM_CTR_BASE + 0x00D8) & 0xC0000000)? ((*(volatile unsigned int *)(APMIXED_BASE + 0x0204) & 0x00000020) ? 1 : 3): 2;
}

/*
 * mtk_mem_bw_ctrl: set EMI bandwidth limiter for memory bandwidth control
 * @sce: concurrency scenario ID
 * @op: either ENABLE_CON_SCE or DISABLE_CON_SCE
 * Return 0 for success; return negative values for failure.
 */
int mtk_mem_bw_ctrl(int sce, int op)
{
    int i, highest;

    if (sce >= NR_CON_SCE) {
        return -1;
    }
    if (op != ENABLE_CON_SCE && op != DISABLE_CON_SCE) {
        return -1;
    }
    if (in_interrupt()) {
        return -1;
    }

    down(&emi_bwl_sem);

    if (op == ENABLE_CON_SCE) {
        ctrl_tbl[sce].ref_cnt++;
    } else if (op == DISABLE_CON_SCE) {
        if (ctrl_tbl[sce].ref_cnt != 0) {
            ctrl_tbl[sce].ref_cnt--;
        } 
    }

    /* find the scenario with the highest priority */
    highest = -1;
    for (i = 0; i < NR_CON_SCE; i++) {
        if (ctrl_tbl[i].ref_cnt != 0) {
            highest = i;
            break;
        }
    }
    if (highest == -1) {
        highest = CON_SCE_NORMAL;
    }

    /* set new EMI bandwidth limiter value */
    if (highest != cur_con_sce) {
        if(get_ddr_type() == 1)
        {
            writel(emi_arba_val[highest], EMI_ARBA);
            writel(emi_arbc_val[highest], EMI_ARBC);
            writel(emi_arbd_val[highest], EMI_ARBD);
            writel(emi_arbe_val[highest], EMI_ARBE);
            writel(emi_arbf_val[highest], EMI_ARBF);
            mt65xx_reg_sync_writel(emi_arbg_val[highest], EMI_ARBG);
        }
        else if(get_ddr_type() == 2)
        {
            writel(emi_arba_ddr2_val[highest], EMI_ARBA);
            writel(emi_arbc_ddr2_val[highest], EMI_ARBC);
            writel(emi_arbd_ddr2_val[highest], EMI_ARBD);
            writel(emi_arbe_ddr2_val[highest], EMI_ARBE);
            writel(emi_arbf_ddr2_val[highest], EMI_ARBF);
            mt65xx_reg_sync_writel(emi_arbg_ddr2_val[highest], EMI_ARBG);           
        }
        else if(get_ddr_type() == 3)
        {
            writel(emi_arba_ddr3_val[highest], EMI_ARBA);
            writel(emi_arbc_ddr3_val[highest], EMI_ARBC);
            writel(emi_arbd_ddr3_val[highest], EMI_ARBD);
            writel(emi_arbe_ddr3_val[highest], EMI_ARBE);
            writel(emi_arbf_ddr3_val[highest], EMI_ARBF);
            mt65xx_reg_sync_writel(emi_arbg_ddr3_val[highest], EMI_ARBG);           
        }
        cur_con_sce = highest;
    }

    up(&emi_bwl_sem);

    return 0;
}

#if 0
EXPORT_SYMBOL(mtk_mem_bw_ctrl);
#endif

/*
 * con_sce_show: sysfs con_sce file show function.
 * @driver:
 * @buf:
 * Return the number of read bytes.
 */
static ssize_t con_sce_show(struct device_driver *driver, char *buf)
{
    if (cur_con_sce >= NR_CON_SCE) {
        sprintf(buf, "none\n");
    } else {
        sprintf(buf, "%s\n", con_sce_str[cur_con_sce]);
    }

    return strlen(buf);
}

/*
 * con_sce_store: sysfs con_sce file store function.
 * @driver:
 * @buf:
 * @count:
 * Return the number of write bytes.
 */
static ssize_t con_sce_store(struct device_driver *driver, const char *buf, size_t count)
{
    int i;

    for (i = 0; i < NR_CON_SCE; i++) {
        if (!strncmp(buf, con_sce_str[i], strlen(con_sce_str[i]))) {
            if (!strncmp(buf + strlen(con_sce_str[i]) + 1, EN_CON_SCE_STR, strlen(EN_CON_SCE_STR))) {
                mtk_mem_bw_ctrl(i, ENABLE_CON_SCE);
                printk("concurrency scenario %s ON\n", con_sce_str[i]);
                break;
            } else if (!strncmp(buf + strlen(con_sce_str[i]) + 1, DIS_CON_SCE_STR, strlen(DIS_CON_SCE_STR))) {
                mtk_mem_bw_ctrl(i, DISABLE_CON_SCE);
                printk("concurrency scenario %s OFF\n", con_sce_str[i]);
                break;
            }
        }
    }

    return count;
}

DRIVER_ATTR(concurrency_scenario, 0644, con_sce_show, con_sce_store);

/*
 * emi_bwl_mod_init: module init function.
 */
static int __init emi_bwl_mod_init(void)
{
    int ret;
    if(get_ddr_type() == 1) //DDR1
    {
        //write overhead value
        writel(0x090B0B10, EMI_CONB);	//read  overhead for 4~1
        writel(0x09090909, EMI_CONC);	//read  overhead for 8~5
        writel(0x0B0E1013, EMI_COND);	//write overhead for 4~1
        writel(0x0909090A, EMI_CONE);   //write overhead for 8~5
    }
    else if(get_ddr_type() == 2) //DDR2
    {
        //write overhead value
        writel(0x0B0B0E17, EMI_CONB);	//read  overhead for 4~1
        writel(0x0B0B0B0B, EMI_CONC);	//read  overhead for 8~5
        writel(0x1012161E, EMI_COND);	//write overhead for 4~1
        writel(0x0B0B0D0E, EMI_CONE);   //write overhead for 8~5
    }
    else if(get_ddr_type() == 3) //DDR3
    {
        writel(0x2B2C2C2E, EMI_CONB);	//read  overhead for 4~1
        writel(0x2627292B, EMI_CONC);	//read  overhead for 8~5
        writel(0x2B2C2C2E, EMI_COND);	//write overhead for 4~1
        writel(0x2627292B, EMI_CONE);       //write overhead for 8~5
    }
    
    //write Filter Priority Encode
#if 0    
    writel(0x00112480, EMI_ARBI);	//distant=0, urgent=1, miss=1, hit=2, limit=4, age=8, ultra=0
#else
    writel(0x01812488, EMI_ARBI);
#endif

    ret = mtk_mem_bw_ctrl(CON_SCE_NORMAL, ENABLE_CON_SCE);
    if (ret) {
        xlog_printk(ANDROID_LOG_ERROR, "EMI/BWL", "fail to set EMI bandwidth limiter\n");
    }

    ret = platform_driver_register(&mem_bw_ctrl);
    if (ret) {
        xlog_printk(ANDROID_LOG_ERROR, "EMI/BWL", "fail to register EMI_BW_LIMITER driver\n");
    }

    ret = driver_create_file(&mem_bw_ctrl.driver, &driver_attr_concurrency_scenario);
    if (ret) {
        xlog_printk(ANDROID_LOG_ERROR, "EMI/BWL", "fail to create EMI_BW_LIMITER sysfs file\n");
    }

    return 0;
}

/*
 * emi_bwl_mod_exit: module exit function.
 */
static void __exit emi_bwl_mod_exit(void)
{
}
 
module_init(emi_bwl_mod_init);
module_exit(emi_bwl_mod_exit);

