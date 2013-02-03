/*******************************************************************************
 *
 * Filename:
 * ---------
 *   auddrv_kernel_driver.c
 *
 * Project:
 * --------
 *   Android
 *
 * Description:
 * ------------
 *   Kernel Audio Driver
 *
 * Author:
 * -------
 *   Chipeng    (mtk02308)
 *   Stan Huang (mtk01728)
 *   Charlie Lu (mtk02328)
 *
 *------------------------------------------------------------------------------
 * $Revision$
 * $Modtime:$
 * $Log:$
 *
 * 06 17 2012 weiguo.li
 * [ALPS00302429] [Need Patch] [Volunteer Patch]modify speaker driver
 * .
 *
 * 03 26 2012 weiguo.li
 * [ALPS00258848] [Need Patch] [Volunteer Patch]I2c of speaker in ICS2
 * Rollback //ALPS_SW/TRUNK/ALPS.ICS2/alps/mediatek/platform/mt6577/kernel/drivers/sound/auddrv_kernel_driver.c to revision 4
 *
 * 12 29 2011 weiguo.li
 * [ALPS00108162] [Need Patch] [Volunteer Patch]mark amp registers
 * .
 *
 * 11 10 2011 weiguo.li
 * [ALPS00091610] [Need Patch] [Volunteer Patch]chang yusu_android_speaker.c function name and modules use it
 * .
 *
 * 10 14 2011 weiguo.li
 * [ALPS00080205] [Need Patch] [Volunteer Patch] add mutex for amp device
 * .
 *
 * 09 28 2011 weiguo.li
 * [ALPS00076254] [Need Patch] [Volunteer Patch]LGE audio driver using Voicebuffer for incall
 * .
 *
 *
 *
 *******************************************************************************/


/*****************************************************************************
*                C O M P I L E R      F L A G S
******************************************************************************
*/
#undef USE_TASKLET  //if undefined, remove tasklet and do handlings in ISR directly

/*****************************************************************************
*                E X T E R N A L      R E F E R E N C E S
******************************************************************************
*/
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/completion.h>
#include <linux/mm.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/vmalloc.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/sched.h>
#include <linux/wakelock.h>
#include <linux/semaphore.h>
#include <linux/jiffies.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/mutex.h>
#include <linux/aee.h>
#include <mach/irqs.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
//#include <asm/atomic.h>
#include <linux/time.h>
#include <linux/timer.h>

//#include <asm/tcm.h>
#include <linux/pmic6326_sw.h>
#include <linux/xlog.h>
#include <mach/mt_gpio.h>
#include <mach/mt6577_irq.h>
#include <mach/mt6577_typedefs.h>
#include <mach/mt6577_clock_manager.h>
#include <mach/mt6577_boot.h>

#include "auddrv_def.h"
#include "auddrv_ioctl_msg.h"
#include "auddrv_register.h"
#include "auddrv_headset.h"
#include "yusu_android_speaker.h"
#include <mach/camera_sysram.h>
#include <mach/mt6577_pmic_feature_api.h>
#include <mach/mt6577_sc.h>
#include <mach/mt6577_pm_ldo.h>


#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
#include <mach/mt_combo.h>
#endif

#if defined(MTK_MT5192) || defined(MTK_MT5193)
extern int cust_matv_gpio_on(void);
extern int cust_matv_gpio_off(void);
#endif

/*****************************************************************************
*                          C O N S T A N T S
******************************************************************************
*/
#define AUDDRV_NAME   "MediaTek Audio Driver"
#define AUDDRV_AUTHOR "MediaTek WCX"

// internal memory usage
#define AFE_BUF_LENGTH 0x3000   // 12k bytes
#define AWB_BUF_LENGTH 0x5000  //20K bytes
#define I2S_BUF_MAX    0x5000   // 20k bytes

#ifndef MT6577_AFE_MCU_IRQ_LINE
#define MT6577_AFE_MCU_IRQ_LINE    MT6577_AFE_MCU_IRQ_ID
#endif

#define AUD_DUMPFTRACE

//#define AFE_INT_TIMEOUT       (200)  // 200 jiffies = 2000 ms
#define AFE_INT_TIMEOUT       (6)  // 6 jiffies = 60 ms
//#define I2S_INPUT_INT_TIMEOUT (500)  // jiffies
#define I2S_INPUT_INT_TIMEOUT (200)  // jiffies
#define AWB_TIMER_TIMEOUT (200)  // jiffies
#define AWB_TIMER_INTERVAL    (50)  // in ms

// ms to Jiffies--> (ms)* HZ / 1000;
// or
// ms to Jiffies--> ((ms)*HZ + 512) >> 10
#ifdef MM_SYSRAM_BASE_PA
#undef MM_SYSRAM_BASE_PA
#endif
#define MM_SYSRAM_BASE_PA			0xC2000000  //256kByte

#define USE_PM_API

#define AUD_SRAMPRINTK

typedef enum
{
    DAI_STOP,
    DAI_INIT,
    DAI_ASSERTION,
    DAI_NEGATION
}DAIRST_T;

#define    SPKAMP_CLASSD     (0)
#define    SPKAMP_CLASSAB    (1)

#define    AUD_SYSRAM_ALLOC_TIMEOUT   (100)  //= 100ms
#define    DL1_USE_SYSRAM //Audio DL1 playback use slave mode by MMsys memory
/*****************************************************************************
*                         D A T A      T Y P E S
******************************************************************************
*/
typedef struct
{
   volatile UINT32 Suspend_AUDIO_TOP_CON0;
   volatile UINT32 Suspend_AFE_DAC_CON0;
   volatile UINT32 Suspend_AFE_DAC_CON1;
   volatile UINT32 Suspend_AFE_I2S_IN_CON;
   volatile UINT32 Suspend_AFE_FOC_CON;
   volatile UINT32 Suspend_AFE_DAIBT_CON;
   volatile UINT32 Suspend_AFE_CONN0;
   volatile UINT32 Suspend_AFE_CONN1;
   volatile UINT32 Suspend_AFE_CONN2;
   volatile UINT32 Suspend_AFE_CONN3;
   volatile UINT32 Suspend_AFE_CONN4;

   volatile UINT32 Suspend_AFE_I2S_OUT_CON;
   volatile UINT32 Suspend_AFE_DL1_BASE;
   volatile UINT32 Suspend_AFE_DL1_CUR;
   volatile UINT32 Suspend_AFE_DL1_END;
   volatile UINT32 Suspend_AFE_DL2_BASE;
   volatile UINT32 Suspend_AFE_DL2_CUR;
   volatile UINT32 Suspend_AFE_DL2_END;
   volatile UINT32 Suspend_AFE_I2S_BASE;
   volatile UINT32 Suspend_AFE_I2S_CUR;
   volatile UINT32 Suspend_AFE_I2S_END;

   volatile UINT32 Suspend_AFE_AWB_BASE;
   volatile UINT32 Suspend_AFE_AWB_END;

   volatile UINT32 Suspend_AFE_IRQ_CON;
   volatile UINT32 Suspend_AFE_IR_STATUS;
   volatile UINT32 Suspend_AFE_IR_CLR;
   volatile UINT32 Suspend_AFE_IRQ_CNT1;
   volatile UINT32 Suspend_AFE_IRQ_CNT2;
   volatile UINT32 Suspend_AFE_IRQ_MON;
   volatile UINT32 Suspend_AFE_DL_SDM_CON0;
   volatile UINT32 Suspend_AFE_DL_SRC2_1;
   volatile UINT32 Suspend_AFE_DL_SRC2_2;

   volatile UINT32 Suspend_AFE_SIDETONE_CON0;
   volatile UINT32 Suspend_AFE_SIDETONE_CON1;

   volatile UINT32 Suspend_AFE_UL_SRC_0;
   volatile UINT32 Suspend_AFE_UL_SRC_1;
   volatile UINT32 Suspend_AFE_TOP_CONTROL_0;

   volatile UINT32 Suspend_AFE_SDM_GAIN_STAGE;

   volatile UINT32 Suspend_PLL_CON2  ;
   volatile UINT32 Suspend_AUDIO_CON0;
   volatile UINT32 Suspend_AUDIO_CON1;
   volatile UINT32 Suspend_AUDIO_CON2;
   volatile UINT32 Suspend_AUDIO_CON3;
   volatile UINT32 Suspend_AUDIO_CON4;
   volatile UINT32 Suspend_AUDIO_CON5;
   volatile UINT32 Suspend_AUDIO_CON6;
   volatile UINT32 Suspend_AUDIO_CON7;
   volatile UINT32 Suspend_AUDIO_CON8;
   volatile UINT32 Suspend_AUDIO_CON9;
   volatile UINT32 Suspend_AUDIO_CON10;
   volatile UINT32 Suspend_AUDIO_CON14;
   volatile UINT32 Suspend_AUDIO_CON20;
   volatile UINT32 Suspend_AUDIO_CON21;
   volatile UINT32 Suspend_AUDIO_CON22;
   volatile UINT32 Suspend_AUDIO_CON23;
   volatile UINT32 Suspend_AUDIO_CON24;
   volatile UINT32 Suspend_AUDIO_CON25;
   volatile UINT32 Suspend_AUDIO_CON26;
   volatile UINT32 Suspend_AUDIO_CON27;
   volatile UINT32 Suspend_AUDIO_CON28;
   volatile UINT32 Suspend_AUDIO_CON29;
   volatile UINT32 Suspend_AUDIO_CON30;
   volatile UINT32 Suspend_AUDIO_CON31;
   volatile UINT32 Suspend_AUDIO_CON32;

   volatile UINT32 Suspend_AUDIO_NCP0;
   volatile UINT32 Suspend_AUDIO_NCP1;
   volatile UINT32 Suspend_AUDIO_LDO0;
   volatile UINT32 Suspend_AUDIO_LDO1;
   volatile UINT32 Suspend_AUDIO_LDO2;
   volatile UINT32 Suspend_AUDIO_LDO3;
   volatile UINT32 Suspend_AUDIO_GLB0;
   volatile UINT32 Suspend_AUDIO_GLB1;
   volatile UINT32 Suspend_AUDIO_GLB2;
   volatile UINT32 Suspend_AUDIO_REG0;
   volatile UINT32 Suspend_AUDIO_REG1;


   volatile UINT32 Suspend_WR_PATH0;

}AudDrv_Suspend_Reg;

/*****************************************************************************
*           V A R I A B L E     D E L A R A T I O N
******************************************************************************
*/
static struct fasync_struct  *AudDrv_async = NULL;
static AFE_DL_CONTROL_T      AFE_dl_Control_context;
static AFE_DL_CONTROL_T      *AFE_dl_Control = &AFE_dl_Control_context;   // main control of AFE
static I2S_INPUT_CONTROL_T   I2S_input_Control_context;
static I2S_INPUT_CONTROL_T   *I2S_input_Control = &I2S_input_Control_context;   // main control of I2S input
static DAI_OUTPUT_CONTROL_T  DAI_output_Control_context;
//static DAI_OUTPUT_CONTROL_T  *DAI_output_Control = &DAI_output_Control_context;   // main control of DAI output
static AWB_INPUT_CONTROL_T  AWB_input_Control_context;
static AWB_INPUT_CONTROL_T   *AWB_input_Control = &AWB_input_Control_context;   // main control of I2S input
static AudDrv_Suspend_Reg    Suspend_reg;
static SPH_Control           SPH_Ctrl_State;
static volatile kal_uint8    Afe_irq_status  = 0;
static int Aud_Flush_cntr =0;
static int aud_dumpftrace_dbg = 0, aud_dumpftrace_done = 0, aud_blocked_cnt = 0, aud_write_idx = 0, aud_block_framesin6 = 0;
static int audio_blocked_frame[6]={0,0,0,0,0,0};

static kal_uint32 wait_queue_flag     = 0;
static kal_uint32 I2S_wait_queue_flag = 0;
static kal_uint32 AWB_wait_queue_flag = 0;
//static spinlock_t auddrv_lock         = SPIN_LOCK_UNLOCKED;
static DEFINE_SPINLOCK(auddrv_lock);
static DEFINE_SPINLOCK(auddrv_irqstatus_lock);
static DEFINE_SPINLOCK(auddrv_SphCtlState_lock);
static DEFINE_SPINLOCK(auddrv_DL1Ctl_lock);
static DEFINE_SPINLOCK(auddrv_I2SInCtl_lock);
static DEFINE_SPINLOCK(auddrv_AWBInCtl_lock);
static DEFINE_SPINLOCK(auddrv_ANACLKCtl_lock);
static DEFINE_MUTEX(gamp_mutex);

static int        Afe_Pwr_on      = 0;
static int        I2S_Pwr_on          = 0;
static int        Aud_AFE_Clk_cntr    = 0;
static int        Aud_ADC_Clk_cntr    = 0;
static int        Aud_I2S_Clk_cntr    = 0;
static int        Aud_Core_Clk_cntr   = 0;
#if 0
atomic_t Aud_AWB_Clk_cntr = ATOMIC_INIT(0);
#else
static int        Aud_AWB_Clk_cntr    = 0;
static int        Aud_ANA_Clk_cntr    = 0;
#endif
//static kal_bool   AMP_Flag            = false; // AMP status
static kal_bool   AWB_Timer_Flag  = false; // AWB_timer_Flaer
static kal_bool   IsSuspen            = false; // is suspend flag
static kal_bool   Flag_AudDrv_ClkOn_1st  = false; // 1st AudDrv ClkOn
static kal_bool   Flag_Aud_DL1_SlaveOn   = false;
static kal_bool   b_reset_afe_pwr        = false;
static kal_bool   b_reset_i2s_pwr        = false;
static int        b_adc_clk_on           = 0;
static int        b_i2s_clk_on           = 0;
static int        b_afe_clk_on           = 0;
static int        b_afe_line_in_clk_on   = 0;
static int        b_hdmi_clk_on          = 0;
static kal_uint32 m_Interrupt_time       = 0;
static int        m_first_write          = 0;
static kal_int32  gAFE_IRQ1_CNT          = 0;
static unsigned long long gLastISRTime = 0;
static unsigned long long t_ISRTime = 0;
static kal_int32 u4Afe_irq1_cnt 	= 0;
static int irq_cnt = 0;

//static kal_bool   b_SPKSound_Happen      = false;  // exception happened (AEE)

//static kal_bool   b_FM_Analog_Enable     = false;

struct wake_lock  Audio_wake_lock;
struct wake_lock  Audio_record_wake_lock;
//static kal_uint8 *AudSRAMVirtBufAddr;
static kal_uint8 *I2SInVirtBufAddr;
static kal_uint32 I2SInPhyBufAddr;
#if defined(DL1_USE_SYSRAM)
static kal_uint32 DL1InPhyBufAddr;
#endif
static char       auddrv_name[]       = "AudDrv_driver_device";
static u64        AudDrv_dmamask      = 0xffffffffUL;
static struct timer_list AWB_timer;
//For AUDIO_HQA
static int g4SpeakerType = SPKAMP_CLASSD;
/*****************************************************************************
*           F U N C T I O N      D E L A R A T I O N
******************************************************************************
*/
static int AudDrv_fasync(int fd, struct file *flip, int mode);
static int AudDrv_remap_mmap(struct file *flip, struct vm_area_struct *vma);
static int AudDrv_Read_Procmem(char *buf,char **start, off_t offset, int count , int *eof, void *data);
static void AudDrv_Store_reg(void);
static void AudDrv_Recover_reg(void);
static int AudDrv_Reset(void);
void AudDrv_Clk_On(void);
void AudDrv_Clk_Off(void);
void AudDrv_ADC_Clk_On(void);
void AudDrv_ADC_Clk_Off(void);
void AudDrv_ANA_Clk_On(void);
void AudDrv_ANA_Clk_Off(void);

void Auddrv_AWB_timer_Routine(unsigned long data);
bool Auddrv_Check_Afe_Clock(void);
void AudDrv_allcate_AWB_buffer(void);
void AudDrv_Start_AWB_Stream(struct file *fp);
void AudDrv_Standby_AWB_Stream(struct file *fp);

void AudDrv_Init_I2S_InputStream(kal_uint32 I2S_Buffer_Length,struct file *fp);

void AudDrv_Restore_Ana_Reg(void);

#ifdef USE_TASKLET
static void AudDrv_magic_tasklet(unsigned long data );
DECLARE_TASKLET(magic_tasklet_handle, AudDrv_magic_tasklet, 0L);
#endif

DECLARE_WAIT_QUEUE_HEAD(DL_Wait_Queue);
DECLARE_WAIT_QUEUE_HEAD(I2Sin_Wait_Queue);
DECLARE_WAIT_QUEUE_HEAD(AWB_Wait_Queue);

ssize_t AudDrv_I2S_Read(struct file *fp,  char __user *data, size_t count, loff_t *offset);
ssize_t AudDrv_AWB_Read(struct file *fp,  char __user *data, size_t count, loff_t *offset);
static int AudDrv_DL_Handling(void);
void AudDrv_I2Sin_Handling(void);

extern void copy_to_user_fake( char* Read_Data_Ptr , int read_size);
extern void slp_debug_for_audio(void);
/*****************************************************************************
*                  F U N C T I O N        D E F I N I T I O N
******************************************************************************
*/
#if defined(MTK_HDMI_SUPPORT)
extern int hdmi_audio_config(int samplerate);
extern int hdmi_audio_delay_mute(int latency);
#endif

void AudDrv_Test_Print(void)
{
   // AudioSys Register Setting
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_TOP_CON0  = 0x%x\n",Afe_Get_Reg(AUDIO_TOP_CON0));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DAC_CON0    = 0x%x\n",Afe_Get_Reg(AFE_DAC_CON0));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DAC_CON1    = 0x%x\n",Afe_Get_Reg(AFE_DAC_CON1));

   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL1_BASE    = 0x%x\n",Afe_Get_Reg(AFE_DL1_BASE));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL1_CUR     = 0x%x\n",Afe_Get_Reg(AFE_DL1_CUR));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL1_END     = 0x%x\n",Afe_Get_Reg(AFE_DL1_END));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_IRQ_CON     = 0x%x\n",Afe_Get_Reg(AFE_IRQ_CON));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_IR_STATUS   = 0x%x\n",Afe_Get_Reg(AFE_IR_STATUS));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_IR_CLR      = 0x%x\n",Afe_Get_Reg(AFE_IR_CLR));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_IRQ_CNT1    = 0x%x\n",Afe_Get_Reg(AFE_IRQ_CNT1));

   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL_SRC2_1   = 0x%x\n",Afe_Get_Reg(AFE_DL_SRC2_1));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL_SRC2_2   = 0x%x\n",Afe_Get_Reg(AFE_DL_SRC2_2));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL_SRC1_1   = 0x%x\n",Afe_Get_Reg(AFE_DL_SRC1_1));

   // Audio Variable Setting
  // xlog_printk(ANDROID_LOG_INFO, "Sound","AMP_Flag         = %d\n",AMP_Flag);
   xlog_printk(ANDROID_LOG_INFO, "Sound","Afe_Pwr_on   = %d\n",Afe_Pwr_on);
   xlog_printk(ANDROID_LOG_INFO, "Sound","I2S_Pwr_on       = %d\n",I2S_Pwr_on);
   xlog_printk(ANDROID_LOG_INFO, "Sound","Aud_AFE_Clk_cntr = %d\n",Aud_AFE_Clk_cntr);
   xlog_printk(ANDROID_LOG_INFO, "Sound","Aud_ADC_Clk_cntr = %d\n",Aud_ADC_Clk_cntr);
   xlog_printk(ANDROID_LOG_INFO, "Sound","Aud_I2S_Clk_cntr = %d\n",Aud_I2S_Clk_cntr);
   // Analog ABB Register Setting
   xlog_printk(ANDROID_LOG_INFO, "Sound","PLL_CON2   = 0x%x\n",Ana_Get_Reg(PLL_CON2));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON0 = 0x%x\n",Ana_Get_Reg(AUDIO_CON0));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON1 = 0x%x\n",Ana_Get_Reg(AUDIO_CON1));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON2 = 0x%x\n",Ana_Get_Reg(AUDIO_CON2));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON3 = 0x%x\n",Ana_Get_Reg(AUDIO_CON3));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON4 = 0x%x\n",Ana_Get_Reg(AUDIO_CON4));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON5 = 0x%x\n",Ana_Get_Reg(AUDIO_CON5));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON6 = 0x%x\n",Ana_Get_Reg(AUDIO_CON6));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON7 = 0x%x\n",Ana_Get_Reg(AUDIO_CON7));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON8 = 0x%x\n",Ana_Get_Reg(AUDIO_CON8));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON9 = 0x%x\n",Ana_Get_Reg(AUDIO_CON9));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON10 = 0x%x\n",Ana_Get_Reg(AUDIO_CON10));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON14 = 0x%x\n",Ana_Get_Reg(AUDIO_CON14));

    // Analog PMU Register Setting
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_NCP0 = 0x%x\n",Ana_Get_Reg(AUDIO_NCP0));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_NCP1 = 0x%x\n",Ana_Get_Reg(AUDIO_NCP1));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_LDO0 = 0x%x\n",Ana_Get_Reg(AUDIO_LDO0));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_LDO1 = 0x%x\n",Ana_Get_Reg(AUDIO_LDO1));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_LDO2 = 0x%x\n",Ana_Get_Reg(AUDIO_LDO2));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_LDO3 = 0x%x\n",Ana_Get_Reg(AUDIO_LDO3));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_GLB0 = 0x%x\n",Ana_Get_Reg(AUDIO_GLB0));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_GLB1 = 0x%x\n",Ana_Get_Reg(AUDIO_GLB1));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_GLB2 = 0x%x\n",Ana_Get_Reg(AUDIO_GLB2));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_REG0 = 0x%x\n",Ana_Get_Reg(AUDIO_REG0));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_REG1 = 0x%x\n",Ana_Get_Reg(AUDIO_REG1));
}

void AudDrv_AudReg_Log_Print(void)
{
   // AudioSys Register Setting
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_TOP_CON0  = 0x%x\n",Afe_Get_Reg(AUDIO_TOP_CON0));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DAC_CON0    = 0x%x\n",Afe_Get_Reg(AFE_DAC_CON0));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DAC_CON1    = 0x%x\n",Afe_Get_Reg(AFE_DAC_CON1));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_I2S_IN_CON  = 0x%x\n",Afe_Get_Reg(AFE_I2S_IN_CON));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_FOC_CON     = 0x%x\n",Afe_Get_Reg(AFE_FOC_CON));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DAIBT_CON   = 0x%x\n",Afe_Get_Reg(AFE_DAIBT_CON));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_CONN0       = 0x%x\n",Afe_Get_Reg(AFE_CONN0));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_CONN1       = 0x%x\n",Afe_Get_Reg(AFE_CONN1));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_CONN2       = 0x%x\n",Afe_Get_Reg(AFE_CONN2));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_CONN3       = 0x%x\n",Afe_Get_Reg(AFE_CONN3));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_CONN4       = 0x%x\n",Afe_Get_Reg(AFE_CONN4));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_I2S_OUT_CON = 0x%x\n",Afe_Get_Reg(AFE_I2S_OUT_CON));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL1_BASE    = 0x%x\n",Afe_Get_Reg(AFE_DL1_BASE));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL1_CUR     = 0x%x\n",Afe_Get_Reg(AFE_DL1_CUR));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL1_END     = 0x%x\n",Afe_Get_Reg(AFE_DL1_END));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL2_BASE    = 0x%x\n",Afe_Get_Reg(AFE_DL2_BASE));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL2_CUR     = 0x%x\n",Afe_Get_Reg(AFE_DL2_CUR));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL2_END     = 0x%x\n",Afe_Get_Reg(AFE_DL2_END));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_I2S_BASE    = 0x%x\n",Afe_Get_Reg(AFE_I2S_BASE));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_I2S_CUR     = 0x%x\n",Afe_Get_Reg(AFE_I2S_CUR));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_I2S_END     = 0x%x\n",Afe_Get_Reg(AFE_I2S_END));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_AWB_BASE    = 0x%x\n",Afe_Get_Reg(AFE_AWB_BASE));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_AWB_CUR     = 0x%x\n",Afe_Get_Reg(AFE_AWB_CUR));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_AWB_END     = 0x%x\n",Afe_Get_Reg(AFE_AWB_END));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_VUL_CUR     = 0x%x\n",Afe_Get_Reg(AFE_VUL_CUR));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DAI_CUR     = 0x%x\n",Afe_Get_Reg(AFE_DAI_CUR));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_IRQ_CON     = 0x%x\n",Afe_Get_Reg(AFE_IRQ_CON));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_IR_STATUS   = 0x%x\n",Afe_Get_Reg(AFE_IR_STATUS));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_IR_CLR      = 0x%x\n",Afe_Get_Reg(AFE_IR_CLR));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_IRQ_CNT1    = 0x%x\n",Afe_Get_Reg(AFE_IRQ_CNT1));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_IRQ_CNT2    = 0x%x\n",Afe_Get_Reg(AFE_IRQ_CNT2));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_IRQ_MON     = 0x%x\n",Afe_Get_Reg(AFE_IRQ_MON));

   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_MODEM_IRQ_CON   = 0x%x\n",Afe_Get_Reg(AFE_MODEM_IRQ_CON));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_MODEM_IR_STATUS = 0x%x\n",Afe_Get_Reg(AFE_MODEM_IR_STATUS));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_MODEM_IR_CLR    = 0x%x\n",Afe_Get_Reg(AFE_MODEM_IR_CLR));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_MODEM_IRQ_CNT1  = 0x%x\n",Afe_Get_Reg(AFE_MODEM_IRQ_CNT1));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_MODEM_IRQ_CNT2  = 0x%x\n",Afe_Get_Reg(AFE_MODEM_IRQ_CNT2));

   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL_SDM_CON0 = 0x%x\n",Afe_Get_Reg(AFE_DL_SDM_CON0));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL_SRC2_1   = 0x%x\n",Afe_Get_Reg(AFE_DL_SRC2_1));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL_SRC2_2   = 0x%x\n",Afe_Get_Reg(AFE_DL_SRC2_2));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_DL_SRC1_1   = 0x%x\n",Afe_Get_Reg(AFE_DL_SRC1_1));

   // for Audio HQA
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_UL_AGC0 = 0x%x\n",Afe_Get_Reg(AFE_UL_AGC0));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_UL_AGC5     = 0x%x\n",Afe_Get_Reg(AFE_UL_AGC5));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_UL_AGC13    = 0x%x\n",Afe_Get_Reg(AFE_UL_AGC13));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_SIDETONE_CON0 = 0x%x\n",Afe_Get_Reg(AFE_SIDETONE_CON0));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_SIDETONE_CON1 = 0x%x\n",Afe_Get_Reg(AFE_SIDETONE_CON1));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_UL_SRC_0    = 0x%x\n",Afe_Get_Reg(AFE_UL_SRC_0));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_UL_SRC_1    = 0x%x\n",Afe_Get_Reg(AFE_UL_SRC_1));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AFE_SDM_GAIN_STAGE  = 0x%x\n",Afe_Get_Reg(AFE_SDM_GAIN_STAGE));
   // Audio Variable Setting
  // xlog_printk(ANDROID_LOG_INFO, "Sound","AMP_Flag         = %d\n",AMP_Flag);
   xlog_printk(ANDROID_LOG_INFO, "Sound","Afe_Pwr_on   = %d\n",Afe_Pwr_on);
   xlog_printk(ANDROID_LOG_INFO, "Sound","I2S_Pwr_on       = %d\n",I2S_Pwr_on);
   xlog_printk(ANDROID_LOG_INFO, "Sound","Aud_AFE_Clk_cntr = %d\n",Aud_AFE_Clk_cntr);
   xlog_printk(ANDROID_LOG_INFO, "Sound","Aud_ADC_Clk_cntr = %d\n",Aud_ADC_Clk_cntr);
   xlog_printk(ANDROID_LOG_INFO, "Sound","Aud_I2S_Clk_cntr = %d\n",Aud_I2S_Clk_cntr);
   xlog_printk(ANDROID_LOG_INFO, "Sound","Aud_AWB_Clk_cntr = %d\n",Aud_AWB_Clk_cntr);
   xlog_printk(ANDROID_LOG_INFO, "Sound","Aud_ANA_Clk_cntr = %d\n",Aud_ANA_Clk_cntr);
   // Analog ABB Register Setting
   xlog_printk(ANDROID_LOG_INFO, "Sound","WR_PATH2   = 0x%x\n",Ana_Get_Reg(WR_PATH2));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON0 = 0x%x\n",Ana_Get_Reg(AUDIO_CON0));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON1 = 0x%x\n",Ana_Get_Reg(AUDIO_CON1));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON2 = 0x%x\n",Ana_Get_Reg(AUDIO_CON2));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON3 = 0x%x\n",Ana_Get_Reg(AUDIO_CON3));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON4 = 0x%x\n",Ana_Get_Reg(AUDIO_CON4));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON5 = 0x%x\n",Ana_Get_Reg(AUDIO_CON5));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON6 = 0x%x\n",Ana_Get_Reg(AUDIO_CON6));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON7 = 0x%x\n",Ana_Get_Reg(AUDIO_CON7));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON8 = 0x%x\n",Ana_Get_Reg(AUDIO_CON8));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON9 = 0x%x\n",Ana_Get_Reg(AUDIO_CON9));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON10 = 0x%x\n",Ana_Get_Reg(AUDIO_CON10));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON11 = 0x%x\n",Ana_Get_Reg(AUDIO_CON11));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON12 = 0x%x\n",Ana_Get_Reg(AUDIO_CON12));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON13 = 0x%x\n",Ana_Get_Reg(AUDIO_CON13));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON14 = 0x%x\n",Ana_Get_Reg(AUDIO_CON14));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON15 = 0x%x\n",Ana_Get_Reg(AUDIO_CON15));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON16 = 0x%x\n",Ana_Get_Reg(AUDIO_CON16));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON17 = 0x%x\n",Ana_Get_Reg(AUDIO_CON17));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON20 = 0x%x\n",Ana_Get_Reg(AUDIO_CON20));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON21 = 0x%x\n",Ana_Get_Reg(AUDIO_CON21));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON22 = 0x%x\n",Ana_Get_Reg(AUDIO_CON22));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON23 = 0x%x\n",Ana_Get_Reg(AUDIO_CON23));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON24 = 0x%x\n",Ana_Get_Reg(AUDIO_CON24));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON25 = 0x%x\n",Ana_Get_Reg(AUDIO_CON25));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON26 = 0x%x\n",Ana_Get_Reg(AUDIO_CON26));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON27 = 0x%x\n",Ana_Get_Reg(AUDIO_CON27));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON28 = 0x%x\n",Ana_Get_Reg(AUDIO_CON28));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON29 = 0x%x\n",Ana_Get_Reg(AUDIO_CON29));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON30 = 0x%x\n",Ana_Get_Reg(AUDIO_CON30));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON31 = 0x%x\n",Ana_Get_Reg(AUDIO_CON31));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON32 = 0x%x\n",Ana_Get_Reg(AUDIO_CON32));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON33 = 0x%x\n",Ana_Get_Reg(AUDIO_CON33));
   xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_CON34 = 0x%x\n",Ana_Get_Reg(AUDIO_CON34));

    // Analog PMU Register Setting
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_NCP0 = 0x%x\n",Ana_Get_Reg(AUDIO_NCP0));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_NCP1 = 0x%x\n",Ana_Get_Reg(AUDIO_NCP1));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_LDO0 = 0x%x\n",Ana_Get_Reg(AUDIO_LDO0));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_LDO1 = 0x%x\n",Ana_Get_Reg(AUDIO_LDO1));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_LDO2 = 0x%x\n",Ana_Get_Reg(AUDIO_LDO2));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_LDO3 = 0x%x\n",Ana_Get_Reg(AUDIO_LDO3));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_GLB0 = 0x%x\n",Ana_Get_Reg(AUDIO_GLB0));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_GLB1 = 0x%x\n",Ana_Get_Reg(AUDIO_GLB1));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_GLB2 = 0x%x\n",Ana_Get_Reg(AUDIO_GLB2));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_REG0 = 0x%x\n",Ana_Get_Reg(AUDIO_REG0));
    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDIO_REG1 = 0x%x\n",Ana_Get_Reg(AUDIO_REG1));

    xlog_printk(ANDROID_LOG_INFO, "Sound","WR_PATH0 = 0x%x\n",Ana_Get_Reg(WR_PATH0));

    // Speech/Record/BGS/TTY/VT Setting
    xlog_printk(ANDROID_LOG_INFO, "Sound","Speech_on  = %d\n",SPH_Ctrl_State.bSpeechFlag);
    xlog_printk(ANDROID_LOG_INFO, "Sound","BGS_on     = %d\n",SPH_Ctrl_State.bBgsFlag);
    xlog_printk(ANDROID_LOG_INFO, "Sound","Record_on  = %d\n",SPH_Ctrl_State.bRecordFlag);
    xlog_printk(ANDROID_LOG_INFO, "Sound","TTY_on     = %d\n",SPH_Ctrl_State.bTtyFlag);
    xlog_printk(ANDROID_LOG_INFO, "Sound","VT_on      = %d\n",SPH_Ctrl_State.bVT);
    xlog_printk(ANDROID_LOG_INFO, "Sound","Aud_on     = %d\n",SPH_Ctrl_State.bAudioPlay);
}

/*****************************************************************************
 * FUNCTION
 *  LouderSPKSound
 *
 * DESCRIPTION
 *  For testing. Fire a warning tone
 *
 * PARAMETERS
 *  time  [in]  unit in ms
 *
 * RETURNS
 *  None
 *
 *****************************************************************************
 */
static void AudDrv_Store_Reg_LouderSPKSound(void)
{
	xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_Store_Reg_LouderSPKSound \n");
   // Digital register setting
   Suspend_reg.Suspend_AFE_DAC_CON0    = Afe_Get_Reg(AFE_DAC_CON0);
   Suspend_reg.Suspend_AFE_DL_SDM_CON0 = Afe_Get_Reg(AFE_DL_SDM_CON0);
   Suspend_reg.Suspend_AFE_DL_SRC2_1   = Afe_Get_Reg(AFE_DL_SRC2_1);
   Suspend_reg.Suspend_AFE_DL_SRC2_2   = Afe_Get_Reg(AFE_DL_SRC2_2);
   Suspend_reg.Suspend_AFE_UL_SRC_1      = Afe_Get_Reg(AFE_UL_SRC_1);
   Suspend_reg.Suspend_AFE_TOP_CONTROL_0 = Afe_Get_Reg(AFE_TOP_CONTROL_0);
   Suspend_reg.Suspend_AFE_SDM_GAIN_STAGE = Afe_Get_Reg(AFE_SDM_GAIN_STAGE);
   Suspend_reg.Suspend_AFE_I2S_BASE    = Afe_Get_Reg(AFE_I2S_BASE);
   Suspend_reg.Suspend_AFE_I2S_END     = Afe_Get_Reg(AFE_I2S_END);

   // Analog register setting
   Suspend_reg.Suspend_AUDIO_CON0 = Ana_Get_Reg(AUDIO_CON0);
   Suspend_reg.Suspend_AUDIO_CON1 = Ana_Get_Reg(AUDIO_CON1);
   Suspend_reg.Suspend_AUDIO_CON2 = Ana_Get_Reg(AUDIO_CON2);
   Suspend_reg.Suspend_AUDIO_CON3 = Ana_Get_Reg(AUDIO_CON3);
   Suspend_reg.Suspend_AUDIO_CON4 = Ana_Get_Reg(AUDIO_CON4);
   Suspend_reg.Suspend_AUDIO_CON5 = Ana_Get_Reg(AUDIO_CON5);
   Suspend_reg.Suspend_AUDIO_CON6 = Ana_Get_Reg(AUDIO_CON6);
   Suspend_reg.Suspend_AUDIO_CON7 = Ana_Get_Reg(AUDIO_CON7);
   Suspend_reg.Suspend_AUDIO_CON8 = Ana_Get_Reg(AUDIO_CON8);
   Suspend_reg.Suspend_AUDIO_CON9 = Ana_Get_Reg(AUDIO_CON9);
   Suspend_reg.Suspend_AUDIO_CON10 = Ana_Get_Reg(AUDIO_CON10);
   Suspend_reg.Suspend_AUDIO_CON14 = Ana_Get_Reg(AUDIO_CON14);

   Suspend_reg.Suspend_AUDIO_NCP0 = Ana_Get_Reg(AUDIO_NCP0);
   Suspend_reg.Suspend_AUDIO_NCP1 = Ana_Get_Reg(AUDIO_NCP1);
   Suspend_reg.Suspend_AUDIO_LDO0 = Ana_Get_Reg(AUDIO_LDO0);
   Suspend_reg.Suspend_AUDIO_LDO1 = Ana_Get_Reg(AUDIO_LDO1);
   Suspend_reg.Suspend_AUDIO_LDO2 = Ana_Get_Reg(AUDIO_LDO2);
   Suspend_reg.Suspend_AUDIO_LDO3 = Ana_Get_Reg(AUDIO_LDO3);
   Suspend_reg.Suspend_AUDIO_GLB0 = Ana_Get_Reg(AUDIO_GLB0);
   Suspend_reg.Suspend_AUDIO_GLB1 = Ana_Get_Reg(AUDIO_GLB1);
   Suspend_reg.Suspend_AUDIO_GLB2 = Ana_Get_Reg(AUDIO_GLB2);
   Suspend_reg.Suspend_AUDIO_REG0 = Ana_Get_Reg(AUDIO_REG0);
   Suspend_reg.Suspend_AUDIO_REG1 = Ana_Get_Reg(AUDIO_REG1);



   Afe_Set_Reg(AFE_IR_CLR,0x001f,0x00ff);    // clear interrupt
   mdelay(2);                                // wait for a shor latency
   Afe_Set_Reg(AFE_IR_CLR,0x001f,0x00ff);    // clear interrupt

   PRINTK_AUDDRV("-AudDrv_Store_Reg_LouderSPKSound \n");
}

static void AudDrv_Recover_Reg_LouderSPKSound(void)
{
	xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_Recover_Reg_LouderSPKSound \n");

   // Digital register setting
   // don't care the "I2S ON" buts
   Afe_Set_Reg(AFE_I2S_BASE,Suspend_reg.Suspend_AFE_I2S_BASE, MASK_ALL);
   Afe_Set_Reg(AFE_I2S_END,Suspend_reg.Suspend_AFE_I2S_END, MASK_ALL);
   Afe_Set_Reg(AFE_DAC_CON0,Suspend_reg.Suspend_AFE_DAC_CON0, MASK_ALL);
   Afe_Set_Reg(AFE_DL_SDM_CON0,Suspend_reg.Suspend_AFE_DL_SDM_CON0, MASK_ALL);
   Afe_Set_Reg(AFE_DL_SRC2_1,Suspend_reg.Suspend_AFE_DL_SRC2_1, MASK_ALL);
   Afe_Set_Reg(AFE_DL_SRC2_2,Suspend_reg.Suspend_AFE_DL_SRC2_2, MASK_ALL);
   Afe_Set_Reg(AFE_UL_SRC_1,Suspend_reg.Suspend_AFE_UL_SRC_1, MASK_ALL);
   Afe_Set_Reg(AFE_TOP_CONTROL_0,Suspend_reg.Suspend_AFE_TOP_CONTROL_0, MASK_ALL);
   Afe_Set_Reg(AFE_SDM_GAIN_STAGE,Suspend_reg.Suspend_AFE_SDM_GAIN_STAGE, MASK_ALL);

   // Analog register setting
   AudDrv_ANA_Clk_On();
   Ana_Set_Reg(AUDIO_CON0,Suspend_reg.Suspend_AUDIO_CON0, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON1,Suspend_reg.Suspend_AUDIO_CON1, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON2,Suspend_reg.Suspend_AUDIO_CON2, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON3,Suspend_reg.Suspend_AUDIO_CON3, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON4,Suspend_reg.Suspend_AUDIO_CON4, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON5,Suspend_reg.Suspend_AUDIO_CON5, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON6,Suspend_reg.Suspend_AUDIO_CON6, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON7,Suspend_reg.Suspend_AUDIO_CON7, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON8,Suspend_reg.Suspend_AUDIO_CON8, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON9,Suspend_reg.Suspend_AUDIO_CON9, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON10,Suspend_reg.Suspend_AUDIO_CON10, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON14,Suspend_reg.Suspend_AUDIO_CON14, MASK_ALL);

   Ana_Set_Reg(AUDIO_NCP0,Suspend_reg.Suspend_AUDIO_NCP0, MASK_ALL);
   Ana_Set_Reg(AUDIO_NCP1,Suspend_reg.Suspend_AUDIO_NCP1, MASK_ALL);
   Ana_Set_Reg(AUDIO_LDO0,Suspend_reg.Suspend_AUDIO_LDO0, MASK_ALL);
   Ana_Set_Reg(AUDIO_LDO1,Suspend_reg.Suspend_AUDIO_LDO1, MASK_ALL);
   Ana_Set_Reg(AUDIO_LDO2,Suspend_reg.Suspend_AUDIO_LDO2, MASK_ALL);
   Ana_Set_Reg(AUDIO_LDO3,Suspend_reg.Suspend_AUDIO_LDO3, MASK_ALL);
   Ana_Set_Reg(AUDIO_GLB0,Suspend_reg.Suspend_AUDIO_GLB0, MASK_ALL);
   Ana_Set_Reg(AUDIO_GLB1,Suspend_reg.Suspend_AUDIO_GLB1, MASK_ALL);
   Ana_Set_Reg(AUDIO_GLB2,Suspend_reg.Suspend_AUDIO_GLB2, MASK_ALL);
   Ana_Set_Reg(AUDIO_REG0,Suspend_reg.Suspend_AUDIO_REG0, MASK_ALL);
   Ana_Set_Reg(AUDIO_REG1,Suspend_reg.Suspend_AUDIO_REG1, MASK_ALL);
   AudDrv_ANA_Clk_Off();

	PRINTK_AUDDRV("-AudDrv_Recover_Reg_LouderSPKSound \n");
}



int LouderSPKSound(kal_uint32 time)
{
   xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv LouderSPKSound time = %d ms \n",time);
   AudDrv_Clk_On();

   if(time > 250){
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv LouderSPKSound +Dump Audio Reg \n");
//      AudDrv_AudReg_Log_Print();
      xlog_printk(ANDROID_LOG_INFO, "Sound",". \n");
      xlog_printk(ANDROID_LOG_INFO, "Sound",".. \n");
   }

   //start SinWave
   {
      AudDrv_Store_Reg_LouderSPKSound();
      spin_lock_bh(&auddrv_lock);

      AudDrv_ANA_Clk_On();
      Ana_Set_Reg(AUDIO_CON0,0x0000,0xffffffff);
      Ana_Set_Reg(AUDIO_CON1,0x0C0C,0xffffffff);
      Ana_Set_Reg(AUDIO_CON2,0x000C,0xffffffff);
      Ana_Set_Reg(AUDIO_CON3,0x01B0,0xffffffff);
      Ana_Set_Reg(AUDIO_CON4,0x1818,0xffffffff);
      Ana_Set_Reg(AUDIO_CON5,0x0440,0xffffffff);
      Ana_Set_Reg(AUDIO_CON6,0x0533,0xffffffff);
      Ana_Set_Reg(AUDIO_CON7,0x003f,0xffffffff);
      Ana_Set_Reg(AUDIO_CON8,0x0000,0xffffffff);
      Ana_Set_Reg(AUDIO_CON9,0x0052,0xffffffff);
      Ana_Set_Reg(AUDIO_CON14,0x0040,0xffffffff);
      Ana_Set_Reg(AUDIO_CON17,0x0000,0xffffffff);
      Ana_Set_Reg(AUDIO_CON9,0x0057,0xffffffff);
      Ana_Set_Reg(AUDIO_CON10,0x01a1,0xffffffff);
      //analog power
      Ana_Set_Reg(AUDIO_NCP0,0x102B,0xffff);
      Ana_Set_Reg(AUDIO_NCP1,0x0600,0xffff);
      Ana_Set_Reg(AUDIO_LDO0,0x1030,0xffff);
      Ana_Set_Reg(AUDIO_LDO1,0x3010,0xffff);
      Ana_Set_Reg(AUDIO_LDO2,0x0013,0xffff);
      Ana_Set_Reg(AUDIO_GLB0,0x2920,0xffff);
      Ana_Set_Reg(AUDIO_GLB1,0x0000,0xffff);
      Ana_Set_Reg(AUDIO_REG1,0x0001,0xffff);
      AudDrv_ANA_Clk_Off();
      //digital part
      //Afe_Set_Reg(AFE_DL_SRC1_1,0x1,0x1);
      Afe_Set_Reg(AFE_DL_SRC2_1,0x83000003,0xf3000003);
      Afe_Set_Reg(AFE_DL_SRC2_2,0xf0000000,0xffff0000);  // gain setting twice  -66dBv:0x00400000 0dBv:0xf0000000 mute:0x00000000
      Afe_Set_Reg(AFE_UL_SRC_1,0x08828828,0xffffffff);   // freq setting 750Hz: 1.5KHz:0x08828828
      Afe_Set_Reg(AFE_SDM_GAIN_STAGE,0x1e,0xffff00ff);  // SDM gain (1e->1c)
      Afe_Set_Reg(AFE_DL_SDM_CON0,0x08800000,0xffffffff);  // SDM choose (2-Order 9-Bit Scrambler and No Dithering)
      Ana_Set_Reg(WR_PATH0,0x8020,0x8020);   // FIFO Clock Edge Control <--
      Afe_Set_Reg(AFE_DAC_CON0,0x1,0x1);
      udelay(150);//Due to spec definition.
      Afe_Set_Reg(AFE_TOP_CONTROL_0,0x40000000,0xffffffff);

      spin_unlock_bh(&auddrv_lock);

      // turn on speaker
      mutex_lock(&gamp_mutex);
      AudioAMPDevice_SpeakerLouderOpen();
	  mutex_unlock(&gamp_mutex);
      msleep(time); // sleep
   }
   // stop SineWave
   {
      spin_lock_bh(&auddrv_lock);
      Afe_Set_Reg(AFE_UL_SRC_1,0x0,0xffffffff);
      AudDrv_Recover_Reg_LouderSPKSound();
      spin_unlock_bh(&auddrv_lock);

     // if(AMP_Flag== false){
      mutex_lock(&gamp_mutex);
      AudioAMPDevice_SpeakerLouderClose();
	  mutex_unlock(&gamp_mutex);
     // }
      }

   AudDrv_Clk_Off();
   xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv LouderSPKSound \n");
   return true;
}
EXPORT_SYMBOL(LouderSPKSound);

static void AudDrv_Store_reg(void)
{
	xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_Store_reg \n");

   AudDrv_Clk_On();
   // Digital register setting
   Suspend_reg.Suspend_AFE_DAC_CON0    = Afe_Get_Reg(AFE_DAC_CON0);
   Suspend_reg.Suspend_AFE_DAC_CON1    = Afe_Get_Reg(AFE_DAC_CON1);
   Suspend_reg.Suspend_AFE_I2S_IN_CON  = Afe_Get_Reg(AFE_I2S_IN_CON);
   Suspend_reg.Suspend_AFE_FOC_CON     = Afe_Get_Reg(AFE_FOC_CON);
   Suspend_reg.Suspend_AFE_IRQ_CNT2    = Afe_Get_Reg(AFE_IRQ_CNT2);
   Suspend_reg.Suspend_AFE_DAIBT_CON   = Afe_Get_Reg(AFE_DAIBT_CON);
   Suspend_reg.Suspend_AFE_CONN0       = Afe_Get_Reg(AFE_CONN0);
   Suspend_reg.Suspend_AFE_CONN1       = Afe_Get_Reg(AFE_CONN1);
   Suspend_reg.Suspend_AFE_CONN2       = Afe_Get_Reg(AFE_CONN2);
   Suspend_reg.Suspend_AFE_CONN3       = Afe_Get_Reg(AFE_CONN3);
   Suspend_reg.Suspend_AFE_CONN4       = Afe_Get_Reg(AFE_CONN4);
   Suspend_reg.Suspend_AFE_I2S_OUT_CON = Afe_Get_Reg(AFE_I2S_OUT_CON);
   Suspend_reg.Suspend_AFE_DL1_BASE    = Afe_Get_Reg(AFE_DL1_BASE);
//   Suspend_reg.Suspend_AFE_DL1_CUR     = Afe_Get_Reg(AFE_DL1_CUR);
   Suspend_reg.Suspend_AFE_DL1_END     = Afe_Get_Reg(AFE_DL1_END);
   Suspend_reg.Suspend_AFE_DL2_BASE    = Afe_Get_Reg(AFE_DL2_BASE);
//   Suspend_reg.Suspend_AFE_DL2_CUR     = Afe_Get_Reg(AFE_DL2_CUR);
   Suspend_reg.Suspend_AFE_DL2_END     = Afe_Get_Reg(AFE_DL2_END);
   Suspend_reg.Suspend_AFE_I2S_BASE    = Afe_Get_Reg(AFE_I2S_BASE);
//   Suspend_reg.Suspend_AFE_I2S_CUR     = Afe_Get_Reg(AFE_I2S_CUR);
   Suspend_reg.Suspend_AFE_I2S_END     = Afe_Get_Reg(AFE_I2S_END);
   Suspend_reg.Suspend_AFE_AWB_BASE   = Afe_Get_Reg(AFE_AWB_BASE);
   Suspend_reg.Suspend_AFE_AWB_END	   = Afe_Get_Reg(AFE_AWB_END);

   Suspend_reg.Suspend_AFE_DL_SDM_CON0 = Afe_Get_Reg(AFE_DL_SDM_CON0);
   Suspend_reg.Suspend_AFE_DL_SRC2_1   = Afe_Get_Reg(AFE_DL_SRC2_1);
   Suspend_reg.Suspend_AFE_DL_SRC2_2   = Afe_Get_Reg(AFE_DL_SRC2_2);
   Suspend_reg.Suspend_AFE_SIDETONE_CON0 = Afe_Get_Reg(AFE_SIDETONE_CON0);
   Suspend_reg.Suspend_AFE_SIDETONE_CON1 = Afe_Get_Reg(AFE_SIDETONE_CON1);

   Suspend_reg.Suspend_AFE_UL_SRC_0      = Afe_Get_Reg(AFE_UL_SRC_0);
   Suspend_reg.Suspend_AFE_UL_SRC_1      = Afe_Get_Reg(AFE_UL_SRC_1);
   Suspend_reg.Suspend_AFE_TOP_CONTROL_0 = Afe_Get_Reg(AFE_TOP_CONTROL_0);

   Suspend_reg.Suspend_AFE_SDM_GAIN_STAGE = Afe_Get_Reg(AFE_SDM_GAIN_STAGE);
#ifdef AUD_SRAMPRINTK
   aee_sram_printk("AudDrv_Store_reg AFE done \n");
#endif


   // Analog register setting
   //Suspend_reg.Suspend_PLL_CON2   = Ana_Get_Reg(PLL_CON2);
   Suspend_reg.Suspend_AUDIO_CON0 = Ana_Get_Reg(AUDIO_CON0);
   Suspend_reg.Suspend_AUDIO_CON1 = Ana_Get_Reg(AUDIO_CON1);
   Suspend_reg.Suspend_AUDIO_CON2 = Ana_Get_Reg(AUDIO_CON2);
   Suspend_reg.Suspend_AUDIO_CON3 = Ana_Get_Reg(AUDIO_CON3);
   Suspend_reg.Suspend_AUDIO_CON4 = Ana_Get_Reg(AUDIO_CON4);
   Suspend_reg.Suspend_AUDIO_CON5 = Ana_Get_Reg(AUDIO_CON5);
   Suspend_reg.Suspend_AUDIO_CON6 = Ana_Get_Reg(AUDIO_CON6);
   Suspend_reg.Suspend_AUDIO_CON7 = Ana_Get_Reg(AUDIO_CON7);
   Suspend_reg.Suspend_AUDIO_CON8 = Ana_Get_Reg(AUDIO_CON8);
   Suspend_reg.Suspend_AUDIO_CON9 = Ana_Get_Reg(AUDIO_CON9);
   Suspend_reg.Suspend_AUDIO_CON10 = Ana_Get_Reg(AUDIO_CON10);
   Suspend_reg.Suspend_AUDIO_CON14 = Ana_Get_Reg(AUDIO_CON14);

   Suspend_reg.Suspend_AUDIO_CON20 = Ana_Get_Reg(AUDIO_CON20);
   Suspend_reg.Suspend_AUDIO_CON21 = Ana_Get_Reg(AUDIO_CON21);
   Suspend_reg.Suspend_AUDIO_CON22 = Ana_Get_Reg(AUDIO_CON22);
   Suspend_reg.Suspend_AUDIO_CON23 = Ana_Get_Reg(AUDIO_CON23);
   Suspend_reg.Suspend_AUDIO_CON24 = Ana_Get_Reg(AUDIO_CON24);
   Suspend_reg.Suspend_AUDIO_CON25 = Ana_Get_Reg(AUDIO_CON25);
   Suspend_reg.Suspend_AUDIO_CON26 = Ana_Get_Reg(AUDIO_CON26);
   Suspend_reg.Suspend_AUDIO_CON27 = Ana_Get_Reg(AUDIO_CON27);
   Suspend_reg.Suspend_AUDIO_CON28 = Ana_Get_Reg(AUDIO_CON28);
   Suspend_reg.Suspend_AUDIO_CON29 = Ana_Get_Reg(AUDIO_CON29);
   Suspend_reg.Suspend_AUDIO_CON30 = Ana_Get_Reg(AUDIO_CON30);
   Suspend_reg.Suspend_AUDIO_CON31 = Ana_Get_Reg(AUDIO_CON31);
   Suspend_reg.Suspend_AUDIO_CON32 = Ana_Get_Reg(AUDIO_CON32);

   Suspend_reg.Suspend_AUDIO_NCP0 = Ana_Get_Reg(AUDIO_NCP0);
   Suspend_reg.Suspend_AUDIO_NCP1 = Ana_Get_Reg(AUDIO_NCP1);
   Suspend_reg.Suspend_AUDIO_LDO0 = Ana_Get_Reg(AUDIO_LDO0);
   Suspend_reg.Suspend_AUDIO_LDO1 = Ana_Get_Reg(AUDIO_LDO1);
   Suspend_reg.Suspend_AUDIO_LDO2 = Ana_Get_Reg(AUDIO_LDO2);
   Suspend_reg.Suspend_AUDIO_LDO3 = Ana_Get_Reg(AUDIO_LDO3);
   Suspend_reg.Suspend_AUDIO_GLB0 = Ana_Get_Reg(AUDIO_GLB0);
   Suspend_reg.Suspend_AUDIO_GLB1 = Ana_Get_Reg(AUDIO_GLB1);
   Suspend_reg.Suspend_AUDIO_GLB2 = Ana_Get_Reg(AUDIO_GLB2);
   Suspend_reg.Suspend_AUDIO_REG0 = Ana_Get_Reg(AUDIO_REG0);
   Suspend_reg.Suspend_AUDIO_REG1 = Ana_Get_Reg(AUDIO_REG1);


   Suspend_reg.Suspend_WR_PATH0 = Ana_Get_Reg(WR_PATH0);

//   Afe_Set_Reg(AFE_DAC_CON0,0x0,0xffffffff); // turn off all module
   Afe_Set_Reg(AFE_IR_CLR,0x001f,0x00ff);    // clear interrupt
//   mdelay(5);                                // wait for a shor latency
//   Afe_Set_Reg(AFE_IR_CLR,0x001f,0x00ff);    // clear interrupt
#ifdef AUD_SRAMPRINTK
      aee_sram_printk("AudDrv_Store_reg ANA done \n");
#endif

   AudDrv_Clk_Off();
	PRINTK_AUDDRV("-AudDrv_Store_reg \n");
}

static void AudDrv_Store_reg_AFE(void)
{
	//xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_Store_reg_AFE \n");

   // Digital register setting
   Suspend_reg.Suspend_AFE_DAC_CON0    = Afe_Get_Reg(AFE_DAC_CON0);
   Suspend_reg.Suspend_AFE_DAC_CON1    = Afe_Get_Reg(AFE_DAC_CON1);
   Suspend_reg.Suspend_AFE_I2S_IN_CON  = Afe_Get_Reg(AFE_I2S_IN_CON);
   Suspend_reg.Suspend_AFE_FOC_CON     = Afe_Get_Reg(AFE_FOC_CON);
   Suspend_reg.Suspend_AFE_IRQ_CNT2    = Afe_Get_Reg(AFE_IRQ_CNT2);
   Suspend_reg.Suspend_AFE_DAIBT_CON   = Afe_Get_Reg(AFE_DAIBT_CON);
   Suspend_reg.Suspend_AFE_CONN0       = Afe_Get_Reg(AFE_CONN0);
   Suspend_reg.Suspend_AFE_CONN1       = Afe_Get_Reg(AFE_CONN1);
   Suspend_reg.Suspend_AFE_CONN2       = Afe_Get_Reg(AFE_CONN2);
   Suspend_reg.Suspend_AFE_CONN3       = Afe_Get_Reg(AFE_CONN3);
   Suspend_reg.Suspend_AFE_CONN4       = Afe_Get_Reg(AFE_CONN4);
   Suspend_reg.Suspend_AFE_I2S_OUT_CON = Afe_Get_Reg(AFE_I2S_OUT_CON);
   Suspend_reg.Suspend_AFE_DL1_BASE    = Afe_Get_Reg(AFE_DL1_BASE);
//   Suspend_reg.Suspend_AFE_DL1_CUR     = Afe_Get_Reg(AFE_DL1_CUR);
   Suspend_reg.Suspend_AFE_DL1_END     = Afe_Get_Reg(AFE_DL1_END);
   Suspend_reg.Suspend_AFE_DL2_BASE    = Afe_Get_Reg(AFE_DL2_BASE);
//   Suspend_reg.Suspend_AFE_DL2_CUR     = Afe_Get_Reg(AFE_DL2_CUR);
   Suspend_reg.Suspend_AFE_DL2_END     = Afe_Get_Reg(AFE_DL2_END);
   Suspend_reg.Suspend_AFE_I2S_BASE    = Afe_Get_Reg(AFE_I2S_BASE);
//   Suspend_reg.Suspend_AFE_I2S_CUR     = Afe_Get_Reg(AFE_I2S_CUR);
   Suspend_reg.Suspend_AFE_I2S_END     = Afe_Get_Reg(AFE_I2S_END);
   Suspend_reg.Suspend_AFE_AWB_BASE    = Afe_Get_Reg(AFE_AWB_BASE);
   Suspend_reg.Suspend_AFE_AWB_END     = Afe_Get_Reg(AFE_AWB_END);
   Suspend_reg.Suspend_AFE_DL_SDM_CON0 = Afe_Get_Reg(AFE_DL_SDM_CON0);
   Suspend_reg.Suspend_AFE_DL_SRC2_1   = Afe_Get_Reg(AFE_DL_SRC2_1);
   Suspend_reg.Suspend_AFE_DL_SRC2_2   = Afe_Get_Reg(AFE_DL_SRC2_2);
   Suspend_reg.Suspend_AFE_SIDETONE_CON0 = Afe_Get_Reg(AFE_SIDETONE_CON0);
   Suspend_reg.Suspend_AFE_SIDETONE_CON1 = Afe_Get_Reg(AFE_SIDETONE_CON1);

   Suspend_reg.Suspend_AFE_UL_SRC_0      = Afe_Get_Reg(AFE_UL_SRC_0);
   Suspend_reg.Suspend_AFE_UL_SRC_1      = Afe_Get_Reg(AFE_UL_SRC_1);
   Suspend_reg.Suspend_AFE_TOP_CONTROL_0 = Afe_Get_Reg(AFE_TOP_CONTROL_0);

   Suspend_reg.Suspend_AFE_SDM_GAIN_STAGE = Afe_Get_Reg(AFE_SDM_GAIN_STAGE);

	Afe_Set_Reg(AFE_IRQ_CON,0x0,MASK_ALL); 	// disable IRQ_CON
	Afe_Set_Reg(AFE_IR_CLR,0x001f,0x00ff);    // clear interrupt
	Afe_Set_Reg(AFE_IRQ_CNT1,0x0,MASK_ALL);	// set IRQ_CNT1 to 0
	Afe_Set_Reg(AFE_IRQ_CNT2,0x0,MASK_ALL);	// set IRQ_CNT2 to 0

	PRINTK_AUDDRV("-AudDrv_Store_reg_AFE \n");
}

static void AudDrv_Recover_reg(void)
{
   xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_Recover_reg\n");
   AudDrv_Clk_On();
   xlog_printk(ANDROID_LOG_INFO, "Sound","++AudDrv_Recover_reg AudDrv_Clk_On() after\n");
   // Digital register setting
   Afe_Set_Reg(AFE_DAC_CON1,Suspend_reg.Suspend_AFE_DAC_CON1, MASK_ALL);
   Afe_Set_Reg(AFE_I2S_IN_CON,Suspend_reg.Suspend_AFE_I2S_IN_CON, MASK_ALL);
   Afe_Set_Reg(AFE_FOC_CON,Suspend_reg.Suspend_AFE_FOC_CON, MASK_ALL);
   Afe_Set_Reg(AFE_IRQ_CNT2,Suspend_reg.Suspend_AFE_IRQ_CNT2, MASK_ALL);
   Afe_Set_Reg(AFE_DAIBT_CON,Suspend_reg.Suspend_AFE_DAIBT_CON, MASK_ALL);
   Afe_Set_Reg(AFE_CONN0,Suspend_reg.Suspend_AFE_CONN0, MASK_ALL);
   Afe_Set_Reg(AFE_CONN1,Suspend_reg.Suspend_AFE_CONN1, MASK_ALL);
   Afe_Set_Reg(AFE_CONN2,Suspend_reg.Suspend_AFE_CONN2, MASK_ALL);
   Afe_Set_Reg(AFE_CONN3,Suspend_reg.Suspend_AFE_CONN3, MASK_ALL);
   Afe_Set_Reg(AFE_CONN4,Suspend_reg.Suspend_AFE_CONN4, MASK_ALL);
   Afe_Set_Reg(AFE_DAC_CON0,Suspend_reg.Suspend_AFE_DAC_CON0, MASK_ALL);
   Afe_Set_Reg(AFE_I2S_OUT_CON,Suspend_reg.Suspend_AFE_I2S_OUT_CON, MASK_ALL);

   Afe_Set_Reg(AFE_DL1_BASE,Suspend_reg.Suspend_AFE_DL1_BASE, MASK_ALL);
   Afe_Set_Reg(AFE_DL1_END,Suspend_reg.Suspend_AFE_DL1_END, MASK_ALL);
   Afe_Set_Reg(AFE_DL2_BASE,Suspend_reg.Suspend_AFE_DL2_BASE, MASK_ALL);
   Afe_Set_Reg(AFE_DL2_END,Suspend_reg.Suspend_AFE_DL2_END, MASK_ALL);
   Afe_Set_Reg(AFE_I2S_BASE,Suspend_reg.Suspend_AFE_I2S_BASE, MASK_ALL);
   Afe_Set_Reg(AFE_I2S_END,Suspend_reg.Suspend_AFE_I2S_END, MASK_ALL);
   Afe_Set_Reg(AFE_AWB_BASE,Suspend_reg.Suspend_AFE_AWB_BASE, MASK_ALL);
   Afe_Set_Reg(AFE_AWB_END,Suspend_reg.Suspend_AFE_AWB_END, MASK_ALL);

   Afe_Set_Reg(AFE_DL_SDM_CON0,Suspend_reg.Suspend_AFE_DL_SDM_CON0, MASK_ALL);
   Afe_Set_Reg(AFE_DL_SRC2_1,Suspend_reg.Suspend_AFE_DL_SRC2_1, MASK_ALL);
   Afe_Set_Reg(AFE_DL_SRC2_2,Suspend_reg.Suspend_AFE_DL_SRC2_2, MASK_ALL);
   Afe_Set_Reg(AFE_SIDETONE_CON0,Suspend_reg.Suspend_AFE_SIDETONE_CON0, MASK_ALL);
   Afe_Set_Reg(AFE_SIDETONE_CON1,Suspend_reg.Suspend_AFE_SIDETONE_CON1, MASK_ALL);

   Afe_Set_Reg(AFE_UL_SRC_0,Suspend_reg.Suspend_AFE_UL_SRC_0, MASK_ALL);
   Afe_Set_Reg(AFE_UL_SRC_1,Suspend_reg.Suspend_AFE_UL_SRC_1, MASK_ALL);
   Afe_Set_Reg(AFE_TOP_CONTROL_0,Suspend_reg.Suspend_AFE_TOP_CONTROL_0, MASK_ALL);

   Afe_Set_Reg(AFE_SDM_GAIN_STAGE,Suspend_reg.Suspend_AFE_SDM_GAIN_STAGE, MASK_ALL);

	#ifdef AUD_SRAMPRINTK
	aee_sram_printk("AudDrv_Recover_reg AFE recover done \n");
	#endif

   // Analog register setting
   xlog_printk(ANDROID_LOG_INFO, "Sound","--AudDrv_Recover_reg  AudDrv_ANA_Clk_On() before \n");
   AudDrv_ANA_Clk_On();
   xlog_printk(ANDROID_LOG_INFO, "Sound","++AudDrv_Recover_reg  AudDrv_ANA_Clk_On() after \n");
   Ana_Set_Reg(AUDIO_CON0,Suspend_reg.Suspend_AUDIO_CON0, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON1,Suspend_reg.Suspend_AUDIO_CON1, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON2,Suspend_reg.Suspend_AUDIO_CON2, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON3,Suspend_reg.Suspend_AUDIO_CON3, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON4,Suspend_reg.Suspend_AUDIO_CON4, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON5,Suspend_reg.Suspend_AUDIO_CON5, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON6,Suspend_reg.Suspend_AUDIO_CON6, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON7,Suspend_reg.Suspend_AUDIO_CON7, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON8,Suspend_reg.Suspend_AUDIO_CON8, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON9,Suspend_reg.Suspend_AUDIO_CON9, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON10,Suspend_reg.Suspend_AUDIO_CON10, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON14,Suspend_reg.Suspend_AUDIO_CON14, MASK_ALL);

   Ana_Set_Reg(AUDIO_CON20,Suspend_reg.Suspend_AUDIO_CON20, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON21,Suspend_reg.Suspend_AUDIO_CON21, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON22,Suspend_reg.Suspend_AUDIO_CON22, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON23,Suspend_reg.Suspend_AUDIO_CON23, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON24,Suspend_reg.Suspend_AUDIO_CON24, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON25,Suspend_reg.Suspend_AUDIO_CON25, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON26,Suspend_reg.Suspend_AUDIO_CON26, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON27,Suspend_reg.Suspend_AUDIO_CON27, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON28,Suspend_reg.Suspend_AUDIO_CON28, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON29,Suspend_reg.Suspend_AUDIO_CON29, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON30,Suspend_reg.Suspend_AUDIO_CON30, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON31,Suspend_reg.Suspend_AUDIO_CON31, MASK_ALL);
   Ana_Set_Reg(AUDIO_CON32,Suspend_reg.Suspend_AUDIO_CON32, MASK_ALL);

   xlog_printk(ANDROID_LOG_INFO, "Sound","--AudDrv_Recover_reg ANA done\n");
/*
   Ana_Set_Reg(VAUDN_CON0,Suspend_reg.Suspend_VAUDN_CON0, MASK_ALL);
   Ana_Set_Reg(VAUDP_CON0,Suspend_reg.Suspend_VAUDP_CON0, MASK_ALL);
   Ana_Set_Reg(VAUDP_CON1,Suspend_reg.Suspend_VAUDP_CON1, MASK_ALL);
   Ana_Set_Reg(VAUDP_CON2,Suspend_reg.Suspend_VAUDP_CON2, MASK_ALL);
   Ana_Set_Reg(VA12_CON0,Suspend_reg.Suspend_VA12_CON0, MASK_ALL);
   Ana_Set_Reg(VMIC_CON0,Suspend_reg.Suspend_VMIC_CON0, MASK_ALL);
   Ana_Set_Reg(VA25_CON0,Suspend_reg.Suspend_VA25_CON0, MASK_ALL);
*/
/*
   Ana_Set_Reg(AUDIO_NCP0,Suspend_reg.Suspend_AUDIO_NCP0, MASK_ALL);
   Ana_Set_Reg(AUDIO_NCP1,Suspend_reg.Suspend_AUDIO_NCP1, MASK_ALL);
   Ana_Set_Reg(AUDIO_LDO0,Suspend_reg.Suspend_AUDIO_LDO0, MASK_ALL);
   Ana_Set_Reg(AUDIO_LDO1,Suspend_reg.Suspend_AUDIO_LDO1, MASK_ALL);
   Ana_Set_Reg(AUDIO_LDO2,Suspend_reg.Suspend_AUDIO_LDO2, MASK_ALL);
   Ana_Set_Reg(AUDIO_LDO3,Suspend_reg.Suspend_AUDIO_LDO3, MASK_ALL);
*/
   Ana_Set_Reg(AUDIO_GLB0,Suspend_reg.Suspend_AUDIO_GLB0, MASK_ALL);
/*
   Ana_Set_Reg(AUDIO_GLB1,Suspend_reg.Suspend_AUDIO_GLB1, MASK_ALL);
   Ana_Set_Reg(AUDIO_GLB2,Suspend_reg.Suspend_AUDIO_GLB2, MASK_ALL);
   Ana_Set_Reg(AUDIO_REG0,Suspend_reg.Suspend_AUDIO_REG0, MASK_ALL);
   Ana_Set_Reg(AUDIO_REG1,Suspend_reg.Suspend_AUDIO_REG1, MASK_ALL);

   Ana_Set_Reg(WR_PATH0,Suspend_reg.Suspend_WR_PATH0, MASK_ALL);
*/
   AudDrv_ANA_Clk_Off();
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Recover_reg  AudDrv_ANA_Clk_Off() \n");
   AudDrv_Clk_Off();
   xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv_Recover_reg  AudDrv_Clk_Off()\n");
	PRINTK_AUDDRV("-AudDrv_Recover_reg \n");
}

static void AudDrv_Recover_reg_AFE(void)
{
	PRINTK_AUDDRV("+AudDrv_Recover_reg_AFE \n");
   // Digital register setting

   Afe_Set_Reg(AFE_DAC_CON1,Suspend_reg.Suspend_AFE_DAC_CON1, MASK_ALL);
   Afe_Set_Reg(AFE_I2S_IN_CON,Suspend_reg.Suspend_AFE_I2S_IN_CON, MASK_ALL);
   Afe_Set_Reg(AFE_FOC_CON,Suspend_reg.Suspend_AFE_FOC_CON, MASK_ALL);
   Afe_Set_Reg(AFE_IRQ_CNT2,Suspend_reg.Suspend_AFE_IRQ_CNT2, MASK_ALL);
   Afe_Set_Reg(AFE_DAIBT_CON,Suspend_reg.Suspend_AFE_DAIBT_CON, MASK_ALL);
   Afe_Set_Reg(AFE_CONN0,Suspend_reg.Suspend_AFE_CONN0, MASK_ALL);
   Afe_Set_Reg(AFE_CONN1,Suspend_reg.Suspend_AFE_CONN1, MASK_ALL);
   Afe_Set_Reg(AFE_CONN2,Suspend_reg.Suspend_AFE_CONN2, MASK_ALL);
   Afe_Set_Reg(AFE_CONN3,Suspend_reg.Suspend_AFE_CONN3, MASK_ALL);
   Afe_Set_Reg(AFE_CONN4,Suspend_reg.Suspend_AFE_CONN4, MASK_ALL);
   Afe_Set_Reg(AFE_DAC_CON0,Suspend_reg.Suspend_AFE_DAC_CON0, MASK_ALL);
   Afe_Set_Reg(AFE_I2S_OUT_CON,Suspend_reg.Suspend_AFE_I2S_OUT_CON, MASK_ALL);
   Afe_Set_Reg(AFE_DL1_BASE,Suspend_reg.Suspend_AFE_DL1_BASE, MASK_ALL);
   Afe_Set_Reg(AFE_DL1_END,Suspend_reg.Suspend_AFE_DL1_END, MASK_ALL);
   Afe_Set_Reg(AFE_DL2_BASE,Suspend_reg.Suspend_AFE_DL2_BASE, MASK_ALL);
   Afe_Set_Reg(AFE_DL2_END,Suspend_reg.Suspend_AFE_DL2_END, MASK_ALL);
   Afe_Set_Reg(AFE_I2S_BASE,Suspend_reg.Suspend_AFE_I2S_BASE, MASK_ALL);
   Afe_Set_Reg(AFE_I2S_END,Suspend_reg.Suspend_AFE_I2S_END, MASK_ALL);
   Afe_Set_Reg(AFE_AWB_BASE,Suspend_reg.Suspend_AFE_AWB_BASE, MASK_ALL);
   Afe_Set_Reg(AFE_AWB_END,Suspend_reg.Suspend_AFE_AWB_END, MASK_ALL);
   Afe_Set_Reg(AFE_DL_SDM_CON0,Suspend_reg.Suspend_AFE_DL_SDM_CON0, MASK_ALL);
   Afe_Set_Reg(AFE_DL_SRC2_1,Suspend_reg.Suspend_AFE_DL_SRC2_1, MASK_ALL);
   Afe_Set_Reg(AFE_DL_SRC2_2,Suspend_reg.Suspend_AFE_DL_SRC2_2, MASK_ALL);
   Afe_Set_Reg(AFE_SIDETONE_CON0,Suspend_reg.Suspend_AFE_SIDETONE_CON0, MASK_ALL);
   Afe_Set_Reg(AFE_SIDETONE_CON1,Suspend_reg.Suspend_AFE_SIDETONE_CON1, MASK_ALL);

   Afe_Set_Reg(AFE_UL_SRC_0,Suspend_reg.Suspend_AFE_UL_SRC_0, MASK_ALL);
   Afe_Set_Reg(AFE_UL_SRC_1,Suspend_reg.Suspend_AFE_UL_SRC_1, MASK_ALL);
   Afe_Set_Reg(AFE_TOP_CONTROL_0,Suspend_reg.Suspend_AFE_TOP_CONTROL_0, MASK_ALL);

   Afe_Set_Reg(AFE_SDM_GAIN_STAGE,Suspend_reg.Suspend_AFE_SDM_GAIN_STAGE, MASK_ALL);

	PRINTK_AUDDRV("-AudDrv_Recover_reg_AFE \n");
}


bool Auddrv_Check_Afe_Clock(void)
{
	return true;
}

void Auddrv_AWB_timer_Routine(unsigned long data)
{
    AFE_BLOCK_T *AWB_Block = &AWB_input_Control->rBlock;
    kal_uint32 AWB_Base =Afe_Get_Reg(AFE_AWB_BASE);
    volatile kal_uint32 AWB_Current = Afe_Get_Reg(AFE_AWB_CUR);
    volatile kal_uint32 AWB_Input_Index = AWB_Current - AWB_Base;
    volatile kal_uint32 AWB_Input_Size =0;

	#if 0
	 volatile kal_uint64 t1, t2;
	 t1 = sched_clock();
	#endif

    PRINTK_AUDDRV("Auddrv_AWB_timer_Routine \n");

    if(AWB_Input_Index >AWB_Block->u4BufferSize){
        xlog_printk(ANDROID_LOG_ERROR, "Sound","error !! AWB_Input_Size = %d> u4BufferSize = %d\n",AWB_Input_Index,AWB_Block->u4BufferSize);
        mod_timer(&AWB_timer, jiffies+msecs_to_jiffies(AWB_TIMER_INTERVAL));
        return;
    }

	 spin_lock_bh(&auddrv_AWBInCtl_lock);

    PRINTK_AUDDRV("1: AWB_Base = %x AWB_Current = %x AWB_Input_Index = %d\n",AWB_Base,AWB_Current,AWB_Input_Index);
    if(AWB_Input_Index > AWB_Block->u4WriteIdx){
        PRINTK_AUDDRV("1: u4DataRemained = %d AWB_Block->u4WriteIdx = %d\n",AWB_Block->u4DataRemained,AWB_Block->u4WriteIdx);
        AWB_Input_Size = AWB_Input_Index - AWB_Block->u4WriteIdx;
        AWB_Block->u4DataRemained +=AWB_Input_Size;
        AWB_Block->u4WriteIdx += AWB_Input_Size;
        if(AWB_Block->u4WriteIdx >= AWB_Block->u4BufferSize){
            AWB_Block->u4WriteIdx -= AWB_Block->u4BufferSize;
        }
        if(AWB_Block->u4DataRemained > AWB_Block->u4BufferSize){
            AWB_Block->u4DataRemained -= AWB_Block->u4BufferSize;
        }
        PRINTK_AUDDRV("1: u4DataRemained = %d AWB_Block->u4WriteIdx = %d\n",AWB_Block->u4DataRemained,AWB_Block->u4WriteIdx);
    }
    else{
        kal_uint32 Copy_Size1 =  AWB_Block->u4BufferSize -AWB_Block->u4WriteIdx;
        kal_uint32 Copy_Size2 = AWB_Input_Index;
        AWB_Input_Size = Copy_Size1 + Copy_Size2;
        PRINTK_AUDDRV("2: u4DataRemained = %d AWB_Block->u4WriteIdx = %d\n",AWB_Block->u4DataRemained,AWB_Block->u4WriteIdx);
        AWB_Block->u4DataRemained += AWB_Input_Size;
        AWB_Block->u4WriteIdx += AWB_Input_Size;
        if(AWB_Block->u4WriteIdx >= AWB_Block->u4BufferSize){
            AWB_Block->u4WriteIdx -= AWB_Block->u4BufferSize;
        }
        if(AWB_Block->u4DataRemained > AWB_Block->u4BufferSize){
            AWB_Block->u4DataRemained -= AWB_Block->u4BufferSize;
        }
        PRINTK_AUDDRV("2: u4DataRemained = %d AWB_Block->u4WriteIdx = %d\n",AWB_Block->u4DataRemained,AWB_Block->u4WriteIdx);
    }
    AWB_wait_queue_flag =1;

	 spin_unlock_bh(&auddrv_AWBInCtl_lock);

    wake_up_interruptible(&AWB_Wait_Queue);
    mod_timer(&AWB_timer, jiffies+msecs_to_jiffies(AWB_TIMER_INTERVAL));
	 #if 0
	 t2 = sched_clock() - t1; // in ns (10^9)
	 xlog_printk(ANDROID_LOG_VERBOSE,"Sound","Auddrv_AWB_timer_Routine takes (%ld)ns\n",t2);
	 #endif
}

// Timer nned to be protect, add_timer twice wiil cause kernel panic
int Auddrv_AWB_timer_on(void)
{
    xlog_printk(ANDROID_LOG_INFO, "Sound","Auddrv_AWB_timer_on is called! \n");

	 spin_lock_bh(&auddrv_lock);

    if(AWB_Timer_Flag == false){
        AWB_timer.expires = jiffies + msecs_to_jiffies(AWB_TIMER_INTERVAL);
        AWB_timer.data = 0;
        AWB_timer.function = Auddrv_AWB_timer_Routine;
        add_timer(&AWB_timer);
        AWB_Timer_Flag = true;
    }

	 spin_unlock_bh(&auddrv_lock);

    return 0;
}

int Auddrv_AWB_timer_off(void)
{
    xlog_printk(ANDROID_LOG_INFO, "Sound","Auddrv_AWB_timer_off is called! \n");

	 spin_lock_bh(&auddrv_lock);
    if(AWB_Timer_Flag == true){
        AWB_Timer_Flag = false;
        spin_unlock_bh(&auddrv_lock);
        del_timer_sync(&AWB_timer);
    }
    else{
        spin_unlock_bh(&auddrv_lock);
    }
    return 0;
}


/*****************************************************************************
 * FUNCTION
 *  AudDrv_allcate_AWB_buffer
 *
 * DESCRIPTION
 *  allocate buffer for AWB
 *
 *****************************************************************************
 */

void AudDrv_allcate_AWB_buffer()
{
   AFE_BLOCK_T *AWB_Block = NULL;
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_allcate_AWB_buffer\n");
   AWB_Block   = &(AWB_input_Control->rBlock);
   if(AWB_Block->pucPhysBufAddr == 0){
       //allocate memory for AWB
       AWB_Block->pucVirtBufAddr = dma_alloc_coherent(0,AWB_BUF_LENGTH,&AWB_Block->pucPhysBufAddr,GFP_KERNEL);
       if((0 == AWB_Block->pucPhysBufAddr)||(NULL == AWB_Block->pucVirtBufAddr)){
          xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_allcate_AWB_buffer dma_alloc_coherent fail \n");
          return;
       }
       AWB_input_Control->u4BufferSize = AWB_BUF_LENGTH;
       AWB_Block->u4BufferSize = AWB_BUF_LENGTH;
       if(AWB_Block->pucVirtBufAddr && Auddrv_Check_Afe_Clock() ){
           memset((void*)AWB_Block->pucVirtBufAddr,0,AWB_Block->u4BufferSize);
       }
       AWB_Block->u4DMAReadIdx =0;
       AWB_Block->u4DataRemained =0;
       AWB_Block->u4WriteIdx =0;
       xlog_printk(ANDROID_LOG_INFO, "Sound","AWB_Block->pucVirtBufAddr = %p  AWB_Block->pucPhysBufAddr = 0x%x\n" ,
           AWB_Block->pucVirtBufAddr, AWB_Block->pucPhysBufAddr);
   }
}

/*****************************************************************************
 * FUNCTION
 *  AudDrv_Start_AWB_Stream / AudDrv_Standby_AWB_Stream
 *
 * DESCRIPTION
 *  AEB enable /disable
 *
 *****************************************************************************
 */

void AudDrv_Start_AWB_Stream(struct file *fp)
{
    AFE_BLOCK_T *AWB_Block = &AWB_input_Control->rBlock;
    xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Start_AWB_Stream  fp = %p\n", fp);
    if(AWB_Block->pucPhysBufAddr && AWB_Block->pucVirtBufAddr)
    {
        Afe_Set_Reg(AFE_AWB_BASE,AWB_Block->pucPhysBufAddr,0xffffffff);
        Afe_Set_Reg(AFE_AWB_END, (AWB_Block->pucPhysBufAddr + AWB_Block->u4BufferSize -1),0xffffffff);
    }
	 spin_lock_bh(&auddrv_lock);
    Aud_AWB_Clk_cntr++;
    if(Aud_AWB_Clk_cntr ==1)
	 {
	 	  spin_unlock_bh(&auddrv_lock);
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Start_AWB_Stream Aud_AWB_Clk_cntr = %d\n",Aud_AWB_Clk_cntr);
        AudDrv_Clk_On();
        AudDrv_ADC_Clk_On();
		  spin_lock_bh(&auddrv_lock);
    }
	 spin_unlock_bh(&auddrv_lock);
    xlog_printk(ANDROID_LOG_INFO, "Sound","AWB_Block->pucVirtBufAddr = %p  AWB_Block->pucPhysBufAddr = %x\n",
           AWB_Block->pucVirtBufAddr, AWB_Block->pucPhysBufAddr);
    AWB_Block->flip = fp;
}

void AudDrv_Standby_AWB_Stream(struct file *fp)
{
    AFE_BLOCK_T *AWB_Block = &AWB_input_Control->rBlock;
    xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Standby_AWB_Stream fp = %p\n", fp);

	 spin_lock_bh(&auddrv_lock);
    Aud_AWB_Clk_cntr--;
    if(Aud_AWB_Clk_cntr == 0)
	 {
	 	spin_unlock_bh(&auddrv_lock);
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Standby_AWB_Stream Aud_AWB_Clk_cntr = %d", Aud_AWB_Clk_cntr);
      AudDrv_Clk_Off();
      AudDrv_ADC_Clk_Off();
		spin_lock_bh(&auddrv_lock);
    }
	 spin_unlock_bh(&auddrv_lock);

	 if(AWB_Block->pucVirtBufAddr && Auddrv_Check_Afe_Clock()){
        memset((void*)AWB_Block->pucVirtBufAddr, 0, AWB_Block->u4BufferSize);
    }
    AWB_Block->u4DMAReadIdx =0;
    AWB_Block->u4DataRemained =0;
    AWB_Block->u4WriteIdx =0;
    AWB_Block->flip = NULL;
}

void AudDrv_Reset_AWB_Stream(void)
{
    AFE_BLOCK_T *AWB_Block = &AWB_input_Control->rBlock;
    xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Reset_AWB_Stream \n");

	 spin_lock_bh(&auddrv_lock);
    while (Aud_AWB_Clk_cntr > 0){
	 	  spin_unlock_bh(&auddrv_lock);
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Reset_AWB_Stream Aud_AWB_Clk_cntr = %d",Aud_AWB_Clk_cntr);
        AudDrv_Clk_Off();
        AudDrv_ADC_Clk_Off();
        Aud_AWB_Clk_cntr --;
		  spin_lock_bh(&auddrv_lock);
    }
    spin_unlock_bh(&auddrv_lock);

    AWB_Block->u4DMAReadIdx =0;
    AWB_Block->u4DataRemained =0;
    AWB_Block->u4WriteIdx =0;
    AWB_Block->flip = NULL;
}

void AudDrv_Restore_Ana_Reg(void)
{
	 spin_lock_bh(&auddrv_SphCtlState_lock);
    if ((!SPH_Ctrl_State.bSpeechFlag)&&(!SPH_Ctrl_State.bVT)&&(!SPH_Ctrl_State.bTtyFlag)&&(!SPH_Ctrl_State.bTtyFlag))
    {
		  spin_unlock_bh(&auddrv_SphCtlState_lock);
        AudDrv_ANA_Clk_On();
        Ana_Recover_AudioPlayback();
        AudDrv_ANA_Clk_Off();
		  spin_lock_bh(&auddrv_SphCtlState_lock);
    }
    if (SPH_Ctrl_State.bRecordFlag)
    {
        //Ana_Recover_AudioRecord();
    }
	 spin_unlock_bh(&auddrv_SphCtlState_lock);
}

int AudDrv_CheckAudioHWStatus(void)
{

    int i4AFE_DAC_CON0,i4AFE_IRQ_CON,i4AFE_DL_SRC2_1,i4AFE_UL_SRC_0,i4AFE_I2S_IN_CON,i4AFE_I2S_OUT_CON;
    int ret = 0;
    i4AFE_DAC_CON0    = Afe_Get_Reg(AFE_DAC_CON0)&0x007f;
    if(i4AFE_DAC_CON0!=0){
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_CheckAudioHWStatus AFE_DAC_CON0(0x%x)\n",i4AFE_DAC_CON0);
    }
    i4AFE_IRQ_CON     = Afe_Get_Reg(AFE_IRQ_CON)&0x007;
    if(i4AFE_IRQ_CON!=0){
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_CheckAudioHWStatus AFE_IRQ_CON(0x%x)\n",i4AFE_IRQ_CON);
    }
    i4AFE_DL_SRC2_1   = Afe_Get_Reg(AFE_DL_SRC2_1)&0x1;
    if(i4AFE_DL_SRC2_1!=0){
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_CheckAudioHWStatus AFE_DL_SRC2_1(0x%x)\n",i4AFE_DL_SRC2_1);
    }
    i4AFE_UL_SRC_0    = Afe_Get_Reg(AFE_UL_SRC_0)&0x1;
    if(i4AFE_UL_SRC_0!=0){
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_CheckAudioHWStatus AFE_UL_SRC_0(0x%x)\n",i4AFE_UL_SRC_0);
    }
    i4AFE_I2S_IN_CON  = Afe_Get_Reg(AFE_I2S_IN_CON)&0x1;
    if(i4AFE_I2S_IN_CON!=0){
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_CheckAudioHWStatus AFE_I2S_IN_CON(0x%x)\n",i4AFE_I2S_IN_CON);
    }
    i4AFE_I2S_OUT_CON = Afe_Get_Reg(AFE_I2S_OUT_CON)&0x1;
    if(i4AFE_I2S_OUT_CON!=0){
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_CheckAudioHWStatus AFE_I2S_OUT_CON(0x%x)\n",i4AFE_I2S_OUT_CON);
    }
    if(i4AFE_DAC_CON0|i4AFE_IRQ_CON|i4AFE_DL_SRC2_1|i4AFE_UL_SRC_0|i4AFE_I2S_IN_CON|i4AFE_I2S_OUT_CON)
        ret = 1;
    return ret;
}
/*****************************************************************************
 * FUNCTION
 *  AudDrv_Clk_On / AudDrv_Clk_Off
 *
 * DESCRIPTION
 *  Enable/Disable PLL(26M clock) \ AFE clock
 *
 *****************************************************************************
 */
void AudDrv_Clk_On(void)
{
	unsigned long flags;

   spin_lock_irqsave(&auddrv_lock, flags);
   if(Aud_AFE_Clk_cntr == 0 )
   {
      PRINTK_AUDDRV("+AudDrv_Clk_On, Aud_AFE_Clk_cntr:%d \n", Aud_AFE_Clk_cntr);
      //sc_request_mdbus_clk(SC_MDBUS_USER_AUDIO);
#ifdef USE_PM_API
      if(enable_clock(MT65XX_PDN_AUDIO_AFE,"AUDIO")){
         xlog_printk(ANDROID_LOG_ERROR, "Sound","Aud enable_clock MT65XX_PDN_AUDIO_AFE fail !!!\n");
      }
      Afe_Set_Reg(AUDIO_TOP_CON0, 0x0, 0x00010000);  // bit16: power on AFE_CK_DIV_RST

      if (!Flag_AudDrv_ClkOn_1st)
      {
         AudDrv_Recover_reg_AFE();
      }
#else
      Afe_Set_Reg(AUDIO_TOP_CON0, 0x00000000, 0x0001000f);  //power on AFE clock
#endif
      Ana_Set_Reg(AUDIO_CON3,0x0,0x2);             // Enable Audio Bias (VaudlbiasDistrib)

   }
   else
	{
   }

   Aud_AFE_Clk_cntr++;
   spin_unlock_irqrestore(&auddrv_lock, flags);

   PRINTK_AUDDRV("!! AudDrv_Clk_On, bSpeechFlag:%d, bRecordFlag:%d, bBgsFlag:%d, bVT:%d \n",
   SPH_Ctrl_State.bSpeechFlag,SPH_Ctrl_State.bRecordFlag,SPH_Ctrl_State.bBgsFlag,SPH_Ctrl_State.bVT);

   PRINTK_AUDDRV("-AudDrv_Clk_On, Aud_AFE_Clk_cntr:%d \n",Aud_AFE_Clk_cntr);
}

void AudDrv_Clk_Off(void)
{
   unsigned long flags;
	PRINTK_AUDDRV("+!! AudDrv_Clk_Off, Aud_AFE_Clk_cntr:%d \n",Aud_AFE_Clk_cntr);

   spin_lock_irqsave(&auddrv_lock, flags);
   Aud_AFE_Clk_cntr--;
   if(Aud_AFE_Clk_cntr == 0)
   {
      PRINTK_AUDDRV("+ AudDrv_Clk_Off, Aud_AFE_Clk_cntr:%d \n",Aud_AFE_Clk_cntr);
      //Check Hw Status
      if(AudDrv_CheckAudioHWStatus()!=0){
         xlog_printk(ANDROID_LOG_WARN, "Sound","!! AudDrv_Clk_Off, AudioHW is not idle. Aud_AFE_Clk_cntr(%d) \n",Aud_AFE_Clk_cntr);
      }
      {
         // Disable AFE clock
         Ana_Set_Reg(AUDIO_CON3,0x2,0x2);       // Disable Audio Bias (VaudlbiasDistrib power down)
#ifdef USE_PM_API
         AudDrv_Store_reg_AFE();
		 Afe_Set_Reg(AUDIO_TOP_CON0, 0x00010000, 0x00010000);  // bit16: power off AFE_CK_DIV_RST

         if(disable_clock(MT65XX_PDN_AUDIO_AFE,"AUDIO")){
           xlog_printk(ANDROID_LOG_ERROR, "Sound","disable_clock MT65XX_PDN_AUDIO_AFE fail");
         }
#else
         Afe_Set_Reg(AUDIO_TOP_CON0, 0x10074, 0x10074);   // bit2: power down AFE clock
#endif
         //sc_unrequest_mdbus_clk(SC_MDBUS_USER_AUDIO);
      }

   }
   if(Aud_AFE_Clk_cntr < 0){
      xlog_printk(ANDROID_LOG_ERROR, "Sound","!! AudDrv_Clk_Off, Aud_AFE_Clk_cntr<0 (%d) \n",Aud_AFE_Clk_cntr);
      Aud_AFE_Clk_cntr =0;
   }
   spin_unlock_irqrestore(&auddrv_lock, flags);

   PRINTK_AUDDRV("-!! AudDrv_Clk_Off, Aud_AFE_Clk_cntr:%d \n",Aud_AFE_Clk_cntr);
}

void CCCI_AudDrv_Clk_On(void)
{
    xlog_printk(ANDROID_LOG_INFO, "Sound","CCCI_AudDrv_Clk_On(%d+1)\n",Aud_AFE_Clk_cntr);
    AudDrv_Clk_On();
}

void CCCI_AudDrv_Clk_Off(void)
{
    xlog_printk(ANDROID_LOG_INFO, "Sound","CCCI_AudDrv_Clk_Off(%d-1)\n",Aud_AFE_Clk_cntr);
    AudDrv_Clk_Off();
}
EXPORT_SYMBOL(CCCI_AudDrv_Clk_On);
EXPORT_SYMBOL(CCCI_AudDrv_Clk_Off);

void AudDrv_Core_Clk_On(void)
{
    PRINTK_AUDDRV("+AudDrv_Core_Clk_On, Aud_Core_Clk_cntr:%d \n", Aud_Core_Clk_cntr);
	spin_lock_bh(&auddrv_lock);
	if(Aud_Core_Clk_cntr == 0 )
	{
#ifdef USE_PM_API
	  if(enable_clock(MT65XX_PDN_MM_AUDIO_CORE, "AUDIO")){
		 xlog_printk(ANDROID_LOG_ERROR, "Sound","Aud enable_clock MT65XX_PDN_MM_AUDIO_CORE fail !!!\n");
	  }
#endif
	}
	Aud_Core_Clk_cntr++;
	spin_unlock_bh(&auddrv_lock);
	PRINTK_AUDDRV("-AudDrv_Core_Clk_On, Aud_Core_Clk_cntr:%d \n", Aud_Core_Clk_cntr);
}

void AudDrv_Core_Clk_Off(void)
{
	PRINTK_AUDDRV("+AudDrv_Core_Clk_Off, Aud_Core_Clk_cntr:%d \n", Aud_Core_Clk_cntr);
	spin_lock_bh(&auddrv_lock);
	Aud_Core_Clk_cntr--;
	if(Aud_Core_Clk_cntr == 0 )
	{
#ifdef USE_PM_API
	  if(disable_clock(MT65XX_PDN_MM_AUDIO_CORE, "AUDIO")){
		 xlog_printk(ANDROID_LOG_ERROR, "Sound","Aud disable_clock MT65XX_PDN_MM_AUDIO_CORE fail !!!\n");
	  }
#endif
	}
	if(Aud_Core_Clk_cntr < 0){
	   PRINTK_AUDDRV("!! AudDrv_Core_Clk_Off, Aud_Core_Clk_cntr<0 (%d) \n", Aud_Core_Clk_cntr);
	   Aud_Core_Clk_cntr =0;
	}
	spin_unlock_bh(&auddrv_lock);
	PRINTK_AUDDRV("-AudDrv_Core_Clk_Off, Aud_Core_Clk_cntr:%d \n", Aud_Core_Clk_cntr);
}


void AudDrv_I2S_Clk_On(void)
{
   PRINTK_AUDDRV("+AudDrv_I2S_Clk_On, Aud_I2S_Clk_cntr:%d \n", Aud_I2S_Clk_cntr);
   spin_lock_bh(&auddrv_lock);
   if(Aud_I2S_Clk_cntr == 0 )
   {
#ifdef USE_PM_API
      PRINTK_AUDDRV("+Aud enable_clock I2S clk(%x)\n",Afe_Get_Reg(AUDIO_TOP_CON0)& 0x40);
      if(enable_clock(MT65XX_PDN_AUDIO_I2S, "AUDIO")){
         xlog_printk(ANDROID_LOG_ERROR, "Sound","Aud enable_clock MT65XX_PDN_AUDIO_I2S fail !!!\n");
      }
#else
	  Afe_Set_Reg(AUDIO_TOP_CON0, 0x00000000, 0x00000040);  //power on I2S clock
#endif
   }
   Aud_I2S_Clk_cntr++;
   spin_unlock_bh(&auddrv_lock);
}

void AudDrv_I2S_Clk_Off(void)
{
   PRINTK_AUDDRV("+AudDrv_I2S_Clk_Off, Aud_I2S_Clk_cntr:%d \n", Aud_I2S_Clk_cntr);
   spin_lock_bh(&auddrv_lock);
   Aud_I2S_Clk_cntr--;
   if(Aud_I2S_Clk_cntr == 0)
   {
#ifdef USE_PM_API
      PRINTK_AUDDRV("+Aud disable_clock I2S clk(%x)\n",Afe_Get_Reg(AUDIO_TOP_CON0)& 0x40);
      if(disable_clock(MT65XX_PDN_AUDIO_I2S, "AUDIO")){
         xlog_printk(ANDROID_LOG_ERROR, "Sound","disable_clock MT65XX_PDN_AUDIO_I2S fail");
      }
#else
      Afe_Set_Reg(AUDIO_TOP_CON0, 0x00000040, 0x00000040);  //power off I2S clock
#endif
   }
   if(Aud_I2S_Clk_cntr < 0){
      xlog_printk(ANDROID_LOG_ERROR, "Sound","!! AudDrv_I2S_Clk_Off, Aud_I2S_Clk_cntr<0 (%d) \n", Aud_I2S_Clk_cntr);
      Aud_I2S_Clk_cntr =0;
   }
   spin_unlock_bh(&auddrv_lock);

   PRINTK_AUDDRV("-AudDrv_I2S_Clk_Off, Aud_I2S_Clk_cntr:%d \n", Aud_I2S_Clk_cntr);
}
void AudDrv_ADC_Clk_On(void)
{
   PRINTK_AUDDRV("+AudDrv_ADC_Clk_On, Aud_ADC_Clk_cntr:%d \n", Aud_ADC_Clk_cntr);
   spin_lock_bh(&auddrv_lock);
   if(Aud_ADC_Clk_cntr == 0 )
   {
      PRINTK_AUDDRV("+Aud enable_clock ADC clk(%x)\n",Afe_Get_Reg(AUDIO_TOP_CON0)& 0x20);
      if(enable_clock(MT65XX_PDN_AUDIO_ADC,"AUDIO")){
         xlog_printk(ANDROID_LOG_ERROR, "Sound","Aud enable_clock MT65XX_PDN_AUDIO_ADC fail !!!\n");
      }
   }
   Aud_ADC_Clk_cntr++;
   spin_unlock_bh(&auddrv_lock);
}

void AudDrv_ADC_Clk_Off(void)
{
   PRINTK_AUDDRV("+AudDrv_ADC_Clk_Off, Aud_ADC_Clk_cntr:%d \n", Aud_ADC_Clk_cntr);
   spin_lock_bh(&auddrv_lock);
   Aud_ADC_Clk_cntr--;

   if(Aud_ADC_Clk_cntr == 0)
   {
      PRINTK_AUDDRV("+Aud disable_clock ADC clk(%x)\n",Afe_Get_Reg(AUDIO_TOP_CON0)& 0x20);
      if(disable_clock(MT65XX_PDN_AUDIO_ADC, "AUDIO")){
         xlog_printk(ANDROID_LOG_ERROR, "Sound","disable_clock MT65XX_PDN_AUDIO_ADC fail");
   }
   }
   if(Aud_ADC_Clk_cntr < 0){
      PRINTK_AUDDRV("!! AudDrv_ADC_Clk_Off, Aud_ADC_Clk_cntr<0 (%d) \n", Aud_ADC_Clk_cntr);
      Aud_ADC_Clk_cntr =0;
   }
   spin_unlock_bh(&auddrv_lock);

   PRINTK_AUDDRV("-AudDrv_ADC_Clk_Off, Aud_ADC_Clk_cntr:%d \n", Aud_ADC_Clk_cntr);
}

void AudDrv_ANA_Clk_On(void)
{
	unsigned long flags;

   spin_lock_irqsave(&auddrv_ANACLKCtl_lock, flags);
   if(Aud_ANA_Clk_cntr == 0 )
   {
      PRINTK_AUDDRV("+AudDrv_ANA_Clk_On, Aud_ANA_Clk_cntr:%d \n", Aud_ANA_Clk_cntr);
      sc_request_mdbus_clk(SC_MDBUS_USER_AUDIO);
   }
   Aud_ANA_Clk_cntr++;
   spin_unlock_irqrestore(&auddrv_ANACLKCtl_lock, flags);

   PRINTK_AUDDRV("!! AudDrv_ANA_Clk_On, bSpeechFlag:%d, bRecordFlag:%d, bBgsFlag:%d, bVT:%d \n",
   SPH_Ctrl_State.bSpeechFlag,SPH_Ctrl_State.bRecordFlag,SPH_Ctrl_State.bBgsFlag,SPH_Ctrl_State.bVT);

   PRINTK_AUDDRV("-AudDrv_ANA_Clk_On, Aud_ANA_Clk_cntr:%d \n",Aud_ANA_Clk_cntr);
}

void AudDrv_ANA_Clk_Off(void)
{
	unsigned long flags;
   PRINTK_AUDDRV("+!! AudDrv_ANA_Clk_Off, Aud_ANA_Clk_cntr:%d \n",Aud_ANA_Clk_cntr);

	spin_lock_irqsave(&auddrv_ANACLKCtl_lock, flags);

   Aud_ANA_Clk_cntr--;
   if(Aud_ANA_Clk_cntr == 0)
   {
      PRINTK_AUDDRV("+ AudDrv_ANA_Clk_Off, Aud_ANA_Clk_cntr:%d \n",Aud_ANA_Clk_cntr);
      sc_unrequest_mdbus_clk(SC_MDBUS_USER_AUDIO);
   }
   if(Aud_ANA_Clk_cntr < 0){
      xlog_printk(ANDROID_LOG_ERROR, "Sound","!! AudDrv_ANA_Clk_Off, Aud_ANA_Clk_cntr<0 (%d) \n",Aud_ANA_Clk_cntr);
      Aud_ANA_Clk_cntr =0;
   }
   spin_unlock_irqrestore(&auddrv_ANACLKCtl_lock, flags);

   PRINTK_AUDDRV("-!! AudDrv_ANA_Clk_Off, Aud_ANA_Clk_cntr:%d \n",Aud_ANA_Clk_cntr);
}

/*****************************************************************************
 * FUNCTION
 *  AudDrv_DL1_Stream_Standby
 *
 * DESCRIPTION
 *  Stop the Output Stream. Reset the AFE block
 *
 * PARAMETERS
 *  fp  [in]
 *  arg [in] input argument
 *
 * RETURNS
 *  None
 *
 *****************************************************************************
 */
void AudDrv_DL1_Stream_Standby(struct file *fp,unsigned long arg)
{
	unsigned long flags;

   PRINTK_AUDDRV("+AudDrv_DL1_Stream_Standby \n");

	spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);

	memset(AFE_dl_Control->rBlock.pucVirtBufAddr,0,AFE_dl_Control->rBlock.u4BufferSize);

   AFE_dl_Control->rBlock.u4WriteIdx	  = 0;
   AFE_dl_Control->rBlock.u4DMAReadIdx	  = 0;
   AFE_dl_Control->rBlock.u4DataRemained = 0;
   AFE_dl_Control->rBlock.u4fsyncflag    = false;
   AFE_dl_Control->rBlock.uResetFlag     = true;

	spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);
   PRINTK_AUDDRV("-AudDrv_DL1_Stream_Standby \n");
}

/*****************************************************************************
 * FUNCTION
 *  AudDrv_Init_DL1_Stream, AudDrv_DeInit_DL1_Stream, AudDrv_Reset_DL1_Stream_Buf
 *
 * DESCRIPTION
 *  Init/DeInit DL1 output Stream.
 *
 *****************************************************************************
 */
void AudDrv_Init_DL1_Stream(kal_uint32 Afe_Buf_Length)
{
   kal_uint32 ptr = 0;
	unsigned long flags;
   if(Afe_Buf_Length > AFE_BUF_LENGTH)
   {
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_DL1_Stream Afe_Buf_Length=%d \n",Afe_Buf_Length);
      Afe_Buf_Length = AFE_BUF_LENGTH;
   }

   PRINTK_AUDDRV("AudDrv_Init_DL1_Stream Afe_Buf_Length:%x \n",Afe_Buf_Length);
   AFE_dl_Control->u4BufferSize = Afe_Buf_Length;  // 32 byte align

   // init output stream
   AFE_dl_Control->rBlock.u4BufferSize    = AFE_dl_Control->u4BufferSize;
    /* //no slave mode in 6575
    if(Flag_Aud_DL1_SlaveOn)
    {
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_DL1_Stream Slave Mode \n");
        AFE_dl_Control->rBlock.pucPhysBufAddr = 0xD4000000;
        AFE_dl_Control->rBlock.pucVirtBufAddr = AudSRAMVirtBufAddr;  // use ioremap to map sram
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_DL1_Stream slave address %p\n",AFE_dl_Control->rBlock.pucVirtBufAddr );
    }
    else
    */
    {//Audio DL1 master mode used

#if defined(DL1_USE_FLEXL2)
            ////////////
            // Use SysRam (FlexL2)

            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_DL1_Stream FlexL2 \n");
            AFE_dl_Control->rBlock.pucPhysBufAddr = 0x90000000;
            AFE_dl_Control->rBlock.pucVirtBufAddr = (kal_uint8*)((AFE_dl_Control->rBlock.pucPhysBufAddr - 0x90000000) + 0xf9000000);
#elif defined(DL1_USE_SYSRAM)
            ////////////
            // Use SysRam (only for Test)
            if (DL1InPhyBufAddr == (kal_uint32)NULL)
            {
                kal_uint32 u4PhyAddr = 0;
                xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_DL1_Stream MTxxx_SYSRAM_ALLOC \n");
                u4PhyAddr = (kal_uint32)SYSRAM_AUDIO_ALLOC_TIMEOUT(AFE_dl_Control->rBlock.u4BufferSize,32,AUD_SYSRAM_ALLOC_TIMEOUT);
                if((kal_uint32)NULL == u4PhyAddr){
                    xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_Init_DL1_Stream MTxxx_SYSRAM_ALLOC fail \n");
                    return;
                }else{
                    SYSRAM_AUDIO_FREE();//deallocation to release mmsysram clock.
                    DL1InPhyBufAddr = u4PhyAddr;
                }
            }
            AFE_dl_Control->rBlock.pucPhysBufAddr = DL1InPhyBufAddr;
            AFE_dl_Control->rBlock.pucVirtBufAddr = (kal_uint8*)(AFE_dl_Control->rBlock.pucPhysBufAddr - MM_SYSRAM_BASE_PA) + SYSRAM_BASE;
#else
    ////////////
    // Use DMA alloc coherent
       AFE_dl_Control->rBlock.pucVirtBufAddr = dma_alloc_coherent(0, AFE_dl_Control->u4BufferSize, &AFE_dl_Control->rBlock.pucPhysBufAddr, GFP_KERNEL);
       if((kal_uint32)NULL == AFE_dl_Control->rBlock.pucPhysBufAddr){
          xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_DL1_Stream dma_alloc_coherent fail \n");
          return;
       }
       ////////////
       // Use kmalloc
       //   AFE_dl_Control->rBlock.pucVirtBufAddr  = (kal_uint8*)kmalloc(AFE_dl_Control->u4BufferSize,GFP_KERNEL);
       //   AFE_dl_Control->rBlock.pucPhysBufAddr  = virt_to_phys((void*)(AFE_dl_Control->rBlock.pucVirtBufAddr));
#endif
    }

   if((AFE_dl_Control->rBlock.pucPhysBufAddr & 0x1f) != 0 ){
      xlog_printk(ANDROID_LOG_INFO, "Sound","[Auddrv] DL1 DMA address is not aligned (0x%x) \n",AFE_dl_Control->rBlock.pucPhysBufAddr);
   }

	spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);

   AFE_dl_Control->rBlock.u4SampleNumMask = 0x001f;  // 32 byte align
   AFE_dl_Control->rBlock.u4WriteIdx	   = 0;
   AFE_dl_Control->rBlock.u4DMAReadIdx    = 0;
   AFE_dl_Control->rBlock.u4DataRemained  = 0;
   AFE_dl_Control->rBlock.u4fsyncflag     = false;
   AFE_dl_Control->rBlock.uResetFlag      = true;

	spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);

	memset((void*)AFE_dl_Control->rBlock.pucVirtBufAddr,0,AFE_dl_Control->u4BufferSize);

/*
   ptr   = AFE_dl_Control->rBlock.pucPhysBufAddr;
   align = (ptr & 0x1f);
   ptr   = ptr + (32-align);
   Afe_Set_Reg(AFE_DL1_BASE ,ptr                     ,0xffffffff);
   Afe_Set_Reg(AFE_DL1_END  ,ptr+(Afe_Buf_Length -1) ,0xffffffff);
*/
    if(!Flag_Aud_DL1_SlaveOn)
    {
        Afe_Set_Reg(AFE_DL1_BASE , AFE_dl_Control->rBlock.pucPhysBufAddr , 0xffffffff);
        Afe_Set_Reg(AFE_DL1_END  , AFE_dl_Control->rBlock.pucPhysBufAddr+(AFE_dl_Control->u4BufferSize -1) , 0xffffffff);
    }

   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_DL1_Stream pucVirtBufAddr=%p, pucPhysBufAddr=%x, ptr=%x\n",
    AFE_dl_Control->rBlock.pucVirtBufAddr,AFE_dl_Control->rBlock.pucPhysBufAddr,ptr);
}

void AudDrv_DeInit_DL1_Stream(void)
{
	unsigned long flags;

        PRINTK_AUDDRV("+AudDrv_DeInit_DL1_Stream ");

        if(AFE_dl_Control->rBlock.pucVirtBufAddr != NULL){
                /* //no slave mode in 6575
                if(Flag_Aud_DL1_SlaveOn)
                {
                        iounmap( 0xD4000000 );
                }
                else
                */
                {//Audio DL1 master mode used
#ifdef DL1_USE_SYSRAM
                        SYSRAM_AUDIO_FREE();
#elif (!defined (DL1_USE_FLEXL2))
                        ////////////
                        // Use DMA free coherent
                        dma_free_coherent(0, AFE_dl_Control->rBlock.u4BufferSize, AFE_dl_Control->rBlock.pucVirtBufAddr, AFE_dl_Control->rBlock.pucPhysBufAddr);
#endif
                        ////////////
                        // Use kfree
                        //      kfree((void*)AFE_dl_Control->rBlock.pucVirtBufAddr);
                }
      AFE_dl_Control->rBlock.pucVirtBufAddr = NULL;
   }

	spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);

   AFE_dl_Control->rBlock.u4BufferSize    = 0;
   AFE_dl_Control->rBlock.pucPhysBufAddr  = 0;
   AFE_dl_Control->rBlock.u4SampleNumMask = 0;
   AFE_dl_Control->rBlock.u4WriteIdx	   = 0;
   AFE_dl_Control->rBlock.u4DMAReadIdx    = 0;
   AFE_dl_Control->rBlock.u4DataRemained  = 0;
   AFE_dl_Control->rBlock.u4fsyncflag     = false;
   AFE_dl_Control->rBlock.uResetFlag      = true;

	spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);

   PRINTK_AUDDRV("-AudDrv_DeInit_DL1_Stream ");
}

void AudDrv_Reset_DL1_Stream_Buf(void)
{
   if(AFE_dl_Control->rBlock.u4BufferSize > 0 && AFE_dl_Control->rBlock.pucVirtBufAddr != NULL)
	{
		memset((void*)AFE_dl_Control->rBlock.pucVirtBufAddr,0,AFE_dl_Control->rBlock.u4BufferSize);
	}
	dsb();
}

// check if still in normal mode , ex::record.
static bool AudDrv_NormalPlayback(void)
{
	spin_lock_bh(&auddrv_SphCtlState_lock);

   if(SPH_Ctrl_State.bBgsFlag || SPH_Ctrl_State.bSpeechFlag || SPH_Ctrl_State.bTtyFlag || SPH_Ctrl_State.bVT)
	{
		spin_unlock_bh(&auddrv_SphCtlState_lock);
      return false;
	}
   else
	{
		spin_unlock_bh(&auddrv_SphCtlState_lock);
      return true;
	}
}

// check if data is fine to go
static bool AudDrv_Check(AFE_BLOCK_T *Afe_Block)
{
    if(Afe_Block == NULL){
       return true;
    }
    if(Afe_Block->u4BufferSize <= 0 || Afe_Block->u4DMAReadIdx < 0 || Afe_Block->u4DataRemained  <0){
        return true;
    }
    else{
        return false;
    }
}

/*****************************************************************************
 * FUNCTION
 *  AudDrv_Do_Handling
 *
 * DESCRIPTION
 *  1.Clear the IRQ
 *  2.Handle different IRQ case
 *  3.Update the parameters
 *    HW read index(u4DMAReadIdx) and Data remained(u4DataRemained) in buffer
 *
 * PARAMETERS
 *  None
 *
 * RETURNS
 *  None
 *
 *****************************************************************************
 */
void AudDrv_Do_Handling(void)  // ISR handling
{
   unsigned long flags;
	kal_int32 Afe_consumed_bytes = 0;
	kal_uint32 Reg_AFE_IRQ_MCU_CNT1;
	AFE_BLOCK_T *Afe_Block = NULL;
	kal_bool Check_Afe_Block;
	kal_uint32 u4Afe_irq_status = 0;
	kal_int32 ret;
	//volatile kal_uint64 t1, t2;

   //Protect Afe_irq_status in critical section. (Not allowed be interrupted)
	 spin_lock_irqsave(&auddrv_irqstatus_lock, flags);

	 u4Afe_irq_status = Afe_irq_status;
	 u4Afe_irq1_cnt = gAFE_IRQ1_CNT;
	 Afe_irq_status = 0;
	 if(u4Afe_irq1_cnt > 0)
	 {
	 	gAFE_IRQ1_CNT = 0;//Only Reset counter while AFE_IRQ1 is triggerred.
	 }
	 spin_unlock_irqrestore(&auddrv_irqstatus_lock, flags);

	 if(u4Afe_irq1_cnt > 0)
	 {
	 	t_ISRTime = sched_clock(); // in ns (10^9)
	 }

   Afe_Block   = &(AFE_dl_Control->rBlock);

   PRINTK_AUDDRV("+AudDrv_Do_Handling Afe_irq_status:%x \n",u4Afe_irq_status);
   if(u4Afe_irq_status == 0 )
   {
      PRINTK_AUDDRV("AudDrv_Do_Handling Afe_irq_status=0 \n");
      goto EXIT_HANDLING;
   }

	spin_lock_irqsave(&auddrv_DL1Ctl_lock,flags);
	Check_Afe_Block = AudDrv_Check(Afe_Block);
	spin_unlock_irqrestore(&auddrv_DL1Ctl_lock,flags);

	if(Check_Afe_Block){
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Do_Handling: u4BufferSize= %d, Afe_Block->u4DMAReadIdx = %d   Afe_Block->u4DataRemained  = %d Error!!\n"
          ,Afe_Block->u4BufferSize,Afe_Block->u4DMAReadIdx,Afe_Block->u4DataRemained );
      u4Afe_irq_status =0;
      goto EXIT_HANDLING;
   }

HW_READ_DATA_AGAIN:
   if ( (u4Afe_irq_status&0xf) & IRQ1_MCU_CLR) // VUL/DL1/DL2/AWB
   {
      u4Afe_irq_status &=(~IRQ1_MCU_CLR);
      PRINTK_AUDDRV("2Do_Handling Afe_irq_status=%x \n",u4Afe_irq_status);
		//t1 = sched_clock(); // in ns (10^9)
		ret = AudDrv_DL_Handling();
		//t2 = sched_clock(); // in ns (10^9)
		//xlog_printk(ANDROID_LOG_VERBOSE,"Sound","AudDrv_DL_Handling takes (%ld)ns\n",(t2-t1));
		if(ret < 0)
		{
			goto EXIT_HANDLING;
		}
   }
   else if( (u4Afe_irq_status&0xf) & IRQ2_MCU_CLR)  // I2S
   {
      PRINTK_AUDDRV("AudDrv_Do_Handling I2S \n");
      u4Afe_irq_status &=(~IRQ2_MCU_CLR);
      AudDrv_I2Sin_Handling();
      goto EXIT_HANDLING;
   }
   else if( (u4Afe_irq_status&0xf) & IRQ_MCU_DAI_SET_CLR)  // BT DAI
   {
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Do_Handling IRQ_MCU_DAI_SET_CLR \n");
      DAI_output_Control_context.status =DAI_ASSERTION;
      // reset buffer data for BT , pretend has buffer.

		memset(Afe_Block->pucVirtBufAddr,0,Afe_Block->u4BufferSize);

		spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);
      Reg_AFE_IRQ_MCU_CNT1= Afe_Get_Reg(AFE_IRQ_CNT1);
      Afe_consumed_bytes = Reg_AFE_IRQ_MCU_CNT1<<2;
      Afe_Block->u4DMAReadIdx += Afe_consumed_bytes;
      Afe_Block->u4DMAReadIdx %= Afe_Block->u4BufferSize;
      if(Afe_Block->u4WriteIdx   > Afe_Block->u4DMAReadIdx){
          Afe_Block->u4DataRemained = Afe_Block->u4WriteIdx  -  Afe_Block->u4DMAReadIdx;
      }
      else{
          Afe_Block->u4DataRemained =   Afe_Block->u4WriteIdx + Afe_Block->u4BufferSize - Afe_Block->u4DMAReadIdx;
      }
		spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);

      Afe_Set_Reg(AFE_DAIBT_CON ,0x8, 0x8); // set data ready
      u4Afe_irq_status &=(~IRQ_MCU_DAI_SET_CLR);
      goto EXIT_HANDLING;
   }
   else if( (u4Afe_irq_status&0xf) & IRQ_MCU_DAI_RST_CLR)  // BT DAI
   {
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Do_Handling IRQ_MCU_DAI_RST_CLR \n");
      DAI_output_Control_context.status =DAI_NEGATION;
      u4Afe_irq_status &=(~IRQ_MCU_DAI_RST_CLR);
      goto EXIT_HANDLING;
   }

   //should wake up before read data again, because DL data has been processed.
   wait_queue_flag =1;
   wake_up_interruptible(&DL_Wait_Queue);

   if( (u4Afe_irq_status&0xf) != 0){
      PRINTK_AUDDRV("AudDrv_Do_Handling HW_READ_DATA_AGAIN \n");
      goto HW_READ_DATA_AGAIN;
   }

EXIT_HANDLING:
   PRINTK_AUDDRV("-AudDrv_Do_Handling Afe_irq_status:%x \n",u4Afe_irq_status);
   return ;
}

/*****************************************************************************
 * FUNCTION
 *  AudDrv_Init_I2S_InputStream, AudDrv_DeInit_I2S_InputStream
 *
 * DESCRIPTION
 *  Init/DeInit I2S input Stream.
 *
 *****************************************************************************
 */
void AudDrv_Init_I2S_InputStream(kal_uint32 I2S_Buffer_Length,struct file *fp)
{
	unsigned long flags;
   if(I2S_Buffer_Length > I2S_BUF_MAX)
   {
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_I2S_InputStream I2S buf Length=%d\n",I2S_Buffer_Length);
      I2S_Buffer_Length = I2S_BUF_MAX;
   }
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_I2S_InputStream I2S_Buffer_Length=%x\n",I2S_Buffer_Length);
   I2S_input_Control->u4BufferSize = I2S_Buffer_Length;

   // init stream
   I2S_input_Control->rBlock.u4BufferSize = I2S_Buffer_Length;

////////////
// Use SysRam (only for Test)
/*
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_I2S_InputStream MT6573_SYSRAM_ALLOC \n");
   I2S_input_Control->rBlock.pucPhysBufAddr = (void*)MT6573_SYSRAM_ALLOC(MT6573SYSRAMUSR_FD,I2S_input_Control->rBlock.u4BufferSize,32);
   if(NULL == I2S_input_Control->rBlock.pucPhysBufAddr){
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_I2S_InputStream MT6573_SYSRAM_ALLOC fail \n");
      return;
   }
   I2S_input_Control->rBlock.pucVirtBufAddr = (I2S_input_Control->rBlock.pucPhysBufAddr - MM_SYSRAM_BASE_PA) + MM_SYSRAM_BASE;
*/

   I2S_input_Control->rBlock.pucPhysBufAddr = I2SInPhyBufAddr;
   I2S_input_Control->rBlock.pucVirtBufAddr = I2SInVirtBufAddr;

////////////
// Use kmalloc
//   I2S_input_Control->rBlock.pucVirtBufAddr   = (kal_uint8*)kmalloc(I2S_Buffer_Length , GFP_KERNEL);  // use ioremap to map sram
//   I2S_input_Control->rBlock.pucPhysBufAddr   = virt_to_phys((void*)(I2S_input_Control->rBlock.pucVirtBufAddr));

   if((I2S_input_Control->rBlock.pucPhysBufAddr & 0x1f) != 0 ){
      xlog_printk(ANDROID_LOG_INFO, "Sound","[Auddrv] I2S In address is not aligned (0x%x) \n",I2S_input_Control->rBlock.pucPhysBufAddr);
   }

	spin_lock_irqsave(&auddrv_I2SInCtl_lock, flags);

   I2S_input_Control->rBlock.u4SampleNumMask  = 0x001f;  // 32 byte align
   I2S_input_Control->rBlock.u4WriteIdx	    = 0;
   I2S_input_Control->rBlock.u4DMAReadIdx     = 0;
   I2S_input_Control->rBlock.u4DataRemained   = 0;
   I2S_input_Control->rBlock.u4fsyncflag      = false;
   I2S_input_Control->rBlock.flip                   = fp;
   memset((void*)I2S_input_Control->rBlock.pucVirtBufAddr,0,I2S_Buffer_Length);

	spin_unlock_irqrestore(&auddrv_I2SInCtl_lock, flags);

   Afe_Set_Reg(AFE_I2S_BASE ,I2S_input_Control->rBlock.pucPhysBufAddr, 0xffffffff);
   Afe_Set_Reg(AFE_I2S_END  ,I2S_input_Control->rBlock.pucPhysBufAddr + (I2S_input_Control->u4BufferSize-1), 0xffffffff);

   xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv_Init_I2S_InputStream  pucVirtBufAddr:%p, pucPhysBufAddr:0x%x \n",
   I2S_input_Control->rBlock.pucVirtBufAddr, I2S_input_Control->rBlock.pucPhysBufAddr);
}

void AudDrv_DeInit_I2S_InputStream(struct file *fp)
{
	unsigned long flags;
   xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_DeInit_I2S_InputStream ");
   if(I2S_input_Control->rBlock.pucVirtBufAddr != NULL){

#if (!defined (I2SIN_USE_FLEXL2))
////////////
// Use DMA free coherent
      xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_DeInit_I2S_InputStream I2S_input_Control->rBlock.u4BufferSize=%d", I2S_input_Control->rBlock.u4BufferSize);
//      dma_free_coherent(0, I2S_input_Control->rBlock.u4BufferSize, I2S_input_Control->rBlock.pucVirtBufAddr, I2S_input_Control->rBlock.pucPhysBufAddr);
#endif
////////////
// Use kfree
//      kfree((void*)I2S_input_Control->rBlock.pucVirtBufAddr);

      I2S_input_Control->rBlock.pucVirtBufAddr = NULL;
   }
   I2S_input_Control->rBlock.u4BufferSize    = 0;
   I2S_input_Control->rBlock.pucPhysBufAddr  = 0;

	spin_lock_irqsave(&auddrv_I2SInCtl_lock, flags);

   I2S_input_Control->rBlock.u4SampleNumMask = 0;
   I2S_input_Control->rBlock.u4WriteIdx	   = 0;
   I2S_input_Control->rBlock.u4DMAReadIdx    = 0;
   I2S_input_Control->rBlock.u4DataRemained  = 0;
   I2S_input_Control->rBlock.flip                   = NULL;
   I2S_input_Control->rBlock.u4fsyncflag     =  false;

	spin_unlock_irqrestore(&auddrv_I2SInCtl_lock, flags);

   PRINTK_AUDDRV("-AudDrv_DeInit_I2S_InputStream ");
}

static int AudDrv_DL_Handling(void)
{
   kal_int32 Afe_consumed_bytes = 0;
   kal_int32 HW_Cur_ReadIdx = 0;
   kal_uint32 Reg_AFE_DAC_CON1, Reg_DL1_OPMODE, Reg_AFE_IRQ_MCU_CNT1, Reg_AFE_MODEM_IR_STATUS;
   AFE_BLOCK_T *Afe_Block = NULL;
	unsigned long flags;

	Afe_Block   = &(AFE_dl_Control->rBlock);

   //If DL1 slave mode, return directly without updating R/W prt and clearing buffer
	Reg_AFE_DAC_CON1= Afe_Get_Reg(AFE_DAC_CON1);
	Reg_DL1_OPMODE = (Reg_AFE_DAC_CON1>>19)&0x1;
	if(Reg_DL1_OPMODE == 0){

		  Reg_AFE_MODEM_IR_STATUS= Afe_Get_Reg(AFE_MODEM_IR_STATUS);
		  if((Reg_AFE_MODEM_IR_STATUS&0x1)&&(!AudDrv_NormalPlayback()))
		  {
				xlog_printk(ANDROID_LOG_INFO, "Sound","Modem DL1 Slave mode:  DL1_OPMODE=0x%x, AFE_MODEM_IR_STATUS=0x%x\n", Reg_DL1_OPMODE, Reg_AFE_MODEM_IR_STATUS);
				return -1;
		  }
		  else
		  {
				if(Flag_Aud_DL1_SlaveOn)
				{
					 PRINTK_AUDDRV("AP DL1 Slave\n");
					 //update current pointer
					 Reg_AFE_IRQ_MCU_CNT1= Afe_Get_Reg(AFE_IRQ_CNT1);
					 Afe_consumed_bytes = Reg_AFE_IRQ_MCU_CNT1<<2;
				}
				else
				{
					 xlog_printk(ANDROID_LOG_ERROR, "Sound","Error! Not AP and Modem use DL1 slave!!\n");
				}
		  }
	 }
	else
	{
		  // Calculate the number of data that has been HW read

		  spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);
		  if(Afe_Block->uResetFlag == true){
			 Afe_Block->uResetFlag = false;
			 HW_Cur_ReadIdx = Afe_Get_Reg(AFE_DL1_CUR);
			 PRINTK_AUDDRV("DL_Handling HW_Cur_ReadIdx=%x, ReadIdx=%x, WriteIdx=%x, DataRemained=%x \n",HW_Cur_ReadIdx,Afe_Block->u4DMAReadIdx,Afe_Block->u4WriteIdx,Afe_Block->u4DataRemained);
		  }
		  spin_unlock_irqrestore(&auddrv_DL1Ctl_lock,flags);

		  HW_Cur_ReadIdx = Afe_Get_Reg(AFE_DL1_CUR);

		  if(HW_Cur_ReadIdx == 0){
			 xlog_printk(ANDROID_LOG_ERROR, "Sound","Aud DL_Handling error, AFE_DL1_CUR, NULL pointer ");
			 return -1;
		  }

		  Reg_AFE_IRQ_MCU_CNT1= Afe_Get_Reg(AFE_IRQ_CNT1);
		  Afe_consumed_bytes = Reg_AFE_IRQ_MCU_CNT1<<2;

		  if(Afe_consumed_bytes <0){
			 Afe_consumed_bytes += Afe_Block->u4BufferSize;
		  }
/*
		  Reg_AFE_IRQ_MCU_CNT1= Afe_Get_Reg(AFE_IRQ_CNT1);
		  Afe_consumed_bytes = Reg_AFE_IRQ_MCU_CNT1<<2;
*/
		  PRINTK_AUDDRV("DL_Handling HW_con:%x \n",Afe_consumed_bytes);

		  // 32 bytes alignment
		  //	 Afe_consumed_bytes &= (~Afe_Block->u4SampleNumMask);
		  //	 xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Do_Handling consumed_bytes:%d \n",Afe_consumed_bytes);
		  if((Afe_consumed_bytes & 0x1f) != 0 ){
			 //xlog_printk(ANDROID_LOG_INFO, "Sound","[Auddrv] DMA address is not aligned 32 bytes \n");
		  }
	}

	if(u4Afe_irq1_cnt > 0)
	{
		if(gLastISRTime != 0)
		{
			unsigned long long tTimeInterval = (t_ISRTime - gLastISRTime);
			if (tTimeInterval > ((unsigned long long)m_Interrupt_time)*11*1000000/8)
			{
				xlog_printk(ANDROID_LOG_ERROR,"Sound","WARNING: ISR interval(%llu)ns, delayed(%llu)ns \n",tTimeInterval,tTimeInterval-((unsigned long long)m_Interrupt_time*1000000));
			}
		}
		gLastISRTime = t_ISRTime;
	}

	spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);
	if((Afe_Block->u4DataRemained <= Afe_consumed_bytes)||(u4Afe_irq1_cnt > 1))
	{
		// buffer underflow --> clear the whole buffer and notify user space

		memset(Afe_Block->pucVirtBufAddr,0,Afe_Block->u4BufferSize);

		xlog_printk(ANDROID_LOG_WARN, "Sound","+DL_Handling underflow ReadIdx:%x WriteIdx:%x, DataRemained:%x, HW_cons:%x \n",
				  Afe_Block->u4DMAReadIdx,Afe_Block->u4WriteIdx,Afe_Block->u4DataRemained,Afe_consumed_bytes);
		if (u4Afe_irq1_cnt > 1)
			 xlog_printk(ANDROID_LOG_ERROR,"Sound","+DL_Handling is blocked %d times\n",u4Afe_irq1_cnt-1);

		Afe_Block->u4DMAReadIdx   += Afe_consumed_bytes;
		Afe_Block->u4DMAReadIdx %= Afe_Block->u4BufferSize;

		Afe_Block->u4WriteIdx  = Afe_Block->u4DMAReadIdx + Afe_Block->u4BufferSize/2;
		Afe_Block->u4WriteIdx %= Afe_Block->u4BufferSize;

		Afe_Block->u4DataRemained = Afe_Block->u4BufferSize/2;
		spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);

		if(Afe_Block->u4fsyncflag == false)
		{
			kill_fasync(&AudDrv_async, SIGIO, POLL_IN);	// notify the user space
			spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);
			Afe_Block->u4fsyncflag = true;
			spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);
		}

		xlog_printk(ANDROID_LOG_VERBOSE, "Sound","-DL_Handling 2underflow ReadIdx:%x, WriteIdx:%x, DataRemained:%x \n",
				  Afe_Block->u4DMAReadIdx,Afe_Block->u4WriteIdx,Afe_Block->u4DataRemained);
	}
	else
	{
		Afe_Block->u4DataRemained -= Afe_consumed_bytes;
		Afe_Block->u4DMAReadIdx += Afe_consumed_bytes;
		Afe_Block->u4DMAReadIdx %= Afe_Block->u4BufferSize;
		spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);

		PRINTK_AUDDRV("DL_Handling normal ReadIdx:%x ,DataRemained:%x, WriteIdx:%x \n",
				  Afe_Block->u4DMAReadIdx,Afe_Block->u4DataRemained,Afe_Block->u4WriteIdx);
	}

	return 0;

}

/*****************************************************************************
 * FUNCTION
 *  AudDrv_I2Sin_Handling
 *
 * DESCRIPTION
 *  1.Clear the IRQ
 *  2.Handle different IRQ case
 *  3.Update the parameters
 *    HW read index(u4WriteIdx) and Data remained(u4DataRemained) in buffer
 *
 * PARAMETERS
 *  None
 *
 * RETURNS
 *  None
 *
 *****************************************************************************
 */
void  AudDrv_I2Sin_Handling(void)
{
   kal_int32 I2S_filled_bytes=0;
   kal_int32 HW_Cur_WriteIdx=0;
	unsigned long flags;
   AFE_BLOCK_T *I2Sin_Block = &(I2S_input_Control->rBlock);
   HW_Cur_WriteIdx = Afe_Get_Reg(AFE_I2S_CUR); //todo

   if(HW_Cur_WriteIdx == 0){
      xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_I2Sin_Handling, Error, AFE_I2S_CUR, NULL pointer ");
      return;
   }

   if(I2Sin_Block->u4BufferSize == 0){
      xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_I2Sin_Handling: u4BufferSize=0, Error");
      return;
   }

   // HW already fill in
   I2S_filled_bytes = (HW_Cur_WriteIdx-I2Sin_Block->pucPhysBufAddr)-I2Sin_Block->u4WriteIdx;
   if(I2S_filled_bytes <0){
   	I2S_filled_bytes += I2Sin_Block->u4BufferSize;
   }

   PRINTK_AUDDRV("AudDrv_I2Sin_Handling1 +I2S_filled_bytes:%x \n",I2S_filled_bytes);

   // 32 bytes alignment
   I2S_filled_bytes &= (~I2Sin_Block->u4SampleNumMask);

   PRINTK_AUDDRV("AudDrv_I2Sin_Handling +I2S_filled_bytes:%x, HW_Cur_WriteIdx:%x, u4DMAReadIdx:%x, u4WriteIdx:0x%x, pucPhysBufAddr:%x \n",
      I2S_filled_bytes,HW_Cur_WriteIdx,I2Sin_Block->u4DMAReadIdx,I2Sin_Block->u4WriteIdx,I2Sin_Block->pucPhysBufAddr);

   if(I2S_filled_bytes ==0)
      return;

	spin_lock_irqsave(&auddrv_I2SInCtl_lock, flags);
   I2Sin_Block->u4WriteIdx  += I2S_filled_bytes;
   I2Sin_Block->u4WriteIdx  %= I2Sin_Block->u4BufferSize;
   I2Sin_Block->u4DataRemained += I2S_filled_bytes;
   // buffer overflow
   if(I2Sin_Block->u4DataRemained > I2Sin_Block->u4BufferSize)
   {
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_I2Sin_Handling buffer overflow u4DMAReadIdx:%x, u4WriteIdx:%x, u4DataRemained:%x, u4BufferSize:%x \n",
      I2Sin_Block->u4DMAReadIdx,I2Sin_Block->u4WriteIdx,I2Sin_Block->u4DataRemained, I2Sin_Block->u4BufferSize);

      I2Sin_Block->u4DataRemained= I2Sin_Block->u4BufferSize/2;
      I2Sin_Block->u4DMAReadIdx = I2Sin_Block->u4WriteIdx - I2Sin_Block->u4BufferSize/2;
      if( I2Sin_Block->u4DMAReadIdx <0){
          I2Sin_Block->u4DMAReadIdx+=I2Sin_Block->u4BufferSize;
      }
   }
	spin_unlock_irqrestore(&auddrv_I2SInCtl_lock, flags);

   PRINTK_AUDDRV("AudDrv_I2Sin_Handling -u4DMAReadIdx:0x%x, u4WriteIdx:0x%x, u4DataRemained:%x, u4BufferSize:%x \n",
     I2Sin_Block->u4DMAReadIdx,I2Sin_Block->u4WriteIdx,I2Sin_Block->u4DataRemained,I2Sin_Block->u4BufferSize);

   I2S_wait_queue_flag =1;
   wake_up_interruptible(&I2Sin_Wait_Queue);
}

/*****************************************************************************
 * FUNCTION
 *  AudDrv_I2S_InputStream_Standby
 *
 * DESCRIPTION
 *  Stop the Output Stream. Reset the I2S block
 *
 * PARAMETERS
 *  fp  [in]
 *  arg [in] input argument
 *
 * RETURNS
 *  None
 *
 *****************************************************************************
 */
void AudDrv_I2S_InputStream_Standby(struct file *fp,unsigned long arg)
{
	unsigned long flags;
   xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_I2S_InputStream_Standby \n");

	spin_lock_irqsave(&auddrv_I2SInCtl_lock, flags);
   memset(I2S_input_Control->rBlock.pucVirtBufAddr,0,I2S_input_Control->rBlock.u4BufferSize);
   I2S_input_Control->rBlock.u4WriteIdx     = 0;
   I2S_input_Control->rBlock.u4DMAReadIdx	  = 0;
   I2S_input_Control->rBlock.u4DataRemained = 0;
	spin_unlock_irqrestore(&auddrv_I2SInCtl_lock, flags);
}

/*****************************************************************************
 * FUNCTION
 *  AudDrv_IRQ_handler / AudDrv_magic_tasklet
 *
 * DESCRIPTION
 *  IRQ handler
 *
 *****************************************************************************
 */
static irqreturn_t AudDrv_IRQ_handler(int irq, void *dev_id)
{
   unsigned long flags;
   kal_uint32 u4RegValue;

   irq_cnt++;

   u4RegValue = Afe_Get_Reg(AFE_IR_STATUS);

   if((irq_cnt&0xfffff)==0)
   {
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_IRQ_handler AFE_IR_STATUS=0x%x, AFE_IRQ_CON=0x%x \n",u4RegValue,Afe_Get_Reg(AFE_IRQ_CON));
   }

   if((u4RegValue&0xf)==0) //To handle the corner case of ISR execution between SetAfeReg(AFE_IRQ_CON,0x0,0x1) and SetAfeReg(AFE_IR_CLR,0x1,0x1) when Afe_DL_Stop()
   {
        xlog_printk(ANDROID_LOG_WARN, "Sound","AudDrv_IRQ_handler All IRQ need clear, 0x%x, 0x%x \n",u4RegValue,Afe_Get_Reg(AFE_IRQ_CON));
        Afe_Set_Reg(AFE_IR_CLR,0x1f,0x001f);
        goto AudDrv_IRQ_handler_exit;
   }

   spin_lock_irqsave(&auddrv_irqstatus_lock, flags);
   Afe_irq_status |= u4RegValue;
   Afe_irq_status &= 0xf;
   spin_unlock_irqrestore(&auddrv_irqstatus_lock, flags);

   #ifdef USE_TASKLET
   Afe_Set_Reg(AFE_IR_CLR,u4RegValue&0xf,0x00ff);
   #endif

   if (u4RegValue&0x1){
       gAFE_IRQ1_CNT ++;
   }

   #ifdef USE_TASKLET
   tasklet_schedule(&magic_tasklet_handle);
   #else
   AudDrv_Do_Handling();
   Afe_Set_Reg(AFE_IR_CLR,u4RegValue&0xf,0x001f);
   #endif

   AudDrv_IRQ_handler_exit:
   return IRQ_HANDLED;
}

#ifdef USE_TASKLET
static void AudDrv_magic_tasklet(unsigned long data)
{
   AudDrv_Do_Handling();
}
#endif

/*****************************************************************************
 * FILE OPERATION FUNCTION
 *  AudDrv_open / AudDrv_release
 *
 * DESCRIPTION
 *
 *
 *****************************************************************************
 */
static int AudDrv_open(struct inode *inode, struct file *fp)
{
   // do nothing
	PRINTK_AUDDRV("AudDrv_open do nothing inode:%p, file:%p \n",inode,fp);
	return 0;
}

static int AudDrv_release(struct inode *inode, struct file *fp)
{
	PRINTK_AUDDRV("AudDrv_release inode:%p, file:%p \n",inode,fp);
   AudDrv_fasync(-1,fp,0);
	if (!(fp->f_mode & FMODE_WRITE || fp->f_mode & FMODE_READ)){
      return -ENODEV;
   }
	return 0;
}

/*****************************************************************************
 * FILE OPERATION FUNCTION
 *  AudDrv_ioctl
 *
 * DESCRIPTION
 *  IOCTL Msg handle
 *
 *****************************************************************************
 */

static long AudDrv_ioctl(struct file *fp, unsigned int cmd, unsigned long arg)
{
   int ret =0;
   Register_Control Reg_Data;
   AMP_Control AMPParam;
   if((cmd!=AUDDRV_ENABLE_ATV_I2S_GPIO)&&(cmd!=AUDDRV_DISABLE_ATV_I2S_GPIO)&&(cmd!=AUDDRV_BEE_IOCTL)) //prevent additional time cost
   {
   AudDrv_Clk_On();
   }
   switch(cmd)
   {
// for DL1 output SRC
      case OPEN_DL1_STREAM:
      {
         PRINTK_AUDDRV("AudDrv OPEN_DL1_STREAM \n");
         AFE_dl_Control->open_cnt++;
         break;  // allocate this fp with one asm block
      }
      case INIT_DL1_STREAM:
      {
         AudDrv_Clk_On();
         if(Aud_Flush_cntr !=0){
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_ioctl(INIT_DL1_STREAM) flush cntr(%d)",Aud_Flush_cntr);

			spin_lock_bh(&auddrv_SphCtlState_lock);

            if( (SPH_Ctrl_State.bSpeechFlag==false) && (SPH_Ctrl_State.bVT==false) && (SPH_Ctrl_State.bRecordFlag==false) ){
               // Disable interrupt (timer)
               xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_ioctl(INIT_DL1_STREAM), Dis DL_SRC2, IRQ, AFE clk(%d) \n",Aud_AFE_Clk_cntr);
               AudDrv_ANA_Clk_On();
               Ana_Set_Reg(AUDIO_CON3,0x0000,0x0030);   // disable Audio Buffer
               Ana_Set_Reg(AUDIO_CON3,0x0000,0x0180);   // disable DAC
               AudDrv_ANA_Clk_Off();
               Afe_Set_Reg(AFE_DL_SRC2_1,0x0,0x1);   // DL2_SRC2_ON=0
               Afe_Set_Reg(AFE_IRQ_CON,0x0,0x1);
               Afe_Set_Reg(AFE_DAC_CON0,0x0,0x2);  // DL1_ON=0
               Afe_Set_Reg(AFE_IR_CLR,0x1,0x1);
               Afe_Set_Reg(AFE_IRQ_CON,0x0,0x2);  // bit1: I2S,  IRQ2_MCU_ON
               Afe_Set_Reg(AFE_IR_CLR,0x2,0x2);           // bit1: I2S,  IRQ1_MCU_CLR
            }

			spin_unlock_bh(&auddrv_SphCtlState_lock);

            // Timing:
            // If IRQ is keep running, but MediaServer die, the u4BufferSize would be reset to 0.
            // but Tasklet function maybe use u4BufferSize to calculate the size.
            // One case, AudDrv_Reset executed, then printk happen. Then Schedule to Tasklet function, the u4BufferSize maybe 0.
            // So, this maybe cause KE.
            AudDrv_Reset();
            Aud_Flush_cntr =0;
            // For one case:
            // Audio start play, but mediaServer die. So the power status maybe keep the previous state.
            // Need to check the Afe_Pwr_on flag and execute power off.
            if(Afe_Pwr_on > 0 )
            {
               xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv INIT_DL1_STREAM MediaServer abnormal,AFE(%d),AFEp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on);
               spin_lock_bh(&auddrv_lock);
               Afe_Pwr_on = 0;
               spin_unlock_bh(&auddrv_lock);
#if defined(DL1_USE_SYSRAM)
               //free sysram here
               SYSRAM_AUDIO_FREE();
#endif
               AudDrv_Clk_Off();
               b_reset_afe_pwr = true;
            }

				spin_lock_bh(&auddrv_lock);
            if(I2S_Pwr_on > 0 )
            {
               xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv INIT_DL1_STREAM MediaServer abnormal,I2S(%d),I2Sp(%d)\n",Aud_I2S_Clk_cntr,I2S_Pwr_on);
               I2S_Pwr_on = 0;
               spin_unlock_bh(&auddrv_lock);
               AudDrv_I2S_Clk_Off();
               AudDrv_Clk_Off();
               b_reset_i2s_pwr = true;
            }
				else
				{
					spin_unlock_bh(&auddrv_lock);
				}

				spin_lock_bh(&auddrv_lock);
            if(b_adc_clk_on > 0){
               // When enter in-call mode, the adc counter will add 1.
               // If media server die, need to sub the reference count of adc clk
               // For the speech/record/bgs, the media server die handling will
               //   be in AudioYusuHardware::Recover_State() function. (Don't care the power control)
               xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv INIT_DL1_STREAM, ADC(%d),b_adc_clk_on(%d) \n",Aud_ADC_Clk_cntr,b_adc_clk_on);
               b_adc_clk_on--;
					spin_unlock_bh(&auddrv_lock);
               AudDrv_ADC_Clk_Off();
               AudDrv_Clk_Off();
            }
				else
				{
					spin_unlock_bh(&auddrv_lock);
				}

				spin_lock_bh(&auddrv_lock);
            if(b_afe_line_in_clk_on > 0){
               xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv INIT_DL1_STREAM,AFE(%d),AFEp(%d),b_afe_line_in_clk_on(%d) \n",Aud_AFE_Clk_cntr,Afe_Pwr_on,b_afe_line_in_clk_on);
               b_afe_line_in_clk_on = 0;
					spin_unlock_bh(&auddrv_lock);
               AudDrv_Clk_Off();
            }
				else
				{
					spin_unlock_bh(&auddrv_lock);
				}

				spin_lock_bh(&auddrv_lock);
            if(b_hdmi_clk_on > 0)
            {
               xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv INIT_DL1_STREAM,AFE(%d),AFEp(%d),b_hdmi_clk_on(%d) \n",Aud_AFE_Clk_cntr,Afe_Pwr_on,b_hdmi_clk_on);
               xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv INIT_DL1_STREAM 2S(%d),I2Sp(%d)\n",Aud_I2S_Clk_cntr,I2S_Pwr_on);
               b_hdmi_clk_on = 0;
					spin_unlock_bh(&auddrv_lock);
               AudDrv_I2S_Clk_Off();
               AudDrv_Clk_Off();
            }
				else
				{
					spin_unlock_bh(&auddrv_lock);
				}

				spin_lock_bh(&auddrv_lock);
            if(Aud_ANA_Clk_cntr > 0)
            {

                xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv INIT_DL1_STREAM,ANAp(%d) \n",Aud_ANA_Clk_cntr);
                while(Aud_ANA_Clk_cntr>0)
                {
                	  spin_unlock_bh(&auddrv_lock);
                    AudDrv_ANA_Clk_Off();
						  spin_lock_bh(&auddrv_lock);
                }
            }
				spin_unlock_bh(&auddrv_lock);
         }

         xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv INIT_DL1_STREAM bsize(%ld),AFE(%d),AFEp(%d)\n",arg,Aud_AFE_Clk_cntr,Afe_Pwr_on);
         xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv INIT_DL1_STREAM I2S(%d),I2Sp(%d)\n",Aud_I2S_Clk_cntr,I2S_Pwr_on);
         //init stream
         AudDrv_Init_DL1_Stream(arg);
		 mutex_lock(&gamp_mutex);
         AudioAMPDevice_mute();
		 mutex_unlock(&gamp_mutex);
         AudDrv_Clk_Off();
         break;
      }
      case START_DL1_STREAM:
      {
         AudDrv_Clk_On();
         PRINTK_AUDDRV("+Aud START_DL1_STREAM arg:%ld \n",arg);

        if(!Flag_Aud_DL1_SlaveOn)
        {
            if (AFE_dl_Control->rBlock.pucPhysBufAddr==(kal_uint32)NULL){
                xlog_printk(ANDROID_LOG_ERROR, "Sound","START_DL1_STREAM AFE_dl_Control->rBlock.pucPhysBufAddr=%x\n",AFE_dl_Control->rBlock.pucPhysBufAddr);
            }
            Afe_Set_Reg(AFE_DL1_BASE , AFE_dl_Control->rBlock.pucPhysBufAddr , 0xffffffff);
            Afe_Set_Reg(AFE_DL1_END  , AFE_dl_Control->rBlock.pucPhysBufAddr+(AFE_dl_Control->u4BufferSize -1) , 0xffffffff);
        }

         if(b_reset_afe_pwr==true){
            b_reset_afe_pwr=false;
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv START_DL1_STREAM, reset AFE clk AFE(%d),AFEp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on);
         }

         PRINTK_AUDDRV("+Aud START_DL1_STREAM VirtBuf=%p, PhysBuf=%x \n",
           AFE_dl_Control->rBlock.pucVirtBufAddr,AFE_dl_Control->rBlock.pucPhysBufAddr);

         spin_lock_bh(&auddrv_lock);
         Afe_Pwr_on++;
         spin_unlock_bh(&auddrv_lock);
         if(Afe_Pwr_on == 1 ){
            xlog_printk(ANDROID_LOG_INFO, "Sound","!! AudDrv START_DL1_STREAM, AFE(%d),AFEp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on);
            AudDrv_Clk_On();
#if defined(DL1_USE_SYSRAM)
            //allocate sysram here
            SYSRAM_AUDIO_ALLOC_TIMEOUT(AFE_dl_Control->rBlock.u4BufferSize,32,AUD_SYSRAM_ALLOC_TIMEOUT);
#endif
            PRINTK_AUDDRV("AudDrv START_DL1_STREAM, wake_lock \n");
            wake_lock(&Audio_wake_lock);
         }
         else{
            xlog_printk(ANDROID_LOG_INFO, "Sound","!! AudDrv START_DL1_STREAM, AFE(%d),AFEp>1(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on);
         }
         AudDrv_Reset_DL1_Stream_Buf();
         gLastISRTime = 0;//reset ISR timer
         xlog_printk(ANDROID_LOG_INFO, "Sound","-Aud START_DL1_STREAM AFE(%d),AFEp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on);
         AudDrv_Clk_Off();
         break;
      }
      case STANDBY_DL1_STREAM:
      {
         AudDrv_Clk_On();

         xlog_printk(ANDROID_LOG_INFO, "Sound","+Aud STANDBY_DL1_STREAM, AFE(%d),AFEp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on);
         AudDrv_DL1_Stream_Standby(fp,arg);
         m_first_write =0;
         spin_lock_bh(&auddrv_lock);
         Afe_Pwr_on--;
         spin_unlock_bh(&auddrv_lock);
         if(Afe_Pwr_on == 0 )
         {
            PRINTK_AUDDRV(" AudDrv STANDBY_DL1_STREAM, AFE(%d),AFEp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on);
            PRINTK_AUDDRV("!!! AudDrv STANDBY_DL1_STREAM Speech:%d, VT:%d \n",SPH_Ctrl_State.bSpeechFlag,SPH_Ctrl_State.bVT);
#if defined(DL1_USE_SYSRAM)
            //free sysram here
            SYSRAM_AUDIO_FREE();
#endif
            AudDrv_Clk_Off();
            PRINTK_AUDDRV("AudDrv STANDBY_DL1_STREAM,1 wake_unlock \n");
            wake_unlock(&Audio_wake_lock);
         }
         else if(Afe_Pwr_on < 0)
         {
            xlog_printk(ANDROID_LOG_INFO, "Sound"," AudDrv STANDBY_DL1_STREAM, AFE(%d),AFEp(%d)<0\n",Aud_AFE_Clk_cntr,Afe_Pwr_on);
            spin_lock_bh(&auddrv_lock);
            Afe_Pwr_on = 0;
            spin_unlock_bh(&auddrv_lock);
            PRINTK_AUDDRV("AudDrv STANDBY_DL1_STREAM,2 wake_unlock \n");
            wake_unlock(&Audio_wake_lock);
         }
         gLastISRTime = 0;//reset ISR timer
         xlog_printk(ANDROID_LOG_INFO, "Sound","-Aud STANDBY_DL1_STREAM AFE(%d),AFEp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on);

         AudDrv_Clk_Off();
         break;
      }
      case CLOSE_DL1_STREAM:
      {
         PRINTK_AUDDRV("!!! AudDrv CLOSE_DL1_STREAM \n");
         AudDrv_Clk_On();
         AudDrv_DeInit_DL1_Stream();
         AFE_dl_Control->open_cnt--;
         AudDrv_Clk_Off();
         break;
      }
      case SET_DL1_AFE_BUFFER:
      {
         PRINTK_AUDDRV("!!! Aud SET_DL1_AFE_BUFFER PhyBufAddr:0x%x \n",AFE_dl_Control->rBlock.pucPhysBufAddr);
         AudDrv_Clk_On();
         if(!Flag_Aud_DL1_SlaveOn)
         {
             Afe_Set_Reg(AFE_DL1_BASE , AFE_dl_Control->rBlock.pucPhysBufAddr , 0xffffffff);
             Afe_Set_Reg(AFE_DL1_END  , AFE_dl_Control->rBlock.pucPhysBufAddr+(AFE_dl_Control->u4BufferSize -1) , 0xffffffff);
         }
         AudDrv_Clk_Off();
         break;
      }

    case SET_DL1_SLAVE_MODE:
    {
        if(arg ==1)
        {
            Flag_Aud_DL1_SlaveOn = true;
        }
        else
        {
            Flag_Aud_DL1_SlaveOn = false;
        }
       PRINTK_AUDDRV("!!! Aud SET_DL1_SLAVE_MODE Flag_Aud_DL1_SlaveOn:%d \n", Flag_Aud_DL1_SlaveOn);
       break;
    }
    case GET_DL1_SLAVE_MODE:
    {
       ret = Flag_Aud_DL1_SlaveOn;
       PRINTK_AUDDRV("!!! Aud GET_DL1_SLAVE_MODE Flag_Aud_DL1_SlaveOn:%d \n", Flag_Aud_DL1_SlaveOn);
       break;
    }

      case GET_AFE_BUFFER_SIZE:
      {
	    	PRINTK_AUDDRV("AudDrv GET_AFE_BUFFER_SIZE \n");
	      ret = AFE_dl_Control->u4BufferSize;
	    	break;
      }
// for I2S input
      case OPEN_I2S_INPUT_STREAM:
      {
         AudDrv_Clk_On();
         AudDrv_I2S_Clk_On();
         PRINTK_AUDDRV("AudDrv OPEN_I2S_INPUT_STREAM, AFE(%d),AFEp(%d),I2S(%d),I2Sp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_I2S_Clk_cntr,I2S_Pwr_on);
         AudDrv_Init_I2S_InputStream(arg,fp);
    	   I2S_input_Control->open_cnt++;
         AudDrv_I2S_Clk_Off();
         AudDrv_Clk_Off();
         break;
      }
      case START_I2S_INPUT_STREAM:
      {
         AudDrv_Clk_On();
         AudDrv_I2S_Clk_On();
         PRINTK_AUDDRV("+AudDrv START_I2S_INPUT, AFE(%d),AFEp(%d),I2S(%d),I2Sp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_I2S_Clk_cntr,I2S_Pwr_on);

         Afe_Set_Reg(AFE_I2S_BASE ,I2S_input_Control->rBlock.pucPhysBufAddr, 0xffffffff);
         Afe_Set_Reg(AFE_I2S_END  ,I2S_input_Control->rBlock.pucPhysBufAddr + (I2S_input_Control->u4BufferSize-1), 0xffffffff);

         if(b_reset_i2s_pwr==true){
            b_reset_i2s_pwr=false;
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv START_I2S_INPUT, reset AFE/I2S AFE(%d),AFEp(%d),I2S(%d),I2Sp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_I2S_Clk_cntr,I2S_Pwr_on);
         }

         xlog_printk(ANDROID_LOG_INFO, "Sound","+START_I2S_INPUT pucVirtBufAddr:%p,pucPhysBufAddr:%x\n",
          I2S_input_Control->rBlock.pucVirtBufAddr, I2S_input_Control->rBlock.pucPhysBufAddr);

			spin_lock_bh(&auddrv_lock);
         I2S_Pwr_on++;
         if(I2S_Pwr_on==1)
         {
         	spin_unlock_bh(&auddrv_lock);
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv START_I2S_INPUT, I2Sp(%d)\n",I2S_Pwr_on);
            AudDrv_Clk_On();
            AudDrv_I2S_Clk_On();
         }
         else if (I2S_Pwr_on > 1)
         {
         	spin_unlock_bh(&auddrv_lock);
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv START_I2S_INPUT, I2Sp>1(%d)\n",I2S_Pwr_on);
         }
			else
			{
				spin_unlock_bh(&auddrv_lock);
			}
         AudDrv_I2S_Clk_Off();
         AudDrv_Clk_Off();
         xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv START_I2S_INPUT, AFE(%d),AFEp(%d),I2S(%d),I2Sp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_I2S_Clk_cntr,I2S_Pwr_on);
       	break;
      }
      case STANDBY_I2S_INPUT_STREAM:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv STANDBY_I2S_INPUT, AFE(%d),AFEp(%d),I2S(%d),I2Sp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_I2S_Clk_cntr,I2S_Pwr_on);
         AudDrv_Clk_On();
         AudDrv_I2S_Clk_On();

         AudDrv_I2S_InputStream_Standby(fp,arg);

   		spin_lock_bh(&auddrv_lock);
         I2S_Pwr_on--;
         if(I2S_Pwr_on == 0)
         {
         	spin_unlock_bh(&auddrv_lock);
            PRINTK_AUDDRV("AudDrv STANDBY_I2S_INPUT,I2Sp(%d),AFEp(%d)\n",I2S_Pwr_on,Afe_Pwr_on);
            AudDrv_I2S_Clk_Off();
            AudDrv_Clk_Off();
         }
         else if(I2S_Pwr_on < 0)
         {
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv STANDBY_I2S_INPUT, I2Sp(%d)<0\n",I2S_Pwr_on);
            I2S_Pwr_on = 0;
            spin_unlock_bh(&auddrv_lock);
         }
			else
			{
				spin_unlock_bh(&auddrv_lock);
			}
         AudDrv_I2S_Clk_Off();
         AudDrv_Clk_Off();
         xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv STANDBY_I2S_INPUT,AFE(%d),AFEp(%d),I2S(%d),I2Sp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_I2S_Clk_cntr,I2S_Pwr_on);
       	break;
      }
      case SET_AWB_INPUT_STREAM_STATE:
      {
          xlog_printk(ANDROID_LOG_INFO, "Sound","SET_AWB_INPUT_STREAM_STATE arg = %ld fp = %p \n", arg, fp);
          AudDrv_Clk_On();
          if(arg){
              AudDrv_Start_AWB_Stream(fp);
              Auddrv_AWB_timer_on();
              wake_lock(&Audio_record_wake_lock);
          }
          else{
              Auddrv_AWB_timer_off();
              AudDrv_Standby_AWB_Stream(fp);
              wake_unlock(&Audio_record_wake_lock);
          }
          AudDrv_Clk_Off();
          break;
      }
      case CLOSE_I2S_INPUT_STREAM:
      {
         PRINTK_AUDDRV("+AudDrv CLOSE_I2S_INPUT \n");
         I2S_input_Control->open_cnt--;
         AudDrv_DeInit_I2S_InputStream(fp);
         PRINTK_AUDDRV("-AudDrv CLOSE_I2S_INPUT \n");
         break;
      }
      case SET_I2S_Input_BUFFER:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","+Aud SET_I2S_Input_BUFFER PhysBufAddr:0x%x\n", I2S_input_Control->rBlock.pucPhysBufAddr );
         AudDrv_Clk_On();
         AudDrv_I2S_Clk_On();
         Afe_Set_Reg(AFE_I2S_BASE , I2S_input_Control->rBlock.pucPhysBufAddr , 0xffffffff);
         Afe_Set_Reg(AFE_I2S_END  , I2S_input_Control->rBlock.pucPhysBufAddr+(I2S_input_Control->u4BufferSize -1) , 0xffffffff);
         AudDrv_I2S_Clk_Off();
         AudDrv_Clk_Off();
         PRINTK_AUDDRV("-Aud SET_I2S_Input_BUFFER \n");
         break;
      }
      case SET_I2S_Output_BUFFER:
      {
         AudDrv_Clk_On();
         AudDrv_I2S_Clk_On();
         xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv SET_I2S_Output_BUFFER\n");
         Afe_Set_Reg(AFE_I2S_BASE, 0x90008000, 0xffffffff);
         Afe_Set_Reg(AFE_I2S_END, 0x9000B000, 0xffffffff);
         AudDrv_I2S_Clk_Off();
         AudDrv_Clk_Off();
       	break;
      }
      case AUD_SET_LINE_IN_CLOCK:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv AUD_SET_LINE_IN_CLOCK(%ld), lineIn_clk(%d) \n",arg,b_afe_line_in_clk_on);
         if(arg ==1)
         {
         	spin_lock_bh(&auddrv_lock);
            if(b_afe_line_in_clk_on == 0){
					spin_unlock_bh(&auddrv_lock);
               AudDrv_Clk_On();
					spin_lock_bh(&auddrv_lock);
            }
            b_afe_line_in_clk_on++;
				spin_unlock_bh(&auddrv_lock);
         }
         else
         {
         	spin_lock_bh(&auddrv_lock);
            b_afe_line_in_clk_on--;
            if(b_afe_line_in_clk_on == 0){
					spin_unlock_bh(&auddrv_lock);
               AudDrv_Clk_Off();
					spin_lock_bh(&auddrv_lock);
            }
				spin_unlock_bh(&auddrv_lock);
         }
         PRINTK_AUDDRV("-AudDrv AUD_SET_LINE_IN_CLOCK, AFE(%d),AFEp(%d) \n",Aud_AFE_Clk_cntr,Afe_Pwr_on);
         break;
      }
      case AUD_SET_CLOCK:
      {
         PRINTK_AUDDRV("+AudDrv AUD_SET_CLOCK(%ld) \n", arg);

         if( b_reset_i2s_pwr==true || b_reset_afe_pwr==true ){
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv AUD_SET_CLOCK, reset AFE(%d),AFEp(%d),I2S(%d),I2Sp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_I2S_Clk_cntr,I2S_Pwr_on);
         }

         if(arg ==1)
         {
         	spin_lock_bh(&auddrv_lock);
            b_afe_clk_on++;
				spin_unlock_bh(&auddrv_lock);
            AudDrv_Clk_On();
         }
         else
         {
            AudDrv_Clk_Off();
				spin_lock_bh(&auddrv_lock);
            b_afe_clk_on--;
				spin_unlock_bh(&auddrv_lock);
         }
         PRINTK_AUDDRV("-AudDrv AUD_SET_CLOCK, AFE(%d),AFEp(%d) \n",Aud_AFE_Clk_cntr,Afe_Pwr_on);
         break;
      }
      case AUD_SET_26MCLOCK:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv AUD_SET_26MCLOCK \n");
         break;
      }
      case AUD_SET_ADC_CLOCK:
      {
         PRINTK_AUDDRV("+AudDrv AUD_SET_ADC_CLOCK(%ld) \n", arg);

         if( b_reset_i2s_pwr==true || b_reset_afe_pwr==true ){
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv AUD_SET_ADC_CLOCK, reset AFE(%d),AFEp(%d),I2S(%d),I2Sp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_I2S_Clk_cntr,I2S_Pwr_on);
         }

         if(arg ==1)
         {
         	spin_lock_bh(&auddrv_lock);
            b_adc_clk_on++;
				spin_unlock_bh(&auddrv_lock);
            AudDrv_Clk_On();
            AudDrv_ADC_Clk_On();
         }
         else
         {
            AudDrv_ADC_Clk_Off();
            AudDrv_Clk_Off();
				spin_lock_bh(&auddrv_lock);
            b_adc_clk_on--;
				spin_unlock_bh(&auddrv_lock);
         }
         PRINTK_AUDDRV("-AudDrv AUD_SET_ADC_CLOCK, AFE(%d),AFEp(%d),ADC(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_ADC_Clk_cntr);
         break;
      }
      case AUD_SET_I2S_CLOCK:
      {
         PRINTK_AUDDRV("+AudDrv AUD_SET_I2S_CLOCK(%ld) \n", arg);

         if( b_reset_i2s_pwr==true || b_reset_afe_pwr==true ){
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv AUD_SET_I2S_CLOCK, reset AFE(%d),AFEp(%d),I2S(%d),I2Sp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_I2S_Clk_cntr,I2S_Pwr_on);
         }

         if(arg ==1)
         {
         	spin_lock_bh(&auddrv_lock);
            b_i2s_clk_on++;
				spin_unlock_bh(&auddrv_lock);
            AudDrv_Clk_On();
            AudDrv_I2S_Clk_On();
         }
         else
         {
            AudDrv_I2S_Clk_Off();
            AudDrv_Clk_Off();
				spin_lock_bh(&auddrv_lock);
            b_i2s_clk_on--;
				spin_unlock_bh(&auddrv_lock);
         }
         PRINTK_AUDDRV("-AudDrv AUD_SET_I2S_CLOCK, AFE(%d),AFEp(%d),I2S(%d),I2Sp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_I2S_Clk_cntr,I2S_Pwr_on);
         break;
      }
      case AUD_SET_ANA_CLOCK:
      {
          PRINTK_AUDDRV("+AudDrv AUD_SET_ANA_CLOCK(%ld) \n", arg);

          if(arg ==1)
          {
             AudDrv_ANA_Clk_On();
          }
          else
          {
             AudDrv_ANA_Clk_Off();
          }
          PRINTK_AUDDRV("-AudDrv AUD_SET_ANA_CLOCK, AFE(%d),AFEp(%d),I2S(%d),I2Sp(%d)\n",Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_I2S_Clk_cntr,I2S_Pwr_on);
          break;
      }
      case AUD_GET_ANA_CLOCK_CNT:
      {
          ret = Aud_ANA_Clk_cntr;
          PRINTK_AUDDRV("!!! Aud AUD_GET_ANA_CLOCK_CNT Aud_ANA_Clk_cntr:%d \n", Aud_ANA_Clk_cntr);
          break;
      }
      case AUD_SET_HDMI_CLOCK:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv AUD_SET_HDMI_CLOCK(%ld), hdmi_clk(%d)",arg,b_hdmi_clk_on);
         if(arg ==1)
         {
         	spin_lock_bh(&auddrv_lock);
            if(b_hdmi_clk_on == 0){
					spin_unlock_bh(&auddrv_lock);
               AudDrv_Clk_On();
               AudDrv_I2S_Clk_On();
					spin_lock_bh(&auddrv_lock);
            }
            else if(b_hdmi_clk_on > 0){
               xlog_printk(ANDROID_LOG_INFO, "Sound","AUD_SET_HDMI_CLOCK, hdmi_clk(%d)",b_hdmi_clk_on);
            }
            b_hdmi_clk_on++;
				spin_unlock_bh(&auddrv_lock);
         }
         else
         {
         	spin_lock_bh(&auddrv_lock);
            b_hdmi_clk_on--;
            if(b_hdmi_clk_on == 0){
					spin_unlock_bh(&auddrv_lock);
               AudDrv_I2S_Clk_Off();
               AudDrv_Clk_Off();
					spin_lock_bh(&auddrv_lock);
            }
            else if(b_hdmi_clk_on < 0){
               b_hdmi_clk_on = 0;
            }
				spin_unlock_bh(&auddrv_lock);
         }
         break;
      }
/*
      case AUD_SET_HDMI_GPIO:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv AUD_SET_HDMI_GPIO(%ld)",arg);
         // for demo phone and EVB, use GPIO53/54/55 as I2S1
         // Need to remove this part after DCT tool config ready
         #if defined(MTK_HDMI_SUPPORT)
         if(arg ==1)
         {
            mt_set_gpio_mode(GPIO_I2S1_DAT_PIN, GPIO_MODE_00);  //58 mode0
            mt_set_gpio_dir(GPIO_I2S1_DAT_PIN,GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_I2S1_DAT_PIN,GPIO_OUT_ZERO);
            mt_set_gpio_mode(GPIO_I2S1_WS_PIN, GPIO_MODE_00);  //57 mode0
            mt_set_gpio_dir(GPIO_I2S1_WS_PIN,GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_I2S1_WS_PIN,GPIO_OUT_ZERO);
            mt_set_gpio_mode(GPIO_I2S1_CK_PIN, GPIO_MODE_00);  //56 mode0
            mt_set_gpio_dir(GPIO_I2S1_CK_PIN,GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_I2S1_CK_PIN,GPIO_OUT_ZERO);

            mt_set_gpio_mode(GPIO_I2S0_CK_PIN, GPIO_MODE_04);    //55  mode4:I2S1
            mt_set_gpio_mode(GPIO_I2S0_WS_PIN, GPIO_MODE_04);    //54  mode4:I2S1
            mt_set_gpio_mode(GPIO_I2S0_DAT_PIN, GPIO_MODE_04); //53  mode4:I2S1
         }
         else
         {
            mt_set_gpio_mode(GPIO_I2S0_CK_PIN, GPIO_MODE_00);  //55  mode0
            mt_set_gpio_dir(GPIO_I2S0_CK_PIN,GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_I2S0_CK_PIN,GPIO_OUT_ZERO);
            mt_set_gpio_mode(GPIO_I2S0_WS_PIN, GPIO_MODE_00);  //54  mode0
            mt_set_gpio_dir(GPIO_I2S0_WS_PIN,GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_I2S0_WS_PIN,GPIO_OUT_ZERO);
            mt_set_gpio_mode(GPIO_I2S0_DAT_PIN, GPIO_MODE_00); //53  mode0
            mt_set_gpio_dir(GPIO_I2S0_DAT_PIN,GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_I2S0_DAT_PIN,GPIO_OUT_ZERO);
         }
         #endif
         break;
      }
*/
      case AUD_SET_HDMI_SR:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv AUD_SET_HDMI_SR, SR(%d)\n",arg);
#if defined(MTK_HDMI_SUPPORT)
         hdmi_audio_config(arg);
#endif
         xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv AUD_SET_HDMI_SR\n");
         break;
      }
      case AUD_SET_HDMI_MUTE:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv AUD_SET_HDMI_MUTE\n");
#if defined(MTK_HDMI_SUPPORT)
           // hdmi mute function has problem.
           // rollback to original design. Wait HDMI's RD to check.
//         hdmi_audio_delay_mute(arg);
#endif
         break;
      }
// Set/Get AudioSys Register
      case SET_AUDSYS_REG:
      {
         AudDrv_Clk_On();
         if(copy_from_user((void *)(&Reg_Data), (const void __user *)( arg), sizeof(Reg_Data))){
             AudDrv_Clk_Off();
             return -EFAULT;
         }
         //xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv SET_AUDSYS_REG offset=%x, value=%x, mask=%x \n",Reg_Data.offset,Reg_Data.value,Reg_Data.mask);
         spin_lock_bh(&auddrv_lock);
         Afe_Set_Reg(Reg_Data.offset,Reg_Data.value,Reg_Data.mask);
         spin_unlock_bh(&auddrv_lock);
         AudDrv_Clk_Off();
         break;
      }
      case GET_AUDSYS_REG:
      {
         AudDrv_Clk_On();
         //xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv GET_AUDSYS_REG \n");
         if(copy_from_user((void *)(&Reg_Data), (const void __user *)( arg), sizeof(Reg_Data))){
            AudDrv_Clk_Off();
            return -EFAULT;
         }
         spin_lock_bh(&auddrv_lock);
         Reg_Data.value = Afe_Get_Reg(Reg_Data.offset);
         spin_unlock_bh(&auddrv_lock);
         if(copy_to_user((void __user *)( arg),(void *)(&Reg_Data), sizeof(Reg_Data))){
            AudDrv_Clk_Off();
            return -EFAULT;
         }
         AudDrv_Clk_Off();
         break;
      }
// Set/Get AnalogAFE Register
      case SET_ANAAFE_REG:
      {
         AudDrv_Clk_On();
         if(copy_from_user((void *)(&Reg_Data), (const void __user *)( arg), sizeof(Reg_Data))){
            AudDrv_Clk_Off();
            return -EFAULT;
         }
         //xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv SET_ANAAFE_REG offset=%x, value=%x, mask=%x \n",Reg_Data.offset,Reg_Data.value,Reg_Data.mask);
         spin_lock_bh(&auddrv_lock);
         Ana_Set_Reg(Reg_Data.offset,Reg_Data.value,Reg_Data.mask);
         spin_unlock_bh(&auddrv_lock);
         AudDrv_Clk_Off();
         break;
      }
      case GET_ANAAFE_REG:
      {
         AudDrv_Clk_On();
	    	//xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv GET_ANAAFE_REG \n");
	    	if(copy_from_user((void *)(&Reg_Data), (const void __user *)( arg), sizeof(Reg_Data))){
	    	    AudDrv_Clk_Off();
	    	    return -EFAULT;
	    	}
	    	spin_lock_bh(&auddrv_lock);
	    	Reg_Data.value = Ana_Get_Reg(Reg_Data.offset);
	    	spin_unlock_bh(&auddrv_lock);
	    	if(copy_to_user((void __user *)( arg),(void *)(&Reg_Data), sizeof(Reg_Data))){
	    	    AudDrv_Clk_Off();
	    	    return -EFAULT;
	    	}
         AudDrv_Clk_Off();
	    	break;
      }
// for Speaker Setting
      case SET_SPEAKER_VOL:
      {
         PRINTK_AUDDRV("AudDrv SET_SPEAKER_VOL level:%u \n",arg);
         Sound_Speaker_SetVolLevel(arg);
         break;
      }
      case SET_SPEAKER_ON:
      {
	  	/*
         spin_lock_bh(&auddrv_lock);
         if(AMP_Flag != true){
            AMP_Flag = true;
            spin_unlock_bh(&auddrv_lock);
            PRINTK_AUDDRV("AudDrv SET_SPEAKER_ON arg:%u \n",arg);

            Sound_Speaker_Turnon(arg);
         }
         else{
            spin_unlock_bh(&auddrv_lock);
         }
         */
     	PRINTK_AUDDRV("AudDrv SET_SPEAKER_ON arg:%u \n",arg);
		mutex_lock(&gamp_mutex);
	 	Sound_Speaker_Turnon(arg);
		mutex_unlock(&gamp_mutex);
         break;
      }
      case SET_SPEAKER_OFF:
      {
	  	/*
         spin_lock_bh(&auddrv_lock);
         if(AMP_Flag !=false){
             AMP_Flag = false;
             spin_unlock_bh(&auddrv_lock);
             PRINTK_AUDDRV("AudDrv SET_SPEAKER_OFF arg:%u \n",arg);
             Sound_Speaker_Turnoff(arg);
             //Sound_Speaker_Turnoff(arg);
         }
         else{
             spin_unlock_bh(&auddrv_lock);
         }
		 */
		 PRINTK_AUDDRV("AudDrv SET_SPEAKER_OFF arg:%u \n",arg);
		 mutex_lock(&gamp_mutex);
		 Sound_Speaker_Turnoff(arg);
		 mutex_unlock(&gamp_mutex);

         break;
      }
// for HeadPhone Setting
	  case SET_HEADPHONE_ON:
	  {
         PRINTK_AUDDRV("AudDrv SET_HEADPHONE_ON arg:%u \n",arg);
		 mutex_lock(&gamp_mutex);
         Audio_eamp_command(EAMP_HEADPHONE_OPEN,arg,1);
		 mutex_unlock(&gamp_mutex);
         break;
	  }
	  case SET_HEADPHONE_OFF:
	  {
	  	PRINTK_AUDDRV("AudDrv SET_HEADPHONE_OFF arg:%u \n",arg);
		mutex_lock(&gamp_mutex);
	  	Audio_eamp_command(EAMP_HEADPHONE_CLOSE,arg,1);
		mutex_unlock(&gamp_mutex);
	  	break;
	  }
//for EarPiece Setting
	  case SET_EARPIECE_ON:
	  {
         PRINTK_AUDDRV("AudDrv SET_EARPIECE_ON arg:%u \n",arg);
		 mutex_lock(&gamp_mutex);
         Audio_eamp_command(EAMP_EARPIECE_OPEN,arg,1);
		 mutex_unlock(&gamp_mutex);
         break;

	  }
	  case SET_EARPIECE_OFF:
	  {
	  	PRINTK_AUDDRV("AudDrv SET_EARPIECE_OFF arg:%u \n",arg);
		mutex_lock(&gamp_mutex);
	  	Audio_eamp_command(EAMP_EARPIECE_CLOSE,arg,1);
		mutex_unlock(&gamp_mutex);
	  	break;
	  }
// amp registers
	  case GET_EAMP_PARAMETER:
	  {
		PRINTK_AUDDRV("AudDrv GET_EAMP_PARAMETER \n");
		mutex_lock(&gamp_mutex);
		if(copy_from_user((void *)(&AMPParam), (const void __user *)(arg), sizeof(AMPParam))){
			  mutex_unlock(&gamp_mutex);
			  printk("Failed to copy from user");
			  return -EFAULT;
	    }
		PRINTK_AUDDRV("command=%lu,param1=%lu,param2=%lu\n",AMPParam.command,AMPParam.param1,AMPParam.param2);
		switch(AMPParam.command)
		{
			case AUD_AMP_GET_REGISTER:
			{
				ret = Audio_eamp_command(EAMP_GETREGISTER_VALUE,AMPParam.param1,1);
				break;
			}
			case AUD_AMP_GET_CTRP_NUM:
			{
				ret = Audio_eamp_command(EAMP_GET_CTRP_NUM,AMPParam.param1,1);
				break;
			}
			case AUD_AMP_GET_CTRP_BITS:
			{
				ret = Audio_eamp_command(EAMP_GET_CTRP_BITS,AMPParam.param1,1);
				break;
			}
			case AUD_AMP_GET_CTRP_TABLE:
			{
				Audio_eamp_command(EAMP_GET_CTRP_TABLE,(unsigned long)&AMPParam,1);
				break;
			}
			case AUD_AMP_GET_AMPGAIN:
			{
				ret = Audio_eamp_command(EAMP_GETAMP_GAIN,0,1);
				break;
			}
			default:
				break;
		}
		mutex_unlock(&gamp_mutex);
		return ret;
	  	break;
	  }
	  case SET_EAMP_PARAMETER:
	  {
	  	PRINTK_AUDDRV("AudDrv SET_EAMP_PARAMETER \n");
		mutex_lock(&gamp_mutex);
		if(copy_from_user((void *)(&AMPParam), (const void __user *)( arg), sizeof(AMPParam))){
			  mutex_unlock(&gamp_mutex);
			  printk("Failed to copy from user");
			  return -EFAULT;
	    }
		PRINTK_AUDDRV("command=%lu,param1=%lu,param2=%lu\n",AMPParam.command,AMPParam.param1,AMPParam.param2);
		switch(AMPParam.command)
		{
			case AUD_AMP_SET_AMPGAIN:
			{
				Audio_eamp_command(EAMP_SETAMP_GAIN,AMPParam.param1,1);
				break;
			}
			case AUD_AMP_SET_REGISTER:
			{
				Audio_eamp_command(EAMP_SETREGISTER_VALUE,(unsigned long)&AMPParam,1);
				break;
			}
			case AUD_AMP_SET_MODE:
			{
				Audio_eamp_command(EAMP_SETMODE,AMPParam.param1,1);
				break;
			}
			default:
				break;
		}
		mutex_unlock(&gamp_mutex);
		break;
	  }
      case SET_HEADSET_:
      {
         spin_lock_bh(&auddrv_lock);
         PRINTK_AUDDRV("!! AudDrv SET_HEADSET_ arg:%u \n",arg);
         if(arg){
             spin_unlock_bh(&auddrv_lock);
             Sound_Headset_Turnon();
         }
         else{
             spin_unlock_bh(&auddrv_lock);
             Sound_Headset_Turnoff();
         }

         break;
      }
// record the Speech/Background/Recording state in kernel
      case SET_AUDIO_STATE:
      {
 			SPH_Control SPH_Ctrl_State_Temp;
         PRINTK_AUDDRV("AudDrv SET_AUDIO_STATE \n");
		 	if(copy_from_user((void *)(&SPH_Ctrl_State_Temp), (const void __user *)( arg), sizeof(SPH_Control))){
            	return -EFAULT;
         	}
		 	// Need to use a temp struct since it is dangerous to enter copy_from_user() when spin_lock is held. Because copy_from_user() may sleep.
			spin_lock_bh(&auddrv_SphCtlState_lock);
		 	memcpy((void *)&SPH_Ctrl_State, (void *)&SPH_Ctrl_State_Temp, sizeof(SPH_Control));
			spin_unlock_bh(&auddrv_SphCtlState_lock);

		 	dmb();

         xlog_printk(ANDROID_LOG_INFO, "Sound","SET_AUDIO_STATE bBgsFlag:%d,bRecordFlag:%d,bSpeechFlag:%d,bTtyFlag:%d,bVT:%d,bAudio:%d \n",
         SPH_Ctrl_State.bBgsFlag,
         SPH_Ctrl_State.bRecordFlag,
         SPH_Ctrl_State.bSpeechFlag,
         SPH_Ctrl_State.bTtyFlag,
         SPH_Ctrl_State.bVT,
         SPH_Ctrl_State.bAudioPlay);
         break;
      }
      case GET_AUDIO_STATE:
      {
         PRINTK_AUDDRV("AudDrv GET_AUDIO_STATE \n");
         if(copy_to_user((void __user *)arg,(void *)&SPH_Ctrl_State, sizeof(SPH_Control))){
            return -EFAULT;
         }
         xlog_printk(ANDROID_LOG_INFO,"Sound","GET_AUDIO_STATE bBgsFlag:%d,bRecordFlag:%d,bSpeechFlag:%d,bTtyFlag:%d,bVT:%d,bAudio:%d \n",
         SPH_Ctrl_State.bBgsFlag,
         SPH_Ctrl_State.bRecordFlag,
         SPH_Ctrl_State.bSpeechFlag,
         SPH_Ctrl_State.bTtyFlag,
         SPH_Ctrl_State.bVT,
         SPH_Ctrl_State.bAudioPlay);
         break;
      }
// Set GPIO pin mux
// for MT6620
// typedef enum {
//    COMBO_AUDIO_STATE_0 = 0, /* 0000: BT_PCM_OFF & FM line in/out */
//    COMBO_AUDIO_STATE_1 = 1, /* 0001: BT_PCM_ON & FM line in/out */
//    COMBO_AUDIO_STATE_2 = 2, /* 0010: BT_PCM_OFF & FM I2S */
//    COMBO_AUDIO_STATE_3 = 3, /* 0011: BT_PCM_ON & FM I2S (invalid in 73evb & 1.2 phone configuration) */
//} COMBO_AUDIO_STATE;
#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
      case AUDDRV_RESET_BT_FM_GPIO:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","!! AudDrv, RESET, COMBO_AUDIO_STATE_1 \n");
         mt_combo_audio_ctrl(COMBO_AUDIO_STATE_1);
         break;
      }
      case AUDDRV_SET_BT_PCM_GPIO:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","!! AudDrv, BT PCM, COMBO_AUDIO_STATE_1 \n");
         mt_combo_audio_ctrl(COMBO_AUDIO_STATE_1);
         break;
      }
      case AUDDRV_SET_FM_I2S_GPIO:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","!! AudDrv, FM I2S, COMBO_AUDIO_STATE_2 \n");
         mt_combo_audio_ctrl(COMBO_AUDIO_STATE_2);
         break;
      }
#else
      case AUDDRV_RESET_BT_FM_GPIO:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","!! NoCombo, COMBO_AUDIO_STATE_0 \n");
         break;
      }
      case AUDDRV_SET_BT_PCM_GPIO:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","!! NoCombo, COMBO_AUDIO_STATE_1 \n");
         break;
      }
      case AUDDRV_SET_FM_I2S_GPIO:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","!! NoCombo, COMBO_AUDIO_STATE_2 \n");
         break;
      }
#endif

// Set GPIO pin mux
// for MT519x(ATV) & MT6620(FM) use I2S path for the same project
      case AUDDRV_ENABLE_ATV_I2S_GPIO:
	  {
#if defined(MTK_MT5192) || defined(MTK_MT5193)
	     xlog_printk(ANDROID_LOG_INFO, "Sound","!! AudDrv, AUDDRV_ENABLE_ATV_I2S_GPIO \n");
	     cust_matv_gpio_on();
#endif
	     break;
	  }
	  case AUDDRV_DISABLE_ATV_I2S_GPIO:
	  {
#if defined(MTK_MT5192) || defined(MTK_MT5193)
	     xlog_printk(ANDROID_LOG_INFO, "Sound","!! AudDrv, AUDDRV_DISABLE_ATV_I2S_GPIO \n");
	     cust_matv_gpio_off();
#endif
	     break;
	  }

      case AUDDRV_MT6573_CHIP_VER:
      {
        /*
         if (get_chip_eco_ver() == CHIP_E1)
         {
            // MT6573 E1 chip
            ret = CHIP_E1;
            xlog_printk(ANDROID_LOG_INFO, "Sound","!! E1(0x%x) AUDDRV_MT6573_CHIP_VER \n",ret );
         }
         else if(get_chip_eco_ver() == CHIP_E2)
         {
            // MT6573 E2 chip or later.
            ret = CHIP_E2;
            xlog_printk(ANDROID_LOG_INFO, "Sound","!! E2(0x%x) AUDDRV_MT6573_CHIP_VER \n",ret );
         }
         else
         {
            xlog_printk(ANDROID_LOG_INFO, "Sound","!! No: AUDDRV_MT6573_CHIP_VER \n");
         }
        */
         break;
      }
#if defined(MTK_DT_SUPPORT)
      case AUDDRV_SET_RECEIVER_GPIO:
      {
          xlog_printk(ANDROID_LOG_INFO, "Sound","AUDDRV_SET_RECEIVER_GPIO type = %d\n", arg);
          if(arg == 0)
          {
              mt_set_gpio_dir(GPIO_52_REC_SW, GPIO_DIR_OUT); //output
              mt_set_gpio_out(GPIO_52_REC_SW, GPIO_OUT_ZERO);//low mt6573
          }
          else if (arg == 1)
          {
              mt_set_gpio_dir(GPIO_52_REC_SW, GPIO_DIR_OUT); //output
              mt_set_gpio_out(GPIO_52_REC_SW, GPIO_OUT_ONE);//high mt6252
          }
          //xlog_printk(ANDROID_LOG_INFO, "Sound","Receiver Swich is not controlled by SW, need to use hw switch by hand.\n", arg);
          break;
      }
#endif

		#ifdef AUD_DUMPFTRACE
// for debug
      case AUDDRV_DUMPFTRACE_DBG: // this ioctl is only use for debugging
      {
			int i;
			if(arg==1)
			{
				aud_dumpftrace_dbg = 1;
				aud_dumpftrace_done = 0;
				aud_blocked_cnt = 0;
				for(i=0;i<5;i++)
				{
					audio_blocked_frame[i] = 0;
				}
				xlog_printk(ANDROID_LOG_INFO, "Sound","ioctl: AUDDRV_DUMPFTRACE_DBG = 1 \n");
			}
			else if(arg==0)
			{
				aud_dumpftrace_dbg = 0;
				xlog_printk(ANDROID_LOG_INFO, "Sound","ioctl: AUDDRV_DUMPFTRACE_DBG = 0 \n");
			}
			break;
      }
		#endif
      case AUDDRV_BEE_IOCTL: // this ioctl is only use for debugging
      {
	  xlog_printk(ANDROID_LOG_INFO, "Sound","BEE arg:%ld \n",arg);
          //LouderSPKSound(300);
         break;
      }
      case SET_2IN1_SPEAKER:
      {
         break;
      }
      case YUSU_INFO_FROM_USER:
      {
         _Info_Data InfoData;
         PRINTK_AUDDRV("YUSU_INFO_FROM_USER\n");
      	if(copy_from_user((void *)(&InfoData), (const void __user *)( arg), sizeof(InfoData))){
      	   return -EFAULT;
         }
         spin_lock_bh(&auddrv_lock);

         //call sound extenstion function
         xlog_printk(ANDROID_LOG_INFO, "Sound","YUSU_INFO_FROM_USER  Info:%d, param1:%d, param2:%d\n",InfoData.info, InfoData.param1, InfoData.param2);
         switch(InfoData.info) {
           case INFO_U2K_MATV_AUDIO_START:
              Sound_ExtFunction("InfoMATVAudioStart", &(InfoData.param1), sizeof(InfoData.param1) + sizeof(InfoData.param2));
              break;
           case INFO_U2K_MATV_AUDIO_STOP:
              Sound_ExtFunction("InfoMATVAudioStop", &(InfoData.param1), sizeof(InfoData.param1) + sizeof(InfoData.param2));
              break;
           default:
              break;
         }

         spin_unlock_bh(&auddrv_lock);
         break;
      }
      // use to DAI_OUTPUT
      case AUDDRV_START_DAI_OUTPUT:
      {
          xlog_printk(ANDROID_LOG_INFO, "Sound","AUDDRV_START_DAI_OUTPUT\n");
          DAI_output_Control_context.status =DAI_INIT;
          break;
      }
      case AUDDRV_STOP_DAI_OUTPUT:
      {
          xlog_printk(ANDROID_LOG_INFO, "Sound","AUDDRV_STOP_DAI_OUTPUT\n");
          DAI_output_Control_context.status = DAI_STOP;
          AudDrv_Clk_On();
          Afe_Set_Reg(AFE_DAIBT_CON ,0x0, 0x8); // set data ready
          AudDrv_Clk_Off();
          break;
      }
      // used for AUDIO_HQA
      case AUDDRV_HQA_AMP_MODESEL:
      {
	  	    xlog_printk(ANDROID_LOG_INFO, "Sound","AUDDRV_HQA_AMP_MODESEL(%ld)\n",arg);
	  	    //arg = 0: class D
	  	    //arg = 1: class AB
	  	    if (arg != SPKAMP_CLASSD)
	  	    {
		  	    hwSPKClassABTurnOn(1,1);
				g4SpeakerType = SPKAMP_CLASSAB;
	  	  	}
		    else
		    {
		  	    hwSPKClassDTurnOn(1,1);
				g4SpeakerType = SPKAMP_CLASSD;
		    }
	  	    break;
      };
      case AUDDRV_HQA_AMP_AMPEN:
	  {
	  	  xlog_printk(ANDROID_LOG_INFO, "Sound","AUDDRV_HQA_AMP_AMPEN(%ld) AMP(%d)\n",arg,g4SpeakerType);
	  	  //arg = 0: disable
	  	  //arg = 1: enable
	  	  if (arg == 1)//enalbe
	  	  {
			if (g4SpeakerType == SPKAMP_CLASSAB)
			{
       			hwSPKClassABInit();
		    	hwSPKClassABTurnOn(1,1);
			}
			else if (g4SpeakerType == SPKAMP_CLASSD)
			{
			    hwSPKClassDInit();
			    hwSPKClassDTurnOn(1,1);
			}
	  	  }
		  else
		  {
			  if (g4SpeakerType == SPKAMP_CLASSAB)
			  {
				  hwSPKClassABTurnOff();
			  }
			  else if (g4SpeakerType == SPKAMP_CLASSD)
			  {
				  hwSPKClassDTurnOff();
			  }
    	  }
		  break;
	  };
      case AUDDRV_HQA_AMP_AMPVOL:
	  {
	  	  xlog_printk(ANDROID_LOG_INFO, "Sound","AUDDRV_HQA_AMP_AMPVOL(%ld) AMP(%d)\n",arg,g4SpeakerType);
		  if (g4SpeakerType == SPKAMP_CLASSAB)
		  {
			  hwSPKClassABVolumeControl(arg,arg);
		  }
		  else if (g4SpeakerType == SPKAMP_CLASSD)
		  {
			  hwSPKClassDVolumeControl(arg,arg);
		  }
		  break;
	  };
      case AUDDRV_HQA_AMP_RECEIVER:
	  {
     	  xlog_printk(ANDROID_LOG_INFO, "Sound","AUDDRV_HQA_AMP_RECEIVER(%ld) AMP(%d)\n",arg,g4SpeakerType);
		  hw2in1SpeakerSetting(arg,arg);
		  break;
	  };
      case AUDDRV_HQA_AMP_RECGAIN:
	  {
	  	  xlog_printk(ANDROID_LOG_INFO, "Sound","AUDDRV_HQA_AMP_RECGAIN(%ld) AMP(%d)\n",arg,g4SpeakerType);
		  hwSPKClassABVoiceBypassAnalogGain(arg);
		  break;
	  };
	  case AUDDRV_AMP_OC_CFG:
	  {
    	  unsigned short u2LVal, u2RVal;
		  xlog_printk(ANDROID_LOG_INFO, "Sound","AUDDRV_AMP_OC_CFG (%ld)\n",arg);
		  u2LVal = arg & 0xff;
  		  u2RVal = (arg>>16) & 0xff;
		  hwSPKOCControl(u2LVal,u2RVal);
		  break;
	  };
	  case AUDDRV_AMP_OC_READ:
      {
	  	  xlog_printk(ANDROID_LOG_INFO, "Sound","AUDDRV_AMP_OC_READ \n");
		  ret = hwSPKOCStatus();
		  break;
	  };
      case AUDDRV_MD_RST_RECOVERY:
      {
          AudDrv_Clk_On();
          spin_lock_bh(&auddrv_lock);
          AudDrv_Restore_Ana_Reg();
          spin_unlock_bh(&auddrv_lock);
          AudDrv_Clk_Off();
          break;
      }
      case AUDDRV_LOG_PRINT: // this ioctl is only use for debugging
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv AUDDRV_LOG_PRINT \n");
         AudDrv_Clk_On();
         AudDrv_ANA_Clk_On();
         AudDrv_AudReg_Log_Print();
         AudDrv_ANA_Clk_Off();
         AudDrv_Clk_Off();
         break;
      }
      case AUDDRV_ASSERT_IOCTL:
      {
         xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv AUDDRV_ASSERT_IOCTL \n");
         BUG_ON(1);
         break;
      }
      default:
      {
      	xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv Fail IOCTL command (%x)\r\n", cmd);
      	break;
      }
	}
    if((cmd!=AUDDRV_ENABLE_ATV_I2S_GPIO)&&(cmd!=AUDDRV_DISABLE_ATV_I2S_GPIO)&&(cmd!=AUDDRV_BEE_IOCTL)) //prevent additional time cost
    {
   AudDrv_Clk_Off();
    }
	return ret;
}


/*****************************************************************************
 * FILE OPERATION FUNCTION
 *  Get_Afe_wait_period
 *
 * DESCRIPTION
 *  calculate afe interrupt wait time
 *
 * PARAMETERS
 *  period  [out]  return time in jiffies
 *
 * RETURNS
 *  None
 *
 *****************************************************************************/

static kal_uint32 Get_Afe_wait_period(void)
{
    kal_uint32 samplerate , interrupt_cnt;
    samplerate = Afe_Get_Reg(AFE_IRQ_CON);
    samplerate = samplerate >> 4;
    samplerate = samplerate &0xf;
    switch(samplerate)
    {
        case 0:
            samplerate = 8000;
            break;
        case 1:
            samplerate = 11025;
            break;
        case 2:
            samplerate = 12000;
            break;
        case 4:
            samplerate = 16000;
            break;
        case 5:
            samplerate = 22050;
            break;
        case 6:
            samplerate = 24000;
            break;
        case 8:
            samplerate = 32000;
            break;
        case 9:
            samplerate = 44100;
            break;
        case 10:
            samplerate = 48000;
            break;
        default:{
            xlog_printk(ANDROID_LOG_INFO, "Sound","Get_Afe_wait_period default samplerate = 44100\n");
            samplerate = 44100;
        }
        break;
    }
    interrupt_cnt = Afe_Get_Reg (AFE_IRQ_CNT1);
    if(interrupt_cnt ==0){
        m_Interrupt_time = 0;
        return 0;
    }

    m_Interrupt_time = ( ((interrupt_cnt*1000)+(samplerate/2)) /samplerate ); // 1 interrupt timer(HW buffer time).  unit in ms.
    return ( ((interrupt_cnt*100)+(samplerate/2)) /samplerate);  // unit in jiffies (10ms)
}


/*****************************************************************************
 * FILE OPERATION FUNCTION
 *  AudDrv_write
 *
 * DESCRIPTION
 *  User space Write data to (kernel)HW buffer
 *
 * PARAMETERS
 *  fp      [in]
 *  data    [in] data pointer
 *  count   [in] number of bytes to be written
 *  offset  [in] no use
 *
 * RETURNS
 *  None
 *
 *****************************************************************************
 */
static ssize_t AudDrv_write(struct file *fp, const char __user *data, size_t count, loff_t *offset)
{
   //for DL1 output stream write data to HW buffer
   int ret, ret1=1;
   AFE_BLOCK_T  *Afe_Block = NULL;
   ssize_t written_size = count;
//   kal_int32 HW_Cur_ReadIdx = 0;
   kal_int32 copy_size = 0,Afe_WriteIdx_tmp;
	unsigned long flags;
   char *data_w_ptr = (char*)data;
   kal_uint32 /*size_1, size_2,*/Afe_wait_priod =0;
   PRINTK_AUDDRV("+AudDrv_write count:%x \n", count);

   Afe_Block = &(AFE_dl_Control->rBlock);

//   HW_Cur_ReadIdx = Afe_Get_Reg(AFE_DL1_CUR);  // for debugging
    if(m_first_write == 0){
        xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_write m_first_write =%d",m_first_write);
        xlog_printk(ANDROID_LOG_INFO, "Sound","aud_top  =%u Aud_AFE_Clk_cntr = %d Afe_Pwr_on = %d Aud_ANA_Clk_cntr = %d",
            Afe_Get_Reg(AUDIO_TOP_CON0),Aud_AFE_Clk_cntr,Afe_Pwr_on,Aud_ANA_Clk_cntr);
        m_first_write ++;
    }

   PRINTK_AUDDRV("AudDrv_write WriteIdx=%x, ReadIdx=%x, DataRemained=%x \n",
      Afe_Block->u4WriteIdx, Afe_Block->u4DMAReadIdx,Afe_Block->u4DataRemained);  // audio data count to write

   if(Afe_Block->u4BufferSize == 0){
      xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write: u4BufferSize=0, Error");
      // do sleep
      msleep(AFE_INT_TIMEOUT);
      return -1;
   }
   // base on samplerate and counter
   Afe_wait_priod = Get_Afe_wait_period();  // just return 1 interrupt timer. unit: jiffies
   if(Afe_wait_priod <= 1){
       Afe_wait_priod = AFE_INT_TIMEOUT;
   }

   while(count)
   {
		spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);
      copy_size = Afe_Block->u4BufferSize - Afe_Block->u4DataRemained;  //free space of the buffer
		spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);

      if(count <= (kal_uint32) copy_size)
      {
          copy_size = count;
          PRINTK_AUDDRV("AudDrv_write copy_size:%x \n", copy_size);  // (free space of buffer)
      }

      if(copy_size != 0)
      {
			spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);
			Afe_WriteIdx_tmp = Afe_Block->u4WriteIdx;
			spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);

			if(Afe_WriteIdx_tmp + copy_size < Afe_Block->u4BufferSize ) // copy once
         {
            if(!access_ok(VERIFY_READ,data_w_ptr,copy_size)){
                xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write 0ptr invalid data_w_ptr=%x, size=%d",(kal_uint32)data_w_ptr,copy_size);
                xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write u4BufferSize=%d, u4DataRemained=%d",Afe_Block->u4BufferSize, Afe_Block->u4DataRemained);
            }
            else
            {
					if(copy_from_user((Afe_Block->pucVirtBufAddr+Afe_WriteIdx_tmp),data_w_ptr,copy_size))
					{
						xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write Fail copy from user");
						return -1;
					}
            }
            spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);
            Afe_Block->u4DataRemained += copy_size;
            Afe_Block->u4WriteIdx = Afe_WriteIdx_tmp + copy_size;
            Afe_Block->u4WriteIdx %= Afe_Block->u4BufferSize;
            spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);
            data_w_ptr += copy_size;
            count -= copy_size;
            PRINTK_AUDDRV("AudDrv_write finish1, copy_size:%x, WriteIdx:%x, ReadIdx=%x, DataRemained:%x, count=%x \r\n",copy_size,Afe_Block->u4WriteIdx,Afe_Block->u4DMAReadIdx,Afe_Block->u4DataRemained,count);
         }
         else  // copy twice
         {
            kal_uint32 size_1 = 0;
            kal_uint32 size_2 = 0;

            size_1 = Afe_Block->u4BufferSize - Afe_WriteIdx_tmp;
            size_2 = copy_size - size_1;

            if(!access_ok (VERIFY_READ,data_w_ptr,size_1)){
                xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write 1ptr invalid data_w_ptr=%x, size_1=%d",(kal_uint32)data_w_ptr,size_1);
                xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write u4BufferSize=%d, u4DataRemained=%d",Afe_Block->u4BufferSize, Afe_Block->u4DataRemained);
            }
            else
            {
					if ((copy_from_user( (Afe_Block->pucVirtBufAddr + Afe_WriteIdx_tmp), data_w_ptr , size_1)) )
					{
						xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write Fail 1 copy from user");
						return -1;
					}
            }
            spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);
            Afe_Block->u4DataRemained += size_1;
            Afe_Block->u4WriteIdx = Afe_WriteIdx_tmp+size_1;
            Afe_Block->u4WriteIdx %= Afe_Block->u4BufferSize;
				Afe_WriteIdx_tmp = Afe_Block->u4WriteIdx;
            spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);
            if(!access_ok (VERIFY_READ,data_w_ptr+size_1, size_2)){
                xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write 2ptr invalid data_w_ptr=%x, size_1=%d, size_2=%d",(kal_uint32)data_w_ptr,size_1,size_2);
                xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write u4BufferSize=%d, u4DataRemained=%d",Afe_Block->u4BufferSize, Afe_Block->u4DataRemained);
            }
            else
            {
					if ((copy_from_user((Afe_Block->pucVirtBufAddr+Afe_WriteIdx_tmp),(data_w_ptr+size_1), size_2)))
               {
               	xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write Fail 2 copy from user");
                  return -1;
               }
            }
            spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);
            Afe_Block->u4DataRemained += size_2;
            Afe_Block->u4WriteIdx = Afe_WriteIdx_tmp+size_2;
            Afe_Block->u4WriteIdx %= Afe_Block->u4BufferSize;
            spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);
            count -= copy_size;
            data_w_ptr += copy_size;
            PRINTK_AUDDRV("AudDrv_write finish2, copy size:%x, WriteIdx:%x, ReadIdx=%x DataRemained:%x \r\n",copy_size,Afe_Block->u4WriteIdx,Afe_Block->u4DMAReadIdx,Afe_Block->u4DataRemained );
         }
      }

      if(count != 0)
      {
         unsigned long long t1,t2;
			int i;
         PRINTK_AUDDRV("AudDrv_write wait for interrupt count=%x \n",count);

         wait_queue_flag =0;
         t1 = sched_clock(); // in ns (10^9)
         ret = wait_event_interruptible_timeout(DL_Wait_Queue, wait_queue_flag,(Afe_wait_priod*2)); // 100ms

         t2 = sched_clock()-t1; // in ns (10^9)
         ret1=1;

			#ifdef AUD_DUMPFTRACE
			aud_write_idx++;
			if(aud_write_idx==6)
			{
				aud_write_idx = 0;
			}
			audio_blocked_frame[aud_write_idx] = 0;
			#endif

         if((m_Interrupt_time!=0) && ((t2) > ((((unsigned long long)m_Interrupt_time)*11*1000000/8)))) // 1.x times interrupt timer. unit: ns
         {
            xlog_printk(ANDROID_LOG_ERROR, "Sound","timeout,Audio blocked by other modules(%llu)ns,(%d)ms,(%d)jiffies \n",t2,m_Interrupt_time,Afe_wait_priod);
				#ifdef AUD_DUMPFTRACE //dump ftrace if (1)continuously blocked 3 times or (2) blocked 3 times in last 6 times
				if(aud_dumpftrace_dbg==1 && aud_dumpftrace_done==0 )
				{
					audio_blocked_frame[aud_write_idx] = 1;
					aud_block_framesin6 = 0;

					for(i=0;i<5;i++)
					{
						aud_block_framesin6 += audio_blocked_frame[i];
				 	}

					if(aud_blocked_cnt==2 || aud_block_framesin6>=3)
					{
						xlog_printk(ANDROID_LOG_ERROR, "Sound","Audio Dump FTrace, aud_blocked_cnt=%d, aud_block_framesin6=%d \n",aud_blocked_cnt,aud_block_framesin6);
					 	aee_kernel_exception_api(__FILE__, __LINE__, DB_OPT_FTRACE, "Audio is blocked", "audio blocked dump ftrace");
					 	aud_dumpftrace_done = 1;
					}
					else
					{
						xlog_printk(ANDROID_LOG_ERROR, "Sound","aud_blocked_cnt++ \n");
						aud_blocked_cnt++;
					}
				}
				#endif
            ret1 = -1;
         }
         else if((m_Interrupt_time!=0)&&((t2) > ((((unsigned long long)m_Interrupt_time)+4)*1000000))) // check the int timer+4ms
         {
             xlog_printk(ANDROID_LOG_ERROR, "Sound","[Warning]Audio is blocked by others(%llu)ns,(%d)ms \n",t2,m_Interrupt_time);
             xlog_printk(ANDROID_LOG_ERROR, "Sound","[Warning]Audio Sound may be abnormal \n");
         }

         if((ret <= 0) || (ret1 < 0))
         {
             kal_uint32 irq_mcu     = Afe_Get_Reg(AFE_IRQ_CON);
             kal_uint32 irq1_mcu_on = irq_mcu & 0x1;
				 if(irq1_mcu_on == 0x1)
             {
                 xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write timeout,[Warning]blocked by others..(0x%x),(%llu)ns,(%d)ms,(%d)jiffies \n",irq_mcu,t2,m_Interrupt_time,Afe_wait_priod);
					spin_lock_bh(&auddrv_SphCtlState_lock);
               if( (SPH_Ctrl_State.bSpeechFlag==false) && (SPH_Ctrl_State.bBgsFlag==false) && (SPH_Ctrl_State.bTtyFlag==false)
                   && (SPH_Ctrl_State.bVT==false) && (SPH_Ctrl_State.bRecordFlag==false) ){
					 	   spin_unlock_bh(&auddrv_SphCtlState_lock);

                     Afe_Set_Reg(AFE_IRQ_CON,0x0,0x1);
                     Afe_Set_Reg(AFE_DAC_CON0,0x0,0x2);  // DL1_ON=0
                     spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);
                     AFE_dl_Control->rBlock.u4WriteIdx    = 0;
                     AFE_dl_Control->rBlock.u4DMAReadIdx     = 0;
                     AFE_dl_Control->rBlock.u4DataRemained = AFE_dl_Control->rBlock.u4BufferSize;
                     AFE_dl_Control->rBlock.u4fsyncflag    = false;
                     spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);
                     xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write timeout,restart \n");
                     msleep(1);
                     Afe_Set_Reg(AFE_DAC_CON0,0x2,0x2);
                     Afe_Set_Reg(AFE_IRQ_CON,0x1,0x1);
                 }else{
						 	xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write timeout, not restart, bBgsFlag:%d,bRecordFlag:%d,bSpeechFlag:%d,bTtyFlag:%d,bVT:%d,bAudio:%d \n",
                            SPH_Ctrl_State.bBgsFlag,
                            SPH_Ctrl_State.bRecordFlag,
                            SPH_Ctrl_State.bSpeechFlag,
                            SPH_Ctrl_State.bTtyFlag,
                            SPH_Ctrl_State.bVT,
                            SPH_Ctrl_State.bAudioPlay);
							spin_unlock_bh(&auddrv_SphCtlState_lock);
                 }
             }
             else
             {
                 xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_write timeout, No Aud Int (0x%x),(%llu)ns,(%d)ms,(%d)jiffies!\n",irq_mcu,t2,m_Interrupt_time,Afe_wait_priod);
             }
             return written_size;
         }
         #ifdef AUD_DUMPFTRACE
			//xlog_printk(ANDROID_LOG_ERROR, "Sound","aud_blocked_cnt reset to 0 \n");
			aud_blocked_cnt = 0;
         #endif
      }

   }

   return written_size;
}

ssize_t AudDrv_AWB_Read(struct file *fp,  char __user *data, size_t count, loff_t *offset){

    AFE_BLOCK_T  *AWB_Block = NULL;
    int ret = 0;
    char *Read_Data_Ptr = (char*)data;
    ssize_t DMA_Read_Ptr =0 , read_size = 0,read_count = 0;
    AWB_Block = &(AWB_input_Control->rBlock);

    PRINTK_AUDDRV("+AudDrv_AWB_Read count:%x \n", count);  //I2S input data count that can be copy_block
    if(AWB_Block->u4BufferSize == 0){
        xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_AWB_Read: u4BufferSize=0, Error");
        msleep(AWB_TIMER_INTERVAL);
        return -1;
    }

    while(count)
    {
        if(AWB_Block->pucPhysBufAddr == 0){
            xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_read pucPhysBufAddr == NULL \n");
            break;
        }

		  spin_lock_bh(&auddrv_AWBInCtl_lock);
        if( AWB_Block->u4DataRemained >  AWB_Block->u4BufferSize){
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_AWB_Read u4DataRemained=%x > u4BufferSize=%x" ,AWB_Block->u4DataRemained, AWB_Block->u4BufferSize);
            AWB_Block->u4DataRemained = 0;
            AWB_Block->u4DMAReadIdx   = AWB_Block->u4WriteIdx;
        }
        if(count >  AWB_Block->u4DataRemained){
            read_size = AWB_Block->u4DataRemained;
        }
        else{
            read_size = count;
        }
        DMA_Read_Ptr = AWB_Block->u4DMAReadIdx;
		  spin_unlock_bh(&auddrv_AWBInCtl_lock);

        PRINTK_AUDDRV("AudDrv_read finish0, read_count:%x, read_size:%x, u4DataRemained:%x, u4DMAReadIdx:0x%x, u4WriteIdx:%x \r\n",
            read_count,read_size,AWB_Block->u4DataRemained,AWB_Block->u4DMAReadIdx,AWB_Block->u4WriteIdx);
        if(DMA_Read_Ptr+read_size < AWB_Block->u4BufferSize)
        {
        #ifndef SOUND_FAKE_READ
            if(DMA_Read_Ptr != AWB_Block->u4DMAReadIdx){
                xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_read 1, read_size:%x, DataRemained:%x, DMA_Read_Ptr:0x%x, DMAReadIdx:%x \r\n",
                    read_size,AWB_Block->u4DataRemained,DMA_Read_Ptr,AWB_Block->u4DMAReadIdx);
            }
            if(copy_to_user((void __user *)Read_Data_Ptr,(AWB_Block->pucVirtBufAddr+DMA_Read_Ptr),read_size))
            {
                xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_read Fail 1 copy to user Read_Data_Ptr:%p, pucVirtBufAddr:%p, u4DMAReadIdx:0x%x, DMA_Read_Ptr:0x%x, read_size:%x",
                Read_Data_Ptr,AWB_Block->pucVirtBufAddr, AWB_Block->u4DMAReadIdx, DMA_Read_Ptr, read_size);
            if(read_count == 0){
                return -1;
            }
            else {
                return read_count;
            }
         }
        #else
            copy_to_user_fake(Read_Data_Ptr ,read_size);
        #endif
         read_count += read_size;

				spin_lock_bh(&auddrv_AWBInCtl_lock);
            AWB_Block->u4DataRemained -= read_size;
            AWB_Block->u4DMAReadIdx += read_size;
            AWB_Block->u4DMAReadIdx %= AWB_Block->u4BufferSize;
            DMA_Read_Ptr = AWB_Block->u4DMAReadIdx;
				spin_unlock_bh(&auddrv_AWBInCtl_lock);

            Read_Data_Ptr += read_size;
            count -= read_size;
            PRINTK_AUDDRV("AudDrv_AWB_Read finish1, copy size:%x, u4DMAReadIdx:0x%x, u4WriteIdx:%x, u4DataRemained:%x \r\n",read_size,AWB_Block->u4DMAReadIdx,AWB_Block->u4WriteIdx,AWB_Block->u4DataRemained );
        }
        else
        {
            kal_uint32 size_1 = AWB_Block->u4BufferSize - DMA_Read_Ptr;
            kal_uint32 size_2 = read_size - size_1;
        #ifndef SOUND_FAKE_READ
            if(DMA_Read_Ptr != AWB_Block->u4DMAReadIdx){
                xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_AWB_Read 2, read_size1:%x, DataRemained:%x, DMA_Read_Ptr:0x%x, DMAReadIdx:%x \r\n",
                    size_1,AWB_Block->u4DataRemained,DMA_Read_Ptr,AWB_Block->u4DMAReadIdx);
            }
            if (copy_to_user( (void __user *)Read_Data_Ptr,(AWB_Block->pucVirtBufAddr+DMA_Read_Ptr),size_1))
            {
                xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_AWB_Read Fail 2 copy to user Read_Data_Ptr:%p, pucVirtBufAddr:%p, u4DMAReadIdx:0x%x, DMA_Read_Ptr:0x%x, read_size:%x",
                Read_Data_Ptr,AWB_Block->pucVirtBufAddr, AWB_Block->u4DMAReadIdx, DMA_Read_Ptr, read_size);
            if(read_count == 0){
                return -1;
            }
            else {
                return read_count;
            }
	 }
        #else
            copy_to_user_fake(Read_Data_Ptr,size_1);
        #endif
         read_count += size_1;

				spin_lock_bh(&auddrv_AWBInCtl_lock);
            AWB_Block->u4DataRemained -= size_1;
            AWB_Block->u4DMAReadIdx += size_1;
            AWB_Block->u4DMAReadIdx %= AWB_Block->u4BufferSize;
            DMA_Read_Ptr = AWB_Block->u4DMAReadIdx;
				spin_unlock_bh(&auddrv_AWBInCtl_lock);

            PRINTK_AUDDRV("AudDrv_read finish2, copy size_1:%x, u4DMAReadIdx:0x%x, u4WriteIdx:0x%x, u4DataRemained:%x \r\n",
                size_1,AWB_Block->u4DMAReadIdx,AWB_Block->u4WriteIdx,AWB_Block->u4DataRemained );
        #ifndef SOUND_FAKE_READ
            if(DMA_Read_Ptr != AWB_Block->u4DMAReadIdx){
                xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_AWB_Read 3, read_size2:%x, DataRemained:%x, DMA_Read_Ptr:0x%x, DMAReadIdx:%x \r\n",
                    size_2,AWB_Block->u4DataRemained,DMA_Read_Ptr,AWB_Block->u4DMAReadIdx);
            }
            if(copy_to_user(  (void __user *)(Read_Data_Ptr+size_1),(AWB_Block->pucVirtBufAddr + DMA_Read_Ptr), size_2))
            {
                xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_AWB_Read Fail 3 copy to user Read_Data_Ptr:%p, pucVirtBufAddr:%p, u4DMAReadIdx:0x%x, DMA_Read_Ptr:0x%x, read_size:%x",
                    Read_Data_Ptr, AWB_Block->pucVirtBufAddr, AWB_Block->u4DMAReadIdx, DMA_Read_Ptr, read_size);
            if(read_count == 0){
                return -1;
            }
            else {
                return read_count;
            }
         }
        #else
            copy_to_user_fake((Read_Data_Ptr+size_1),size_2);
        #endif
         read_count += size_2;

				spin_lock_bh(&auddrv_AWBInCtl_lock);
            AWB_Block->u4DataRemained -= size_2;
            AWB_Block->u4DMAReadIdx += size_2;
            DMA_Read_Ptr = AWB_Block->u4DMAReadIdx;
				spin_unlock_bh(&auddrv_AWBInCtl_lock);

            count -= read_size;
            Read_Data_Ptr += read_size;
            PRINTK_AUDDRV("AudDrv_AWB_Read finish3, copy size_2:%x, u4DMAReadIdx:0x%x, u4WriteIdx:0x%x u4DataRemained:%x \r\n",
                size_2,AWB_Block->u4DMAReadIdx,AWB_Block->u4WriteIdx,AWB_Block->u4DataRemained );
        }
        if(count != 0){
            PRINTK_AUDDRV("AudDrv_AWB_Read wait for interrupt signal\n");
            AWB_wait_queue_flag =0;
            ret = wait_event_interruptible_timeout(AWB_Wait_Queue,AWB_wait_queue_flag,AWB_TIMER_TIMEOUT);
            if(ret <= 0)
            {
                xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_AWB_Read wait_event_interruptible_timeout, No Audio Interrupt! \n");
                return read_count;
            }
        }
    }
   return read_count;
}

ssize_t AudDrv_I2S_Read(struct file *fp,  char __user *data, size_t count, loff_t *offset)
{
   //for I2S input stream read data to HW buffer
   AFE_BLOCK_T  *I2Sin_Block = NULL;
   int ret;
	unsigned long flags;
   char *Read_Data_Ptr = (char*)data;
   int DMA_Read_Ptr=0;
   ssize_t read_size = 0;
   ssize_t read_count = 0;
   PRINTK_AUDDRV("+AudDrv_I2S_Read count:%x \n", count);  //I2S input data count that can be copy_block
   PRINTK_AUDDRV("+AudDrv_read count:0x%x \n", count);  //I2S input data count that can be copy

   I2Sin_Block = &(I2S_input_Control->rBlock);

   if(I2Sin_Block->u4BufferSize == 0){
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_read: u4BufferSize=0, Error");
      msleep(AFE_INT_TIMEOUT);
      return -1;
   }

   while(count)
   {
      if(I2Sin_Block->pucPhysBufAddr == 0){
         xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_read pucPhysBufAddr == NULL \n");
         break;
      }

		spin_lock_irqsave(&auddrv_I2SInCtl_lock, flags);
      if( I2Sin_Block->u4DataRemained >  I2Sin_Block->u4BufferSize){
         xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_read u4DataRemained=%x > u4BufferSize=%x" ,I2Sin_Block->u4DataRemained, I2Sin_Block->u4BufferSize);
         I2Sin_Block->u4DataRemained = 0;
         I2Sin_Block->u4DMAReadIdx   = I2Sin_Block->u4WriteIdx;
      }
      if(count >  I2Sin_Block->u4DataRemained){    // u4DataRemained --> data store in HW buffer
         read_size = I2Sin_Block->u4DataRemained;
      }
      else{
         read_size = count;
      }
      DMA_Read_Ptr = I2Sin_Block->u4DMAReadIdx;
		spin_unlock_irqrestore(&auddrv_I2SInCtl_lock, flags);

      PRINTK_AUDDRV("AudDrv_read finish0, read_count:%x, read_size:%x, u4DataRemained:%x, u4DMAReadIdx:0x%x, u4WriteIdx:%x \r\n",read_count,read_size,I2Sin_Block->u4DataRemained,I2Sin_Block->u4DMAReadIdx,I2Sin_Block->u4WriteIdx);

      if(DMA_Read_Ptr+read_size < I2Sin_Block->u4BufferSize)
      {
#ifndef SOUND_FAKE_READ
         if(DMA_Read_Ptr != I2Sin_Block->u4DMAReadIdx){
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_read 1, read_size:%x, DataRemained:%x, DMA_Read_Ptr:0x%x, DMAReadIdx:%x \r\n",read_size,I2Sin_Block->u4DataRemained,DMA_Read_Ptr,I2Sin_Block->u4DMAReadIdx);
         }
         if(copy_to_user((void __user *)Read_Data_Ptr,(I2Sin_Block->pucVirtBufAddr+DMA_Read_Ptr),read_size))
         {
            xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_read Fail 1 copy to user Read_Data_Ptr:%p, pucVirtBufAddr:%p, u4DMAReadIdx:0x%x, DMA_Read_Ptr:0x%x, read_size:%x",
                     Read_Data_Ptr,I2Sin_Block->pucVirtBufAddr, I2Sin_Block->u4DMAReadIdx, DMA_Read_Ptr, read_size);
            if(read_count == 0){
            return -1;
         }
            else {
                return read_count;
            }
         }
#else
         copy_to_user_fake(Read_Data_Ptr ,read_size);
#endif
         read_count += read_size;

			spin_lock_irqsave(&auddrv_I2SInCtl_lock, flags);
         I2Sin_Block->u4DataRemained -= read_size;
         I2Sin_Block->u4DMAReadIdx += read_size;
         I2Sin_Block->u4DMAReadIdx %= I2Sin_Block->u4BufferSize;
         DMA_Read_Ptr = I2Sin_Block->u4DMAReadIdx;
			spin_unlock_irqrestore(&auddrv_I2SInCtl_lock, flags);

         Read_Data_Ptr += read_size;
         count -= read_size;
         PRINTK_AUDDRV("AudDrv_read finish1, copy size:%x, u4DMAReadIdx:0x%x, u4WriteIdx:%x, u4DataRemained:%x \r\n",read_size,I2Sin_Block->u4DMAReadIdx,I2Sin_Block->u4WriteIdx,I2Sin_Block->u4DataRemained );
      }
      else
      {
         kal_uint32 size_1 = I2Sin_Block->u4BufferSize - DMA_Read_Ptr;
         kal_uint32 size_2 = read_size - size_1;

#ifndef SOUND_FAKE_READ
         if(DMA_Read_Ptr != I2Sin_Block->u4DMAReadIdx){
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_read 2, read_size1:%x, DataRemained:%x, DMA_Read_Ptr:0x%x, DMAReadIdx:%x \r\n",size_1,I2Sin_Block->u4DataRemained,DMA_Read_Ptr,I2Sin_Block->u4DMAReadIdx);
         }
         if(copy_to_user( (void __user *)Read_Data_Ptr,(I2Sin_Block->pucVirtBufAddr+DMA_Read_Ptr),size_1))
         {
            xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_read Fail 2 copy to user Read_Data_Ptr:%p, pucVirtBufAddr:%p, u4DMAReadIdx:0x%x, DMA_Read_Ptr:0x%x, read_size:%x",
                     Read_Data_Ptr,I2Sin_Block->pucVirtBufAddr, I2Sin_Block->u4DMAReadIdx, DMA_Read_Ptr, read_size);
            if(read_count == 0){
	         return -1;
	      }
            else {
                return read_count;
            }
	 }
#else
	      copy_to_user_fake(Read_Data_Ptr,size_1);
#endif
         read_count += size_1;

			spin_lock_irqsave(&auddrv_I2SInCtl_lock, flags);
         I2Sin_Block->u4DataRemained -= size_1;
         I2Sin_Block->u4DMAReadIdx += size_1;
         I2Sin_Block->u4DMAReadIdx %= I2Sin_Block->u4BufferSize;
         DMA_Read_Ptr = I2Sin_Block->u4DMAReadIdx;
			spin_unlock_irqrestore(&auddrv_I2SInCtl_lock, flags);

         PRINTK_AUDDRV("AudDrv_read finish2, copy size_1:%x, u4DMAReadIdx:0x%x, u4WriteIdx:0x%x, u4DataRemained:%x \r\n",size_1,I2Sin_Block->u4DMAReadIdx,I2Sin_Block->u4WriteIdx,I2Sin_Block->u4DataRemained );

#ifndef SOUND_FAKE_READ
         if(DMA_Read_Ptr != I2Sin_Block->u4DMAReadIdx){
            xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_read 3, read_size2:%x, DataRemained:%x, DMA_Read_Ptr:0x%x, DMAReadIdx:%x \r\n",size_2,I2Sin_Block->u4DataRemained,DMA_Read_Ptr,I2Sin_Block->u4DMAReadIdx);
         }
         if (copy_to_user(  (void __user *)(Read_Data_Ptr+size_1),(I2Sin_Block->pucVirtBufAddr + DMA_Read_Ptr), size_2))
         {
            xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_read Fail 3 copy to user Read_Data_Ptr:%p, pucVirtBufAddr:%p, u4DMAReadIdx:0x%x, DMA_Read_Ptr:0x%x, read_size:%x",
                     Read_Data_Ptr, I2Sin_Block->pucVirtBufAddr, I2Sin_Block->u4DMAReadIdx, DMA_Read_Ptr, read_size);
            if(read_count == 0){
            return -1;
         }
            else {
                return read_count;
            }
         }
#else
         copy_to_user_fake((Read_Data_Ptr+size_1),size_2);
#endif
         read_count += size_2;

			spin_lock_irqsave(&auddrv_I2SInCtl_lock, flags);
         I2Sin_Block->u4DataRemained -= size_2;
         I2Sin_Block->u4DMAReadIdx += size_2;
         DMA_Read_Ptr = I2Sin_Block->u4DMAReadIdx;
			spin_unlock_irqrestore(&auddrv_I2SInCtl_lock, flags);

         count -= read_size;
         Read_Data_Ptr += read_size;
         PRINTK_AUDDRV("AudDrv_read finish3, copy size_2:%x, u4DMAReadIdx:0x%x, u4WriteIdx:0x%x u4DataRemained:%x \r\n",size_2,I2Sin_Block->u4DMAReadIdx,I2Sin_Block->u4WriteIdx,I2Sin_Block->u4DataRemained );
      }

      if(count != 0){
         //PRINTK_AUDDRV("read wait for interrupt signal\n");
         I2S_wait_queue_flag =0;
         ret = wait_event_interruptible_timeout(I2Sin_Wait_Queue,I2S_wait_queue_flag,I2S_INPUT_INT_TIMEOUT);
         if(ret <= 0)
         {
            xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_read wait_event_interruptible_timeout, No Audio Interrupt! \n");
            return read_count;
         }
      }
   }

   PRINTK_AUDDRV("-AudDrv_read, 0x%x, 0x%x, 0x%x, 0x%x \n",*Read_Data_Ptr,*(Read_Data_Ptr+1),*(Read_Data_Ptr+2),*(Read_Data_Ptr+3));
   PRINTK_AUDDRV("-AudDrv_read, read_count:%x \n",read_count);
   return read_count;
}

/*****************************************************************************
 * FILE OPERATION FUNCTION
 *  AudDrv_read
 *
 * DESCRIPTION
 *  User space Read data from (kernel)HW buffer
 *
 * PARAMETERS
 *  fp      [in]
 *  data    [in] data pointer
 *  count   [in] number of bytes to be written
 *  offset  [in] no use
 *
 * RETURNS
 *  None
 *
 *****************************************************************************
 */
static ssize_t AudDrv_read(struct file *fp,  char __user *data, size_t count, loff_t *offset)
{
    kal_uint32 read_count =0;
    if(fp == I2S_input_Control->rBlock.flip){
        read_count = AudDrv_I2S_Read(fp, data,  count, offset);
    }
    else if(fp == AWB_input_Control->rBlock.flip){
        read_count = AudDrv_AWB_Read(fp,data, count, offset);
    }
    else{
        xlog_printk(ANDROID_LOG_ERROR, "Sound","no match fp to read !!!!\n");
    }
    return read_count;
}

/*****************************************************************************
 * FUNCTION
 *  AudDrv_Reset , AudDrv_flush
 *
 * DESCRIPTION
 *  Notify the message to user space
 *
 *
 *****************************************************************************
 */
static int AudDrv_Reset(void)
{
	unsigned long flags;
   xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_Reset \n");

   if(AFE_dl_Control->rBlock.pucVirtBufAddr != NULL){
      if(!Flag_Aud_DL1_SlaveOn)
       {//Audio DL1 master mode used
#ifdef DL1_USE_SYSRAM
        SYSRAM_AUDIO_FREE();
#elif (!defined (DL1_USE_FLEXL2))
////////////
// Use DMA free coherent
        dma_free_coherent(0, AFE_dl_Control->rBlock.u4BufferSize, AFE_dl_Control->rBlock.pucVirtBufAddr, AFE_dl_Control->rBlock.pucPhysBufAddr);
#endif
       }
////////////
// Use kfree
//      kfree((void*)AFE_dl_Control->rBlock.pucVirtBufAddr);

   }
	spin_lock_irqsave(&auddrv_DL1Ctl_lock, flags);
	AFE_dl_Control->rBlock.u4SampleNumMask = 0;
   AFE_dl_Control->rBlock.u4WriteIdx	   = 0;
   AFE_dl_Control->rBlock.u4DMAReadIdx    = 0;
   AFE_dl_Control->rBlock.u4DataRemained  = 0;
   AFE_dl_Control->rBlock.u4fsyncflag     = false;
	spin_unlock_irqrestore(&auddrv_DL1Ctl_lock, flags);

   if(I2S_input_Control->rBlock.pucVirtBufAddr != NULL){

#if (!defined (I2SIN_USE_FLEXL2))
////////////
// Use DMA free coherent
      xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_DeInit_I2S_InputStream I2S_input_Control->rBlock.u4BufferSize=%d , sizex1\n", I2S_input_Control->rBlock.u4BufferSize);
//      dma_free_coherent(0, I2S_input_Control->rBlock.u4BufferSize, I2S_input_Control->rBlock.pucVirtBufAddr, I2S_input_Control->rBlock.pucPhysBufAddr);
#endif

////////////
// Use kfree
//      kfree((void*)I2S_input_Control->rBlock.pucVirtBufAddr);

   }

	spin_lock_irqsave(&auddrv_I2SInCtl_lock, flags);
	I2S_input_Control->rBlock.u4SampleNumMask = 0;
   I2S_input_Control->rBlock.u4WriteIdx	   = 0;
   I2S_input_Control->rBlock.u4DMAReadIdx    = 0;
   I2S_input_Control->rBlock.u4DataRemained  = 0;
   I2S_input_Control->rBlock.flip                       = NULL;
   I2S_input_Control->rBlock.u4fsyncflag    = false;
	spin_unlock_irqrestore(&auddrv_I2SInCtl_lock, flags);

   //Reset Amp Flag
  // AMP_Flag = false;

   // if AWB is enable , reset AWB stream
   spin_lock_bh(&auddrv_lock);
   if(Aud_AWB_Clk_cntr){
		spin_unlock_bh(&auddrv_lock);
       Auddrv_AWB_timer_off();
       AudDrv_Reset_AWB_Stream ();
       wake_unlock(&Audio_record_wake_lock);
   }
	else
	{
		spin_unlock_bh(&auddrv_lock);
	}
   return 1;
}

static int AudDrv_flush(struct file *flip, fl_owner_t id)
{
   PRINTK_AUDDRV("+AudDrv_flush \n");
   /*
   // flush will set all register to default
   AMP_Flag = false;
   Afe_Pwr_on = 0;
   Aud_AFE_Clk_cntr =0;
   AudDrv_Reset();

   // TBD.. turn off power
   AudDrv_Clk_Off();  // turn off asm afe clock
//   Afe_Disable_Memory_Power ();	//disable memory buffer
*/
/* //Move ot AudDrv_ioctl(INIT_DL1_STREAM)
   if( (SPH_Ctrl_State.bSpeechFlag==false) && (SPH_Ctrl_State.bVT==false) && (SPH_Ctrl_State.bRecordFlag==false) ){
      // Disable interrupt (timer)
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_flush, Dis DL_SRC2, IRQ, AFE clk(%d) \n",Aud_AFE_Clk_cntr);
      if(Aud_AFE_Clk_cntr!=0)
      {
          AudDrv_Clk_On();
          Afe_Set_Reg(AFE_DL_SRC2_1,0x0,0x1);   // DL2_SRC2_ON=0
          Afe_Set_Reg(AFE_IRQ_CON,0x0,0x1);
          Afe_Set_Reg(AFE_DAC_CON0,0x0,0x2);  // DL1_ON=0
          Afe_Set_Reg(AFE_IR_CLR,0x1,0x1);
          Afe_Set_Reg(AFE_IRQ_CON,0x0,0x2);  // bit1: I2S,  IRQ2_MCU_ON
          Afe_Set_Reg(AFE_IR_CLR,0x2,0x2);           // bit1: I2S,  IRQ1_MCU_CLR
          AudDrv_Clk_Off();
      }
   }
*/
   Aud_Flush_cntr++;

   xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv_flush cntr(%d)\n", Aud_Flush_cntr);
   return 0;
}

/*****************************************************************************
 * FUNCTION
 *  AudDrv_fasync
 *
 * DESCRIPTION
 *  Notify the message to user space
 *
 * PARAMETERS
 *  fp   [in]
 *  flip [in]
 *  mode [in]
 *
 * RETURNS
 *  None
 *
 *****************************************************************************
 */
static int AudDrv_fasync(int fd, struct file *flip, int mode)
{
   PRINTK_AUDDRV("AudDrv_fasync \n");
   return fasync_helper(fd,flip,mode,&AudDrv_async);
}


/*****************************************************************************
 * STRUCT
 *  VM Operations
 *
 *****************************************************************************
 */
void AudDrv_vma_open(struct vm_area_struct *vma)
{
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_vma_open virt:%lx, phys:%lx, length:%lx \n",vma->vm_start, vma->vm_pgoff<<PAGE_SHIFT,vma->vm_end - vma->vm_start);
}

void AudDrv_vma_close(struct vm_area_struct *vma)
{
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_vma_close virt");
}

static struct vm_operations_struct AudDrv_remap_vm_ops =
{
   .open  = AudDrv_vma_open,
   .close = AudDrv_vma_close
};

static int AudDrv_remap_mmap(struct file *flip, struct vm_area_struct *vma)
{
	PRINTK_AUDDRV("AudDrv_remap_mmap \n");

   vma->vm_pgoff =( AFE_dl_Control->rBlock.pucPhysBufAddr)>>PAGE_SHIFT;
   if(remap_pfn_range(vma , vma->vm_start , vma->vm_pgoff ,
   	vma->vm_end - vma->vm_start , vma->vm_page_prot) < 0)
   {
      xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_remap_mmap remap_pfn_range Fail \n");
      return -EIO;
   }
   vma->vm_ops = &AudDrv_remap_vm_ops;
   AudDrv_vma_open(vma);
   return 0;
}

/*****************************************************************************
 * STRUCT
 *  File Operations and misc device
 *
 *****************************************************************************
 */

static struct file_operations AudDrv_fops = {
   .owner   = THIS_MODULE,
   .open    = AudDrv_open,
   .release = AudDrv_release,
   .unlocked_ioctl   = AudDrv_ioctl,
   .write   = AudDrv_write,
   .read   	= AudDrv_read,
   .flush   = AudDrv_flush,
   .fasync	= AudDrv_fasync,
   .mmap    = AudDrv_remap_mmap
};

static struct miscdevice AudDrv_audio_device = {
   .minor = MISC_DYNAMIC_MINOR,
   .name = "eac",
   .fops = &AudDrv_fops,
};

/*****************************************************************************
 * FUNCTION
 *  AudDrv_Suspend_Clk_Off / AudDrv_Suspend_Clk_On
 *
 * DESCRIPTION
 *  Enable/Disable AFE clock for suspend
 *
 *****************************************************************************
 */
void AudDrv_Suspend_Clk_On(void)
{
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Suspend_Clk_On Aud_AFE_Clk_cntr:%d ANA_Clk(%d) \n",Aud_AFE_Clk_cntr,Aud_ANA_Clk_cntr);
   spin_lock_bh(&auddrv_lock);
   if(Aud_AFE_Clk_cntr>0)
   {
#ifdef USE_PM_API
   if(enable_clock(MT65XX_PDN_AUDIO_AFE,"AUDIO"))
      xlog_printk(ANDROID_LOG_ERROR, "Sound","Aud enable_clock MT65XX_PDN_AUDIO_AFE fail");
      if(Aud_ADC_Clk_cntr>0)
      {
         if(enable_clock(MT65XX_PDN_AUDIO_ADC,"AUDIO"))
            xlog_printk(ANDROID_LOG_ERROR, "Sound","Aud enable_clock MT65XX_PDN_AUDIO_ADC fail");
      }

      if(Aud_I2S_Clk_cntr>0)
      {
         if(enable_clock(MT65XX_PDN_AUDIO_I2S,"AUDIO"))
            xlog_printk(ANDROID_LOG_ERROR, "Sound","Aud enable_clock MT65XX_PDN_AUDIO_I2S fail");
      }
      // Enable AFE clock
/* No PLL CON2 in 6577
      //Ana_Set_Reg(PLL_CON2,0x20,0x20);             // turn on AFE (26M Clock)
*/
      Ana_Set_Reg(AUDIO_CON3,0x0,0x2);             // Enable Audio Bias (VaudlbiasDistrib)
      Afe_Set_Reg(AUDIO_TOP_CON0, 0x0, 0x00010000);  // bit16: power on AFE_CK_DIV_RST
#else
      Afe_Set_Reg(AUDIO_TOP_CON0,0x0,0xffffffff);  // bit2: power down AFE clock
#endif
   }

    if(Aud_ANA_Clk_cntr>0)
    {
        sc_request_mdbus_clk(SC_MDBUS_USER_AUDIO);
    }
   spin_unlock_bh(&auddrv_lock);
}

void AudDrv_Suspend_Clk_Off(void)
{
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Suspend_Clk_Off Aud_AFE_Clk_cntr:%d ANA_Clk(%d)\n",Aud_AFE_Clk_cntr,Aud_ANA_Clk_cntr);
   spin_lock_bh(&auddrv_lock);
   if(Aud_AFE_Clk_cntr>0)
   {
      // Disable AFE clock
      Ana_Set_Reg(AUDIO_CON3,0x2,0x2);       // Disable Audio Bias (VaudlbiasDistrib power down)
      #ifdef USE_PM_API
      Afe_Set_Reg(AUDIO_TOP_CON0, 0x00010000, 0x00010000);  // bit16: power off AFE_CK_DIV_RST

      if(disable_clock(MT65XX_PDN_AUDIO_AFE,"AUDIO"))
             xlog_printk(ANDROID_LOG_ERROR, "Sound","disable_clock MT65XX_PDN_AUDIO_AFE fail");

      if(Aud_ADC_Clk_cntr>0)
      {
         if(disable_clock(MT65XX_PDN_AUDIO_ADC,"AUDIO"))
             xlog_printk(ANDROID_LOG_ERROR, "Sound","disable_clock MT65XX_PDN_AUDIO_ADC fail");
      }
      if(Aud_I2S_Clk_cntr>0)
      {
         if(disable_clock(MT65XX_PDN_AUDIO_I2S,"AUDIO"))
             xlog_printk(ANDROID_LOG_ERROR, "Sound","disable_clock MT65XX_PDN_AUDIO_I2S fail");
      }
      #else
      Afe_Set_Reg(AUDIO_TOP_CON0,0x10074,0x10074);   // bit2: power down AFE clock
      #endif
   }

    if(Aud_ANA_Clk_cntr>0)
    {
        sc_unrequest_mdbus_clk(SC_MDBUS_USER_AUDIO);
    }
   spin_unlock_bh(&auddrv_lock);
}

/*****************************************************************************
 * PLATFORM DRIVER FUNCTION:
 *
 *  AudDrv_probe / AudDrv_suspend / AudDrv_resume / AudDrv_shutdown / AudDrv_remove
 *
 * DESCRIPTION
 *  Linus Platform Driver
 *
 *****************************************************************************
 */

static int AudDrv_probe(struct platform_device *dev)
{
   int ret = 0;
   int value = 0;
   xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_probe \n");

   //power on
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_probe +AudDrv_Clk_On\n");
   Flag_AudDrv_ClkOn_1st = true;
   AudDrv_Clk_On();
   Flag_AudDrv_ClkOn_1st = false;
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_probe -AudDrv_Clk_On\n");

   //Speaker_Init();
   Ana_Init_Backup_Memory();

   Afe_Set_Reg(AFE_IRQ_CON   ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_IR_STATUS ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_IR_CLR    ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_IRQ_CNT1  ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_IRQ_CNT2  ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_IRQ_MON   ,0x0,0xffffffff);

   // Init Analog Register
   AudDrv_ANA_Clk_On();
   Ana_Set_Reg(AUDIO_CON0,0x0000,0xffff);
   Ana_Set_Reg(AUDIO_CON1,0x000C,0xffff);
   Ana_Set_Reg(AUDIO_CON2,0x000C,0xffff);
   Ana_Set_Reg(AUDIO_CON4,0x1818,0xffff);
   Ana_Set_Reg(AUDIO_CON5,0x0440,0xffff);
   Ana_Set_Reg(AUDIO_CON6,0x001B,0xffff);
   Ana_Set_Reg(AUDIO_CON7,0x0400,0xffff);  //DRTZ 0; disable HP amp startup mode
   Ana_Set_Reg(AUDIO_CON8,0x7000,0xffff);  //Disable short circuit protection
   Ana_Set_Reg(AUDIO_CON9,0x0003,0xffff);
   Ana_Set_Reg(AUDIO_CON10,0x01A1,0xffff); // Zero-Padding off
   Ana_Set_Reg(AUDIO_CON23,0x0000,0x8800); // power down Analog ADC
   //Analog Register settings. (Power down)
   Ana_Set_Reg(AUDIO_CON3,0x0002,0xffff);   //power down HS buffer/ DAC/ LevelShift buffer/power down audio bias
   Ana_Set_Reg(AUDIO_CON9,0x0000,0x0002);   //Reset audio decoder
   Ana_Set_Reg(AUDIO_CON14,0x0000,0x00f0);  //power-down R/L channel's low noise bias
   Ana_Set_Reg(AUDIO_NCP1,0x0800,0x0800);   //enable power-down NCP
   Ana_Set_Reg(AUDIO_LDO2,0x0000,0xffff);   //disable HCLDO/LCLDO ENC/LCLDO
   //Ana_Set_Reg(AUDIO_GLB0,0x0001,0xffff); //Not to disable GLB0.  //enable power down control for global bias circuit
   Ana_Set_Reg(AUDIO_REG1,0x0000,0x0001);   //disable NVREG
   AudDrv_ANA_Clk_Off();

   //Initialize AFE Register
   Afe_Set_Reg(AUDIO_TOP_CON0,0x60,0x0060);

   Afe_Set_Reg(AFE_DL_SDM_CON0,0x08800000,0xffffffff);
   Afe_Set_Reg(AFE_SDM_GAIN_STAGE,0x0000001e,0xffffffff);

   // Reset AudioSys HW registers
   Afe_Set_Reg(AFE_DAC_CON0 ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_DAC_CON1 ,0x0,0xffffffff);

   Afe_Set_Reg(AFE_CONN0,0x0,0xffffffff);
   Afe_Set_Reg(AFE_CONN1,0x0,0xffffffff);
   Afe_Set_Reg(AFE_CONN2,0x0,0xffffffff);
   Afe_Set_Reg(AFE_CONN3,0x0,0xffffffff);
   Afe_Set_Reg(AFE_CONN4,0x80000000,0xffffffff);

   Afe_Set_Reg(AFE_DL1_BASE ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_DL1_CUR  ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_DL1_END  ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_DL2_BASE ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_DL2_CUR  ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_DL2_END  ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_I2S_BASE ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_I2S_CUR  ,0x0,0xffffffff);
   Afe_Set_Reg(AFE_I2S_END  ,0x0,0xffffffff);

   //Afe_Set_Reg(AFE_DL_SRC1_1,0x1,0xffffffff);  // for MT6573 E1, must enable DL1_SRC_ON

   Afe_Set_Reg(AFE_DL_SRC2_1,0x0,0xffffffff);
   Afe_Set_Reg(AFE_DL_SRC2_2,0xffff,0xffffffff);

   value = 0;
   value=(value & ( ~(0x3<<24) )) | (0x3 << 24);  //up-sampleing x8   bit24 ~ bit25
   Afe_Set_Reg(AFE_DL_SRC2_1 ,value,0x03000000);

   /* Set default mute speed */
   value = 0;
   value=(value & ( ~(0x3<<9) )) | (0x3 << 9);   // bit9 ~ bit10
   Afe_Set_Reg(AFE_DL_SRC2_1 ,value,0x0600);
   /* Set PRD value */
   Afe_Set_Reg(0x0260,0x80000fe0,0xffffffff);
   Afe_Set_Reg(0x0264,0x80000fe0,0xffffffff);

   Speaker_Init();

   // register IRQ line
   ret = request_irq(MT6577_AFE_MCU_IRQ_LINE, AudDrv_IRQ_handler, IRQF_TRIGGER_LOW/*IRQF_TRIGGER_FALLING*/, "Afe_ISR_Handle", dev);
   if(ret < 0 ){
      xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_probe request_irq Fail \n");
   }
   //LouderSPKSound(200);//Test for audio hw when power up.

	xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_probe +AudDrv_Clk_Off\n");
   AudDrv_Clk_Off();
	xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_probe -AudDrv_Clk_Off\n");
//   AudSRAMVirtBufAddr = (kal_uint8*)ioremap_nocache( 0xD4000000 , (unsigned long)(AFE_DL1_SYSRAM_END-AFE_DL1_SYSRAM_BASE) );  // use ioremap to map sram
#if defined(I2SIN_USE_FLEXL2)
   ////////////
   // Use SysRam (FlexL2)
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Init_I2S_InputStream FlexL2 \n");
   I2SInPhyBufAddr = 0x90010000;
   I2SInVirtBufAddr = 0xf9001000;
#else
   ////////////
   // Use DMA alloc coherent
   I2SInPhyBufAddr = 0;
   I2SInVirtBufAddr = NULL;

   I2SInVirtBufAddr = dma_alloc_coherent(0, 9216, &I2SInPhyBufAddr, GFP_KERNEL);
   if((0 == I2SInPhyBufAddr)||(NULL == I2SInVirtBufAddr)){
      xlog_printk(ANDROID_LOG_INFO, "Sound","I2SInPhyBufAddr dma_alloc_coherent fail \n");
   }
#endif
    //allocate memory for AWB , high sample rate record
    memset((void*)&AWB_input_Control_context,0,sizeof(AWB_INPUT_CONTROL_T));
    if(AWB_input_Control->rBlock.pucPhysBufAddr == 0){
        AudDrv_allcate_AWB_buffer ();
    }
    PRINTK_AUDDRV("init AWB_timer\n");
    init_timer(&AWB_timer);

   xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv_probe \n");
   return 0;
}


static int AudDrv_suspend(struct platform_device *dev, pm_message_t state)  // only one suspend mode
{
   kal_uint32 Reg_AUDIO_LDO2, Reg_AUDIO_NCP1, Reg_AUDIO_REG1, Reg_AUDIO_GLB0;
   kal_uint32 Reg_AUDIO_CON3, Reg_AUDIO_CON9;
   kal_uint32 Reg_AUDIO_CON21, Reg_AUDIO_CON22, Reg_AUDIO_CON23;
	#ifdef AUD_SRAMPRINTK
	aee_sram_printk("AudDrv_suspend Afe_Pwr_on:%d, Aud_AFE_Clk_cntr:%d, Aud_ANA_Clk_cntr:%d\n",Afe_Pwr_on,Aud_AFE_Clk_cntr,Aud_ANA_Clk_cntr);
	#else
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_suspend Afe_Pwr_on:%d, Aud_AFE_Clk_cntr:%d, Aud_ANA_Clk_cntr:%d\n",Afe_Pwr_on,Aud_AFE_Clk_cntr,Aud_ANA_Clk_cntr);
	#endif
   // if modem side use Audio HW, don't turn off audio AFE.
   spin_lock_bh(&auddrv_SphCtlState_lock);
   if( (SPH_Ctrl_State.bSpeechFlag==true) ||
       (SPH_Ctrl_State.bVT==true) ||
       (SPH_Ctrl_State.bRecordFlag==true) ||
       (SPH_Ctrl_State.bBgsFlag==true) ||
       (SPH_Ctrl_State.bTtyFlag==true)
     )
   {
   	#ifdef AUD_SRAMPRINTK
		aee_sram_printk("AudDrv_suspend bBgsFlag:%d,bRecordFlag:%d,bSpeechFlag:%d,bTtyFlag:%d,bVT:%d,bAudio:%d \n",
		SPH_Ctrl_State.bBgsFlag,
      SPH_Ctrl_State.bRecordFlag,
      SPH_Ctrl_State.bSpeechFlag,
      SPH_Ctrl_State.bTtyFlag,
      SPH_Ctrl_State.bVT,
      SPH_Ctrl_State.bAudioPlay);
		#else
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_suspend bBgsFlag:%d,bRecordFlag:%d,bSpeechFlag:%d,bTtyFlag:%d,bVT:%d,bAudio:%d \n",
      SPH_Ctrl_State.bBgsFlag,
      SPH_Ctrl_State.bRecordFlag,
      SPH_Ctrl_State.bSpeechFlag,
      SPH_Ctrl_State.bTtyFlag,
      SPH_Ctrl_State.bVT,
      SPH_Ctrl_State.bAudioPlay);
		#endif
		spin_unlock_bh(&auddrv_SphCtlState_lock);

      return 0;
   }

   if(IsSuspen == false)
   {
      // when suspend , if is not in speech mode , close amp.
      if(/*AMP_Flag ==true &&*/ (SPH_Ctrl_State.bSpeechFlag== false) && (SPH_Ctrl_State.bVT== false) )
      {
   		spin_unlock_bh(&auddrv_SphCtlState_lock);
         //Sound_Speaker_Turnoff(Channel_Stereo);//turn off speaker
         mutex_lock(&gamp_mutex);
         AudioAMPDevice_Suspend();
		   mutex_unlock(&gamp_mutex);
         mdelay(1);
      }
		else
		{
			spin_unlock_bh(&auddrv_SphCtlState_lock);
      }
      AudDrv_Clk_On();
	  // Analog register setting
      AudDrv_ANA_Clk_On();
	   #ifdef AUD_SRAMPRINTK
		aee_sram_printk("AudDrv_suspend open_clock done\n");
	   #else
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_suspend open_clock done\n");
	   #endif
/*
	  //SetAnaReg(AUDIO_LDO2,0x0000,0x0013);//Not to disable LD02 for record mute issue.
	  Ana_Set_Reg(AUDIO_NCP1,0x0800,0x0E00);
	  Ana_Set_Reg(AUDIO_REG1,0x0000,0x0001);
	  //Ana_Set_Reg(AUDIO_GLB0,0x0001,0x0001);//Not to disable GLB0.
*/
      slp_debug_for_audio();
      Ana_Set_Reg(AUDIO_CON9,0x0000,0x0007);
      // Analog register reading
		//Temp to disable[Charlie]
      Reg_AUDIO_LDO2 = Ana_Get_Reg(AUDIO_LDO2);
      Reg_AUDIO_NCP1 = Ana_Get_Reg(AUDIO_NCP1);
      Reg_AUDIO_REG1 = Ana_Get_Reg(AUDIO_REG1);
      Reg_AUDIO_GLB0 = Ana_Get_Reg(AUDIO_GLB0);

      Reg_AUDIO_CON3 = Ana_Get_Reg(AUDIO_CON3);
      Reg_AUDIO_CON9 = Ana_Get_Reg(AUDIO_CON9);
      Reg_AUDIO_CON21 = Ana_Get_Reg(AUDIO_CON21);
      Reg_AUDIO_CON22 = Ana_Get_Reg(AUDIO_CON22);
      Reg_AUDIO_CON23 = Ana_Get_Reg(AUDIO_CON23);

		#ifdef AUD_SRAMPRINTK
		aee_sram_printk("AudDrv_suspend PMUA AuDec Reg_AUDIO_LDO2=0x%x, Reg_AUDIO_NCP1=0x%x, Reg_AUDIO_REG1=0x%x, Reg_AUDIO_GLB0=0x%x\n", Reg_AUDIO_LDO2, Reg_AUDIO_NCP1, Reg_AUDIO_REG1, Reg_AUDIO_GLB0);
      aee_sram_printk("AudDrv_suspend Ana AuDec Reg_AUDIO_CON3=0x%x, Reg_AUDIO_CON9=0x%x\n", Reg_AUDIO_CON3, Reg_AUDIO_CON9);
      aee_sram_printk("AudDrv_suspend AuEnc Reg_AUDIO_CON21=0x%x, Reg_AUDIO_CON22=0x%x, Reg_AUDIO_CON23=0x%x\n", Reg_AUDIO_CON21, Reg_AUDIO_CON22, Reg_AUDIO_CON23);
      aee_sram_printk("AudDrv_suspend Afe_Pwr_on:%d, Aud_AFE_Clk_cntr:%d, Aud_ADC_Clk_cntr:%d, Aud_I2S_Clk_cntr:%d \n",Afe_Pwr_on,Aud_AFE_Clk_cntr, Aud_ADC_Clk_cntr, Aud_I2S_Clk_cntr);
	  	#else
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_suspend PMUA AuDec Reg_AUDIO_LDO2=0x%x, Reg_AUDIO_NCP1=0x%x, Reg_AUDIO_REG1=0x%x, Reg_AUDIO_GLB0=0x%x\n", Reg_AUDIO_LDO2, Reg_AUDIO_NCP1, Reg_AUDIO_REG1, Reg_AUDIO_GLB0);
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_suspend Ana AuDec Reg_AUDIO_CON3=0x%x, Reg_AUDIO_CON9=0x%x\n", Reg_AUDIO_CON3, Reg_AUDIO_CON9);
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_suspend AuEnc Reg_AUDIO_CON21=0x%x, Reg_AUDIO_CON22=0x%x, Reg_AUDIO_CON23=0x%x\n", Reg_AUDIO_CON21, Reg_AUDIO_CON22, Reg_AUDIO_CON23);
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_suspend Afe_Pwr_on:%d, Aud_AFE_Clk_cntr:%d, Aud_ADC_Clk_cntr:%d, Aud_I2S_Clk_cntr:%d \n",Afe_Pwr_on,Aud_AFE_Clk_cntr, Aud_ADC_Clk_cntr, Aud_I2S_Clk_cntr);
		#endif

		#if !defined(DL1_USE_SYSRAM)
      AudDrv_Reset_DL1_Stream_Buf();
		#endif

		#if defined(DL1_USE_SYSRAM)
      //free sysram to disable sysram power
		//SYSRAM_AUDIO_FREE();
		#endif
      AudDrv_Store_reg();
		//Force to power down
      Ana_Set_Reg(AUDIO_GLB0,0x1,0x1);//power down global bias
      Ana_Set_Reg(AUDIO_LDO0,0x0,0xffff);
      Ana_Set_Reg(AUDIO_CON3,0x0,0x01f0);
      Ana_Set_Reg(AUDIO_CON14,0x0,0xffff);
      Ana_Set_Reg(AUDIO_CON21,0x0,0xffff);
	  	Ana_Set_Reg(AUDIO_CON22,0x0,0xffff);
	  	Ana_Set_Reg(AUDIO_CON23,0x0,0xffff);
	  	Ana_Set_Reg(AUDIO_CON31,0x0,0xffff);
      AudDrv_ANA_Clk_Off();
	  	//~
	  	AudDrv_Clk_Off();
		#ifdef USE_TASKLET
      tasklet_disable(&magic_tasklet_handle);  // disable tasklet
		#endif
		//      Afe_Disable_Memory_Power();	//disable memory buffer
      AudDrv_Suspend_Clk_Off();  // turn off asm afe clock
#ifdef AUD_SRAMPRINTK
      aee_sram_printk("AudDrv_Suspend_Clk_Off done \n");
#endif

      hwPowerDown(MT65XX_POWER_LDO_VA2, "audio");
      IsSuspen = true;// set suspend mode to true

   	spin_lock_bh(&auddrv_SphCtlState_lock);

   }

	spin_unlock_bh(&auddrv_SphCtlState_lock);

   return 0;
}

static int AudDrv_resume(struct platform_device *dev) // wake up
{
	#ifdef AUD_SRAMPRINTK
   aee_sram_printk("+AudDrv_resume \n");
	#else
   xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_resume \n");
	#endif
   if(IsSuspen == true)
   {
   	#ifdef AUD_SRAMPRINTK
      aee_sram_printk("AudDrv_resume Afe_Pwr_on:%d, Aud_AFE_Clk_cntr:%d, Aud_ANA_Clk_cntr:%d\n",Afe_Pwr_on,Aud_AFE_Clk_cntr,Aud_ANA_Clk_cntr);
		#else
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_resume Afe_Pwr_on:%d, Aud_AFE_Clk_cntr:%d, Aud_ANA_Clk_cntr:%d\n",Afe_Pwr_on,Aud_AFE_Clk_cntr,Aud_ANA_Clk_cntr);
		#endif
		hwPowerOn(MT65XX_POWER_LDO_VA2, VOL_2500, "audio");

      AudDrv_Suspend_Clk_On();
		#ifdef AUD_SRAMPRINTK
		aee_sram_printk(" AudDrv_resume AudDrv_Suspend_Clk_On done \n");
		#else
      xlog_printk(ANDROID_LOG_INFO, "Sound"," AudDrv_resume AudDrv_Suspend_Clk_On done \n");
		#endif
		#ifdef USE_TASKLET
      tasklet_enable(&magic_tasklet_handle); // enable tasklet
		#endif

      IsSuspen = false;  // set suspend mode to false
      AudDrv_Clk_On();
      AudDrv_ANA_Clk_On();
      #ifdef AUD_SRAMPRINTK
      aee_sram_printk(" AudDrv_resume open clk done \n");
      #else
      xlog_printk(ANDROID_LOG_INFO, "Sound"," AudDrv_resume open clk done \n");
      #endif
      slp_debug_for_audio();
      AudDrv_Recover_reg();
		#ifdef AUD_SRAMPRINTK
		aee_sram_printk(" AudDrv_resume AudDrv_Recover_reg done \n");
		#else
      xlog_printk(ANDROID_LOG_INFO, "Sound"," AudDrv_resume AudDrv_Recover_reg done \n");
		#endif
		#if defined(DL1_USE_SYSRAM)
      //allocate sysram to enable sysram power
      //SYSRAM_AUDIO_ALLOC_TIMEOUT(AFE_dl_Control->rBlock.u4BufferSize,32,AUD_SYSRAM_ALLOC_TIMEOUT);
		#endif

		#if !defined(DL1_USE_SYSRAM)
      AudDrv_Reset_DL1_Stream_Buf();
		#endif
      /*
      // Analog register setting
      Ana_Set_Reg(AUDIO_CON9,0x0002,0x0006);
      udelay(1);
      Ana_Set_Reg(AUDIO_CON9,0x0001,0x0001);
      */
      #ifdef AUD_SRAMPRINTK
      aee_sram_printk("Clk_Off start \n");
      #else
      xlog_printk(ANDROID_LOG_INFO, "Sound","Clk_Off start \n");
      #endif
      AudDrv_ANA_Clk_Off();
      AudDrv_Clk_Off();

      // when resume , if amp is closed , reopen it.
      #ifdef AUD_SRAMPRINTK
      aee_sram_printk("AudDrv_Clk_Off() done \n");
      #else
      xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_Clk_Off() done \n");
      #endif
      spin_lock_bh(&auddrv_SphCtlState_lock);

      if(/*AMP_Flag ==true &&*/ (SPH_Ctrl_State.bSpeechFlag== false) && (SPH_Ctrl_State.bVT== false))
      {
   		spin_unlock_bh(&auddrv_SphCtlState_lock);
			#ifdef AUD_SRAMPRINTK
			aee_sram_printk("AudDrv_resume, AudioAMPDevice_Resume \n");
			#else
         xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_resume, AudioAMPDevice_Resume \n");
			#endif
		 	mutex_lock(&gamp_mutex);
		 	AudioAMPDevice_Resume();
		 	mutex_unlock(&gamp_mutex);
      }
		else
		{
			spin_unlock_bh(&auddrv_SphCtlState_lock);
      }
   }
	#ifdef AUD_SRAMPRINTK
   aee_sram_printk("-AudDrv_resume done \n");
	#else
   xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv_resume done \n");
	#endif
   return 0;
}

static void AudDrv_shutdown(struct platform_device *dev)
{
	xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_shutdown \n");

	Afe_Set_Reg(AFE_IRQ_CON,0x0,0x7); 	// disable IRQ_CON
	Afe_Set_Reg(AFE_DAC_CON0,0x0,0x7f); // disable DAC_CON0
	Afe_Set_Reg(AFE_DL_SRC2_1,0x0,0x1);	// disable DL_SRC2_1
	Afe_Set_Reg(AFE_UL_SRC_0,0x0,0x1);	// disable UL_SRC_0
	Afe_Set_Reg(AFE_I2S_IN_CON,0x0,0x1);	// disable I2S_IN_CON
	Afe_Set_Reg(AFE_I2S_OUT_CON,0x0,0x1);	// disable I2S_OUT_CON

	AudDrv_Clk_Off();  // disable afe clock

	xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv_shutdown \n");
}


static int AudDrv_remove(struct platform_device *dev)
{
	xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_remove \n");
	#if (!defined (I2SIN_USE_FLEXL2))
    ////////////
    // Use DMA free coherent
   dma_free_coherent(0, 9216, I2SInVirtBufAddr, I2SInPhyBufAddr);
	dma_free_coherent(0, AWB_input_Control->u4BufferSize ,AWB_input_Control->rBlock.pucVirtBufAddr, AWB_input_Control->rBlock.pucPhysBufAddr);
	#endif

	#ifdef USE_TASKLET
	tasklet_kill(&magic_tasklet_handle);
	#endif
//	spin_lock_bh(&auddrv_lock);
	AudDrv_Clk_Off();  // disable afe clock
//	spin_unlock_bh(&auddrv_lock);

	xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv_remove \n");
	return 0;
}

/*****************************************************************************
 * FUNCTION
 *  AudDrv_Read_Procmem
 *
 * DESCRIPTION
 *  dump AFE/Analog register
 *  cat /proc/Audio
 *
 * PARAMETERS
 *
 *
 * RETURNS
 *  length
 *
 *****************************************************************************
 */

static int AudDrv_Read_Procmem(char *buf,char **start, off_t offset, int count , int *eof, void *data)
{
    int len = 0;
	AudDrv_Core_Clk_On();
    AudDrv_ANA_Clk_On();
    // AudioSys Register Setting
    xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_Read_Procmem\n");
    len += sprintf(buf+ len, "AUDIO_TOP_CON0  = 0x%x\n",Afe_Get_Reg(AUDIO_TOP_CON0));
    len += sprintf(buf+ len, "AFE_DAC_CON0    = 0x%x\n",Afe_Get_Reg(AFE_DAC_CON0));
    len += sprintf(buf+ len, "AFE_DAC_CON1    = 0x%x\n",Afe_Get_Reg(AFE_DAC_CON1));
    len += sprintf(buf+ len, "AFE_I2S_IN_CON  = 0x%x\n",Afe_Get_Reg(AFE_I2S_IN_CON));
    len += sprintf(buf+ len, "AFE_FOC_CON     = 0x%x\n",Afe_Get_Reg(AFE_FOC_CON));
    len += sprintf(buf+ len, "AFE_DAIBT_CON   = 0x%x\n",Afe_Get_Reg(AFE_DAIBT_CON));
    len += sprintf(buf+ len, "AFE_CONN0       = 0x%x\n",Afe_Get_Reg(AFE_CONN0));
    len += sprintf(buf+ len, "AFE_CONN1       = 0x%x\n",Afe_Get_Reg(AFE_CONN1));
    len += sprintf(buf+ len, "AFE_CONN2       = 0x%x\n",Afe_Get_Reg(AFE_CONN2));
    len += sprintf(buf+ len, "AFE_CONN3       = 0x%x\n",Afe_Get_Reg(AFE_CONN3));
    len += sprintf(buf+ len, "AFE_CONN4       = 0x%x\n",Afe_Get_Reg(AFE_CONN4));
    len += sprintf(buf+ len, "AFE_I2S_OUT_CON = 0x%x\n",Afe_Get_Reg(AFE_I2S_OUT_CON));
    len += sprintf(buf+ len, "AFE_DL1_BASE    = 0x%x\n",Afe_Get_Reg(AFE_DL1_BASE));
    len += sprintf(buf+ len, "AFE_DL1_CUR     = 0x%x\n",Afe_Get_Reg(AFE_DL1_CUR));
    len += sprintf(buf+ len, "AFE_DL1_END     = 0x%x\n",Afe_Get_Reg(AFE_DL1_END));
    len += sprintf(buf+ len, "AFE_DL2_BASE    = 0x%x\n",Afe_Get_Reg(AFE_DL2_BASE));
    len += sprintf(buf+ len, "AFE_DL2_CUR     = 0x%x\n",Afe_Get_Reg(AFE_DL2_CUR));
    len += sprintf(buf+ len, "AFE_DL2_END     = 0x%x\n",Afe_Get_Reg(AFE_DL2_END));
    len += sprintf(buf+ len, "AFE_I2S_BASE    = 0x%x\n",Afe_Get_Reg(AFE_I2S_BASE));
    len += sprintf(buf+ len, "AFE_I2S_CUR     = 0x%x\n",Afe_Get_Reg(AFE_I2S_CUR));
    len += sprintf(buf+ len, "AFE_I2S_END     = 0x%x\n",Afe_Get_Reg(AFE_I2S_END));
    len += sprintf(buf+ len, "AFE_AWB_BASE    = 0x%x\n",Afe_Get_Reg(AFE_AWB_BASE));
    len += sprintf(buf+ len, "AFE_AWB_CUR     = 0x%x\n",Afe_Get_Reg(AFE_AWB_CUR));
    len += sprintf(buf+ len, "AFE_AWB_END     = 0x%x\n",Afe_Get_Reg(AFE_AWB_END));

    len += sprintf(buf+ len, "AFE_VUL_CUR     = 0x%x\n",Afe_Get_Reg(AFE_VUL_CUR));
    len += sprintf(buf+ len, "AFE_DAI_CUR     = 0x%x\n",Afe_Get_Reg(AFE_DAI_CUR));

    len += sprintf(buf+ len, "AFE_IRQ_CON     = 0x%x\n",Afe_Get_Reg(AFE_IRQ_CON));
    len += sprintf(buf+ len, "AFE_IR_STATUS   = 0x%x\n",Afe_Get_Reg(AFE_IR_STATUS));
    len += sprintf(buf+ len, "AFE_IR_CLR      = 0x%x\n",Afe_Get_Reg(AFE_IR_CLR));
    len += sprintf(buf+ len, "AFE_IRQ_CNT1    = 0x%x\n",Afe_Get_Reg(AFE_IRQ_CNT1));
    len += sprintf(buf+ len, "AFE_IRQ_CNT2    = 0x%x\n",Afe_Get_Reg(AFE_IRQ_CNT2));
    len += sprintf(buf+ len, "AFE_IRQ_MON     = 0x%x\n",Afe_Get_Reg(AFE_IRQ_MON));

    len += sprintf(buf+ len, "AFE_MODEM_IRQ_CON   = 0x%x\n",Afe_Get_Reg(AFE_MODEM_IRQ_CON));
    len += sprintf(buf+ len, "AFE_MODEM_IR_STATUS = 0x%x\n",Afe_Get_Reg(AFE_MODEM_IR_STATUS));
    len += sprintf(buf+ len, "AFE_MODEM_IR_CLR    = 0x%x\n",Afe_Get_Reg(AFE_MODEM_IR_CLR));
    len += sprintf(buf+ len, "AFE_MODEM_IRQ_CNT1  = 0x%x\n",Afe_Get_Reg(AFE_MODEM_IRQ_CNT1));
    len += sprintf(buf+ len, "AFE_MODEM_IRQ_CNT2  = 0x%x\n",Afe_Get_Reg(AFE_MODEM_IRQ_CNT2));

    len += sprintf(buf+ len, "AFE_DL_SDM_CON0 = 0x%x\n",Afe_Get_Reg(AFE_DL_SDM_CON0));
    len += sprintf(buf+ len, "AFE_DL_SRC2_1   = 0x%x\n",Afe_Get_Reg(AFE_DL_SRC2_1));
    len += sprintf(buf+ len, "AFE_DL_SRC2_2   = 0x%x\n",Afe_Get_Reg(AFE_DL_SRC2_2));
    len += sprintf(buf+ len, "AFE_DL_SRC1_1   = 0x%x\n",Afe_Get_Reg(AFE_DL_SRC1_1));

// for Audio HQA
    len += sprintf(buf+ len, "AFE_UL_AGC0 = 0x%x\n",Afe_Get_Reg(AFE_UL_AGC0));
    len += sprintf(buf+ len, "AFE_UL_AGC5     = 0x%x\n",Afe_Get_Reg(AFE_UL_AGC5));
    len += sprintf(buf+ len, "AFE_UL_AGC13    = 0x%x\n",Afe_Get_Reg(AFE_UL_AGC13));

    len += sprintf(buf+ len, "AFE_SIDETONE_CON0 = 0x%x\n",Afe_Get_Reg(AFE_SIDETONE_CON0));
    len += sprintf(buf+ len, "AFE_SIDETONE_CON1 = 0x%x\n",Afe_Get_Reg(AFE_SIDETONE_CON1));

    len += sprintf(buf+ len, "AFE_UL_SRC_0    = 0x%x\n",Afe_Get_Reg(AFE_UL_SRC_0));
    len += sprintf(buf+ len, "AFE_UL_SRC_1    = 0x%x\n",Afe_Get_Reg(AFE_UL_SRC_1));
    len += sprintf(buf+ len, "AFE_SDM_GAIN_STAGE  = 0x%x\n",Afe_Get_Reg(AFE_SDM_GAIN_STAGE));


    // Audio Variable Setting
    //len += sprintf(buf+ len, "AMP_Flag         = %d\n",AMP_Flag);
    len += sprintf(buf+ len, "Afe_Pwr_on   = %d\n",Afe_Pwr_on);
    len += sprintf(buf+ len, "I2S_Pwr_on       = %d\n",I2S_Pwr_on);
    len += sprintf(buf+ len, "Aud_AFE_Clk_cntr = %d\n",Aud_AFE_Clk_cntr);
    len += sprintf(buf+ len, "Aud_ADC_Clk_cntr = %d\n",Aud_ADC_Clk_cntr);
    len += sprintf(buf+ len, "Aud_I2S_Clk_cntr = %d\n",Aud_I2S_Clk_cntr);
    len += sprintf(buf+ len, "Aud_ANA_Clk_cntr = %d\n",Aud_ANA_Clk_cntr);
    // Analog ABB Register Setting
    len += sprintf(buf+ len, "WR_PATH2   = 0x%x[0x%x]\n",Ana_Get_Reg(WR_PATH2),Ana_Get_Backup_Memory(WR_PATH2));
    len += sprintf(buf+ len, "AUDIO_CON0 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON0),Ana_Get_Backup_Memory(AUDIO_CON0));
    len += sprintf(buf+ len, "AUDIO_CON1 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON1),Ana_Get_Backup_Memory(AUDIO_CON1));
    len += sprintf(buf+ len, "AUDIO_CON2 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON2),Ana_Get_Backup_Memory(AUDIO_CON2));
    len += sprintf(buf+ len, "AUDIO_CON3 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON3),Ana_Get_Backup_Memory(AUDIO_CON3));
    len += sprintf(buf+ len, "AUDIO_CON4 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON4),Ana_Get_Backup_Memory(AUDIO_CON4));
    len += sprintf(buf+ len, "AUDIO_CON5 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON5),Ana_Get_Backup_Memory(AUDIO_CON5));
    len += sprintf(buf+ len, "AUDIO_CON6 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON6),Ana_Get_Backup_Memory(AUDIO_CON6));
    len += sprintf(buf+ len, "AUDIO_CON7 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON7),Ana_Get_Backup_Memory(AUDIO_CON7));
    len += sprintf(buf+ len, "AUDIO_CON8 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON8),Ana_Get_Backup_Memory(AUDIO_CON8));
    len += sprintf(buf+ len, "AUDIO_CON9 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON9),Ana_Get_Backup_Memory(AUDIO_CON9));
    len += sprintf(buf+ len, "AUDIO_CON10 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON10),Ana_Get_Backup_Memory(AUDIO_CON10));
    len += sprintf(buf+ len, "AUDIO_CON11 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON11),Ana_Get_Backup_Memory(AUDIO_CON11));
    len += sprintf(buf+ len, "AUDIO_CON12 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON12),Ana_Get_Backup_Memory(AUDIO_CON12));
    len += sprintf(buf+ len, "AUDIO_CON13 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON13),Ana_Get_Backup_Memory(AUDIO_CON13));
    len += sprintf(buf+ len, "AUDIO_CON14 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON14),Ana_Get_Backup_Memory(AUDIO_CON14));
    len += sprintf(buf+ len, "AUDIO_CON15 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON15),Ana_Get_Backup_Memory(AUDIO_CON15));
    len += sprintf(buf+ len, "AUDIO_CON16 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON16),Ana_Get_Backup_Memory(AUDIO_CON16));
    len += sprintf(buf+ len, "AUDIO_CON17 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON17),Ana_Get_Backup_Memory(AUDIO_CON17));

    len += sprintf(buf+ len, "AUDIO_CON20 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON20),Ana_Get_Backup_Memory(AUDIO_CON20));
    len += sprintf(buf+ len, "AUDIO_CON21 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON21),Ana_Get_Backup_Memory(AUDIO_CON21));
    len += sprintf(buf+ len, "AUDIO_CON22 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON22),Ana_Get_Backup_Memory(AUDIO_CON22));
    len += sprintf(buf+ len, "AUDIO_CON23 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON23),Ana_Get_Backup_Memory(AUDIO_CON23));
    len += sprintf(buf+ len, "AUDIO_CON24 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON24),Ana_Get_Backup_Memory(AUDIO_CON24));
    len += sprintf(buf+ len, "AUDIO_CON25 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON25),Ana_Get_Backup_Memory(AUDIO_CON25));
    len += sprintf(buf+ len, "AUDIO_CON26 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON26),Ana_Get_Backup_Memory(AUDIO_CON26));
    len += sprintf(buf+ len, "AUDIO_CON27 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON27),Ana_Get_Backup_Memory(AUDIO_CON27));
    len += sprintf(buf+ len, "AUDIO_CON28 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON28),Ana_Get_Backup_Memory(AUDIO_CON28));
    len += sprintf(buf+ len, "AUDIO_CON29 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON29),Ana_Get_Backup_Memory(AUDIO_CON29));
    len += sprintf(buf+ len, "AUDIO_CON30 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON30),Ana_Get_Backup_Memory(AUDIO_CON30));
    len += sprintf(buf+ len, "AUDIO_CON31 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON31),Ana_Get_Backup_Memory(AUDIO_CON31));
    len += sprintf(buf+ len, "AUDIO_CON32 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON32),Ana_Get_Backup_Memory(AUDIO_CON32));
    len += sprintf(buf+ len, "AUDIO_CON33 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON33),Ana_Get_Backup_Memory(AUDIO_CON33));

    len += sprintf(buf+ len, "AUDIO_CON34 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_CON34),Ana_Get_Backup_Memory(AUDIO_CON34));

    // Analog PMU Register Setting

    len += sprintf(buf+ len, "AUDIO_NCP0 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_NCP0),Ana_Get_Backup_Memory(AUDIO_NCP0));
    len += sprintf(buf+ len, "AUDIO_NCP1 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_NCP1),Ana_Get_Backup_Memory(AUDIO_NCP1));
    len += sprintf(buf+ len, "AUDIO_LDO0 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_LDO0),Ana_Get_Backup_Memory(AUDIO_LDO0));
    len += sprintf(buf+ len, "AUDIO_LDO1 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_LDO1),Ana_Get_Backup_Memory(AUDIO_LDO1));
    len += sprintf(buf+ len, "AUDIO_LDO2 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_LDO2),Ana_Get_Backup_Memory(AUDIO_LDO2));
    len += sprintf(buf+ len, "AUDIO_LDO3 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_LDO3),Ana_Get_Backup_Memory(AUDIO_LDO3));
    len += sprintf(buf+ len, "AUDIO_GLB0 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_GLB0),Ana_Get_Backup_Memory(AUDIO_GLB0));
    len += sprintf(buf+ len, "AUDIO_GLB1 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_GLB1),Ana_Get_Backup_Memory(AUDIO_GLB1));
    len += sprintf(buf+ len, "AUDIO_GLB2 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_GLB2),Ana_Get_Backup_Memory(AUDIO_GLB2));
    len += sprintf(buf+ len, "AUDIO_REG0 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_REG0),Ana_Get_Backup_Memory(AUDIO_REG0));
    len += sprintf(buf+ len, "AUDIO_REG1 = 0x%x[0x%x]\n",Ana_Get_Reg(AUDIO_REG1),Ana_Get_Backup_Memory(AUDIO_REG1));

    len += sprintf(buf+ len, "WR_PATH0 = 0x%x[0x%x]\n",Ana_Get_Reg(WR_PATH0),Ana_Get_Backup_Memory(WR_PATH0));

    // Speech/Record/BGS/TTY/VT Setting
    len += sprintf(buf+ len, "Speech_on  = %d\n",SPH_Ctrl_State.bSpeechFlag);
    len += sprintf(buf+ len, "BGS_on     = %d\n",SPH_Ctrl_State.bBgsFlag);
    len += sprintf(buf+ len, "Record_on  = %d\n",SPH_Ctrl_State.bRecordFlag);
    len += sprintf(buf+ len, "TTY_on     = %d\n",SPH_Ctrl_State.bTtyFlag);
    len += sprintf(buf+ len, "VT_on      = %d\n",SPH_Ctrl_State.bVT);
    len += sprintf(buf+ len, "Audio_on   = %d\n",SPH_Ctrl_State.bAudioPlay);

	// Audio External AMP
	if(ExternalAmp())
	{
		mutex_lock(&gamp_mutex);
		len += sprintf(buf+ len, "subsystem = 0x%x\n",Audio_eamp_command(EAMP_GETREGISTER_VALUE,0,1));
		len += sprintf(buf+ len, "inputcontrol = 0x%x\n",Audio_eamp_command(EAMP_GETREGISTER_VALUE,1,1));
		len += sprintf(buf+ len, "limitercontrol = 0x%x\n",Audio_eamp_command(EAMP_GETREGISTER_VALUE,2,1));
		len += sprintf(buf+ len, "speakercontrol = 0x%x\n",Audio_eamp_command(EAMP_GETREGISTER_VALUE,3,1));
		len += sprintf(buf+ len, "headphonecontrol = 0x%x\n",Audio_eamp_command(EAMP_GETREGISTER_VALUE,4,1));
		len += sprintf(buf+ len, "speakervol = 0x%x\n",Audio_eamp_command(EAMP_GETREGISTER_VALUE,5,1));
		len += sprintf(buf+ len, "headphoneLVol = 0x%x\n",Audio_eamp_command(EAMP_GETREGISTER_VALUE,6,1));
		len += sprintf(buf+ len, "HeadphoneRVol= 0x%x\n",Audio_eamp_command(EAMP_GETREGISTER_VALUE,7,1));
		mutex_unlock(&gamp_mutex);
	}
	AudDrv_ANA_Clk_Off();
AudDrv_Core_Clk_Off();
    return len;
}



/*****************************************************************************
 * FUNCTION
 *  AudDrv_mod_init / AudDrv_mod_exit
 *
 * DESCRIPTION
 *  Module init and de-init (only be called when system boot up)
 *
 *****************************************************************************
 */

static struct platform_driver AudDrv_driver = {
   .probe	 = AudDrv_probe,
   .remove	 = AudDrv_remove,
   .shutdown = AudDrv_shutdown,
   .suspend	 = AudDrv_suspend,
   .resume	 = AudDrv_resume,
   .driver   = {
                  .name = auddrv_name,
               },
};

struct platform_device AudDrv_device = {
	.name  = auddrv_name,
	.id    = 0,
	.dev   = {
		        .dma_mask = &AudDrv_dmamask,
		        .coherent_dma_mask =  0xffffffffUL
	         }
};

static int AudDrv_mod_init(void)
{
        int ret = 0;
	xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_mod_init \n");

	//register speaker driver
	Speaker_Register();
	// Register platform DRIVER
	ret = platform_driver_register(&AudDrv_driver);
	if(ret)
   {
		xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv Fail:%d - Register DRIVER \n",ret);
		return ret;
	}

   // register MISC device
   if((ret = misc_register(&AudDrv_audio_device)))
   {
   	xlog_printk(ANDROID_LOG_ERROR, "Sound","AudDrv_probe misc_register Fail:%d \n", ret);
   	return ret;
   }

   // cat /proc/audio
	create_proc_read_entry("audio",
	                       	     0,
                        	  NULL,
         	  AudDrv_Read_Procmem,
  		                       NULL);

   wake_lock_init(&Audio_wake_lock,WAKE_LOCK_SUSPEND,"Audio_WakeLock");
   wake_lock_init(&Audio_record_wake_lock,WAKE_LOCK_SUSPEND,"Audio_Record_WakeLock");
   xlog_printk(ANDROID_LOG_INFO, "Sound","AudDrv_mod_init: Init Audio Wake Lock\n");

   Ana_Alloc_Backup_Memory();

   memset(&AFE_dl_Control_context,0,sizeof(AFE_DL_CONTROL_T));
   memset(&I2S_input_Control_context,0,sizeof(I2S_INPUT_CONTROL_T));
   memset(&DAI_output_Control_context,0,sizeof(DAI_OUTPUT_CONTROL_T));
   xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv_mod_init return \n");

	return 0;
}

static void  AudDrv_mod_exit(void)
{
   xlog_printk(ANDROID_LOG_INFO, "Sound","+AudDrv_mod_exit \n");

   remove_proc_entry("audio", NULL);
   platform_driver_unregister(&AudDrv_driver);

   Ana_Free_Backup_Memory();

   xlog_printk(ANDROID_LOG_INFO, "Sound","-AudDrv_mod_exit \n");
}



MODULE_LICENSE("GPL");
MODULE_DESCRIPTION(AUDDRV_NAME);
MODULE_AUTHOR(AUDDRV_AUTHOR);

module_init(AudDrv_mod_init);
module_exit(AudDrv_mod_exit);


