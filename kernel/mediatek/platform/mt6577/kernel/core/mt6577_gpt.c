/******************************************************************************
 * mt6575_gpt.c - This file is for the GPT module !
 *
 * DESCRIPTION: -
 *     This file is a kernel module that includes essential functions about
 *     GPT module !
 *
 * modification history
 * ----------------------------------------
 * 2010/09/07, v0.1 by Terry Chang
 * 2010/10/21, v0.2 by Terry Chang
 * 2010/12/10, v0.3 by Terry Chang
 * ----------------------------------------
 ******************************************************************************/

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/cnt32_to_63.h>
#include <linux/proc_fs.h>

#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_gpt.h>
#include <mach/timer.h>
#include <mach/irqs.h>

#define GPT_SOFTWARE_WORKAROUND
/* GPT registers */
#define GPT_IRQEN       (APMCU_GPTIMER_BASE + 0x0000)
#define GPT_IRQSTA      (APMCU_GPTIMER_BASE + 0x0004)
#define GPT_IRQACK      (APMCU_GPTIMER_BASE + 0x0008)
#define GPT1_CON        (APMCU_GPTIMER_BASE + 0x0010)
#define GPT1_CLK        (APMCU_GPTIMER_BASE + 0x0014)
#define GPT1_COUNT      (APMCU_GPTIMER_BASE + 0x0018)
#define GPT1_COMPARE    (APMCU_GPTIMER_BASE + 0x001c)
#define GPT2_CON        (APMCU_GPTIMER_BASE + 0x0020)
#define GPT2_CLK        (APMCU_GPTIMER_BASE + 0x0024)
#define GPT2_COUNT      (APMCU_GPTIMER_BASE + 0x0028)
#define GPT2_COMPARE    (APMCU_GPTIMER_BASE + 0x002c)
#define GPT3_CON        (APMCU_GPTIMER_BASE + 0x0030)
#define GPT3_CLK        (APMCU_GPTIMER_BASE + 0x0034)
#define GPT3_COUNT      (APMCU_GPTIMER_BASE + 0x0038)
#define GPT3_COMPARE    (APMCU_GPTIMER_BASE + 0x003c)
#define GPT4_CON        (APMCU_GPTIMER_BASE + 0x0040)
#define GPT4_CLK        (APMCU_GPTIMER_BASE + 0x0044)
#define GPT4_COUNT      (APMCU_GPTIMER_BASE + 0x0048)
#define GPT4_COMPARE    (APMCU_GPTIMER_BASE + 0x004c)
#define GPT5_CON        (APMCU_GPTIMER_BASE + 0x0050)
#define GPT5_CLK        (APMCU_GPTIMER_BASE + 0x0054)
#define GPT5_COUNT      (APMCU_GPTIMER_BASE + 0x0058)
#define GPT5_COMPARE    (APMCU_GPTIMER_BASE + 0x005c)
#define GPT6_CON        (APMCU_GPTIMER_BASE + 0x0060)
#define GPT6_CLK        (APMCU_GPTIMER_BASE + 0x0064)
#define GPT6_COUNTL     (APMCU_GPTIMER_BASE + 0x0068)
#define GPT6_COMPAREL   (APMCU_GPTIMER_BASE + 0x006c)
#define GPT6_COUNTH     (APMCU_GPTIMER_BASE + 0x0078)
#define GPT6_COMPAREH   (APMCU_GPTIMER_BASE + 0x007c)

/******************************************************************************
 Debug Control
******************************************************************************/

//#define MT6575_GPT_DEBUG
#ifdef MT6577_GPT_DEBUG
#define GPT_DEBUG printk
#else
#define GPT_DEBUG(x, ...)
#endif

/******************************************************************************
 GPT-Related Setting
******************************************************************************/

#define GPT_CON_ENABLE          0x0001
#define GPT_CON_CLEAR           0x0002
#define GPT_CLK_DIVISOR_MASK    0x000f
#define GPT_CLK_SRC_MASK        0x0030
#define GPT_CLK_SRC_SHIFT_BITS  4

#define TICK_GPT_ID		(GPT1)

const UINT32 g_gpt_mask[GPT_TOTAL_COUNT+1] = {
    0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0
};

const UINT32 g_gpt_addr_con[GPT_TOTAL_COUNT] = {
    GPT1_CON, GPT2_CON, GPT3_CON, GPT4_CON, GPT5_CON, GPT6_CON
};

const UINT32 g_gpt_addr_clk[GPT_TOTAL_COUNT] = {
    GPT1_CLK, GPT2_CLK, GPT3_CLK, GPT4_CLK, GPT5_CLK, GPT6_CLK
};

const UINT32 g_gpt_addr_cnt[GPT_TOTAL_COUNT] = {
    GPT1_COUNT, GPT2_COUNT, GPT3_COUNT, GPT4_COUNT, GPT5_COUNT, GPT6_COUNTL
};

const UINT32 g_gpt_addr_compare[GPT_TOTAL_COUNT] = {
    GPT1_COMPARE, GPT2_COMPARE, GPT3_COMPARE, GPT4_COMPARE, GPT5_COMPARE, GPT6_COMPAREL
};

const BOOL g_gpt_reserved_table[] = {
    TRUE, TRUE, FALSE, FALSE, FALSE, FALSE /* GPT1 and GPT2 are reserved for Timer module */
};

const GPT_NUM g_gpt_timer_index[] = {
    GPT1, GPT2, GPT3, GPT4, GPT5, GPT6
};

static BOOL gpt_free_table[] = {
    TRUE, TRUE, TRUE, TRUE, TRUE, TRUE
};

static DEFINE_SPINLOCK(g_gpt_lock);
static unsigned long g_gpt_flags;

/* This spin lock is used for protecting the free GPT table */
static DEFINE_SPINLOCK(free_gpt_lock);
static unsigned long free_gpt_flags;

/******************************************************************************
 Timer Handler
******************************************************************************/

//GPT3~6 Handle Function
static GPT_Func g_gpt_func;

/******************************************************************************
 Tasklet that will call user registered call-back function
******************************************************************************/

static void GPT3_Tasklet(unsigned long arg)
{
    if (g_gpt_func.gpt3_func != NULL)
        g_gpt_func.gpt3_func(GPT3);
    else
        printk("g_gpt_func.gpt3_fun (arg:%lu) = NULL!\n", arg);
}

#if 0
static void GPT4_Tasklet(unsigned long arg)
{
    if (g_gpt_func.gpt4_func != NULL)
        g_gpt_func.gpt4_func(GPT4);
    else
        printk("g_gpt_func.gpt4_fun (arg:%lu) = NULL!\n", arg);
}
#endif 

static void GPT5_Tasklet(unsigned long arg)
{
    if (g_gpt_func.gpt5_func != NULL)
        g_gpt_func.gpt5_func(GPT5);
    else
        printk("g_gpt_func.gpt5_fun (arg:%lu) = NULL!\n", arg);
}

static void GPT6_Tasklet(unsigned long arg)
{
    if (g_gpt_func.gpt6_func != NULL)
        g_gpt_func.gpt6_func(GPT6);
    else
        printk("g_gpt_func.gpt6_fun (arg:%lu) = NULL!\n", arg);
}

static DECLARE_TASKLET(g_gpt3_tlet, GPT3_Tasklet, 0);
//static DECLARE_TASKLET(g_gpt4_tlet, GPT4_Tasklet, 0);
static DECLARE_TASKLET(g_gpt5_tlet, GPT5_Tasklet, 0);
static DECLARE_TASKLET(g_gpt6_tlet, GPT6_Tasklet, 0);

/******************************************************************************
 * DESCRIPTION:
 *   Enable Interrupt of GPT
 * PARAMETERS:
 *   GPT Number (GPT3~6)
 ******************************************************************************/
void GPT_EnableIRQ(GPT_NUM numGPT)
{
    if (numGPT == GPT1 || numGPT == GPT2)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    DRV_SetReg32(GPT_IRQEN, g_gpt_mask[numGPT]);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

/******************************************************************************
 * DESCRIPTION:
 *   Disable Interrupt of GPT
 * PARAMETERS:
 *   GPT Number (GPT3~6)
 ******************************************************************************/
void GPT_DisableIRQ(GPT_NUM numGPT)
{
    if (numGPT == GPT1 || numGPT == GPT2)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    DRV_ClrReg32(GPT_IRQEN, g_gpt_mask[numGPT]);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

/******************************************************************************
 * DESCRIPTION:
 *   Check GPT Interrupt is enable or disable
 * PARAMETERS:
 *   GPT Number (GPT1~6)
 * RETURNS:
 *   TRUE : Interrupt is enabled
 *   FALSE: Interrupt is not enabled
 ******************************************************************************/
BOOL GPT_IsIRQEnable(GPT_NUM numGPT)
{
    return (DRV_Reg32(GPT_IRQEN) & g_gpt_mask[numGPT]) ? TRUE : FALSE;
}

BOOL GPT_Get_IRQ(GPT_NUM numGPT)
{
	return (DRV_Reg32(GPT_IRQSTA) | g_gpt_mask[numGPT]) ? TRUE : FALSE;
}
/******************************************************************************
 * DESCRIPTION:
 *   This function is to read IRQ status. if none of gpt channel has issued an interrupt
 *   ,then the return value is GPT_TOTAL_COUNT.
 *   Otherwise, the return value would be the number of gpt has issued an interrupt
 * RETURNS:
 *   numGPT : GPT Number
 *           (GPT1,GPT2,GPT3,GPT4,GPT5,GPT6)
 ******************************************************************************/
static GPT_NUM GPT_Get_IRQSTA(void)
{
    UINT32 sta;
    GPT_NUM numGPT;

    sta = DRV_Reg32(GPT_IRQSTA);

    for (numGPT = GPT1; numGPT < GPT_TOTAL_COUNT; numGPT++) {
        if (sta & g_gpt_mask[numGPT])
            break;
    }

    return numGPT;
}

/******************************************************************************
 * DESCRIPTION:
 *   Clean IRQ status bit by writing IRQ ack bit
 * PARAMETERS:
 *   GPT number
 ******************************************************************************/
static void GPT_AckIRQ(GPT_NUM numGPT)
{
    spin_lock(&g_gpt_lock);

    DRV_WriteReg32(GPT_IRQACK, g_gpt_mask[numGPT]);

    spin_unlock(&g_gpt_lock);
}

/******************************************************************************
 * DESCRIPTION:
 *   Start GPT Timer
 * PARAMETERS:
 *   GPT number (GPT3~6)
 ******************************************************************************/
void GPT_Start(GPT_NUM numGPT)
{
    if (numGPT == GPT1 || numGPT == GPT2)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    DRV_SetReg32(g_gpt_addr_con[numGPT], GPT_CON_CLEAR);
    DRV_SetReg32(g_gpt_addr_con[numGPT], GPT_CON_ENABLE);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

void GPT_Restart(GPT_NUM numGPT)
{
    if (numGPT == GPT1 || numGPT == GPT2)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    DRV_SetReg32(g_gpt_addr_con[numGPT], GPT_CON_ENABLE);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

/******************************************************************************
 * DESCRIPTION:
 *   Stop GPT Timer
 * PARAMETERS:
 *   GPT number (GPT3~6)
 ******************************************************************************/
void GPT_Stop(GPT_NUM numGPT)
{
    if (numGPT == GPT1 || numGPT == GPT2)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    DRV_ClrReg32(g_gpt_addr_con[numGPT], GPT_CON_ENABLE);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

/******************************************************************************
 * DESCRIPTION:
 *   Check whether GPT is running or not
 * PARAMETERS:
 *   GPT number
 * RETURNS:
 *   TRUE : running
 *   FALSE: stop
 ******************************************************************************/
BOOL GPT_IsStart(GPT_NUM numGPT)
{
    return (DRV_Reg32(g_gpt_addr_con[numGPT]) & GPT_CON_ENABLE) ? TRUE : FALSE;
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to clean GPT counter value
 *   If it has been started, stop the timer.
 *   After cleaning GPT counter value, restart this GPT counter again
 * PARAMETERS:
 *   GPT number (GPT3~6)
 ******************************************************************************/
void GPT_ClearCount(GPT_NUM numGPT)
{
    BOOL bIsStarted;

    if (numGPT == GPT1 || numGPT == GPT2)
        return;

    if(numGPT > GPT6)
        return;

    //In order to get the counter as 0 after the clear command
    //Stop the timer if it has been started.
    //after get the counter as 0, restore the timer status
    bIsStarted = GPT_IsStart(numGPT);
    if (bIsStarted)
        GPT_Stop(numGPT);

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    DRV_SetReg32(g_gpt_addr_con[numGPT], GPT_CON_CLEAR);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);

    if (bIsStarted)
        GPT_Restart(numGPT);
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to set GPT mode
 *   (GPT_ONE_SHOT,GPT_REPEAT,GPT_KEEP_GO,GPT_FREE_RUN)
 * PARAMETERS:
 *   GPT number (GPT3~6) and GPT Mode
 ******************************************************************************/
void GPT_SetOpMode(GPT_NUM numGPT, GPT_CON_MODE mode)
{
    UINT32 gptMode;

    if (numGPT == GPT1 || numGPT == GPT2)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    gptMode = DRV_Reg32(g_gpt_addr_con[numGPT]) & (~GPT_FREE_RUN);
    gptMode |= mode;
    DRV_WriteReg32(g_gpt_addr_con[numGPT], gptMode);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to get GPT mode
 *   (GPT_ONE_SHOT,GPT_REPEAT,GPT_KEEP_GO,GPT_FREE_RUN)
 * PARAMETERS:
 *   GPT number (GPT1~6)
 * RETURNS:
 *   GPT Mode
 ******************************************************************************/
GPT_CON_MODE GPT_GetOpMode(GPT_NUM numGPT)
{
    UINT32 u4Con;
    GPT_CON_MODE mode = GPT_ONE_SHOT;

    u4Con = DRV_Reg32(g_gpt_addr_con[numGPT]) & GPT_FREE_RUN;
    if (u4Con == GPT_FREE_RUN)
        mode = GPT_FREE_RUN;
    else if (u4Con == GPT_KEEP_GO)
        mode = GPT_KEEP_GO;
    else if (u4Con == GPT_REPEAT)
        mode = GPT_REPEAT;

    return mode;
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to set GPT clock divisor
 * PARAMETERS:
 *   GPT number (GPT3~6) and clock divisor
 ******************************************************************************/
void GPT_SetClkDivisor(GPT_NUM numGPT, GPT_CLK_DIV clkDiv)
{
    UINT32 div;

    if (numGPT == GPT1 || numGPT == GPT2)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    div = DRV_Reg32(g_gpt_addr_clk[numGPT]) & (~GPT_CLK_DIVISOR_MASK);
    div |= clkDiv;
    DRV_WriteReg32(g_gpt_addr_clk[numGPT], div);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to get GPT clock divisor
 * PARAMETERS:
 *   GPT number (GPT1~6)
 * RETURNS:
 *   GPT clock divisor
 ******************************************************************************/
GPT_CLK_DIV GPT_GetClkDivisor(GPT_NUM numGPT)
{
    return DRV_Reg32(g_gpt_addr_clk[numGPT]) & GPT_CLK_DIVISOR_MASK;
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to set GPT clock source
 * PARAMETERS:
 *   GPT number (GPT3~6) and clock source
 ******************************************************************************/
void GPT_SetClkSource(GPT_NUM numGPT, GPT_CLK_SRC clkSrc)
{
    UINT32 src;

    if (numGPT == GPT1 || numGPT == GPT2)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    src = DRV_Reg32(g_gpt_addr_clk[numGPT]) & (~GPT_CLK_SRC_MASK);
    src |= (clkSrc<<GPT_CLK_SRC_SHIFT_BITS);
    DRV_WriteReg32(g_gpt_addr_clk[numGPT], src);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to get GPT clock source
 * PARAMETERS:
 *   GPT number (GPT1~6)
 * RETURNS:
 *   GPT clock source
 ******************************************************************************/
GPT_CLK_SRC GPT_GetClkSource(GPT_NUM numGPT)
{
    UINT32 u4Div;
    GPT_CLK_SRC clkSrc = GPT_CLK_SRC_RESERVED;

    u4Div = DRV_Reg32(g_gpt_addr_clk[numGPT]) >> GPT_CLK_SRC_SHIFT_BITS;
    if (u4Div == GPT_CLK_SRC_SYS)
        clkSrc = GPT_CLK_SRC_SYS;
    else if (u4Div == GPT_CLK_SRC_RTC)
        clkSrc = GPT_CLK_SRC_RTC;

    return clkSrc;
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to get GPT counter
 * PARAMETERS:
 *   GPT number (GPT1~6)
 * RETURNS:
 *   GPT counter value
 ******************************************************************************/
#ifdef GPT_SOFTWARE_WORKAROUND 
UINT32 GPT_GetCounter(GPT_NUM numGPT)
{
    unsigned int cnt1, cnt2, cnt3,value1;
		cnt1 = DRV_Reg32(g_gpt_addr_cnt[numGPT]);
    cnt2 = DRV_Reg32(g_gpt_addr_cnt[numGPT]);
    cnt3 = DRV_Reg32(g_gpt_addr_cnt[numGPT]);

    if(cnt2 < cnt1)
    {
    	if(cnt1 < cnt3)
				value1 = cnt1;
			else
			{
				value1 = ((cnt2 > cnt3) ? cnt2 :cnt3);
			}
    }
    else
		{
			if(cnt2 < cnt3)
				value1 = cnt2;
			else
			{
				value1= ((cnt1 > cnt3) ? cnt1 :cnt3);
			}
		}
		return value1;
}
#else
UINT32 GPT_GetCounter(GPT_NUM numGPT)
{
    return DRV_Reg32(g_gpt_addr_cnt[numGPT]);
}
#endif

void GPT_GetCounter64(UINT32 *cntL32, UINT32 *cntH32)
{
    if (cntL32 == NULL || cntH32 == NULL)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    *cntL32 = DRV_Reg32(GPT6_COUNTL);
    *cntH32 = DRV_Reg32(GPT6_COUNTH);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to set GPT compare
 * PARAMETERS:
 *   GPT number (GPT3~6) and compare
 ******************************************************************************/
void GPT_SetCompare(GPT_NUM numGPT, UINT32 u4Compare)
{
    if (numGPT == GPT1 || numGPT == GPT2)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    DRV_WriteReg32(g_gpt_addr_compare[numGPT], u4Compare);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

void GPT_SetCompare64(UINT32 *cmpL32, UINT32 *cmpH32)
{
    if (cmpL32 == NULL || cmpH32 == NULL)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    DRV_WriteReg32(GPT6_COMPAREL, *cmpL32);
    DRV_WriteReg32(GPT6_COMPAREH, *cmpH32);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to get GPT compare
 * PARAMETERS:
 *   GPT number (GPT1~6)
 * RETURNS:
 *   GPT compare value
 ******************************************************************************/
UINT32 GPT_GetCompare(GPT_NUM numGPT)
{
    return DRV_Reg32(g_gpt_addr_compare[numGPT]);
}

void GPT_GetCompare64(UINT32 *cmpL32, UINT32 *cmpH32)
{
    if (cmpL32 == NULL || cmpH32 == NULL)
        return;

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    *cmpL32 = DRV_Reg32(GPT6_COMPAREL);
    *cmpH32 = DRV_Reg32(GPT6_COMPAREH);

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to reset GPT register
 * PARAMETERS:
 *   GPT number (GPT3~6)
 ******************************************************************************/
void GPT_Reset(GPT_NUM numGPT)
{
    UINT32 cmpL32 = 0, cmpH32 = 0;

    if (numGPT == GPT1 || numGPT == GPT2)
        return;

    if(numGPT > GPT6)
        return;

    //(1) Stop GPT
    GPT_Stop(numGPT);

    //(2) Clean GPT counter
    GPT_ClearCount(numGPT);

    //(3) Disable Interrupt
    GPT_DisableIRQ(numGPT);

    //(4) Default mode should be one shot
    GPT_SetOpMode(numGPT, GPT_ONE_SHOT);

    //(5) Default Clock Divisor should be GPT_CLK_DIV_1
    GPT_SetClkDivisor(numGPT, GPT_CLK_DIV_1);

    //(6) Clean GPT Comparator
    if (numGPT == GPT6)
        GPT_SetCompare64(&cmpL32, &cmpH32);
    else
        GPT_SetCompare(numGPT, 0);
}

/******************************************************************************
 * DESCRIPTION:
 *   Register GPT3~6 interrupt handler (call back function)
 * PARAMETERS:
 *   GPT number (GPT3~6) and handler
 ******************************************************************************/
void GPT_Init(GPT_NUM timerNum, void (*GPT_Callback)(UINT16))
{
    if (timerNum == GPT3)
        g_gpt_func.gpt3_func = GPT_Callback;
    else if (timerNum == GPT4)
        g_gpt_func.gpt4_func = GPT_Callback;
    else if (timerNum == GPT5)
        g_gpt_func.gpt5_func = GPT_Callback;
    else if (timerNum == GPT6)
        g_gpt_func.gpt6_func = GPT_Callback;
}

/******************************************************************************
 * DESCRIPTION:
 *   This function is used to config GPT register
 *   (1) mode
 *   (2) clkSrc
 *   (3) clkDiv
 *   (4) bIrqEnable
 *   (5) u4CompareL
 *   (6) u4CompareH
 * PARAMETERS:
 *   GPT configuration data structure
 * RETURNS:
 *   TRUE : config GPT pass
 *   FALSE: config GPT fail
 ******************************************************************************/
BOOL GPT_Config(GPT_CONFIG config)
{
    GPT_NUM num = config.num;
    GPT_CON_MODE mode = config.mode;
    GPT_CLK_SRC clkSrc = config.clkSrc;
    GPT_CLK_DIV clkDiv = config.clkDiv;
    BOOL bIrqEnable = config.bIrqEnable;
    UINT32 u4CompareL = config.u4CompareL;
    UINT32 u4CompareH = config.u4CompareH;

    if (num == GPT1 || num == GPT2) {
        printk("GPT_Config : GPT1 and GPT2 should not be used\n");
        return FALSE;
    }

    if(num > GPT6){
        printk("GPT_Config : Invalid GPT id:%d\n", num);
        return FALSE;
    }

    if (GPT_IsStart(num)) {
        printk("GPT_Config : GPT%d is already in use\n", num + 1);
        return FALSE;
    }

    GPT_SetOpMode(num, mode);
    GPT_SetClkSource(num, clkSrc);
    GPT_SetClkDivisor(num, clkDiv);

    if (bIrqEnable)
        GPT_EnableIRQ(num);
    else
        GPT_DisableIRQ(num);

    if (num == GPT6)
        GPT_SetCompare64(&u4CompareL, &u4CompareH);
    else
        GPT_SetCompare(num, u4CompareL);

    return TRUE;
}

/******************************************************************************
 * DESCRIPTION:
 *   Request a free timer
 * PARAMETERS:
 *   None
 ******************************************************************************/
GPT_NUM GPT_Request(void)
{
    int i=0;
    GPT_NUM free_timer;
    BOOL has_found = FALSE;
    for(i=0; i<GPT_TOTAL_COUNT; i++){
        if(g_gpt_reserved_table[i]){
            continue;
        }else{
            spin_lock_irqsave(&free_gpt_lock, free_gpt_flags);
            if(gpt_free_table[i]){
                gpt_free_table[i] = FALSE;
                has_found = TRUE;
                free_timer = g_gpt_timer_index[i];
            }
            spin_unlock_irqrestore(&free_gpt_lock, free_gpt_flags);
            if(has_found)
                break;
        }
    }
    return -1;
}


/******************************************************************************
 * DESCRIPTION:
 *   Free a un-used GPT
 * PARAMETERS:
 *   GPT number (GPT3~6)
 ******************************************************************************/
void GPT_Free(GPT_NUM timerNum)
{
    if( (timerNum < GPT1)||(timerNum >= GPT_TOTAL_COUNT) )
        return;
    if(g_gpt_reserved_table[timerNum])
        return;

    GPT_Stop(timerNum);
    GPT_DisableIRQ(timerNum);

    spin_lock_irqsave(&free_gpt_lock, free_gpt_flags);
    gpt_free_table[timerNum - GPT1] = TRUE;
    spin_unlock_irqrestore(&free_gpt_lock, free_gpt_flags);
}

/******************************************************************************
 *   GPT LISR to handle GPT1~6 interrupt
 *   In this function, it will schedule corresonding GPT tasklet
 ******************************************************************************/
static irqreturn_t GPT_LISR(int irq, void *dev_id)
{
    struct clock_event_device *evt = dev_id;
    GPT_NUM numGPT;

    numGPT = GPT_Get_IRQSTA();

    GPT_AckIRQ(numGPT);

    switch (numGPT) {
    case GPT1:
        evt->event_handler(evt);
        break;
    case GPT3:
        tasklet_schedule(&g_gpt3_tlet);
        break;
    case GPT4:
#if 0
        tasklet_schedule(&g_gpt4_tlet);
#endif
        break;
    case GPT5:
        tasklet_schedule(&g_gpt5_tlet);
        break;
    case GPT6:
        tasklet_schedule(&g_gpt6_tlet);
        break;
    default:    /* GPT2: FREERUN mode */
        printk("GPT_LISR : Invalid GPT%d interrupt\n", numGPT + 1);
        break;
    }

    //GPT_AckIRQ(numGPT);

    return IRQ_HANDLED;
}

/******************************************************************************
 Initialize GPT Module
******************************************************************************/

static void mtk_gpt_init(void);
#ifdef GPT_SOFTWARE_WORKAROUND
unsigned long long sched_clock(void)
{
 //   unsigned long long v = cnt32_to_63(DRV_Reg32(GPT2_COUNT));
     unsigned int cnt1, cnt2, cnt3,value1;
     unsigned long long v;
     cnt1 = DRV_Reg32(GPT2_COUNT);
     cnt2 = DRV_Reg32(GPT2_COUNT);
     cnt3 = DRV_Reg32(GPT2_COUNT);

     if(cnt2 < cnt1)
     {
    		if(cnt1 < cnt3)
			    value1 = cnt1;
		 		else
		 		{
					value1 = ((cnt2 > cnt3) ? cnt2 :cnt3);
		 		}
     }
     else
	   {
			if(cnt2 < cnt3)
				value1 = cnt2;
			else
			{
				value1= ((cnt1 > cnt3) ? cnt1 :cnt3);
			}
		 }
    v = cnt32_to_63(value1);

    /* the <<1 gets rid of the cnt_32_to_63 top bit saving on a bic insn */
    // 1000000000 / 13000000 = 1000 / 13
    v *= 1000 << 1;
    do_div(v, 13 << 1);

    return v;
}

static cycle_t mtk_gpt_read(struct clocksource *cs)
{
    unsigned int cnt1, cnt2, cnt3,value1;
		cnt1 = DRV_Reg32(GPT2_COUNT);
    cnt2 = DRV_Reg32(GPT2_COUNT);
    cnt3 = DRV_Reg32(GPT2_COUNT);

     if(cnt2 < cnt1)
    	{
    		if(cnt1 < cnt3)
				value1 = cnt1;
			else
			{
				value1 = ((cnt2 > cnt3) ? cnt2 :cnt3);
			}
        }
      else
		{
			if(cnt2 < cnt3)
				value1 = cnt2;
			else
			{
				value1= ((cnt1 > cnt3) ? cnt1 :cnt3);
			}
		}
	return value1;
}
#else
unsigned long long sched_clock(void)
{
    unsigned long long v = cnt32_to_63(DRV_Reg32(GPT2_COUNT));

    /* the <<1 gets rid of the cnt_32_to_63 top bit saving on a bic insn */
    // 1000000000 / 13000000 = 1000 / 13
    v *= 1000 << 1;
    do_div(v, 13 << 1);

    return v;
}

static cycle_t mtk_gpt_read(struct clocksource *cs)
{
    return DRV_Reg32(GPT2_COUNT);
}
#endif

static int mtk_gpt_set_next_event(unsigned long cycles,
                                     struct clock_event_device *evt)
{
    unsigned long ctrl;
    unsigned int count = 5000;

    //printk("Enter mt6575_gpt_set_next_event\n");

    // Stop GPT and clear count
    ctrl = DRV_Reg32(g_gpt_addr_con[TICK_GPT_ID]);
    ctrl &= (~GPT_CON_ENABLE);
    ctrl |= GPT_CON_CLEAR;
    DRV_WriteReg32(g_gpt_addr_con[TICK_GPT_ID] , ctrl);
    // Check count is cleared to zero
    while (GPT_GetCounter(TICK_GPT_ID) != 0){if(count-- == 0) break;}

    //the evt is the cycle need to wait, we just set it to compare.
    //DRV_WriteReg32(MT6573_XGPT1_COMPARE, cycles);
    DRV_WriteReg32(g_gpt_addr_compare[TICK_GPT_ID], cycles);

    // Start GPT	    
    ctrl |= GPT_CON_ENABLE;
    //DRV_WriteReg32(MT6573_XGPT1_CON, ctrl);  
    DRV_WriteReg32(g_gpt_addr_con[TICK_GPT_ID], ctrl);   	

    return 0;
}

int gpt_set_next_compare(unsigned long cycles)
{
	return mtk_gpt_set_next_event(cycles, NULL);
}

static void mtk_gpt_set_mode(enum clock_event_mode mode,
                                struct clock_event_device *evt)
{
    unsigned long ctrl;	
    //printk("Enter mt6575_gpt_set_mode\n");
    //ctrl = DRV_Reg32(MT6573_XGPT1_CON);
    ctrl = DRV_Reg32(g_gpt_addr_con[TICK_GPT_ID]);

    switch (mode) {
    case CLOCK_EVT_MODE_PERIODIC:
        //printk("Tick mode CLOCK_EVT_MODE_PERIODIC\n");
        // reset counter value		
        ctrl |= GPT_CON_CLEAR;				
        // clear gpt mode		
        ctrl = ctrl & (~GPT_FREE_RUN);		
        // set gpt for repeat mode		
        ctrl |= GPT_REPEAT;		

        // enable interrupt		
        //DRV_SetReg32(MT6573_XGPT_IRQEN, 0x0001);		
        DRV_SetReg32(GPT_IRQEN, (0x1<<TICK_GPT_ID));		

        // enable gpt		
        ctrl |= GPT_CON_ENABLE;	
        //DRV_WriteReg32(MT6573_XGPT1_CON, ctrl); 	
        DRV_WriteReg32(g_gpt_addr_con[TICK_GPT_ID], ctrl); 		
        break;
    case CLOCK_EVT_MODE_ONESHOT:
        //printk("Tick mode CLOCK_EVT_MODE_ONESHOT\n");
        // reset counter value		
        ctrl |= GPT_CON_CLEAR;		
        // clear xgpt mode		
        ctrl = ctrl & (~GPT_FREE_RUN);		
        // set xgpt for repeat mode		
        ctrl |= GPT_ONE_SHOT;		

        // enable interrupt 		
        //DRV_SetReg32(MT6573_XGPT_IRQEN, 0x0001);				
        DRV_SetReg32(GPT_IRQEN, (0x1<<TICK_GPT_ID));	

        // enable gpt		
        ctrl |= GPT_CON_ENABLE;	
        //DRV_WriteReg32(MT6573_XGPT1_CON, ctrl); 	
        DRV_WriteReg32(g_gpt_addr_con[TICK_GPT_ID], ctrl); 		
        break;
    case CLOCK_EVT_MODE_UNUSED:
    case CLOCK_EVT_MODE_SHUTDOWN:
    case CLOCK_EVT_MODE_RESUME:
        break;
    }
}

struct mt65xx_clock mtk_gpt =
{
    .clockevent =
    {
        .name           = "mtk-gpt",
        //.features       = CLOCK_EVT_FEAT_PERIODIC|CLOCK_EVT_FEAT_ONESHOT,
        .features       = CLOCK_EVT_FEAT_ONESHOT,
        .shift          = 32,
        .rating         = 300,
        .set_next_event = mtk_gpt_set_next_event,
        .set_mode       = mtk_gpt_set_mode,
    },
    .clocksource =
    {
        .name   = "mtk-gpt",
        .rating = 300,
        .read   = mtk_gpt_read,
        .mask   = CLOCKSOURCE_MASK(32),
        .shift  = 25,
        .flags  = CLOCK_SOURCE_IS_CONTINUOUS,
    },
    .irq =
    {
        .name       = "mtk-gpt",
        .flags      = IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
        .handler    = GPT_LISR,
        .dev_id     = &mtk_gpt.clockevent,
        .irq        = MT6577_APARM_GPTTIMER_IRQ_LINE,
    },
    .init_func = mtk_gpt_init,
};

static void mtk_gpt_init(void)
{
    struct clock_event_device *evt = &mtk_gpt.clockevent;
    struct clocksource *cs = &mtk_gpt.clocksource;
    GPT_NUM numGPT;

    evt->mult = div_sc(13000000, NSEC_PER_SEC, evt->shift);
    evt->max_delta_ns = clockevent_delta2ns(0xffffffff, evt);
    evt->min_delta_ns = clockevent_delta2ns(3, evt);
    evt->cpumask = cpumask_of(0);
    cs->mult = clocksource_hz2mult(13000000, cs->shift);

    mt65xx_irq_set_sens(MT6577_APARM_GPTTIMER_IRQ_LINE, MT65xx_LEVEL_SENSITIVE);
    mt65xx_irq_set_polarity(MT6577_APARM_GPTTIMER_IRQ_LINE, MT65xx_POLARITY_LOW);

    spin_lock_irqsave(&g_gpt_lock, g_gpt_flags);

    for (numGPT = GPT1; numGPT < GPT_TOTAL_COUNT; numGPT++) {
        DRV_WriteReg32(g_gpt_addr_con[numGPT], 0x0000);     /* stop */
        DRV_WriteReg32(g_gpt_addr_con[numGPT], 0x0002);     /* clear counter */
        DRV_WriteReg32(GPT_IRQACK, g_gpt_mask[numGPT]);     /* ack old irq */
    }

    DRV_WriteReg32(GPT_IRQEN, 0x0001);              /* enable GPT1 irq */

    DRV_WriteReg32(GPT1_CLK, 0x0000);               /* 13MHz */
    DRV_WriteReg32(GPT1_COMPARE, 13000000 / HZ);    /* 13000000 / 100 = 130000 */
    DRV_WriteReg32(GPT1_CON, 0x0011);               /* start REPEAT mode */

    DRV_WriteReg32(GPT2_CLK, 0x0000);               /* 13MHz */
    DRV_WriteReg32(GPT2_CON, 0x0031);               /* start FREERUN mode */

    spin_unlock_irqrestore(&g_gpt_lock, g_gpt_flags);

    printk("GPT1_COMPARE = %d, HZ = %d\n", DRV_Reg32(GPT1_COMPARE), HZ);
}

static int GPT_Proc(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;

    p += sprintf(p, "\n(HW Timer) GPT Status :\n");
    p += sprintf(p, "=========================================\n");
    p += sprintf(p, "0 = disable, 1 = enable\n");
    p += sprintf(p, "GPT1 : %d\n", GPT_IsStart(GPT1));
    p += sprintf(p, "GPT2 : %d\n", GPT_IsStart(GPT2));
    p += sprintf(p, "GPT3 : %d\n", GPT_IsStart(GPT3));
    p += sprintf(p, "GPT4 : %d\n", GPT_IsStart(GPT4));
    p += sprintf(p, "GPT5 : %d\n", GPT_IsStart(GPT5));
    p += sprintf(p, "GPT6 : %d\n", GPT_IsStart(GPT6));

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len : count;
}

static int __init gpt_mod_init(void)
{
    //Crate proc entry at /proc/gpt_stat
    create_proc_read_entry("gpt_stat", S_IRUGO, NULL, GPT_Proc, NULL);

    return 0;
}

module_init(gpt_mod_init);

EXPORT_SYMBOL(GPT_EnableIRQ);
EXPORT_SYMBOL(GPT_DisableIRQ);
EXPORT_SYMBOL(GPT_IsIRQEnable);
EXPORT_SYMBOL(GPT_Start);
EXPORT_SYMBOL(GPT_Restart);
EXPORT_SYMBOL(GPT_Stop);
EXPORT_SYMBOL(GPT_IsStart);
EXPORT_SYMBOL(GPT_ClearCount);
EXPORT_SYMBOL(GPT_SetOpMode);
EXPORT_SYMBOL(GPT_GetOpMode);
EXPORT_SYMBOL(GPT_SetClkDivisor);
EXPORT_SYMBOL(GPT_GetClkDivisor);
EXPORT_SYMBOL(GPT_SetClkSource);
EXPORT_SYMBOL(GPT_GetClkSource);
EXPORT_SYMBOL(GPT_GetCounter);
EXPORT_SYMBOL(GPT_GetCounter64);
EXPORT_SYMBOL(GPT_SetCompare);
EXPORT_SYMBOL(GPT_SetCompare64);
EXPORT_SYMBOL(GPT_GetCompare);
EXPORT_SYMBOL(GPT_GetCompare64);
EXPORT_SYMBOL(GPT_Reset);
EXPORT_SYMBOL(GPT_Init);
EXPORT_SYMBOL(GPT_Config);
EXPORT_SYMBOL(GPT_Request);
EXPORT_SYMBOL(GPT_Free);

MODULE_DESCRIPTION("MT6577 GPT Driver v0.3");
MODULE_LICENSE("GPL");
