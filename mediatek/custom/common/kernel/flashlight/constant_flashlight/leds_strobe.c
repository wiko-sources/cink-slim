#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/wait.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/poll.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/errno.h>
#include <linux/time.h>
#include "kd_flashlight.h"
#include <asm/io.h>
#include <asm/uaccess.h>
#include "kd_camera_hw.h"
/******************************************************************************
 * Definition
******************************************************************************/
#ifndef TRUE
#define TRUE KAL_TRUE
#endif
#ifndef FALSE
#define FALSE KAL_FALSE
#endif
 
/* device name and major number */
#define STROBE_DEVNAME            "leds_strobe"

#define DELAY_MS(ms) {mdelay(ms);}//unit: ms(10^-3)
#define DELAY_US(us) {mdelay(us);}//unit: us(10^-6)
#define DELAY_NS(ns) {mdelay(ns);}//unit: ns(10^-9)
/*
    non-busy dealy(/kernel/timer.c)(CANNOT be used in interrupt context):
        ssleep(sec)
        msleep(msec)
        msleep_interruptible(msec)

    kernel timer


*/

/******************************************************************************
 * Debug configuration
******************************************************************************/
#define PFX "[LEDS_STROBE]"
#define PK_DBG_NONE(fmt, arg...)    do {} while (0)
#define PK_DBG_FUNC(fmt, arg...)    printk(KERN_INFO PFX "%s: " fmt, __FUNCTION__ ,##arg)

#define PK_WARN(fmt, arg...)        printk(KERN_WARNING PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_NOTICE(fmt, arg...)      printk(KERN_NOTICE PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_INFO(fmt, arg...)        printk(KERN_INFO PFX "%s: " fmt, __FUNCTION__ ,##arg)
#define PK_TRC_FUNC(f)              printk(PFX "<%s>\n", __FUNCTION__);
#define PK_TRC_VERBOSE(fmt, arg...) printk(PFX fmt, ##arg)

#define DEBUG_LEDS_STROBE
#ifdef DEBUG_LEDS_STROBE
#define PK_DBG PK_DBG_FUNC
#define PK_TRC PK_DBG_NONE /* PK_TRC_FUNC */
#define PK_VER PK_DBG_NONE /* PK_TRC_VERBOSE */
#define PK_ERR(fmt, arg...)         printk(KERN_ERR PFX "%s: " fmt, __FUNCTION__ ,##arg)
#else
#define PK_DBG(a,...)
#define PK_TRC(a,...)
#define PK_VER(a,...)
#define PK_ERR(a,...)
#endif

/*******************************************************************************
* structure & enumeration
*******************************************************************************/

/* cotta-- added for high current solution */

#define FLASH_LIGHT_WDT_DISABLE 0
/*#define FLASH_LIGHT_WDT_TIMEOUT_MS          300	 max operation time of high current strobe */
/*#define FLASH_LIGHT_CAPTURE_DELAY_FRAME     (1)  should be same as pSensorInfo->CaptureDelayFrame = 1; */

/* --cotta */

struct strobe_data
{
    spinlock_t lock;
    wait_queue_head_t read_wait;
    struct semaphore sem;
};

/******************************************************************************
 * local variables
******************************************************************************/
static struct work_struct g_tExpEndWorkQ;   /* --- Work queue ---*/
static struct work_struct g_tTimeOutWorkQ;  /* --- Work queue ---*/
static GPT_CONFIG g_strobeGPTConfig;          /*cotta : Interrupt Config, modified for high current solution */
 

#if 0
static struct class *strobe_class = NULL;
static struct device *strobe_device = NULL;
static dev_t strobe_devno;
static struct cdev strobe_cdev;
#endif

/* static struct strobe_data strobe_private;*/

static DEFINE_SPINLOCK(g_strobeSMPLock); /* cotta-- SMP proection */

static u32 strobe_Res = 0;
static u32 strobe_Timeus = 0;
static BOOL g_strobe_On = FALSE;
static u32 strobe_width = 0; /* 0 is disable */
static eFlashlightState strobe_eState = FLASHLIGHTDRV_STATE_PREVIEW;
static MUINT32 sensorCaptureDelay = 0;                      /* cotta-- added for auto-set sensor capture delay mechanism*/
static MUINT32 g_WDTTimeout_ms = FLASH_LIGHT_WDT_DISABLE;   /* cotta-- disable WDT, added for high current solution */
static MUINT32 g_CaptureDelayFrame = 0;                     /* cotta-- added  for count down capture delay */

/* cotta-- added for level mapping 

     user can select 1-32 level value
     but driver will map 1-32 level to 1-12 level

     the reason why driver only uses 12 levels is protection reason
     according to LED spec
     if driving current larger than 200mA, LED might have chance to be damaged
     therefore, level 13-32 must use carafully and with strobe protection mechanism
     on the other hand, level 1-12 can use freely 
*/
static const MUINT32 strobeLevelLUT[32] = {1,2,3,4,5,6,7,8,9,10,11,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12,12}; 

/*****************************************************************************
Functions
*****************************************************************************/
#if 0
#define GPIO_CAMERA_FLASH_MODE GPIO95
#define GPIO_CAMERA_FLASH_MODE_M_GPIO  GPIO_MODE_00
    /*CAMERA-FLASH-T/F
           H:flash mode
           L:torch mode*/
#define GPIO_CAMERA_FLASH_EN GPIO46
#define GPIO_CAMERA_FLASH_EN_M_GPIO  GPIO_MODE_00
    /*CAMERA-FLASH-EN */


ssize_t gpio_FL_Init(void)
{
    /*set torch mode*/
    if(mt_set_gpio_mode(GPIO_CAMERA_FLASH_MODE,GPIO_CAMERA_FLASH_MODE_M_GPIO)){PK_DBG("[constant_flashlight] set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(GPIO_CAMERA_FLASH_MODE,GPIO_DIR_OUT)){PK_DBG("[constant_flashlight] set gpio dir failed!! \n");}
    if(mt_set_gpio_out(GPIO_CAMERA_FLASH_MODE,GPIO_OUT_ZERO)){PK_DBG("[constant_flashlight] set gpio failed!! \n");}
    /*Init. to disable*/
    if(mt_set_gpio_mode(GPIO_CAMERA_FLASH_EN,GPIO_CAMERA_FLASH_MODE_M_GPIO)){PK_DBG("[constant_flashlight] set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(GPIO_CAMERA_FLASH_EN,GPIO_DIR_OUT)){PK_DBG("[constant_flashlight] set gpio dir failed!! \n");}
    if(mt_set_gpio_out(GPIO_CAMERA_FLASH_EN,GPIO_OUT_ZERO)){PK_DBG("[constant_flashlight] set gpio failed!! \n");}
    return 0;
}

ssize_t gpio_FL_Uninit(void)
{
    /*Uninit. to disable*/
    if(mt_set_gpio_mode(GPIO_CAMERA_FLASH_EN,GPIO_CAMERA_FLASH_MODE_M_GPIO)){PK_DBG("[constant_flashlight] set gpio mode failed!! \n");}
    if(mt_set_gpio_dir(GPIO_CAMERA_FLASH_EN,GPIO_DIR_OUT)){PK_DBG("[constant_flashlight] set gpio dir failed!! \n");}
    if(mt_set_gpio_out(GPIO_CAMERA_FLASH_EN,GPIO_OUT_ZERO)){PK_DBG("[constant_flashlight] set gpio failed!! \n");}
    return 0;
}

ssize_t gpio_FL_Enable(void)
{
    /*Enable*/
    if(mt_set_gpio_out(GPIO_CAMERA_FLASH_EN,GPIO_OUT_ONE)){PK_DBG("[constant_flashlight] set gpio failed!! \n");}
    return 0;
}

ssize_t gpio_FL_Disable(void)
{
    /*Enable*/
    if(mt_set_gpio_out(GPIO_CAMERA_FLASH_EN,GPIO_OUT_ZERO)){PK_DBG("[constant_flashlight] set gpio failed!! \n");}
    return 0;
}

ssize_t gpio_FL_dim_duty(kal_uint8 duty)
{
    /*N/A*/
    
    return 0;
}

/* abstract interface */
#define FL_Init gpio_FL_Init
#define FL_Uninit gpio_FL_Uninit
#define FL_Enable gpio_FL_Enable
#define FL_Disable gpio_FL_Disable
#define FL_dim_duty gpio_FL_dim_duty

#elif 1
extern void hwBacklightFlashlightTuning(kal_uint32 duty, kal_uint32 mode);
extern void hwBacklightFlashlightTurnOn(void);
extern void hwBacklightFlashlightTurnOff(void);
extern void upmu_flash_step_sel(kal_uint32 val);
extern void upmu_flash_mode_select(kal_uint32 val);
extern void upmu_flash_dim_duty(kal_uint32 val);

/* abstract interface */
    
/*  cotta-- modified for high current solution */
int FL_Init(void)   /* Low current setting fuction*/
{
    upmu_flash_dim_duty(12); 
    
    spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */
    
    g_WDTTimeout_ms = FLASH_LIGHT_WDT_DISABLE;  /* disable WDT */
    
    spin_unlock(&g_strobeSMPLock);
    
    PK_DBG(" Low current mode\n");
    return 0;
}

int FL_Uninit(void)
{    
    return 0;
}

int FL_Enable(void) /* turn on strobe */
{
    hwBacklightFlashlightTurnOn();    
    return 0;
}

int FL_Disable(void)    /* turn off strobe */
{
    hwBacklightFlashlightTurnOff();   
    return 0;
}

int FL_dim_duty(kal_uint32 duty)    /* adjust duty */
{
    PK_DBG(" strobe duty : %u\n",duty);
   
    upmu_flash_step_sel(7);  
    upmu_flash_dim_duty(duty); 

    /*upmu_flash_mode_select(1);   register mode */
    /*hwBacklightFlashlightTuning(duty,0x0); */
    return 0;
}

/*  cotta-- added for high current solution */
int FL_HighCurrent_Setting(void)  /*  high current setting fuction*/
{    
    upmu_flash_step_sel(7);      /* max current */
    upmu_flash_dim_duty(0x1F);   /* max duty */   
    spin_lock(&g_strobeSMPLock);        /* cotta-- SMP proection */
    
    g_WDTTimeout_ms = FLASH_LIGHT_WDT_TIMEOUT_MS;   /* enable WDT */
    
    spin_unlock(&g_strobeSMPLock);

    PK_DBG(" High current mode\n");

    /* upmu_flash_mode_select(1);    register mode */
    /* hwBacklightFlashlightTuning(0x1F,0x0); */
    
    return 0;
}


#else
    #error "unknown arch"
#endif


/*****************************************************************************
debug
*****************************************************************************/
#define PREFLASH_PROFILE 0

#if PREFLASH_PROFILE
static void ledDrvProfile(unsigned int logID)
{
struct timeval tv1={0,0};
unsigned int u4MS;
    do_gettimeofday(&tv1);
    u4MS =  (unsigned int)(tv1.tv_sec * 1000 + tv1.tv_usec /1000);
    printk("[ledDrvProfile]state:[%d],time:[%d]ms \n",logID,u4MS);
}
#endif


/*****************************************************************************
User interface
*****************************************************************************/
static void strobe_xgptIsr(UINT16 a_input)
{
	schedule_work(&g_tTimeOutWorkQ);
}

/* cotta : modified to GPT usage */
static int strobe_WDTConfig(void)
{
    spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */
    
    /* --- Config Interrupt --- */
    g_strobeGPTConfig.num         = GPT3;             /* GPT3 is assigned to strobe */
    g_strobeGPTConfig.mode        = GPT_ONE_SHOT;     /* trigger ones responed ones */
    g_strobeGPTConfig.clkSrc      = GPT_CLK_SRC_RTC;  /* clock source 32K */
    g_strobeGPTConfig.clkDiv      = GPT_CLK_DIV_11;   /* divided to 3.2k */
    g_strobeGPTConfig.bIrqEnable  = TRUE;             /* enable interrupt */
    g_strobeGPTConfig.u4CompareL  = ((g_WDTTimeout_ms * 3200) + 500) / 1000; /* 960 -> 300ms , 3200 -> 1000ms */
    g_strobeGPTConfig.u4CompareH  = 0;

    spin_unlock(&g_strobeSMPLock);
    
     /*
    if (GPT_IsStart(g_strobeGPTConfig.num))
    {
        PK_ERR("GPT_ISR is busy now\n");
        return -EBUSY;
    }
    */

    GPT_Reset(GPT3);
    
    if(GPT_Config(g_strobeGPTConfig) == FALSE)
    {
        PK_ERR("GPT_ISR Config Fail\n");
        return -EPERM;
    }   
    
    GPT_Init(g_strobeGPTConfig.num, strobe_xgptIsr);      

    PK_DBG("[GPT_ISR Config OK] g_strobeGPTConfig.u4CompareL = %u, WDT_timeout = %d\n",g_strobeGPTConfig.u4CompareL,FLASH_LIGHT_WDT_TIMEOUT_MS);
    
#if 0
    XGPT_IsStart(g_tStrobeXGPTConfig.num);
	XGPT_Start(g_tStrobeXGPTConfig.num);
	XGPT_Stop(g_tStrobeXGPTConfig.num);
    XGPT_Restart(g_tStrobeXGPTConfig.num);
#endif

    return 0;
}

static void strobe_WDTStart(void)
{
    GPT_Start(g_strobeGPTConfig.num);
    PK_DBG(" WDT start\n");
}

static void strobe_WDTStop(void)
{
    GPT_Stop(g_strobeGPTConfig.num);
    PK_DBG(" WDT stop\n");
}


static int constant_flashlight_ioctl(MUINT32 cmd, MUINT32 arg)
{
    int i4RetValue = 0;
    int iFlashType = (int)FLASHLIGHT_NONE;
    struct timeval now; /* cotta--log */
    
    PK_DBG(" start: %u, %u, %d, %d\n",cmd,arg,strobe_width,g_strobe_On);

    /* when ON state , only disable command is permitted */
    if((TRUE == g_strobe_On) && !((FLASHLIGHTIOC_T_ENABLE == cmd) && (0 == arg)))
    {
        PK_DBG(" is already ON OR check parameters!\n");
        
        /* goto strobe_ioctl_error; */
        return i4RetValue;
    }

    switch(cmd)
    {
    	case FLASHLIGHTIOC_T_ENABLE :
            if (arg && strobe_width)
            {                
                if( (FLASHLIGHTDRV_STATE_PREVIEW == strobe_eState) || \
                    (FLASHLIGHTDRV_STATE_STILL == strobe_eState && KD_STROBE_HIGH_CURRENT_WIDTH != strobe_width) )
                {                
                    /* turn on strobe */
                    FL_Enable();
                    
                    spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */

                    g_strobe_On = TRUE;

                    spin_unlock(&g_strobeSMPLock);
                    
                    do_gettimeofday(&now);  /* cotta--log */
                    PK_DBG(" flashlight_IOCTL_Enable: %lu\n",now.tv_sec * 1000000 + now.tv_usec);
                }
                else
                {
                    PK_DBG(" flashlight_IOCTL_Enable : skip due to flash mode\n");  /* use VDIrq to control strobe under high current mode */                  
                } 
            }
            else
            {                      
                if(FL_Disable())
                {
                    PK_ERR(" FL_Disable fail!\n");
                    goto strobe_ioctl_error;
                }

                spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */
                
                g_strobe_On = FALSE;                

                spin_unlock(&g_strobeSMPLock);

                if (g_WDTTimeout_ms != FLASH_LIGHT_WDT_DISABLE)
                {                    
                    strobe_WDTStop();   /* disable strobe watchDog timer */ 
                    FL_Init();  /* confirm everytime start from low current mode */
                }                

                do_gettimeofday(&now);  /* cotta--log */
                PK_DBG(" flashlight_IOCTL_Disable: %lu\n",now.tv_sec * 1000000 + now.tv_usec);                
            }
    		break;
        case FLASHLIGHTIOC_T_LEVEL:
            PK_DBG(" user level:%u \n",arg);

            if (KD_STROBE_HIGH_CURRENT_WIDTH == arg)   /* high current mode */
            {
                spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */
                
                strobe_width = KD_STROBE_HIGH_CURRENT_WIDTH;

                spin_unlock(&g_strobeSMPLock);
                FL_HighCurrent_Setting();                
            }
            else    /* low current mode */
            {            
                /* cotta-- added for level mapping */

                if(arg > 32)
                {
                    arg = 32;
                }
                else if(arg < 1)
                {
                    arg = 1;
                }
                
                arg = strobeLevelLUT[arg-1];  
                PK_DBG(" mapping level:%u \n",(int)arg);

                /* --cotta */
            
                spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */
                
                strobe_width = arg;                

                spin_unlock(&g_strobeSMPLock);

                if (arg > 0)
                {
                    /* set to low current mode */
                    FL_Init();
                    
                    if(FL_dim_duty((kal_int8)arg - 1))
                    {
                        /* 0(weak)~31(strong) */
                        PK_ERR(" FL_dim_duty fail!\n");
                        
                        /* i4RetValue = -EINVAL; */
                        goto strobe_ioctl_error;
                    }
                }
            }
            break;
        case FLASHLIGHTIOC_T_FLASHTIME:
            spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */
            
            strobe_Timeus = (u32)arg;
            
            spin_unlock(&g_strobeSMPLock);
            PK_DBG(" strobe_Timeus:%u \n",strobe_Timeus);
            break;
        case FLASHLIGHTIOC_T_STATE:
            spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */
             
            strobe_eState = (eFlashlightState)arg;

            spin_unlock(&g_strobeSMPLock);
            PK_DBG(" strobe_state:%u\n",arg);
            break;
        case FLASHLIGHTIOC_G_FLASHTYPE:
            spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */

            iFlashType = FLASHLIGHT_LED_CONSTANT;

            spin_unlock(&g_strobeSMPLock);
            if(copy_to_user((void __user *) arg , (void*)&iFlashType , _IOC_SIZE(cmd)))
            {
                PK_DBG(" ioctl copy to user failed\n");
                return -EFAULT;
            }
            break;
        case FLASHLIGHTIOC_T_DELAY : /* cotta-- added for auto-set sensor capture delay mechanism*/
            if(arg >= 0)
            {                
                spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */
                
                sensorCaptureDelay = arg;                
                g_CaptureDelayFrame = sensorCaptureDelay;

                spin_unlock(&g_strobeSMPLock);
                PK_DBG("capture delay = %u, count down value = %u\n",sensorCaptureDelay,g_CaptureDelayFrame);
            }
            break;                  
    	default :
    		PK_DBG(" No such command \n");
    		i4RetValue = -EPERM;
    		break;
    }

    /* PK_DBG(" done\n"); */
    return i4RetValue;

strobe_ioctl_error:
    PK_DBG(" Error or ON state!\n");
    return -EPERM;
}


static void strobe_expEndWork(struct work_struct *work)
{
    PK_DBG(" Exposure End Disable\n");
    constant_flashlight_ioctl(FLASHLIGHTIOC_T_ENABLE, 0);
}


static void strobe_timeOutWork(struct work_struct *work)
{
    PK_DBG(" Force OFF LED\n");
    constant_flashlight_ioctl(FLASHLIGHTIOC_T_ENABLE, 0);
}


static int constant_flashlight_open(void *pArg)
{
    int i4RetValue = 0;
    PK_DBG(" start\n");

    if (0 == strobe_Res)
    {
        FL_Init();

		/* enable HW here if necessary */
        if(FL_dim_duty(0))
        {
            /* 0(weak)~31(strong) */
            PK_ERR(" FL_dim_duty fail!\n");
            i4RetValue = -EINVAL;
        }

        /* --- WorkQueue --- */
        INIT_WORK(&g_tExpEndWorkQ,strobe_expEndWork);
        INIT_WORK(&g_tTimeOutWorkQ,strobe_timeOutWork);

        spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */
        
        g_strobe_On = FALSE;
        
        spin_unlock(&g_strobeSMPLock);
    }
    
    spin_lock_irq(&g_strobeSMPLock);
    
    if(strobe_Res)
    {
        PK_ERR(" busy!\n");
        i4RetValue = -EBUSY;
    }
    else
    {
        strobe_Res += 1;
    }

    spin_unlock_irq(&g_strobeSMPLock);

    PK_DBG(" Done\n");

    return i4RetValue;

}


static int constant_flashlight_release(void *pArg)
{    
    PK_DBG(" start\n");

    if (strobe_Res)
    {
        spin_lock_irq(&g_strobeSMPLock);

        strobe_Res = 0;
        strobe_Timeus = 0;

        /* LED On Status */
        g_strobe_On = FALSE;

        spin_unlock_irq(&g_strobeSMPLock);

    	/* uninit HW here if necessary */
        if(FL_dim_duty(0))
        {
            PK_ERR(" FL_dim_duty fail!\n");
        }        
        
    	FL_Uninit();

        GPT_Stop(g_strobeGPTConfig.num);       
    }

    PK_DBG(" Done\n");

    return 0;

}


FLASHLIGHT_FUNCTION_STRUCT	constantFlashlightFunc=
{
	constant_flashlight_open,
	constant_flashlight_release,
	constant_flashlight_ioctl
};


MUINT32 constantFlashlightInit(PFLASHLIGHT_FUNCTION_STRUCT *pfFunc)
{ 
    if (pfFunc != NULL)
    {
        *pfFunc = &constantFlashlightFunc;
    }
    return 0;
}



/* LED flash control for high current capture mode*/
ssize_t strobe_VDIrq(void)
{    
    struct timeval now; /* cotta--log */
    
    if ((strobe_width == KD_STROBE_HIGH_CURRENT_WIDTH) && (strobe_eState == FLASHLIGHTDRV_STATE_STILL))
    {
        PK_DBG(" start \n");
       
        if (g_CaptureDelayFrame == 0)
        {
            /* confirm WDT config value is correct */
            if(g_WDTTimeout_ms == FLASH_LIGHT_WDT_DISABLE)
            {
                spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */
                
                g_WDTTimeout_ms = FLASH_LIGHT_WDT_TIMEOUT_MS;   /* enable WDT */             

                spin_unlock(&g_strobeSMPLock); 
            }
            
            /* enable strobe watchDog timer */
            strobe_WDTConfig();
                       
            if (g_WDTTimeout_ms != FLASH_LIGHT_WDT_DISABLE)
            {                
                strobe_WDTStart(); /* enable strobe watchDog timer */
            } 

            /* turn on strobe */
            FL_Enable(); 
            
            do_gettimeofday(&now);  /* cotta--log */
            PK_DBG(" strobe_VDIrq_Enable: %lu\n",now.tv_sec * 1000000 + now.tv_usec);

            spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */

            g_strobe_On = TRUE;

            /* cotta-- modified for auto-set sensor capture delay mechanism*/
            g_CaptureDelayFrame = sensorCaptureDelay;                 
            
            strobe_eState = FLASHLIGHTDRV_STATE_PREVIEW;            
            
            spin_unlock(&g_strobeSMPLock);    /* cotta-- SMP proection */

            PK_DBG(" g_CaptureDelayFrame : %u\n",g_CaptureDelayFrame);
        }
        else
        {
            spin_lock(&g_strobeSMPLock);    /* cotta-- SMP proection */
            
            --g_CaptureDelayFrame;           

            spin_unlock(&g_strobeSMPLock);    /* cotta-- SMP proection */
        }
    }
    
    return 0;
}

EXPORT_SYMBOL(strobe_VDIrq);


#if 0
/*****************************************************************************/
/* Kernel interface */
static struct file_operations strobe_fops = {
    .owner      = THIS_MODULE,
    .ioctl      = strobe_ioctl,
    .open       = strobe_open,
    .release    = strobe_release,
};

/*****************************************************************************
Driver interface
*****************************************************************************/
#define ALLOC_DEVNO
static int strobe_probe(struct platform_device *dev)
{
    int ret = 0, err = 0;

	PK_DBG("[constant_flashlight] start\n");

#ifdef ALLOC_DEVNO
    ret = alloc_chrdev_region(&strobe_devno, 0, 1, STROBE_DEVNAME);
    if (ret) {
        PK_ERR("[constant_flashlight] alloc_chrdev_region fail: %d\n", ret);
        goto strobe_probe_error;
    } else {
        PK_DBG("[constant_flashlight] major: %d, minor: %d\n", MAJOR(strobe_devno), MINOR(strobe_devno));
    }
    cdev_init(&strobe_cdev, &strobe_fops);
    strobe_cdev.owner = THIS_MODULE;
    err = cdev_add(&strobe_cdev, strobe_devno, 1);
    if (err) {
        PK_ERR("[constant_flashlight] cdev_add fail: %d\n", err);
        goto strobe_probe_error;
    }
#else
    #define STROBE_MAJOR 242
    ret = register_chrdev(STROBE_MAJOR, STROBE_DEVNAME, &strobe_fops);
    if (ret != 0) {
        PK_ERR("[constant_flashlight] Unable to register chardev on major=%d (%d)\n", STROBE_MAJOR, ret);
        return ret;
    }
    strobe_devno = MKDEV(STROBE_MAJOR, 0);
#endif


    strobe_class = class_create(THIS_MODULE, "strobedrv");
    if (IS_ERR(strobe_class)) {
        PK_ERR("[constant_flashlight] Unable to create class, err = %d\n", (int)PTR_ERR(strobe_class));
        goto strobe_probe_error;
    }

    strobe_device = device_create(strobe_class, NULL, strobe_devno, NULL, STROBE_DEVNAME);
    if(NULL == strobe_device){
        PK_ERR("[constant_flashlight] device_create fail\n");
        goto strobe_probe_error;
    }

    /*initialize members*/
    spin_lock_init(&strobe_private.lock);
    init_waitqueue_head(&strobe_private.read_wait);
    init_MUTEX(&strobe_private.sem);

    //LED On Status
    g_strobe_On = FALSE;

    PK_DBG("[constant_flashlight] Done\n");
    return 0;

strobe_probe_error:
#ifdef ALLOC_DEVNO
    if (err == 0)
        cdev_del(&strobe_cdev);
    if (ret == 0)
        unregister_chrdev_region(strobe_devno, 1);
#else
    if (ret == 0)
        unregister_chrdev(MAJOR(strobe_devno), STROBE_DEVNAME);
#endif
    return -1;
}

static int strobe_remove(struct platform_device *dev)
{

    PK_DBG("[constant_flashlight] start\n");

#ifdef ALLOC_DEVNO
    cdev_del(&strobe_cdev);
    unregister_chrdev_region(strobe_devno, 1);
#else
    unregister_chrdev(MAJOR(strobe_devno), STROBE_DEVNAME);
#endif
    device_destroy(strobe_class, strobe_devno);
    class_destroy(strobe_class);

    //LED On Status
    g_strobe_On = FALSE;

    PK_DBG("[constant_flashlight] Done\n");
    return 0;
}


static struct platform_driver strobe_platform_driver =
{
    .probe      = strobe_probe,
    .remove     = strobe_remove,
    .driver     = {
        .name = STROBE_DEVNAME,
		.owner	= THIS_MODULE,
    },
};

static struct platform_device strobe_platform_device = {
    .name = STROBE_DEVNAME,
    .id = 0,
    .dev = {
    }
};

static int __init strobe_init(void)
{
    int ret = 0;
    PK_DBG("[constant_flashlight] start\n");

	ret = platform_device_register (&strobe_platform_device);
	if (ret) {
        PK_ERR("[constant_flashlight] platform_device_register fail\n");
        return ret;
	}

    ret = platform_driver_register(&strobe_platform_driver);
	if(ret){
		PK_ERR("[constant_flashlight] platform_driver_register fail\n");
		return ret;
	}

	PK_DBG("[constant_flashlight] done!\n");
    return ret;
}

static void __exit strobe_exit(void)
{
    PK_DBG("[constant_flashlight] start\n");
    platform_driver_unregister(&strobe_platform_driver);
    //to flush work queue
    //flush_scheduled_work();
    PK_DBG("[constant_flashlight] done!\n");
}

/*****************************************************************************/
module_init(strobe_init);
module_exit(strobe_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jackie Su <jackie.su@mediatek.com>");
MODULE_DESCRIPTION("LED strobe control Driver");


/* LED flash control for capture mode*/
ssize_t strobe_StillExpCfgStart(void)
{
    if (strobe_width&&(FLASHLIGHTDRV_STATE_STILL == strobe_eState))
    {
        strobe_ioctl(0,0,FLASHLIGHTIOC_T_ENABLE, 1);
        PK_DBG("[constant_flashlight] FLASHLIGHTIOC_T_ENABLE:1\n");
    }
#if PREFLASH_PROFILE
    ledDrvProfile(999);
#endif
    return 0;
}

ssize_t strobe_StillExpEndIrqCbf(void)
{
    if (strobe_width&&(FLASHLIGHTDRV_STATE_STILL == strobe_eState))
    {
        schedule_work(&g_tExpEndWorkQ);
        strobe_eState = FLASHLIGHTDRV_STATE_PREVIEW;
    }
    return 0;
}

EXPORT_SYMBOL(strobe_StillExpCfgStart);
EXPORT_SYMBOL(strobe_StillExpEndIrqCbf);
#endif

