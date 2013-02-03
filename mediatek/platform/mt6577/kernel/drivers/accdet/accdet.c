#include "accdet.h"
#include "mach/mt6577_boot.h"
#include <cust_eint.h>
#include <mach/mt6577_gpio.h>

//#define ACCDET_EINT
#define SW_WORK_AROUND_ACCDET_REMOTE_BUTTON_ISSUE

/*----------------------------------------------------------------------
static variable defination
----------------------------------------------------------------------*/
#define REGISTER_VALUE(x)   (x - 1)

static struct switch_dev accdet_data;
static struct input_dev *kpd_accdet_dev;
static struct cdev *accdet_cdev;
static struct class *accdet_class = NULL;
static struct device *accdet_nor_device = NULL;

static dev_t accdet_devno;

static int pre_status = 0;
static int pre_state_swctrl = 0;
static int accdet_status = PLUG_OUT;
static int cable_type = 0;
static s64 long_press_time_ns = 0 ;

static int g_accdet_first = 1;
static bool IRQ_CLR_FLAG = FALSE;
static volatile int call_status =0;
static volatile int button_status = 0;
static int tv_out_debounce = 0;

struct wake_lock accdet_suspend_lock; 
struct wake_lock accdet_irq_lock;
struct wake_lock accdet_key_lock;

static struct work_struct accdet_work;
static struct workqueue_struct * accdet_workqueue = NULL;
static int g_accdet_working_in_suspend =0;

#ifdef ACCDET_EINT
//static int g_accdet_working_in_suspend =0;

static struct work_struct accdet_eint_work;
static struct workqueue_struct * accdet_eint_workqueue = NULL;

static inline void accdet_init(void);

extern void mt65xx_eint_unmask(unsigned int line);
extern void mt65xx_eint_mask(unsigned int line);
extern void mt65xx_eint_set_polarity(kal_uint8 eintno, kal_bool ACT_Polarity);
extern void mt65xx_eint_set_hw_debounce(kal_uint8 eintno, kal_uint32 ms);
extern kal_uint32 mt65xx_eint_set_sens(kal_uint8 eintno, kal_bool sens);
extern void mt65xx_eint_registration(kal_uint8 eintno, kal_bool Dbounce_En,
                                     kal_bool ACT_Polarity, void (EINT_FUNC_PTR)(void),
                                     kal_bool auto_umask);
#endif

//#define SW_WORK_AROUND_ACCDET_DEBOUNCE_HANG

#ifdef SW_WORK_AROUND_ACCDET_DEBOUNCE_HANG
static struct work_struct accdet_check_work;
static struct workqueue_struct * accdet_check_workqueue = NULL;

struct timer_list   check_timer; 

#endif


#ifdef TV_OUT_SUPPORT
extern void TVOUT_EnableColorBar(BOOL enable);
extern TVOUT_STATUS TVOUT_TvCablePlugIn(void);
extern TVOUT_STATUS TVOUT_TvCablePlugOut(void);
extern bool TVOUT_IsTvoutEnabled(void);
extern bool TVOUT_IsUserEnabled(void);

//#define TVOUT_PLUGIN_SLOWLY_WORKAROUND 

#endif

static volatile int double_check_flag = 0;
static bool tv_headset_icon = false;
static int button_press_debounce = 0x400;

#define DEBUG_THREAD 1
static struct platform_driver accdet_driver;



/****************************************************************/
/***export function, for tv out driver                                                                     **/
/****************************************************************/
void switch_asw_to_tv(bool tv_enable)
{
	ACCDET_DEBUG("[Accdet]switch analog switch to tv is %d\n",tv_enable);
	if(tv_enable)
	{
		SETREG32(ACCDET_STATE_SWCTRL,TV_DET_BIT);
		hwSPKClassABAnalogSwitchSelect(ACCDET_TV_CHA);
	}
	else
	{
		CLRREG32(ACCDET_STATE_SWCTRL,TV_DET_BIT);
		hwSPKClassABAnalogSwitchSelect(ACCDET_MIC_CHA);
	}
}
EXPORT_SYMBOL(switch_asw_to_tv);

void switch_NTSC_to_PAL(int mode)
{

#ifdef TV_OUT_SUPPORT
    if((mode < 0)||(mode > 1))
    {
        ACCDET_DEBUG("[Accdet]switch_NTSC_to_PAL:tv mode is invalid: %d!\n", mode);
    }
    else
    {
        ACCDET_DEBUG("[Accdet]switch_NTSC_to_PAL:%s MODE!\n", (mode? "PAL":"NSTC"));
        
        // init the TV out cable detection relative register
        OUTREG32(ACCDET_TV_START_LINE0,cust_tv_settings[mode].start_line0);
	    OUTREG32(ACCDET_TV_END_LINE0,cust_tv_settings[mode].end_line0);
	    OUTREG32(ACCDET_TV_START_LINE1,cust_tv_settings[mode].start_line1);
	    OUTREG32(ACCDET_TV_END_LINE1,cust_tv_settings[mode].end_line1);
	    OUTREG32(ACCDET_TV_PRE_LINE,cust_tv_settings[mode].pre_line);
	    OUTREG32(ACCDET_TV_START_PXL,cust_tv_settings[mode].start_pixel);
	    OUTREG32(ACCDET_TV_END_PXL,cust_tv_settings[mode].end_pixel);
        
	    OUTREG32(ACCDET_TV_EN_DELAY_NUM,
            (cust_tv_settings[mode].fall_delay << 10|cust_tv_settings[mode].rise_delay));
      	
        //set div and debounce in TV-out mode 
        OUTREG32(ACCDET_TV_DIV_RATE,cust_tv_settings[mode].div_rate); 
        OUTREG32(ACCDET_DEBOUNCE2, cust_tv_settings[mode].debounce);
	tv_out_debounce = cust_tv_settings[mode].debounce;
    }
    
#endif

    return;
    
}
EXPORT_SYMBOL(switch_NTSC_to_PAL);


void accdet_detect(void)
{
	int ret = 0 ;
    
	ACCDET_DEBUG("[Accdet]accdet_detect\n");
    
	accdet_status = PLUG_OUT;
    ret = queue_work(accdet_workqueue, &accdet_work);	
    if(!ret)
    {
  		ACCDET_DEBUG("[Accdet]accdet_detect:accdet_work return:%d!\n", ret);  		
    }

	return;
}
EXPORT_SYMBOL(accdet_detect);

static void enable_tv_detect(bool tv_enable);
static void enable_tv_allwhite_signal(bool tv_enable);
void accdet_state_reset(void)
{
    
	ACCDET_DEBUG("[Accdet]accdet_state_reset\n");
    
	accdet_status = PLUG_OUT;
        cable_type = NO_DEVICE;
        enable_tv_allwhite_signal(false);
	enable_tv_detect(false);

	return;
}
EXPORT_SYMBOL(accdet_state_reset);


/****************************************************************/
/*******static function defination                                                                          **/
/****************************************************************/

#ifdef ACCDET_EINT

void inline disable_accdet(void)
{
   // disable ACCDET unit
   ACCDET_DEBUG("accdet: disable_accdet\n");
   pre_state_swctrl = INREG32(ACCDET_STATE_SWCTRL);
   OUTREG32(ACCDET_CTRL, ACCDET_DISABLE);
   OUTREG32(ACCDET_STATE_SWCTRL, 0);
   //unmask EINT
   mt65xx_eint_unmask(CUST_EINT_ACCDET_NUM);  
}



void accdet_eint_work_callback(struct work_struct *work)
{
    ACCDET_DEBUG(" EINT fuc delay %d then enable accdet\n",ACCDET_DELAY_ENABLE_TIME);
	msleep(ACCDET_DELAY_ENABLE_TIME);
    //reset AccDet
    accdet_init();
	//enable ACCDET unit
	OUTREG32(ACCDET_STATE_SWCTRL, ACCDET_SWCTRL_EN);
  	OUTREG32(ACCDET_CTRL, ACCDET_ENABLE); 
    //when unmask EINT
}


void accdet_eint_func(void)
{
	int ret=0;
	//ACCDET_DEBUG(" EINT fuc\n");
	ret = queue_work(accdet_eint_workqueue, &accdet_eint_work);	
    if(!ret)
    {
  	   ACCDET_DEBUG("[Accdet]accdet_eint_func:accdet_work return:%d!\n", ret);  		
    }
}


static inline int accdet_setup_eint(void)
{
	
	/*configure to GPIO function, external interrupt*/
    ACCDET_DEBUG("[Accdet]accdet_setup_eint\n");
	
	mt_set_gpio_mode(GPIO_ACCDET_EINT_PIN, GPIO_ACCDET_EINT_PIN_M_EINT);
        mt_set_gpio_dir(GPIO_ACCDET_EINT_PIN, GPIO_DIR_IN);
        mt_set_gpio_pull_enable(GPIO_ACCDET_EINT_PIN, GPIO_PULL_DISABLE); //To disable GPIO PULL.
/*
	mt_set_gpio_pull_enable(GPIO_ACCDET_EINT_PIN, GPIO_PULL_ENABLE);
	#ifdef ACCDET_EINT_HIGH_ACTIVE
	mt_set_gpio_pull_select(GPIO_ACCDET_EINT_PIN, GPIO_PULL_DOWN);
	#else
	mt_set_gpio_pull_select(GPIO_ACCDET_EINT_PIN, GPIO_PULL_UP);
	#endif
*/	
	
    mt65xx_eint_set_sens(CUST_EINT_ACCDET_NUM, CUST_EINT_ACCDET_SENSITIVE);
	#ifdef ACCDET_EINT_HIGH_ACTIVE
	mt65xx_eint_set_polarity(CUST_EINT_ACCDET_NUM, 1);
	ACCDET_DEBUG("[Accdet]accdet_setup_eint active high \n");
	#else
	mt65xx_eint_set_polarity(CUST_EINT_ACCDET_NUM, 0);
	ACCDET_DEBUG("[Accdet]accdet_setup_eint active low\n");
	#endif
	mt65xx_eint_set_hw_debounce(CUST_EINT_ACCDET_NUM, CUST_EINT_ACCDET_DEBOUNCE_CN);
	mt65xx_eint_registration(CUST_EINT_ACCDET_NUM, CUST_EINT_ACCDET_DEBOUNCE_EN, CUST_EINT_ACCDET_POLARITY, accdet_eint_func, 0);
	mt65xx_eint_unmask(CUST_EINT_ACCDET_NUM);  

	
    return 0;
	
}


#endif


static void enable_tv_detect(bool tv_enable)
{
    kal_uint8 value;

    // disable ACCDET unit before switch the detect way
    OUTREG32(ACCDET_CTRL, ACCDET_DISABLE);
    OUTREG32(ACCDET_STATE_SWCTRL, 0);
    
	if(tv_enable)
	{	
	    if(get_chip_eco_ver()==CHIP_E1)
	    {
		  #ifndef MT6577_EVB_MIC_TV_CHA
		  hwSPKClassABAnalogSwitchSelect(ACCDET_TV_CHA);
		  value = ACCDET_MIC_CHA;
		  while(value != ACCDET_TV_CHA)
		  {
		    pmic_bank1_read_interface(BANK1_ASW_CON0, &value, BANK_1_RG_ANA_SW_SEL_MASK, 
				BANK_1_RG_ANA_SW_SEL_SHIFT);
		  }
		  #else
		  hwSPKClassABAnalogSwitchSelect(0x0);
		  value = 0x1;
		  while(value != 0)
		  {
		        pmic_bank1_read_interface(BANK1_ASW_CON0, &value, BANK_1_RG_ANA_SW_SEL_MASK, 
				BANK_1_RG_ANA_SW_SEL_SHIFT);
		  }
		  #endif
	    }
		if(get_chip_eco_ver()==CHIP_E2)
		{
		   hwSPKClassABAnalogSwitchSelect(ACCDET_TV_CHA);
		   value = ACCDET_MIC_CHA;
		   while(value != ACCDET_TV_CHA)
		   {
		        pmic_bank1_read_interface(BANK1_ASW_CON0, &value, BANK_1_RG_ANA_SW_SEL_MASK, 
				BANK_1_RG_ANA_SW_SEL_SHIFT);
		   }
		}
		else
		{
		   hwSPKClassABAnalogSwitchSelect(ACCDET_TV_CHA);
		   value = ACCDET_MIC_CHA;
		   while(value != ACCDET_TV_CHA)
		   {
		        pmic_bank1_read_interface(BANK1_ASW_CON0, &value, BANK_1_RG_ANA_SW_SEL_MASK, 
				BANK_1_RG_ANA_SW_SEL_SHIFT);
		   }
		}

		// switch analog switch from mic to tv out
		SETREG32(ACCDET_STATE_SWCTRL, TV_DET_BIT|CMP_PWM_EN_BIT);	
		
        // init the accdet tv detect unit
	SETREG32(ACCDET_RSTB, TV_INIT_BIT);
	CLRREG32(ACCDET_RSTB, TV_INIT_BIT); 

        //because tv_div!=0 on MT6573 E2, accdet will spend much time on switching from 
        //double check to hook_switch and mic_bias, so need decrease hook_switch and plug_out debounce time
        //notes: in tv mode, switch time from double to mic_bias is decided by plug_out debounce instead of mic_bias
        OUTREG32(ACCDET_DEBOUNCE0, 0x4);
	OUTREG32(ACCDET_DEBOUNCE3, 0x4);
	}
    else
	{
	    if(get_chip_eco_ver()==CHIP_E1)
	    {
		   #ifndef MT6577_EVB_MIC_TV_CHA
		   hwSPKClassABAnalogSwitchSelect(ACCDET_MIC_CHA);
		   value = ACCDET_TV_CHA;
		   while(value != ACCDET_MIC_CHA)
		   {
		        pmic_bank1_read_interface(BANK1_ASW_CON0, &value, BANK_1_RG_ANA_SW_SEL_MASK, 
				BANK_1_RG_ANA_SW_SEL_SHIFT);
		   }
		   #else
		   hwSPKClassABAnalogSwitchSelect(0x1);
		   value = 0x0;
		   while(value != 0x1)
		   {
		        pmic_bank1_read_interface(BANK1_ASW_CON0, &value, BANK_1_RG_ANA_SW_SEL_MASK, 
				BANK_1_RG_ANA_SW_SEL_SHIFT);
		   }
		   #endif
	    }
		if(get_chip_eco_ver()==CHIP_E2)
		{
		   hwSPKClassABAnalogSwitchSelect(ACCDET_MIC_CHA);
		   value = ACCDET_TV_CHA;
		   while(value != ACCDET_MIC_CHA)
		   {
		        pmic_bank1_read_interface(BANK1_ASW_CON0, &value, BANK_1_RG_ANA_SW_SEL_MASK, 
				BANK_1_RG_ANA_SW_SEL_SHIFT);
		   }
		}
		else
		{
		   hwSPKClassABAnalogSwitchSelect(ACCDET_MIC_CHA);
		   value = ACCDET_TV_CHA;
		   while(value != ACCDET_MIC_CHA)
		   {
		        pmic_bank1_read_interface(BANK1_ASW_CON0, &value, BANK_1_RG_ANA_SW_SEL_MASK, 
				BANK_1_RG_ANA_SW_SEL_SHIFT);
		   }
		}
		
		// switch analog switch from tv out to mic
		OUTREG32(ACCDET_STATE_SWCTRL, ACCDET_SWCTRL_EN);

		// init the accdet MIC detect unit
	  	SETREG32(ACCDET_RSTB, MIC_INIT_BIT);
		CLRREG32(ACCDET_RSTB, MIC_INIT_BIT);  
	}
    
	ACCDET_DEBUG("[Accdet]enable_tv_detect:ACCDET_STATE_SWCTRL =%x\n",
        INREG32(ACCDET_STATE_SWCTRL));

    // enable ACCDET unit
	OUTREG32(ACCDET_CTRL, ACCDET_ENABLE); 
	
}

//enable allwhite signal or color bar from TV-out
static void enable_tv_allwhite_signal(bool tv_enable)
{
    #ifdef TV_OUT_SUPPORT
	ACCDET_DEBUG("[Accdet]enable tv all white signal is %d\n",tv_enable);
    
	TVOUT_EnableColorBar(tv_enable);
    #endif
}

//enable TV to out tv signal
static TVOUT_STATUS enable_tv(bool tv_enable)
{
    TVOUT_STATUS ret = TVOUT_STATUS_OK;
    
    #ifdef TV_OUT_SUPPORT
	ACCDET_DEBUG("[Accdet]enable tv is %d\n",tv_enable);
    
	if(tv_enable)
	{
		ret = TVOUT_TvCablePlugIn();
		if(ret)
	    {
	      ACCDET_DEBUG("[Accdet]enable tv error!!! %d\n",ret);
	    }
	}
	else
	{
		ret = TVOUT_TvCablePlugOut();
		if(ret)
	    {
	      ACCDET_DEBUG("[Accdet]disable tv error!!! %d\n",ret);
	    }
	}
    #endif

	

    return ret;
}

static bool Is_TvoutEnabled(void)
{
	#ifdef TV_OUT_SUPPORT
	return TVOUT_IsTvoutEnabled();
	#endif
	return false;
}

#if 0
static void ACCDET_DumpRegisters(void)
{
	int i;
	int val;
	
	for(i = 0; i <= 30; i++)
        {
   		if(i >= 5 && i <= 8)
			continue;
		
   		val = (ACCDET_BASE + 4*i);
		ACCDET_DEBUG("[0x%x] = 0x%x\n", val, INREG32(val));
        }
	val=0;
	pmic_bank1_read_interface(0x5F, &val, 1, 0);
	ACCDET_DEBUG("[0x5F] = 0x%x\n", val);
}
#endif

//detect if remote button is short pressed or long pressed
static bool is_long_press(void)
{
	int current_status = 0;
	int index = 0;
	int count = long_press_time / 100;
	while(index++ < count)
	{ 
		current_status = INREG32(ACCDET_MEMORIZED_IN) & 0x3;
		if(current_status != 0)
		{
			return false;
		}
			
		msleep(100);
	}
	
	return true;
}

//clear ACCDET IRQ in accdet register
static inline void clear_accdet_interrupt(void)
{
	//SETREG32(ACCDET_IRQ_STS, (IRQ_CLR_BIT|IRQ_CLR_SC_BIT));
	//it is safe by using polling to adjust when to clear IRQ_CLR_BIT
	SETREG32(ACCDET_IRQ_STS, (IRQ_CLR_BIT));
}


///*MT6577 E1*:resolve the hardware issue:
//fast plug out headset after plug in with hook switch pressed, 
//accdet detect hook switch instead of plug_out state.
static inline void double_check_workaround(void)
{
    int mem_in;
       
    mem_in = INREG32(ACCDET_MEMORIZED_IN);
    if(mem_in == 0)
    {
    	OUTREG32(ACCDET_RSV_CON3, ACCDET_DEFVAL_SEL);
		ACCDET_DEBUG("double_check_workaround: ACCDET_RSV_CON3=0x%x \n", INREG32(ACCDET_RSV_CON3));
		
		enable_tv_allwhite_signal(false);
		enable_tv_detect(false);

		OUTREG32(ACCDET_RSV_CON3, 0);

		accdet_status = HOOK_SWITCH;
        cable_type = HEADSET_NO_MIC;
       
    }
    else if(mem_in == 3)
    { 
    	OUTREG32(ACCDET_RSV_CON3, ACCDET_DEFVAL_SEL | 0x55);
		ACCDET_DEBUG("double_check_workaround: ACCDET_RSV_CON3=0x%x \n", INREG32(ACCDET_RSV_CON3));
		
		enable_tv_allwhite_signal(false);
		enable_tv_detect(false);
		
		OUTREG32(ACCDET_RSV_CON3, 0x55);

		accdet_status = MIC_BIAS;	  
		cable_type = HEADSET_MIC;

 		OUTREG32(ACCDET_DEBOUNCE0, cust_headset_settings.debounce0);
    }

    //reduce the detect time of press remote button when work around for
    //tv-out slowly plug in issue, so not resume debounce0 time at this time.
    //OUTREG32(ACCDET_DEBOUNCE0, cust_headset_settings.debounce0);
    OUTREG32(ACCDET_DEBOUNCE3, cust_headset_settings.debounce3);
}

#ifdef SW_WORK_AROUND_ACCDET_REMOTE_BUTTON_ISSUE


#define    ACC_ANSWER_CALL   1
#define    ACC_END_CALL      2
#define    ACC_PLAY_MUSIC    3
#define    ACC_PAUSE_MUSIC   4

static atomic_t send_event_flag = ATOMIC_INIT(0);

static DECLARE_WAIT_QUEUE_HEAD(send_event_wq);


static int accdet_key_event=0;

static int sendKeyEvent(void *unuse)
{
    while(1)
    {
        ACCDET_DEBUG( " accdet:sendKeyEvent wait\n");
        //wait for signal
        wait_event_interruptible(send_event_wq, (atomic_read(&send_event_flag) != 0));

        wake_lock(&accdet_key_lock);    //set the wake lock.
        ACCDET_DEBUG( " accdet:going to send event %d\n", accdet_key_event);
/* 
        Workaround to avoid holding the call when pluging out.
        Added a customized value to in/decrease the delay time.
        The longer delay, the more time key event would take.
*/
#ifdef KEY_EVENT_ISSUE_DELAY
        if(KEY_EVENT_ISSUE_DELAY >= 0)
                msleep(KEY_EVENT_ISSUE_DELAY);
        else
        msleep(500);
#else
        msleep(500);
#endif
        if(PLUG_OUT !=accdet_status)
        {
            //send key event
            if(ACC_ANSWER_CALL == accdet_key_event)
            {
                ACCDET_DEBUG("[Accdet] answer call!\n");
                input_report_key(kpd_accdet_dev, KEY_CALL, 1);
                input_report_key(kpd_accdet_dev, KEY_CALL, 0);
                input_sync(kpd_accdet_dev);
            }
            if(ACC_END_CALL == accdet_key_event)
            {
                ACCDET_DEBUG("[Accdet] end call!\n");
                input_report_key(kpd_accdet_dev, KEY_ENDCALL, 1);
                input_report_key(kpd_accdet_dev, KEY_ENDCALL, 0);
                input_sync(kpd_accdet_dev);
            }
            if(0 != accdet_key_event)
            {
                ACCDET_DEBUG("[Accdet] send key %d!\n", accdet_key_event);
                input_report_key(kpd_accdet_dev, accdet_key_event, 1);
                input_report_key(kpd_accdet_dev, accdet_key_event, 0);
                input_sync(kpd_accdet_dev);
            }
        }

        atomic_set(&send_event_flag, 0);
        wake_unlock(&accdet_key_lock); //unlock wake lock
    }
    return 0;
}

static ssize_t notify_sendKeyEvent(int event)
{
	 
    accdet_key_event = event;
    atomic_set(&send_event_flag, 1);
    wake_up(&send_event_wq);
    ACCDET_DEBUG( " accdet:notify_sendKeyEvent !\n");
    return 0;
}


#endif

//ACCDET state machine switch
static inline void check_cable_type(void)
{
    int current_status = 0;
    
    current_status = INREG32(ACCDET_MEMORIZED_IN) & 0x3; //A=bit1; B=bit0
    ACCDET_DEBUG("[Accdet]accdet interrupt happen:[%s]current AB = %d\n", 
		accdet_status_string[accdet_status], current_status);
	    	
    button_status = 0;
    pre_status = accdet_status;
    if(accdet_status == PLUG_OUT)
        double_check_flag = 0;

    //ACCDET_DEBUG("[Accdet]check_cable_type: clock = 0x%x\n", INREG32(PERI_GLOBALCON_PDN0));
	//ACCDET_DEBUG("[Accdet]check_cable_type: PLL clock = 0x%x\n", INREG32(0xF0007020));
   // clear_accdet_interrupt();
    ACCDET_DEBUG("[Accdet]check_cable_type: ACCDET_IRQ_STS = 0x%x\n", INREG32(ACCDET_IRQ_STS));
    IRQ_CLR_FLAG = FALSE;
    switch(accdet_status)
    {
        case PLUG_OUT:
            if(current_status == 0)
            {
                #ifdef TV_OUT_SUPPORT	
				if(TVOUT_IsUserEnabled())
				{
	        	        accdet_status = DOUBLE_CHECK;	
				cable_type = DOUBLE_CHECK_TV;
				enable_tv_allwhite_signal(true);
				enable_tv_detect(true);
				}
				else
				{
		      		cable_type = HEADSET_NO_MIC;
		      		accdet_status = HOOK_SWITCH;
				}
                
                #else
				cable_type = HEADSET_NO_MIC;
				accdet_status = HOOK_SWITCH;
                #endif	
            }
            else if(current_status == 1)
            {
	         	accdet_status = MIC_BIAS;		
	         	cable_type = HEADSET_MIC;

				//ALPS00038030:reduce the time of remote button pressed during incoming call
                //solution: reduce hook switch debounce time to 0x400
                OUTREG32(ACCDET_DEBOUNCE0, button_press_debounce);
            }
            else if(current_status == 3)
            {
                ACCDET_DEBUG("[Accdet]PLUG_OUT state not change!\n");
				accdet_status = PLUG_OUT;		
	            cable_type = NO_DEVICE;
				#ifdef ACCDET_EINT
		        disable_accdet();
		        #endif
            }
            else
            {
                ACCDET_DEBUG("[Accdet]PLUG_OUT can't change to this state!\n"); 
            }
            break;
            
        case MIC_BIAS:
	    //ALPS00038030:reduce the time of remote button pressed during incoming call
            //solution: resume hook switch debounce time
            OUTREG32(ACCDET_DEBOUNCE0, cust_headset_settings.debounce0);
			
            if(current_status == 0)
            {
                ACCDET_DEBUG("[Accdet]remote button pressed,call_status:%d\n", call_status);
                while(INREG32(ACCDET_IRQ_STS) & IRQ_STATUS_BIT)
	        {
		          ACCDET_DEBUG("[Accdet]check_cable_type: MIC BIAS clear IRQ on-going1....\n");	
				  msleep(10);
	        }
				CLRREG32(ACCDET_IRQ_STS, IRQ_CLR_BIT);
                IRQ_CLR_FLAG = TRUE;
                
		//The work around method is disable, because big noise happens when plug in iPhone headset
		//and then press power key or click touch
		#ifdef TVOUT_PLUGIN_SLOWLY_WORKAROUND
		//ALPS00038885:detect hook switch state when slowly plug in tv-out cable
		//solution: when mic bias status with AB=0 interrupt, force accdet into double check
		//to cofirm tv-out or hook switch once more.
		if((double_check_flag == 0) && TVOUT_IsUserEnabled())
		{
		     accdet_status = DOUBLE_CHECK;	
		     cable_type = DOUBLE_CHECK_TV; 
		     enable_tv_allwhite_signal(true);
		     enable_tv_detect(true);
		     double_check_flag = 1;
		}
		else
		{
		     accdet_status = HOOK_SWITCH;
		     button_status = 1;
		}	
		#else
		accdet_status = HOOK_SWITCH;
		button_status = 1;
		#endif

		if((call_status != 0) && button_status)
		{	
		#ifdef SW_WORK_AROUND_ACCDET_REMOTE_BUTTON_ISSUE
	        if(is_long_press())
	     	{
	        	ACCDET_DEBUG("[Accdet]send long press remote button event %d \n",ACC_END_CALL);
			    //input_report_key(kpd_accdet_dev, KEY_ENDCALL, 1);
			    //input_report_key(kpd_accdet_dev, KEY_ENDCALL, 0);
			    notify_sendKeyEvent(ACC_END_CALL);
  		    }
            else
	   	    {
		        ACCDET_DEBUG("[Accdet]send short press remote button event %d\n",ACC_ANSWER_CALL);
			    //input_report_key(kpd_accdet_dev, KEY_CALL, 1);
			   // input_report_key(kpd_accdet_dev, KEY_CALL, 0);
			   notify_sendKeyEvent(ACC_ANSWER_CALL);
	  	    }
			#else
			 if(is_long_press())
	     	{
	        	ACCDET_DEBUG("[Accdet]long press remote button to end call!\n");
			    input_report_key(kpd_accdet_dev, KEY_ENDCALL, 1);
			    input_report_key(kpd_accdet_dev, KEY_ENDCALL, 0);
                input_sync(kpd_accdet_dev);
  		    }
            else
	   	    {
		        ACCDET_DEBUG("[Accdet]short press remote button to accept call!\n");
			    input_report_key(kpd_accdet_dev, KEY_CALL, 1);
			    input_report_key(kpd_accdet_dev, KEY_CALL, 0);
                input_sync(kpd_accdet_dev);
	  	    }
			
			#endif
		}
                // Tommy added. Send key events while not in-call.
                else if((call_status == 0) && button_status) 
                {
                    if(is_long_press())
                    {
                        ACCDET_DEBUG("[Accdet]send long press remote button event %d \n",KEY_NEXTSONG);
                        notify_sendKeyEvent(KEY_NEXTSONG);
                    } else {
                        ACCDET_DEBUG("[Accdet]send short press remote button event %d\n",KEY_MEDIA);
                        notify_sendKeyEvent(KEY_MEDIA);
                    }
                }

            }
            else if(current_status == 1)
            {
                accdet_status = MIC_BIAS;		
	            cable_type = HEADSET_MIC;
                ACCDET_DEBUG("[Accdet]MIC_BIAS state not change!\n");
            }
            else if(current_status == 3)
            {
		       accdet_status = PLUG_OUT;		
	           cable_type = NO_DEVICE;
			   #ifdef ACCDET_EINT
		       disable_accdet();
		       #endif
            }
            else
            {
               ACCDET_DEBUG("[Accdet]MIC_BIAS can't change to this state!\n"); 
            }
            break;

        case DOUBLE_CHECK:
            if(current_status == 0)
            { 
                ///*MT6577 E1*: resolve the hardware issue:
                double_check_workaround();
            }
            else if(current_status == 2)
            {
                //ALPS00053818:when plug in iphone headset half, accdet detects tv-out plug in,
		//and then plug in headset completely, finally plug out it and can't detect plug out
		//Reason: tv signal(32mA) is abnormal by plug in tv cable in tv-out state.
		//Solution:increase debounce2(tv debounce) to a long delay to prevent accdet
		//stay in tv-out state again because of tv signal(32 mA) isn't stable.
		OUTREG32(ACCDET_DEBOUNCE2, 0xffff);

	        accdet_status = TV_OUT;
                cable_type = HEADSET_NO_MIC;
                
	        OUTREG32(ACCDET_CTRL, ACCDET_DISABLE);
	        enable_tv_allwhite_signal(false);
                enable_tv(true);

		if(double_check_flag == 1)			   	
		{					
		      tv_headset_icon = true;			   	
		}
	
            }
            else if(current_status == 3)
            {
	    	///*MT6577 E1*: resolve the hardware issue:
	    	//fast plug out headset after plug in with hook switch pressed, 
                //accdet detect mic bias instead of plug_out state.
                double_check_workaround();
            }
            else
            {
                ACCDET_DEBUG("[Accdet]DOUBLE_CHECK can't change to this state!\n"); 
            }
            break;

        case HOOK_SWITCH:
            if(current_status == 0)
            {
                cable_type = HEADSET_NO_MIC;
		        accdet_status = HOOK_SWITCH;
                ACCDET_DEBUG("[Accdet]HOOK_SWITCH state not change!\n");
	    }
            else if(current_status == 1)
            {
	        accdet_status = MIC_BIAS;		
	        cable_type = HEADSET_MIC;

		//ALPS00038030:reduce the time of remote button pressed during incoming call
                //solution: reduce hook switch debounce time to 0x400
                OUTREG32(ACCDET_DEBOUNCE0, button_press_debounce);
            }
            else if(current_status == 3)
            {
		       accdet_status = PLUG_OUT;		
		       cable_type = NO_DEVICE;
			   #ifdef ACCDET_EINT
		       disable_accdet();
		       #endif
            }
            else
            {
                ACCDET_DEBUG("[Accdet]HOOK_SWITCH can't change to this state!\n"); 
            }
            break;

        case TV_OUT:	
  	    OUTREG32(ACCDET_DEBOUNCE0, cust_headset_settings.debounce0);
	    OUTREG32(ACCDET_DEBOUNCE3, cust_headset_settings.debounce3);
	    //ALPS00053818: resume debounce2 when jump out from tv-out state.
	    OUTREG32(ACCDET_DEBOUNCE2, tv_out_debounce);
            
            if(current_status == 0)
            {
		OUTREG32(ACCDET_RSV_CON3, ACCDET_DEFVAL_SEL);
		ACCDET_DEBUG("[Accdet]TV-out state: ACCDET_RSV_CON3=0x%x \n\r", INREG32(ACCDET_RSV_CON3));
		
                if(Is_TvoutEnabled())
            	{
                	//*MT6573 E1&E2*:work around method	 for plug in headset  uncompletely with remote button pressed
                	//solution:when plug in headset completely, tv_out->hook_switch because of AB=00
                	accdet_status = HOOK_SWITCH;
					cable_type = HEADSET_NO_MIC;	
	            	ACCDET_DEBUG("[Accdet]headset plug in with remote button completely now!\n");
            	}
		else
		{
                	//work around method	      
			accdet_status = STAND_BY;
			ACCDET_DEBUG("[Accdet]TV out is disabled with tv cable plug in!\n");
		}

		enable_tv(false);
                enable_tv_detect(false);

		OUTREG32(ACCDET_RSV_CON3, 0);
           }
            else if(current_status == 3)
            {
	    		accdet_status = PLUG_OUT;		
				cable_type = NO_DEVICE;
                enable_tv(false);
	        	enable_tv_detect(false);
				#ifdef ACCDET_EINT
			    disable_accdet();//should  disable accdet here because enable_tv(false) will enalbe accdet
			    #endif
           }
		   else if(current_status == 2)
		   {
		        //ALSP00092273
		        OUTREG32(ACCDET_DEBOUNCE0, 0x4);
		        OUTREG32(ACCDET_DEBOUNCE3, 0x4);
		        
		   }
            else
            {
                ACCDET_DEBUG("[Accdet]TV_OUT can't change to this state!\n"); 
            }
            break;

        case STAND_BY:
            if(current_status == 3)
            {
				accdet_status = PLUG_OUT;		
				cable_type = NO_DEVICE;
				#ifdef ACCDET_EINT
			    disable_accdet();
			    #endif
            }
            else
            {
                ACCDET_DEBUG("[Accdet]STAND_BY can't change to this state!\n"); 
            }
            break;

        default:
            ACCDET_DEBUG("[Accdet]check_cable_type: accdet current status error!\n");
            break;
            
    }

    if(!IRQ_CLR_FLAG)
    {
        while(INREG32(ACCDET_IRQ_STS) & IRQ_STATUS_BIT)
	{
           ACCDET_DEBUG("[Accdet]check_cable_type: Clear interrupt on-going2....\n");
		   msleep(10);
	}
		CLRREG32(ACCDET_IRQ_STS, IRQ_CLR_BIT);
		IRQ_CLR_FLAG = TRUE;
        ACCDET_DEBUG("[Accdet]check_cable_type:Clear interrupt:Done[0x%x]!\n", INREG32(ACCDET_IRQ_STS));	
    }
    else
    {
        IRQ_CLR_FLAG = FALSE;
    }
 
    if(accdet_status == TV_OUT)
    {
	OUTREG32(ACCDET_CTRL, ACCDET_ENABLE);
    }

    ACCDET_DEBUG("[Accdet]cable type:[%s], status switch:[%s]->[%s]\n",
        accdet_report_string[cable_type], accdet_status_string[pre_status], 
        accdet_status_string[accdet_status]);
}

#ifdef SW_WORK_AROUND_ACCDET_DEBOUNCE_HANG
void accdet_timer_fn(unsigned long arg) 
{
   ACCDET_DEBUG(" accdet_timer_fn \n");
   queue_work(accdet_check_workqueue, &accdet_check_work); 
   ACCDET_DEBUG(" accdet_timer_fn ok\n");
}

static int check_FSM(void *unused)
{
  int i=0;
  struct timespec time; 
 // struct timespec now_time; 
  int calc_time_flag=0;
  int64_t  fsm_nt;
  int64_t  now_nt;

  
  //ACCDET_DEBUG(" check_FSM +++\n");
  //calc FSM time
  for(i=0; i<200; i++)//check 2000 ms
  {
    if(0 != INREG32(ACCDET_BASE + 0x0050))
    {
		//begain to calc time
		if(0 == calc_time_flag)
		{
		  time.tv_sec = time.tv_nsec = 0;    
	      time = get_monotonic_coarse(); 
	      fsm_nt = time.tv_sec*1000000000LL+time.tv_nsec;
		  calc_time_flag =1;
		}
    }
	
	if(0 == INREG32(ACCDET_BASE + 0x0050))
    {
		//clear calc time
		calc_time_flag =0;
		time.tv_sec = time.tv_nsec = 0;    
	    time = get_monotonic_coarse(); 
	    fsm_nt = time.tv_sec*1000000000LL+time.tv_nsec;
    }
	msleep(10);
  }
  
  time.tv_sec = time.tv_nsec = 0;    
  time = get_monotonic_coarse(); 
  now_nt = time.tv_sec*1000000000LL+time.tv_nsec;

 // ACCDET_DEBUG(" now_nt -fsm_nt = %lld ns \n",now_nt-fsm_nt);
  
  // FSM=2 time > 750 then clear headset icon and reset accdet
  if(((now_nt-fsm_nt)/1000000LL)>750 && 2 != (INREG32(ACCDET_MEMORIZED_IN)&0x3))
  {
     ACCDET_DEBUG(" accdet hw issue !!!!!!!!!!!!!!!!!\n");
	 ACCDET_DEBUG(" now_nt -fsm_nt = %lld ns \n",now_nt-fsm_nt);
     //clear headset icon
     cable_type = NO_DEVICE;
     switch_set_state((struct switch_dev *)&accdet_data, cable_type);
     
     //reset the accdet unit
     OUTREG32(ACCDET_RSTB, RSTB_BIT); 
     OUTREG32(ACCDET_RSTB, RSTB_FINISH_BIT); 
  }

  mod_timer(&check_timer, jiffies + 5000/(1000/HZ));
 // ACCDET_DEBUG(" check_FSM ---\n");
}

static ssize_t start_check_FSM_thread(void)
{
	 struct task_struct *fsm_thread = NULL;
	 int error=0;
	 fsm_thread = kthread_run(check_FSM, 0, "check_FSM");
     if (IS_ERR(fsm_thread)) 
	 { 
        error = PTR_ERR(fsm_thread);
        ACCDET_DEBUG( " failed to create kernel thread: %d\n", error);
     }
}

#endif

#ifdef SW_WORK_AROUND_ACCDET_DEBOUNCE_HANG

void accdet_check_work_callback(struct work_struct *work)
{
   check_FSM(0);
}
#endif


// judge cable type and implement the most job
void accdet_work_callback(struct work_struct *work)
{
#ifdef SW_WORK_AROUND_ACCDET_DEBOUNCE_HANG
	start_check_FSM_thread();
#endif

    wake_lock(&accdet_irq_lock);
    check_cable_type();
    if(cable_type != DOUBLE_CHECK_TV)
    {
	   switch_set_state((struct switch_dev *)&accdet_data, cable_type);
	   ACCDET_DEBUG( " [accdet] set state in cable_type != DOUBLE_CHECK_TV status\n");
    }

    //ALPS00038885:detect hook switch state when slowly plug in tv-out cable
    //when half plug in tv cable, headset with mic icon shows, so it always on even if plug in completely
    //work around method: when detect plug in tv-out, change icon to headset without mic.
    if(tv_headset_icon)
    {
        switch_set_state((struct switch_dev *)&accdet_data, NO_DEVICE);
        msleep(10);
        switch_set_state((struct switch_dev *)&accdet_data, cable_type);
	tv_headset_icon = false;
		ACCDET_DEBUG( " [accdet] set state in tv_headset_icon status\n");
    }

	//enable_irq(MT6577_ACCDET_IRQ_ID);
    wake_unlock(&accdet_irq_lock);
}


static irqreturn_t accdet_irq_handler(int irq,void *dev_id)
{
    int ret = 0 ;
    
    //ACCDET_DEBUG("[Accdet]accdet interrupt happen\n"); //decrease top-half ISR cost time
    //disable_irq_nosync(MT6577_ACCDET_IRQ_ID);
    clear_accdet_interrupt();
    ret = queue_work(accdet_workqueue, &accdet_work);	
    if(!ret)
    {
  		ACCDET_DEBUG("[Accdet]accdet_irq_handler:accdet_work return:%d!\n", ret);  		
    }
	
    
    return IRQ_HANDLED;
}

//ACCDET hardware initial
static inline void accdet_init(void)
{ 
    ACCDET_DEBUG("[Accdet]accdet hardware init\n");
    //kal_uint8 val;

    //resolve MT6573 ACCDET hardware issue: ear bias supply can make Vref drop obviously 
    //during plug in/out headset then cause modem exception or kernel panic
    //solution: set bit3 of PMIC_RESERVE_CON2 to force MIC voltage to 0 when MIC is drop to negative voltage
    //SETREG32(PMIC_RESERVE_CON2, MIC_FORCE_LOW);

    enable_clock(MT65XX_PDN_PERI_ACCDET,"ACCDET");
	
    //reset the accdet unit
	OUTREG32(ACCDET_RSTB, RSTB_BIT); 
	OUTREG32(ACCDET_RSTB, RSTB_FINISH_BIT); 

    //init  pwm frequency and duty
    OUTREG32(ACCDET_PWM_WIDTH, REGISTER_VALUE(cust_headset_settings.pwm_width));
    OUTREG32(ACCDET_PWM_THRESH, REGISTER_VALUE(cust_headset_settings.pwm_thresh));

    OUTREG32(ACCDET_EN_DELAY_NUM,
		(cust_headset_settings.fall_delay << 15 | cust_headset_settings.rise_delay));

    // init the debounce time
    OUTREG32(ACCDET_DEBOUNCE0, cust_headset_settings.debounce0);
    OUTREG32(ACCDET_DEBOUNCE1, cust_headset_settings.debounce1);
    OUTREG32(ACCDET_DEBOUNCE3, cust_headset_settings.debounce3);	
	
		
    //init TV relative register settings, default setting is NTSC standard
    switch_NTSC_to_PAL(0);
    
    #ifdef ACCDET_EINT
    // disable ACCDET unit
	pre_state_swctrl = INREG32(ACCDET_STATE_SWCTRL);
    OUTREG32(ACCDET_CTRL, ACCDET_DISABLE);
    OUTREG32(ACCDET_STATE_SWCTRL, 0);
	#else
	
    // enable ACCDET unit
    OUTREG32(ACCDET_STATE_SWCTRL, ACCDET_SWCTRL_EN);
    OUTREG32(ACCDET_CTRL, ACCDET_ENABLE); 
	#endif
    
	
	if(get_chip_eco_ver()==CHIP_E1)
    {
    //************analog switch*******************/
    //**MT6577 EVB**: 1, MIC channel; 0, TV channel (default)
    //**MT6577 Phone**: 1, TV channel; 0, MIC channel(default)
    #ifndef MT6577_EVB_MIC_TV_CHA
    hwSPKClassABAnalogSwitchSelect(ACCDET_MIC_CHA);
    #else
    hwSPKClassABAnalogSwitchSelect(0x1);
    #endif
    //pmic_bank1_read_interface(0x5F, &val, 1, 0);
    ACCDET_DEBUG("[Accdet]accdet_init : eco version E1\n");
    }
	else if(get_chip_eco_ver()==CHIP_E2)
	{
	  hwSPKClassABAnalogSwitchSelect(ACCDET_MIC_CHA);
	  ACCDET_DEBUG("[Accdet]accdet_init : eco version E2\n");
	}
	else
	{
	  hwSPKClassABAnalogSwitchSelect(ACCDET_MIC_CHA);
	  ACCDET_DEBUG("[Accdet]accdet_init : unknown eco version\n");  
	}
	

}

static long accdet_unlocked_ioctl(struct file *file, unsigned int cmd,unsigned long arg)
{
//	bool ret = true;
		
    switch(cmd)
    {
        case ACCDET_INIT :

/* the old ACCDE driver legacy */
#if 0
 		    ACCDET_DEBUG("[Accdet]accdet_ioctl : ACCDET_INIT\n");  
			if (g_accdet_first == 1) 
			{	
				long_press_time_ns = (s64)long_press_time * NSEC_PER_MSEC;
				
				//Accdet Hardware Init
				accdet_init();
                
				//mt6577_irq_set_sens(MT6577_ACCDET_IRQ_ID, MT65xx_EDGE_SENSITIVE);
				mt6577_irq_set_sens(MT6577_ACCDET_IRQ_ID, MT65xx_LEVEL_SENSITIVE);
				mt6577_irq_set_polarity(MT6577_ACCDET_IRQ_ID, MT65xx_POLARITY_LOW);
				//register accdet interrupt
				ret =  request_irq(MT6577_ACCDET_IRQ_ID, accdet_irq_handler, 0, "ACCDET", NULL);
				if(ret)
				{
					ACCDET_DEBUG("[Accdet]accdet register interrupt error\n");
				}
                
				queue_work(accdet_workqueue, &accdet_work); //schedule a work for the first detection					
				g_accdet_first = 0;
			}
#endif
     		break;
            
		case SET_CALL_STATE :
			call_status = (int)arg;
			ACCDET_DEBUG("[Accdet]accdet_ioctl : CALL_STATE=%d \n", call_status);
			/*
            if(call_status != 0)
            {
                wake_lock(&accdet_suspend_lock); 
                ACCDET_DEBUG("[Accdet]accdet_ioctl : CALL_STATE=%d,require wake lock\n", call_status);
            }
            else
            {
                wake_unlock(&accdet_suspend_lock);
                ACCDET_DEBUG("[Accdet]accdet_ioctl : CALL_STATE=%d,release wake lock\n", call_status);
            }
			*/
			
			break;

		case GET_BUTTON_STATUS :
			ACCDET_DEBUG("[Accdet]accdet_ioctl : Button_Status=%d (state:%d)\n", button_status, accdet_data.state);	
			return button_status;
            
		default:
   		    ACCDET_DEBUG("[Accdet]accdet_ioctl : default\n");
            break;
  }
    return 0;
}

static int accdet_open(struct inode *inode, struct file *file)
{ 
   	return 0;
}

static int accdet_release(struct inode *inode, struct file *file)
{
    return 0;
}
static struct file_operations accdet_fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl		= accdet_unlocked_ioctl,
	.open		= accdet_open,
	.release	= accdet_release,	
};

#if DEBUG_THREAD
int g_start_debug_thread =0;
static struct task_struct *thread = NULL;
int g_dump_register=0;

static int dump_register(void)
{

   int i=0;
   for (i=0; i<= 120; i+=4)
   {
     ACCDET_DEBUG(" ACCDET_BASE + %x=%x\n",i,INREG32(ACCDET_BASE + i));
   }

 OUTREG32(0xf0007420,0x0111);
 // wirte c0007424 x0d01
 OUTREG32(0xf0007424,0x0d01);
 
   ACCDET_DEBUG(" 0xc1016d0c =%x\n",INREG32(0xf1016d0c));
   ACCDET_DEBUG(" 0xc1016d10 =%x\n",INREG32(0xf1016d10));
   ACCDET_DEBUG(" 0xc209e070 =%x\n",INREG32(0xf209e070));
   ACCDET_DEBUG(" 0xc0007160 =%x\n",INREG32(0xf0007160));
   ACCDET_DEBUG(" 0xc00071a8 =%x\n",INREG32(0xf00071a8));
   ACCDET_DEBUG(" 0xc0007440 =%x\n",INREG32(0xf0007440));

   return 0;
}


extern int mt_i2c_polling_writeread(int port, unsigned char addr, unsigned char *buffer, int write_len, int read_len);

static int dump_pmic_register(void)
{ 
	int ret = 0;
	unsigned char data[2];
	
	//u8 data = 0x5f;
	data[0] = 0x5f;
	ret = mt_i2c_polling_writeread(2,0xc2,data,1,1);
	if(ret > 0)
	{
       ACCDET_DEBUG("dump_pmic_register i2c error");
	}
    ACCDET_DEBUG("dump_pmic_register 0x5f= %x",data[0]);

    data[0] = 0xc8;
	ret = mt_i2c_polling_writeread(2,0xc0,data,1,1);
	if(ret > 0)
	{
       ACCDET_DEBUG("dump_pmic_register i2c error");
	}
    ACCDET_DEBUG("dump_pmic_register 0xc8= %x",data[0]);

	return 0;  
}
static int dbug_thread(void *unused) 
{
   while(g_start_debug_thread)
   	{
      ACCDET_DEBUG("dbug_thread INREG32(ACCDET_BASE + 0x0008)=%x\n",INREG32(ACCDET_BASE + 0x0008));
	  ACCDET_DEBUG("[Accdet]dbug_thread:sample_in:%x!\n", INREG32(ACCDET_SAMPLE_IN));	
	  ACCDET_DEBUG("[Accdet]dbug_thread:curr_in:%x!\n", INREG32(ACCDET_CURR_IN));
	  ACCDET_DEBUG("[Accdet]dbug_thread:mem_in:%x!\n", INREG32(ACCDET_MEMORIZED_IN));
	  ACCDET_DEBUG("[Accdet]dbug_thread:FSM:%x!\n", INREG32(ACCDET_BASE + 0x0050));
      ACCDET_DEBUG("[Accdet]dbug_thread:IRQ:%x!\n", INREG32(ACCDET_IRQ_STS));
      if(g_dump_register)
	  {
	    dump_register();
		dump_pmic_register(); 
      }

	  msleep(500);

   	}
   return 0;
}
//static ssize_t store_trace_value(struct device_driver *ddri, const char *buf, size_t count)


static ssize_t store_accdet_start_debug_thread(struct device_driver *ddri, const char *buf, size_t count)
{
	
	unsigned int start_flag;
	int error;

	if (sscanf(buf, "%u", &start_flag) != 1) {
		ACCDET_DEBUG("accdet: Invalid values\n");
		return -EINVAL;
	}

	ACCDET_DEBUG("[Accdet] start flag =%d \n",start_flag);

	g_start_debug_thread = start_flag;

    if(1 == start_flag)
    {
	   thread = kthread_run(dbug_thread, 0, "ACCDET");
       if (IS_ERR(thread)) 
	   { 
          error = PTR_ERR(thread);
          ACCDET_DEBUG( " failed to create kernel thread: %d\n", error);
       }
    }

	return count;
}

static ssize_t store_accdet_set_headset_mode(struct device_driver *ddri, const char *buf, size_t count)
{

    unsigned int value;
	//int error;

	if (sscanf(buf, "%u", &value) != 1) {
		ACCDET_DEBUG("accdet: Invalid values\n");
		return -EINVAL;
	}

	ACCDET_DEBUG("[Accdet]store_accdet_set_headset_mode value =%d \n",value);

	return count;
}

static ssize_t store_accdet_dump_register(struct device_driver *ddri, const char *buf, size_t count)
{
    unsigned int value;
//	int error;

	if (sscanf(buf, "%u", &value) != 1) 
	{
		ACCDET_DEBUG("accdet: Invalid values\n");
		return -EINVAL;
	}

	g_dump_register = value;

	ACCDET_DEBUG("[Accdet]store_accdet_dump_register value =%d \n",value);

	return count;
}




/*----------------------------------------------------------------------------*/
static DRIVER_ATTR(dump_register,      S_IWUSR | S_IRUGO, NULL,         store_accdet_dump_register);

static DRIVER_ATTR(set_headset_mode,      S_IWUSR | S_IRUGO, NULL,         store_accdet_set_headset_mode);

static DRIVER_ATTR(start_debug,      S_IWUSR | S_IRUGO, NULL,         store_accdet_start_debug_thread);

/*----------------------------------------------------------------------------*/
static struct driver_attribute *accdet_attr_list[] = {
	&driver_attr_start_debug,        
	&driver_attr_set_headset_mode,
	&driver_attr_dump_register,
};

static int accdet_create_attr(struct device_driver *driver) 
{
	int idx, err = 0;
	int num = (int)(sizeof(accdet_attr_list)/sizeof(accdet_attr_list[0]));
	if (driver == NULL)
	{
		return -EINVAL;
	}

	for(idx = 0; idx < num; idx++)
	{
		if((err = driver_create_file(driver, accdet_attr_list[idx])))
		{            
			ACCDET_DEBUG("driver_create_file (%s) = %d\n", accdet_attr_list[idx]->attr.name, err);
			break;
		}
	}    
	return err;
}
/*----------------------------------------------------------------------------*/
#if 0
static int accdet_delete_attr(struct device_driver *driver)
{
	int idx ,err = 0;
	int num = (int)(sizeof(accdet_attr_list)/sizeof(accdet_attr_list[0]));

	if(driver == NULL)
	{
		return -EINVAL;
	}
	

	for(idx = 0; idx < num; idx++)
	{
		driver_remove_file(driver, accdet_attr_list[idx]);
	}
	

	return err;
}

#endif

#endif

#if 0
void  temp_func()
{
  //unsigned int start_flag;
	int error;

	

	ACCDET_DEBUG("[Accdet] start flag =%d \n",start_flag);

	

    if(1)
    {
	   thread = kthread_run(dbug_thread, 0, "ACCDET");
       if (IS_ERR(thread)) 
	   { 
          error = PTR_ERR(thread);
          ACCDET_DEBUG( " failed to create kernel thread: %d\n", error);
       }
    }
}

#endif
static int accdet_probe(struct platform_device *dev)	
{
	int ret = 0;
#ifdef SW_WORK_AROUND_ACCDET_REMOTE_BUTTON_ISSUE
     struct task_struct *keyEvent_thread = NULL;
	 int error=0;
#endif
	ACCDET_DEBUG("[Accdet]accdet_probe begin!\n");

	//------------------------------------------------------------------
	// 							below register accdet as switch class
	//------------------------------------------------------------------	
	accdet_data.name = "h2w";
	accdet_data.index = 0;
	accdet_data.state = NO_DEVICE;
			
	ret = switch_dev_register(&accdet_data);
	if(ret)
	{
		ACCDET_DEBUG("[Accdet]switch_dev_register returned:%d!\n", ret);
		return 1;
	}
		
	//------------------------------------------------------------------
	// 							Create normal device for auido use
	//------------------------------------------------------------------
	ret = alloc_chrdev_region(&accdet_devno, 0, 1, ACCDET_DEVNAME);
	if (ret)
	{
		ACCDET_DEBUG("[Accdet]alloc_chrdev_region: Get Major number error!\n");			
	} 
		
	accdet_cdev = cdev_alloc();
    accdet_cdev->owner = THIS_MODULE;
    accdet_cdev->ops = &accdet_fops;
    ret = cdev_add(accdet_cdev, accdet_devno, 1);
	if(ret)
	{
		ACCDET_DEBUG("[Accdet]accdet error: cdev_add\n");
	}
	
	accdet_class = class_create(THIS_MODULE, ACCDET_DEVNAME);

    // if we want auto creat device node, we must call this
	accdet_nor_device = device_create(accdet_class, NULL, accdet_devno, NULL, ACCDET_DEVNAME);  
	
	//------------------------------------------------------------------
	// 							Create input device 
	//------------------------------------------------------------------
	kpd_accdet_dev = input_allocate_device();
	if (!kpd_accdet_dev) 
	{
		ACCDET_DEBUG("[Accdet]kpd_accdet_dev : fail!\n");
		return -ENOMEM;
	}
	__set_bit(EV_KEY, kpd_accdet_dev->evbit);
	__set_bit(KEY_CALL, kpd_accdet_dev->keybit);
	__set_bit(KEY_ENDCALL, kpd_accdet_dev->keybit);
	__set_bit(KEY_MEDIA, kpd_accdet_dev->keybit);
	__set_bit(KEY_NEXTSONG, kpd_accdet_dev->keybit);
    
	kpd_accdet_dev->id.bustype = BUS_HOST;
	kpd_accdet_dev->name = "ACCDET";
	if(input_register_device(kpd_accdet_dev))
	{
		ACCDET_DEBUG("[Accdet]kpd_accdet_dev register : fail!\n");
	}else
	{
		ACCDET_DEBUG("[Accdet]kpd_accdet_dev register : success!!\n");
	} 
	//------------------------------------------------------------------
	// 							Create workqueue 
	//------------------------------------------------------------------	
	accdet_workqueue = create_singlethread_workqueue("accdet");
	INIT_WORK(&accdet_work, accdet_work_callback);

	#ifdef ACCDET_EINT

    accdet_eint_workqueue = create_singlethread_workqueue("accdet_eint");
	INIT_WORK(&accdet_eint_work, accdet_eint_work_callback);
	accdet_setup_eint();
    #endif	

#ifdef SW_WORK_AROUND_ACCDET_DEBOUNCE_HANG
	init_timer(&check_timer);
	check_timer.expires	= jiffies + 5000/(1000/HZ);
	check_timer.function = accdet_timer_fn;

    accdet_check_workqueue = create_singlethread_workqueue("accdet_check");
	INIT_WORK(&accdet_check_work, accdet_check_work_callback);

#endif
    //------------------------------------------------------------------
	//							wake lock
	//------------------------------------------------------------------
	wake_lock_init(&accdet_suspend_lock, WAKE_LOCK_SUSPEND, "accdet wakelock");
    wake_lock_init(&accdet_irq_lock, WAKE_LOCK_SUSPEND, "accdet irq wakelock");
    wake_lock_init(&accdet_key_lock, WAKE_LOCK_SUSPEND, "accdet key wakelock");
#ifdef SW_WORK_AROUND_ACCDET_REMOTE_BUTTON_ISSUE
     init_waitqueue_head(&send_event_wq);
     //start send key event thread
	 keyEvent_thread = kthread_run(sendKeyEvent, 0, "keyEvent_send");
     if (IS_ERR(keyEvent_thread)) 
	 { 
        error = PTR_ERR(keyEvent_thread);
        ACCDET_DEBUG( " failed to create kernel thread: %d\n", error);
     }
#endif
	
	#if DEBUG_THREAD

	if((ret = accdet_create_attr(&accdet_driver.driver)))
	{
		ACCDET_DEBUG("create attribute err = %d\n", ret);
		
	}

	#endif

	/* For early porting before audio driver add */
	//temp_func();
	ACCDET_DEBUG("[Accdet]accdet_probe : ACCDET_INIT\n");  
	if (g_accdet_first == 1) 
	{	
		long_press_time_ns = (s64)long_press_time * NSEC_PER_MSEC;
				
		//Accdet Hardware Init
		accdet_init();
                
		//mt6577_irq_set_sens(MT6577_ACCDET_IRQ_ID, MT65xx_EDGE_SENSITIVE);
		mt6577_irq_set_sens(MT6577_ACCDET_IRQ_ID, MT65xx_LEVEL_SENSITIVE);
		mt6577_irq_set_polarity(MT6577_ACCDET_IRQ_ID, MT65xx_POLARITY_LOW);
		//register accdet interrupt
		ret =  request_irq(MT6577_ACCDET_IRQ_ID, accdet_irq_handler, 0, "ACCDET", NULL);
		if(ret)
		{
			ACCDET_DEBUG("[Accdet]accdet register interrupt error\n");
		}
                
		queue_work(accdet_workqueue, &accdet_work); //schedule a work for the first detection					
		g_accdet_first = 0;
	}

        ACCDET_DEBUG("[Accdet]accdet_probe done!\n");
	return 0;
}

static int accdet_remove(struct platform_device *dev)	
{
	ACCDET_DEBUG("[Accdet]accdet_remove begin!\n");
	if(g_accdet_first == 0)
	{
		free_irq(MT6577_ACCDET_IRQ_ID,NULL);
	}
    
	//cancel_delayed_work(&accdet_work);
	#ifdef ACCDET_EINT
	destroy_workqueue(accdet_eint_workqueue);
	#endif
	destroy_workqueue(accdet_workqueue);
	switch_dev_unregister(&accdet_data);
	device_del(accdet_nor_device);
	class_destroy(accdet_class);
	cdev_del(accdet_cdev);
	unregister_chrdev_region(accdet_devno,1);	
	input_unregister_device(kpd_accdet_dev);
	ACCDET_DEBUG("[Accdet]accdet_remove Done!\n");
    
	return 0;
}

static int accdet_suspend(struct platform_device *dev, pm_message_t state)  // only one suspend mode
{
  
    int i=0;
    //ACCDET_DEBUG("[Accdet]accdet_suspend\n");
    //close vbias
    //SETREG32(0xf0007500, (1<<7));
    //before close accdet clock we must clear IRQ done
    while(INREG32(ACCDET_IRQ_STS) & IRQ_STATUS_BIT)
	{
           ACCDET_DEBUG("[Accdet]check_cable_type: Clear interrupt on-going3....\n");
		   msleep(10);
	}
	while(INREG32(ACCDET_IRQ_STS))
	{
	  msleep(10);
	  CLRREG32(ACCDET_IRQ_STS, IRQ_CLR_BIT);
	  IRQ_CLR_FLAG = TRUE;
      ACCDET_DEBUG("[Accdet]check_cable_type:Clear interrupt:Done[0x%x]!\n", INREG32(ACCDET_IRQ_STS));	
	}
    while(i<50 && (INREG32(ACCDET_BASE + 0x0050)!=0x0))
	{
	  // wake lock
	  wake_lock(&accdet_suspend_lock);
	  msleep(10); //wait for accdet finish IRQ generation
	  g_accdet_working_in_suspend =1;
	  ACCDET_DEBUG("suspend wake lock %d\n",i);
	  i++;
	}
	if(1 == g_accdet_working_in_suspend)
	{
	  wake_unlock(&accdet_suspend_lock);
	  g_accdet_working_in_suspend =0;
	  ACCDET_DEBUG("suspend wake unlock\n");
	}
/*
	ACCDET_DEBUG("[Accdet]suspend:sample_in:%x, curr_in:%x, mem_in:%x, FSM:%x\n"
		, INREG32(ACCDET_SAMPLE_IN)
		,INREG32(ACCDET_CURR_IN)
		,INREG32(ACCDET_MEMORIZED_IN)
		,INREG32(ACCDET_BASE + 0x0050));
*/

// Tommy removed. Don't disable ACCDET even not in call status.
/*
#ifdef ACCDET_EINT

    if(INREG32(ACCDET_CTRL)&& call_status == 0)
    {
	   //record accdet status
	   ACCDET_DEBUG("[Accdet]accdet_working_in_suspend\n");
	   g_accdet_working_in_suspend = 1;
       pre_state_swctrl = INREG32(ACCDET_STATE_SWCTRL);
	   // disable ACCDET unit
       OUTREG32(ACCDET_CTRL, ACCDET_DISABLE);
       OUTREG32(ACCDET_STATE_SWCTRL, 0);
    }
#else
    // disable ACCDET unit
    if(call_status == 0)
    {
       pre_state_swctrl = INREG32(ACCDET_STATE_SWCTRL);
       OUTREG32(ACCDET_CTRL, ACCDET_DISABLE);
       OUTREG32(ACCDET_STATE_SWCTRL, 0);
       disable_clock(MT65XX_PDN_PERI_ACCDET,"ACCDET");
    }
#endif	
*/

    ACCDET_DEBUG("[Accdet]accdet_suspend: ACCDET_CTRL=[0x%x], STATE=[0x%x]->[0x%x]\n", INREG32(ACCDET_CTRL), pre_state_swctrl,INREG32(ACCDET_STATE_SWCTRL));
    //ACCDET_DEBUG("[Accdet]accdet_suspend ok\n");

	return 0;
}

static int accdet_resume(struct platform_device *dev) // wake up
{
    //ACCDET_DEBUG("[Accdet]accdet_resume\n");
// Tommy removed. Don't disable ACCDET even not in call status.
/*
#ifdef ACCDET_EINT

	if(1==g_accdet_working_in_suspend &&  0== call_status)
	{
       // enable ACCDET unit	
       OUTREG32(ACCDET_STATE_SWCTRL, pre_state_swctrl);
       OUTREG32(ACCDET_CTRL, ACCDET_ENABLE); 
       //clear g_accdet_working_in_suspend
	   g_accdet_working_in_suspend =0;
	   ACCDET_DEBUG("[Accdet]accdet_resume : recovery accdet register\n");
	   
	}
#else
	if(call_status == 0)
	{
       enable_clock(MT65XX_PDN_PERI_ACCDET,"ACCDET");

       // enable ACCDET unit	
  
       OUTREG32(ACCDET_STATE_SWCTRL, pre_state_swctrl);
       OUTREG32(ACCDET_CTRL, ACCDET_ENABLE); 
	}
#endif
*/

    ACCDET_DEBUG("[Accdet]accdet_resume: ACCDET_CTRL=[0x%x], STATE_SWCTRL=[0x%x]\n", INREG32(ACCDET_CTRL), INREG32(ACCDET_STATE_SWCTRL));
/*
    ACCDET_DEBUG("[Accdet]resum:sample_in:%x, curr_in:%x, mem_in:%x, FSM:%x\n"
		,INREG32(ACCDET_SAMPLE_IN)
		,INREG32(ACCDET_CURR_IN)
		,INREG32(ACCDET_MEMORIZED_IN)
		,INREG32(ACCDET_BASE + 0x0050));
*/
    //ACCDET_DEBUG("[Accdet]accdet_resume ok\n");
    return 0;
}

#if 0
struct platform_device accdet_device = {
	.name	  ="Accdet_Driver",		
	.id		  = -1,	
	.dev    ={
	.release = accdet_dumy_release,
	}
};
#endif

static struct platform_driver accdet_driver = {
	.probe		= accdet_probe,	
	.suspend	= accdet_suspend,
	.resume		= accdet_resume,
	.remove   = accdet_remove,
	.driver     = {
	.name       = "Accdet_Driver",
	},
};

static int accdet_mod_init(void)
{
	int ret = 0;

	ACCDET_DEBUG("[Accdet]accdet_mod_init begin!\n");

	//------------------------------------------------------------------
	// 							Accdet PM
	//------------------------------------------------------------------
	//remove platform device register
	#if 0
	ret = platform_device_register(&accdet_device);
	if (ret) {
		ACCDET_DEBUG("[Accdet]platform_device_register error:(%d)\n", ret);	
		return ret;
	}
	else
	{
		ACCDET_DEBUG("[Accdet]platform_device_register done!\n");
	}
	#endif
	
	ret = platform_driver_register(&accdet_driver);
	if (ret) {
		ACCDET_DEBUG("[Accdet]platform_driver_register error:(%d)\n", ret);
		return ret;
	}
	else
	{
		ACCDET_DEBUG("[Accdet]platform_driver_register done!\n");
	}

    ACCDET_DEBUG("[Accdet]accdet_mod_init done!\n");
    return 0;

}

static void  accdet_mod_exit(void)
{
	ACCDET_DEBUG("[Accdet]accdet_mod_exit\n");
	platform_driver_unregister(&accdet_driver);
//	platform_device_unregister(&accdet_device);

	ACCDET_DEBUG("[Accdet]accdet_mod_exit Done!\n");
}

module_init(accdet_mod_init);
module_exit(accdet_mod_exit);


module_param(debug_enable,int,0644);

MODULE_DESCRIPTION("MTK MT6577 ACCDET driver");
MODULE_AUTHOR("Anny <Anny.Hu@mediatek.com>");
MODULE_LICENSE("GPL");

