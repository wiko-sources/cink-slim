/* system header files */
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/interrupt.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/pm.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/syscalls.h>
#include <linux/mtd/nand.h>

#include <asm/irq.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include <asm/mach/irq.h>
#include <asm/mach/map.h>
#include <asm/mach/time.h>
#include <asm/setup.h>

#include <mach/system.h>
#include <mach/board.h>
#include <mach/hardware.h>
#include <mach/mt6577_gpio.h>
#include <mach/mt_bt.h>
#include <mach/eint.h>
#include <mach/mtk_rtc.h>
#include <mach/mt6577_typedefs.h>

#include <cust_gpio_usage.h>
#include <cust_eint.h>

#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
#include <mach/mt_combo.h>
#endif

#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
static sdio_irq_handler_t mtk_wcn_cmb_sdio_eirq_handler = NULL;
static pm_callback_t mtk_wcn_cmb_sdio_pm_cb = NULL;
static void *mtk_wcn_cmb_sdio_pm_data = NULL;
static void *mtk_wcn_cmb_sdio_eirq_data = NULL;

const static u32 mtk_wcn_cmb_sdio_eint_pin = GPIO_WIFI_EINT_PIN;
const static u32 mtk_wcn_cmb_sdio_eint_num = CUST_EINT_WIFI_NUM;
const static u32 mtk_wcn_cmb_sdio_eint_m_eint = GPIO_WIFI_EINT_PIN_M_EINT;
const static u32 mtk_wcn_cmb_sdio_eint_m_gpio = GPIO_WIFI_EINT_PIN_M_GPIO;
/*
index: port number of combo chip (1:SDIO1, 2:SDIO2, no SDIO0)
value: slot power status of  (0:off, 1:on, 0xFF:invalid)
*/
    #if (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 0)
        static unsigned char combo_port_pwr_map[4] = {0x0, 0xFF, 0xFF, 0xFF};
    #elif (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 1)
        static unsigned char combo_port_pwr_map[4] = {0xFF, 0x0, 0xFF, 0xFF};
    #elif (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 2)
        static unsigned char combo_port_pwr_map[4] = {0xFF, 0xFF, 0x0, 0xFF};
    #elif (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 3)
        static unsigned char combo_port_pwr_map[4] = {0xFF, 0xFF, 0xFF, 0x0};
    #else
        #error "unsupported CONFIG_MTK_WCN_CMB_SDIO_SLOT" CONFIG_MTK_WCN_CMB_SDIO_SLOT
    #endif
#else
static sdio_irq_handler_t mt_wifi_irq_handler = NULL;
static pm_message_t mt_wifi_pm_state = { .event = PM_EVENT_HIBERNATE };
static pm_callback_t mt_wifi_pm_cb = NULL;
static void *mt_wifi_pm_data = NULL;
static void *mt_wifi_irq_data = NULL;
static int mt_wifi_pm_late_cb = 0;
#endif

//Ivan
static int _32k_set = 0;
static int _wifi_init = 0;
static pm_callback_t bcm_wifi_pm_callback = NULL;
static void *bcm_wifi_pm_data = NULL;


/*=======================================================================*/
/* Board Specific Devices Power Management                               */
/*=======================================================================*/
extern kal_bool pmic_chrdet_status(void);

void mt6577_power_off(void)
{
	printk("mt6577_power_off\n");

	/* pull PWRBB low */
	rtc_bbpu_power_down();

	while (1) {
		printk("mt6577_power_off : check charger\n");
		if (pmic_chrdet_status() == KAL_TRUE)
			arch_reset(0, "charger");
	}
}

/*=======================================================================*/
/* Board Specific Devices                                                */
/*=======================================================================*/
/*GPS driver*/
/*FIXME: remove mt3326 notation */
struct mt3326_gps_hardware mt3326_gps_hw = {
    .ext_power_on =  NULL,
    .ext_power_off = NULL,
};

int is_bt_on=0;
int is_wifi_on=0;
/*=======================================================================*/
/* Board Specific Devices Init                                           */
/*=======================================================================*/
static void bcm_sdio_eirq_handler_stub(void)
{
    //if (bcm_sdio_eirq_handler) {
    //    bcm_sdio_eirq_handler(combo_sdio_eirq_data);
    //}
}

void bcm4330_sleep(int on)
{
    pm_message_t state1 = { .event = PM_EVENT_USER_SUSPEND };
    pm_message_t state0 = { .event = PM_EVENT_USER_RESUME };
    
    printk("Ivan bcm4330_sleep = %d \n",on);
/*    
    if (bcm_wifi_pm_callback) {
        //printk(KERN_INFO "mtk_wcn_cmb_sdio_off(PM_EVENT_USER_SUSPEND, 0x%p, 0x%p) \n", mtk_wcn_cmb_sdio_pm_cb, mtk_wcn_cmb_sdio_pm_data);
        if (on == 1)
	    bcm_wifi_pm_callback(state1, bcm_wifi_pm_data);
	else
	    bcm_wifi_pm_callback(state0, bcm_wifi_pm_data);
	    
    }
*/  
}
EXPORT_SYMBOL(bcm4330_sleep);

void custom_board_init(){
       printk("===========%s=====on=============\n", __func__);
	mt_set_gpio_pull_enable(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_UP);	
	mt_set_gpio_dir(GPIO_WIFI_LDO_EN_PIN, GPIO_DIR_OUT);
	mt_set_gpio_mode(GPIO_WIFI_LDO_EN_PIN, GPIO_MODE_GPIO);
        mt_set_gpio_pull_enable(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_UP);
        msleep(10);
        mt_set_gpio_out(GPIO_WIFI_LDO_EN_PIN, GPIO_OUT_ONE);
        mt_set_gpio_mode(GPIO_BT_CLK_PIN , GPIO_BT_CLK_PIN_M_CLK);
        mt_set_clock_output(GPIO_BT_CLK_PIN_CLK, CLK_SRC_F32K, 1);
}
EXPORT_SYMBOL(custom_board_init);
void bcm4330_wifi_reset(uint on)
{
	mt_set_gpio_pull_enable(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_UP);	
	mt_set_gpio_dir(GPIO_WIFI_LDO_EN_PIN, GPIO_DIR_OUT);
	mt_set_gpio_mode(GPIO_WIFI_LDO_EN_PIN, GPIO_MODE_GPIO);
	
	if (on) {
	    mt_set_gpio_pull_enable(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_ENABLE);
	    mt_set_gpio_pull_select(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_UP);
           msleep(10);
           mt_set_gpio_out(GPIO_WIFI_LDO_EN_PIN, GPIO_OUT_ONE);
           printk("===========%s=====on=============\n", __func__);
	} else {
           
           mt_set_gpio_out(GPIO_WIFI_LDO_EN_PIN, GPIO_OUT_ZERO);
	    mt_set_gpio_pull_select(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_DOWN);
	    mt_set_gpio_pull_enable(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_ENABLE);
          printk("===========%s=====off=============\n", __func__);
	}
}
EXPORT_SYMBOL(bcm4330_wifi_reset);

void bcm4330_wifi_power(uint on)
{

//	mt_set_gpio_pull_enable(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_ENABLE);
//	mt_set_gpio_pull_select(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_UP);	
//	mt_set_gpio_dir(GPIO_WIFI_LDO_EN_PIN, GPIO_DIR_OUT);
//	mt_set_gpio_mode(GPIO_WIFI_LDO_EN_PIN, GPIO_MODE_GPIO);
	

	if (on) {
        mt_set_gpio_mode(GPIO_WIFI_EINT_PIN, GPIO_BT_EINT_PIN_M_GPIO);
        mt_set_gpio_pull_enable(GPIO_WIFI_EINT_PIN, 1);
        mt_set_gpio_pull_select(GPIO_WIFI_EINT_PIN, GPIO_PULL_DOWN);
#if 0	
        mt_set_gpio_mode(GPIO_WIFI_EINT_PIN, GPIO_WIFI_EINT_PIN_M_EINT);  
//Ivan	
//        mt_set_gpio_out(GPIO_WIFI_LDO_EN_PIN, GPIO_OUT_ZERO);

    	mt65xx_eint_set_sens(CUST_EINT_WIFI_NUM, CUST_EINT_LEVEL_SENSITIVE); /*CUST_EINT_WIFI_NUM */
    	mt65xx_eint_set_hw_debounce(CUST_EINT_WIFI_NUM, CUST_EINT_WIFI_DEBOUNCE_CN); /*CUST_EINT_WIFI_NUM */
    	mt65xx_eint_registration(CUST_EINT_WIFI_NUM/*CUST_EINT_WIFI_NUM */,
        CUST_EINT_WIFI_DEBOUNCE_EN,
        CUST_EINT_POLARITY_HIGH,
        bcm_sdio_eirq_handler_stub,
        0);
//    	mt65xx_eint_unmask(CUST_EINT_WIFI_NUM);/*CUST_EINT_WIFI_NUM */
    	mt65xx_eint_mask(CUST_EINT_WIFI_NUM);/*CUST_EINT_WIFI_NUM */	
#endif
	//rtc_gpio_enable_32k(RTC_GPIO_USER_WIFI);
        msleep(100);
    	//mt_set_gpio_mode(GPIO_BT_CLK_PIN , GPIO_BT_CLK_PIN_M_CLK);
    	//mt_set_clock_output(GPIO_BT_CLK_PIN_CLK, CLK_SRC_F32K, 1);
	   // _wifi_init = 1;
        mt_set_gpio_pull_enable(GPIO92, GPIO_PULL_ENABLE);	//->CLK
        mt_set_gpio_pull_select(GPIO92, GPIO_PULL_UP);		
        mt_set_gpio_pull_enable(GPIO91, GPIO_PULL_ENABLE);	//->CMD
        mt_set_gpio_pull_select(GPIO91, GPIO_PULL_UP);
        mt_set_gpio_pull_enable(GPIO94, GPIO_PULL_ENABLE);	//->DAT0
        mt_set_gpio_pull_select(GPIO94, GPIO_PULL_UP);
        mt_set_gpio_pull_enable(GPIO90, GPIO_PULL_ENABLE);	//->DAT1
        mt_set_gpio_pull_select(GPIO90, GPIO_PULL_UP);
        mt_set_gpio_pull_enable(GPIO89, GPIO_PULL_ENABLE);	//->DAT2
        mt_set_gpio_pull_select(GPIO89, GPIO_PULL_UP);
        mt_set_gpio_pull_enable(GPIO93, GPIO_PULL_ENABLE);	//->DAT3
        mt_set_gpio_pull_select(GPIO93, GPIO_PULL_UP);
	mt_set_gpio_mode(GPIO_WIFI_LDO_EN_PIN, GPIO_MODE_GPIO);	
	mt_set_gpio_dir(GPIO_WIFI_LDO_EN_PIN, GPIO_DIR_OUT);	
	mt_set_gpio_pull_enable(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_ENABLE);
	mt_set_gpio_pull_select(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_UP);	
        msleep(10);
        mt_set_gpio_out(GPIO_WIFI_LDO_EN_PIN, GPIO_OUT_ONE);
        msleep(200);
	
            is_wifi_on=1;
            printk("===========%s=====on=============is_wifi_on=%d\n", __func__,is_wifi_on);
	} else {
	    mt_set_gpio_mode(GPIO_WIFI_LDO_EN_PIN, GPIO_MODE_GPIO);		    
	    mt_set_gpio_dir(GPIO_WIFI_LDO_EN_PIN, GPIO_DIR_OUT);
	    mt_set_gpio_pull_select(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_DOWN);
	    mt_set_gpio_pull_enable(GPIO_WIFI_LDO_EN_PIN, GPIO_PULL_ENABLE);	           
            mt_set_gpio_out(GPIO_WIFI_LDO_EN_PIN, GPIO_OUT_ZERO);
            is_wifi_on=0;


            mt_set_gpio_pull_enable(GPIO92, GPIO_PULL_DOWN);	//->CLK
            mt_set_gpio_pull_select(GPIO92, GPIO_PULL_ENABLE);		
            mt_set_gpio_pull_enable(GPIO91, GPIO_PULL_DOWN);	//->CMD
            mt_set_gpio_pull_select(GPIO91, GPIO_PULL_ENABLE);
            mt_set_gpio_pull_enable(GPIO94, GPIO_PULL_DOWN);	//->DAT0
            mt_set_gpio_pull_select(GPIO94, GPIO_PULL_ENABLE);
            mt_set_gpio_pull_enable(GPIO90, GPIO_PULL_DOWN);	//->DAT1
            mt_set_gpio_pull_select(GPIO90, GPIO_PULL_ENABLE);
            mt_set_gpio_pull_enable(GPIO89, GPIO_PULL_DOWN);	//->DAT2
            mt_set_gpio_pull_select(GPIO89, GPIO_PULL_ENABLE);
            mt_set_gpio_pull_enable(GPIO93, GPIO_PULL_DOWN);	//->DAT3
            mt_set_gpio_pull_select(GPIO93, GPIO_PULL_ENABLE);
	    msleep(100);
	   /* 
	    if (is_bt_on == 0)
	    {
#ifdef GPIO_BT_CLK_PIN	
		mt_set_gpio_mode(GPIO_BT_CLK_PIN, GPIO_BT_CLK_PIN_M_GPIO);
		mt_set_gpio_dir(GPIO_BT_CLK_PIN, GPIO_DIR_OUT);
		mt_set_gpio_out(GPIO_BT_CLK_PIN, 0);
#else  
		rtc_gpio_disable_32k(RTC_GPIO_USER_WIFI);
#endif
	    }*/
             printk("===========%s=====off=============is_wifi_on=%d\n", __func__,is_wifi_on);
	}
	

}
EXPORT_SYMBOL(bcm4330_wifi_power);

void bcmBT_power_on(void)
{
    static int _32k_set = 0;

#define BRCM_BT_OFF_TIME (10) /* in ms, workable value */
#define BRCM_BT_RST_TIME (30) /* in ms, workable value */
#define BRCM_BT_STABLE_TIME (30) /* in ms, workable value */
#define BRCM_BT_EXT_INT_TIME (5) /* in ms, workable value */
#define BRCM_BT_32K_STABLE_TIME (100) /* in ms, test value */
   // rtc_gpio_enable_32k(RTC_GPIO_USER_BT);
    msleep(BRCM_BT_32K_STABLE_TIME);
    mt_set_gpio_mode(GPIO_PCM_DAICLK_PIN, GPIO_PCM_DAICLK_PIN_M_CLK);
    mt_set_gpio_mode(GPIO_PCM_DAIPCMOUT_PIN, GPIO_PCM_DAIPCMOUT_PIN_M_DAIPCMOUT);
    mt_set_gpio_mode(GPIO_PCM_DAIPCMIN_PIN, GPIO_PCM_DAIPCMIN_PIN_M_DAIPCMIN);
    mt_set_gpio_mode(GPIO_PCM_DAISYNC_PIN, GPIO_PCM_DAISYNC_PIN_M_BTSYNC);
    
    // EINT1
    mt_set_gpio_mode(GPIO_BT_EINT_PIN, GPIO_BT_EINT_PIN_M_GPIO);
    mt_set_gpio_pull_enable(GPIO_BT_EINT_PIN, 1);
    mt_set_gpio_pull_select(GPIO_BT_EINT_PIN, GPIO_PULL_DOWN);
    mt_set_gpio_mode(GPIO_BT_EINT_PIN, GPIO_BT_EINT_PIN_M_EINT);  
    /* UART Mode */
    mt_set_gpio_mode(GPIO_UART_URXD2_PIN, GPIO_UART_URXD2_PIN_M_URXD);
    mt_set_gpio_mode(GPIO_UART_UTXD2_PIN, GPIO_UART_UTXD2_PIN_M_UTXD);
    mt_set_gpio_mode(GPIO_UART_UCTS2_PIN, GPIO_UART_UCTS2_PIN_M_UCTS);
    mt_set_gpio_out(GPIO_UART_URTS2_PIN, GPIO_OUT_ONE);
    msleep(100);
     mt_set_gpio_mode(GPIO_UART_URTS2_PIN, GPIO_UART_URTS2_PIN_M_URTS);//high
    //printk(KERN_INFO "[mt6620] enable RTC GPIO\n");



    printk("[bcm4330_BT]Add additional pmu_en v2.8 control, it works only for re-worked phone\n");

    mt_set_gpio_pull_enable(GPIO_BT_WAKEUP_PIN, GPIO_PULL_DISABLE);
    mt_set_gpio_dir(GPIO_BT_WAKEUP_PIN, GPIO_DIR_OUT);
    mt_set_gpio_mode(GPIO_BT_WAKEUP_PIN, GPIO_MODE_GPIO);
    mt_set_gpio_out(GPIO_BT_WAKEUP_PIN, GPIO_OUT_ZERO);

    mt_set_gpio_pull_enable(GPIO_BT_POWREN_PIN, GPIO_PULL_DISABLE);
    mt_set_gpio_dir(GPIO_BT_POWREN_PIN, GPIO_DIR_OUT);
    mt_set_gpio_mode(GPIO_BT_POWREN_PIN, GPIO_MODE_GPIO);
    mt_set_gpio_out(GPIO_BT_POWREN_PIN, GPIO_OUT_ZERO);
    msleep(BRCM_BT_OFF_TIME);
    mt_set_gpio_out(GPIO_BT_POWREN_PIN, GPIO_OUT_ONE);
    msleep(BRCM_BT_STABLE_TIME);

    mt_set_gpio_pull_enable(GPIO_BT_RESET_PIN, GPIO_PULL_DISABLE);
    mt_set_gpio_dir(GPIO_BT_RESET_PIN, GPIO_DIR_OUT);
    mt_set_gpio_mode(GPIO_BT_RESET_PIN, GPIO_MODE_GPIO);
    mt_set_gpio_out(GPIO_BT_RESET_PIN, GPIO_OUT_ZERO);
     msleep(BRCM_BT_RST_TIME);
    mt_set_gpio_out(GPIO_BT_RESET_PIN, GPIO_OUT_ONE);
    /* SYSRST_B high */
//    mt_set_gpio_out(GPIO_COMBO_RST_PIN, GPIO_OUT_ONE);
    msleep(BRCM_BT_STABLE_TIME);
  
    
    printk(KERN_INFO "[bcm4330_BT] power on \n");
    is_bt_on = 1;
    //mutex_unlock(&bt_mutex);
    return;
}

void bcmBT_power_off(void)
{
    printk(KERN_INFO "[bcm4330 bt] power off!\n");

    //mt_set_gpio_out(GPIO25, GPIO_OUT_ZERO);
    mt_set_gpio_dir(GPIO_BT_POWREN_PIN, GPIO_DIR_OUT);    
    mt_set_gpio_out(GPIO_BT_POWREN_PIN, GPIO_OUT_ZERO);
    mt_set_gpio_dir(GPIO_BT_RESET_PIN, GPIO_DIR_OUT);        
    mt_set_gpio_out(GPIO_BT_RESET_PIN, GPIO_OUT_ZERO);

    //printk(KERN_INFO "[mt6620] set UART GPIO Mode output 0\n");
    mt_set_gpio_mode(GPIO_UART_URXD2_PIN, GPIO_UART_URXD2_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_UART_URXD2_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_UART_URXD2_PIN, GPIO_OUT_ZERO);

    mt_set_gpio_mode(GPIO_UART_UTXD2_PIN, GPIO_UART_UTXD2_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_UART_UTXD2_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_UART_UTXD2_PIN, GPIO_OUT_ZERO);
    
    mt_set_gpio_mode(GPIO_UART_UCTS2_PIN, GPIO_UART_UCTS2_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_UART_UCTS2_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_UART_UCTS2_PIN, GPIO_OUT_ZERO);

    mt_set_gpio_mode(GPIO_UART_URTS2_PIN, GPIO_UART_URTS2_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_UART_URTS2_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_UART_URTS2_PIN, GPIO_OUT_ZERO);
/*
    mt_set_gpio_mode(GPIO_BT_EINT_PIN, GPIO_BT_EINT_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_BT_EINT_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_BT_EINT_PIN, 0);
    */
    mt_set_gpio_mode(GPIO_PCM_DAICLK_PIN, GPIO_PCM_DAICLK_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_PCM_DAICLK_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_PCM_DAICLK_PIN, 0);

    mt_set_gpio_mode(GPIO_PCM_DAIPCMOUT_PIN, GPIO_PCM_DAIPCMOUT_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_PCM_DAIPCMOUT_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_PCM_DAIPCMOUT_PIN, 0);

    mt_set_gpio_mode(GPIO_PCM_DAIPCMIN_PIN, GPIO_PCM_DAIPCMIN_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_PCM_DAIPCMIN_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_PCM_DAIPCMIN_PIN, 0);

    mt_set_gpio_mode(GPIO_PCM_DAISYNC_PIN, GPIO_PCM_DAISYNC_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_PCM_DAISYNC_PIN, GPIO_DIR_OUT);
    mt_set_gpio_out(GPIO_PCM_DAISYNC_PIN, 0);
    mt_set_gpio_out(GPIO_BT_RESET_PIN, GPIO_OUT_ZERO);
    //printk(KERN_INFO "[mt6620] disable RTC GPIO \n");
    printk("[bcm4330_BT]not to rtc_gpio_disable_32k(RTC_GPIO_USER_GPS)  \n");
    //rtc_gpio_export_32k(false);
/*    if (is_wifi_on == 0)
    {
#ifdef GPIO_BT_CLK_PIN	
	mt_set_gpio_mode(GPIO_BT_CLK_PIN, GPIO_BT_CLK_PIN_M_GPIO);
	mt_set_gpio_dir(GPIO_BT_CLK_PIN, GPIO_DIR_OUT);
	mt_set_gpio_out(GPIO_BT_CLK_PIN, 0);
#else    
	rtc_gpio_disable_32k(RTC_GPIO_USER_BT);
#endif
    }*/
    is_bt_on = 0;
    msleep(100);		
    //printk(KERN_INFO "[mt6620] disable external LDO\n");
    /* external LDO_EN high */
//    mt_set_gpio_out(GPIO_COMBO_6620_LDO_EN_PIN, GPIO_OUT_ZERO);
    
    return;
}

#ifdef BCM_BT_SUPPORT
void brcm_bt_power_on(void)
{
    printk(KERN_INFO "+brcm_bt_power_on\n");

    bcmBT_power_on();
    printk(KERN_INFO "-brcm_bt_power_on\n");
}
EXPORT_SYMBOL(brcm_bt_power_on);

void brcm_bt_power_off(void)
{
    printk(KERN_INFO "+brcm_bt_power_off\n");
    
    bcmBT_power_off();
    printk(KERN_INFO "-brcm_bt_power_off\n");
}
EXPORT_SYMBOL(brcm_bt_power_off);

int brcm_bt_suspend(pm_message_t state)
{
    printk(KERN_INFO "+brcm_bt_suspend\n");
    printk(KERN_INFO "-brcm_bt_suspend\n");
    return MT_BT_OK;
}
EXPORT_SYMBOL(brcm_bt_suspend);

int brcm_bt_resume(pm_message_t state)
{
    printk(KERN_INFO "+brcm_bt_resume\n");
    printk(KERN_INFO "-brcm_bt_resume\n");
    return MT_BT_OK;
}
EXPORT_SYMBOL(brcm_bt_resume);
#endif


#ifdef MTK_BT_SUPPORT
void mt_bt_power_on(void)
{
    printk(KERN_INFO "+mt_bt_power_on\n");

#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
    /* combo chip product */
    /*
     * Ignore rfkill0/state call. Controll BT power on/off through device /dev/stpbt.
     */
#else 
    /* standalone product */
#endif

    printk(KERN_INFO "-mt_bt_power_on\n");
}
EXPORT_SYMBOL(mt_bt_power_on);

void mt_bt_power_off(void)
{
    printk(KERN_INFO "+mt_bt_power_off\n");

#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
    /* combo chip product */
    /*
     * Ignore rfkill0/state call. Controll BT power on/off through device /dev/stpbt.
     */
#else
    /* standalone product */
#endif

    printk(KERN_INFO "-mt_bt_power_off\n");
}
EXPORT_SYMBOL(mt_bt_power_off);

int mt_bt_suspend(pm_message_t state)
{
    printk(KERN_INFO "+mt_bt_suspend\n");
    printk(KERN_INFO "-mt_bt_suspend\n");
    return MT_BT_OK;
}

int mt_bt_resume(pm_message_t state)
{
    printk(KERN_INFO "+mt_bt_resume\n");
    printk(KERN_INFO "-mt_bt_resume\n");
    return MT_BT_OK;
}
#endif


#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
static void mtk_wcn_cmb_sdio_enable_eirq(void)
{
    mt65xx_eint_unmask(mtk_wcn_cmb_sdio_eint_num);/* CUST_EINT_WIFI_NUM */
}

static void mtk_wcn_cmb_sdio_disable_eirq(void)
{
    mt65xx_eint_mask(mtk_wcn_cmb_sdio_eint_num); /* CUST_EINT_WIFI_NUM */
}

static void mtk_wcn_cmb_sdio_eirq_handler_stub(void)
{
    if (mtk_wcn_cmb_sdio_eirq_handler) {
        mtk_wcn_cmb_sdio_eirq_handler(mtk_wcn_cmb_sdio_eirq_data);
    }
}

static void mtk_wcn_cmb_sdio_request_eirq(sdio_irq_handler_t irq_handler, void *data)
{
    mt65xx_eint_set_sens(mtk_wcn_cmb_sdio_eint_num, CUST_EINT_WIFI_SENSITIVE); /*CUST_EINT_WIFI_NUM */
    mt65xx_eint_set_hw_debounce(mtk_wcn_cmb_sdio_eint_num, CUST_EINT_WIFI_DEBOUNCE_CN); /*CUST_EINT_WIFI_NUM */
    mt65xx_eint_registration(mtk_wcn_cmb_sdio_eint_num/*CUST_EINT_WIFI_NUM */,
        CUST_EINT_WIFI_DEBOUNCE_EN,
        CUST_EINT_WIFI_POLARITY,
        mtk_wcn_cmb_sdio_eirq_handler_stub,
        0);
    mt65xx_eint_mask(mtk_wcn_cmb_sdio_eint_num);/*CUST_EINT_WIFI_NUM */

    mtk_wcn_cmb_sdio_eirq_handler = irq_handler;
    mtk_wcn_cmb_sdio_eirq_data    = data;
}

static void mtk_wcn_cmb_sdio_register_pm(pm_callback_t pm_cb, void *data)
{
    printk( KERN_INFO "mtk_wcn_cmb_sdio_register_pm (0x%p, 0x%p)\n", pm_cb, data);
    /* register pm change callback */
    mtk_wcn_cmb_sdio_pm_cb = pm_cb;
    mtk_wcn_cmb_sdio_pm_data = data;
}

static void mtk_wcn_cmb_sdio_on (int sdio_port_num) {
    pm_message_t state = { .event = PM_EVENT_USER_RESUME };

    printk(KERN_INFO "mtk_wcn_cmb_sdio_on (%d) \n", sdio_port_num);

    /* 1. disable sdio eirq */
    mtk_wcn_cmb_sdio_disable_eirq();
    mt_set_gpio_pull_enable(mtk_wcn_cmb_sdio_eint_pin, GPIO_PULL_DISABLE); /* GPIO_WIFI_EINT_PIN */
    mt_set_gpio_mode(mtk_wcn_cmb_sdio_eint_pin, mtk_wcn_cmb_sdio_eint_m_eint); /* EINT mode */

    /* 2. call sd callback */
    if (mtk_wcn_cmb_sdio_pm_cb) {
        //printk(KERN_INFO "mtk_wcn_cmb_sdio_pm_cb(PM_EVENT_USER_RESUME, 0x%p, 0x%p) \n", mtk_wcn_cmb_sdio_pm_cb, mtk_wcn_cmb_sdio_pm_data);
        mtk_wcn_cmb_sdio_pm_cb(state, mtk_wcn_cmb_sdio_pm_data);
    }
    else {
        printk(KERN_WARNING "mtk_wcn_cmb_sdio_on no sd callback!!\n");
    }
}

static void mtk_wcn_cmb_sdio_off (int sdio_port_num) {
    pm_message_t state = { .event = PM_EVENT_USER_SUSPEND };

    printk(KERN_INFO "mtk_wcn_cmb_sdio_off (%d) \n", sdio_port_num);

    /* 1. call sd callback */
    if (mtk_wcn_cmb_sdio_pm_cb) {
        //printk(KERN_INFO "mtk_wcn_cmb_sdio_off(PM_EVENT_USER_SUSPEND, 0x%p, 0x%p) \n", mtk_wcn_cmb_sdio_pm_cb, mtk_wcn_cmb_sdio_pm_data);
        mtk_wcn_cmb_sdio_pm_cb(state, mtk_wcn_cmb_sdio_pm_data);
    }
    else {
        printk(KERN_WARNING "mtk_wcn_cmb_sdio_off no sd callback!!\n");
    }

    /* 2. disable sdio eirq */
    mtk_wcn_cmb_sdio_disable_eirq();
    /*printk(KERN_INFO "[mt6620] set WIFI_EINT input pull down\n");*/
    mt_set_gpio_mode(mtk_wcn_cmb_sdio_eint_pin, mtk_wcn_cmb_sdio_eint_m_gpio); /* GPIO mode */
    mt_set_gpio_dir(mtk_wcn_cmb_sdio_eint_pin, GPIO_DIR_IN);
    mt_set_gpio_pull_select(mtk_wcn_cmb_sdio_eint_pin, GPIO_PULL_DOWN);
    mt_set_gpio_pull_enable(mtk_wcn_cmb_sdio_eint_pin, GPIO_PULL_ENABLE);
}

int board_sdio_ctrl (unsigned int sdio_port_num, unsigned int on) {
#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
			sdio_port_num = CONFIG_MTK_WCN_CMB_SDIO_SLOT;
			printk(KERN_WARNING "mt_combo_sdio_ctrl: force set sdio port to (%d)\n", sdio_port_num);
#endif
    if ((sdio_port_num >= 4) || (combo_port_pwr_map[sdio_port_num] == 0xFF) ) {
        /* invalid sdio port number or slot mapping */
        printk(KERN_WARNING "mt_mtk_wcn_cmb_sdio_ctrl invalid port(%d, %d)\n", sdio_port_num, combo_port_pwr_map[sdio_port_num]);
        return -1;
    }
    /*printk(KERN_INFO "mt_mtk_wcn_cmb_sdio_ctrl (%d, %d)\n", sdio_port_num, on);*/

    if (!combo_port_pwr_map[sdio_port_num] && on) {
    	  printk(KERN_INFO  "board_sdio_ctrl force off before on\n");
        mtk_wcn_cmb_sdio_off(sdio_port_num);
        combo_port_pwr_map[sdio_port_num] = 0;
        /* off -> on */
        mtk_wcn_cmb_sdio_on(sdio_port_num);
        combo_port_pwr_map[sdio_port_num] = 1;
    }
    else if (combo_port_pwr_map[sdio_port_num] && !on) {
        /* on -> off */
        mtk_wcn_cmb_sdio_off(sdio_port_num);
        combo_port_pwr_map[sdio_port_num] = 0;
    }
    else {
        return -2;
    }
    return 0;
}
EXPORT_SYMBOL(board_sdio_ctrl);

#endif /* end of defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) */

    
    

#if defined(CONFIG_WLAN)
#if !defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
static void mt_wifi_enable_irq(void)
{
    mt65xx_eint_unmask(CUST_EINT_WIFI_NUM);
}

static void mt_wifi_disable_irq(void)
{
    mt65xx_eint_mask(CUST_EINT_WIFI_NUM);
}

static void mt_wifi_eirq_handler(void)
{
    if (mt_wifi_irq_handler) {
        mt_wifi_irq_handler(mt_wifi_irq_data);
    }
}

static void mt_wifi_request_irq(sdio_irq_handler_t irq_handler, void *data)
{
    mt65xx_eint_set_sens(CUST_EINT_WIFI_NUM, CUST_EINT_WIFI_SENSITIVE);
    mt65xx_eint_set_hw_debounce(CUST_EINT_WIFI_NUM, CUST_EINT_WIFI_DEBOUNCE_CN);
    mt65xx_eint_registration(CUST_EINT_WIFI_NUM,
        CUST_EINT_WIFI_DEBOUNCE_EN,
        CUST_EINT_WIFI_POLARITY,
        mt_wifi_eirq_handler,
        0);
    mt65xx_eint_mask(CUST_EINT_WIFI_NUM);

    mt_wifi_irq_handler = irq_handler;
    mt_wifi_irq_data    = data;
}

static void mt_wifi_register_pm(pm_callback_t pm_cb, void *data)
{
    /* register pm change callback */
    mt_wifi_pm_cb = pm_cb;
    mt_wifi_pm_data = data;
}

#endif /* end of !defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) */ 

int mt_wifi_resume(pm_message_t state)
{
    int evt = state.event;

    if (evt != PM_EVENT_USER_RESUME && evt != PM_EVENT_RESUME) {
        return -1;
    }

    /*printk(KERN_INFO "[WIFI] %s Resume\n", evt == PM_EVENT_RESUME ? "PM":"USR");*/

#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
    /* combo chip product: notify combo driver to turn on Wi-Fi */

    /* Use new mtk_wcn_cmb_stub APIs instead of old mt_combo ones */
    mtk_wcn_cmb_stub_func_ctrl(COMBO_FUNC_TYPE_WIFI, 1);
    /*mt_combo_func_ctrl(COMBO_FUNC_TYPE_WIFI, 1);*/

#endif

    return 0;
}

int mt_wifi_suspend(pm_message_t state)
{
    int evt = state.event;
#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
    static int is_1st_suspend_from_boot = 1;
#endif

    if (evt != PM_EVENT_USER_SUSPEND && evt != PM_EVENT_SUSPEND) {
        return -1;
    }

#if defined(CONFIG_MTK_COMBO) || defined(CONFIG_MTK_COMBO_MODULE)
    #if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
    /* combo chip product: notify combo driver to turn on Wi-Fi */
    if (is_1st_suspend_from_boot) {
        pm_message_t state = { .event = PM_EVENT_USER_SUSPEND };

        if (mtk_wcn_cmb_sdio_pm_cb) {
            is_1st_suspend_from_boot = 0;
            /*              *** IMPORTANT DEPENDENDY***
            RFKILL: set wifi and bt suspend by default in probe()
            MT6573-SD: sd host is added to MMC stack and suspend is ZERO by default
            (which means NOT suspended).

            When boot up, RFKILL will set wifi off and this function gets
            called. In order to successfully resume wifi at 1st time, pm_cb here
            shall be called once to let MT6573-SD do sd host suspend and remove
            sd host from MMC. Then wifi can be turned on successfully.

            Boot->SD host added to MMC (suspend=0)->RFKILL set wifi off
            ->SD host removed from MMC (suspend=1)->RFKILL set wifi on
            */
            printk(KERN_INFO "1st mt_wifi_suspend (PM_EVENT_USER_SUSPEND) \n");
            (*mtk_wcn_cmb_sdio_pm_cb)(state, mtk_wcn_cmb_sdio_pm_data);
        }
        else {
            printk(KERN_WARNING "1st mt_wifi_suspend but no sd callback!!\n");
        }
    }
    else {
        /* combo chip product, notify combo driver */

        /* Use new mtk_wcn_cmb_stub APIs instead of old mt_combo ones */
        mtk_wcn_cmb_stub_func_ctrl(COMBO_FUNC_TYPE_WIFI, 0);
        /*mt_combo_func_ctrl(COMBO_FUNC_TYPE_WIFI, 0);*/
    }
    #endif
#endif
    return 0;
}

void mt_wifi_power_on(void)
{
    pm_message_t state = { .event = PM_EVENT_USER_RESUME };

    (void)mt_wifi_resume(state);
}
EXPORT_SYMBOL(mt_wifi_power_on);

void mt_wifi_power_off(void)
{
    pm_message_t state = { .event = PM_EVENT_USER_SUSPEND };

    (void)mt_wifi_suspend(state);
}
EXPORT_SYMBOL(mt_wifi_power_off);

#endif /* end of defined(CONFIG_WLAN) */

#if defined(BCM_GPS_SUPPORT)
static int gps_reset_state=0;
static int bcm4751_power_on(void)
{    
//        printk("[bcm4751] pull up \n");
	// 32k CLK
#ifdef GPIO_GPS_CLK_PIN
	mt_set_gpio_mode(GPIO_GPS_CLK_PIN , GPIO_GPS_CLK_PIN_M_CLK);
	mt_set_clock_output(GPIO_GPS_CLK_PIN_CLK, CLK_SRC_F32K, 1);
#else
        rtc_gpio_enable_32k(RTC_GPIO_USER_GPS);
#endif
        msleep(100);
        mt_set_gpio_mode(GPIO_UART_URXD1_PIN, GPIO_UART_URXD1_PIN_M_URXD);
        mt_set_gpio_mode(GPIO_UART_UTXD1_PIN, GPIO_UART_UTXD1_PIN_M_UTXD);
        mt_set_gpio_mode(GPIO_UART_UCTS1_PIN, GPIO_UART_UCTS1_PIN_M_UCTS);
        mt_set_gpio_mode(GPIO_UART_URTS1_PIN, GPIO_UART_URTS1_PIN_M_URTS);
        mt_set_gpio_out(GPIO_GPS_PWREN_PIN, GPIO_OUT_ZERO);
        mt_set_gpio_pull_enable(GPIO_GPS_PWREN_PIN, GPIO_PULL_DISABLE);
        mt_set_gpio_dir(GPIO_GPS_PWREN_PIN, GPIO_DIR_OUT);
        mt_set_gpio_mode(GPIO_GPS_PWREN_PIN, GPIO_MODE_GPIO);
        mt_set_gpio_out(GPIO_GPS_PWREN_PIN, GPIO_OUT_ZERO);
        if((0==gps_reset_state)||(0==mt_get_gpio_in(GPIO_GPS_RST_PIN)))
        {
            mt_set_gpio_pull_enable(GPIO_GPS_RST_PIN, GPIO_PULL_DISABLE);
            mt_set_gpio_dir(GPIO_GPS_RST_PIN, GPIO_DIR_OUT);
            mt_set_gpio_mode(GPIO_GPS_RST_PIN, GPIO_MODE_GPIO);
            
            mt_set_gpio_out(GPIO_GPS_RST_PIN, GPIO_OUT_ZERO);
            msleep(10);
            printk("[bcm4751] reset \n");
            mt_set_gpio_out(GPIO_GPS_RST_PIN, GPIO_OUT_ONE);
            msleep(10);
            gps_reset_state=1;
        }
        mt_set_gpio_out(GPIO_GPS_PWREN_PIN, GPIO_OUT_ONE);

        printk("[bcm4751] power on \n");
    return 0;
}

int bcm4751_power_off(void)
{
    printk("[bcm4751] power off\n");
    mt_set_gpio_out(GPIO_GPS_PWREN_PIN, GPIO_OUT_ZERO);
    //mt_set_gpio_out(GPIO_GPS_RST_PIN, GPIO_OUT_ZERO);
//    rtc_gpio_disable_32k(RTC_GPIO_USER_GPS);
//#ifdef RTC_GPIO_USER_GPS
#ifdef GPIO_GPS_CLK_PIN
    mt_set_gpio_mode(GPIO_GPS_CLK_PIN, GPIO_GPS_CLK_PIN_M_GPIO);
    mt_set_gpio_dir(GPIO_GPS_CLK_PIN, GPIO_DIR_OUT); 
    mt_set_gpio_out(GPIO_GPS_CLK_PIN, 0);
#else
    rtc_gpio_disable_32k(RTC_GPIO_USER_GPS);    
#endif
    return 0;
}

#endif


#if defined(BCM_GPS_SUPPORT)
void brcm_gps_power_on(void)
{
    //pm_message_t state = { .event = PM_EVENT_USER_RESUME };

    bcm4751_power_on();
}
EXPORT_SYMBOL(brcm_gps_power_on);

void brcm_gps_power_off(void)
{
    //pm_message_t state = { .event = PM_EVENT_USER_SUSPEND };

    bcm4751_power_off();
}
EXPORT_SYMBOL(brcm_gps_power_off);
int brcm_gps_suspend(pm_message_t state)
{
    printk(KERN_INFO "+brcm_bt_suspend\n");
    printk(KERN_INFO "-brcm_bt_suspend\n");
    return 0;
}

int brcm_gps_resume(pm_message_t state)
{
    printk(KERN_INFO "+brcm_bt_resume\n");
    printk(KERN_INFO "-brcm_bt_resume\n");
    return 0;
}
#endif

//Ivan added
static void bcm_wifi_sdio_register_pm(pm_callback_t pm_cb, void *data)
{
    printk( KERN_INFO "bcm_wifi_sdio_register_pm (0x%p, 0x%p)\n", pm_cb, data);
    bcm_wifi_pm_callback = pm_cb;
    bcm_wifi_pm_data = data;
    /* register pm change callback */
}

/* Board Specific Devices                                                */
/*=======================================================================*/

/*=======================================================================*/
/* Board Specific Devices Init                                           */
/*=======================================================================*/

/*=======================================================================*/
/* Board Devices Capability                                              */
/*=======================================================================*/
#define MSDC_SDCARD_FLAG  (MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE | MSDC_HIGHSPEED)
#define MSDC_SDIO_FLAG    (MSDC_EXT_SDIO_IRQ | MSDC_HIGHSPEED)

#if defined(CFG_DEV_MSDC0)
#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) && (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 0)
    /* MSDC0 settings for MT66xx combo connectivity chip */
	struct msdc_hw msdc0_hw = {
	    .clk_src        = 1,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .data_edge      = MSDC_SMPL_FALLING,
	    .clk_drv        = 4,
	    .cmd_drv        = 4,
	    .dat_drv        = 4,
	    .data_pins      = 4,
	    .data_offset    = 0,
	    //MT6620 use External IRQ, wifi uses high speed. here wifi manage his own suspend and resume, does not support hot plug
	    .flags          = MSDC_EXT_SDIO_IRQ | MSDC_HIGHSPEED,//MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE,
	    .request_sdio_eirq = mtk_wcn_cmb_sdio_request_eirq,
	    .enable_sdio_eirq  = mtk_wcn_cmb_sdio_enable_eirq,
	    .disable_sdio_eirq = mtk_wcn_cmb_sdio_disable_eirq,
	    .register_pm       = mtk_wcn_cmb_sdio_register_pm,
	};
    #else
	struct msdc_hw msdc0_hw = {
	    .clk_src        = 1,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .data_edge      = MSDC_SMPL_FALLING,
	    .clk_drv        = 0,
	    .cmd_drv        = 0,
	    .dat_drv        = 0,
	    .data_pins      = 8,
	    .data_offset    = 0,
#ifdef MTK_EMMC_SUPPORT
	    .flags          = MSDC_SYS_SUSPEND | MSDC_HIGHSPEED,
#else
	    .flags          = MSDC_SDCARD_FLAG,
#endif
	};
	#endif
#endif
#if defined(CFG_DEV_MSDC1)
    #if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) && (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 1)
    	struct msdc_hw msdc1_hw = {
	    .clk_src        = 1,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .data_edge      = MSDC_SMPL_FALLING,
	    .clk_drv        = 4,
	    .cmd_drv        = 4,
	    .dat_drv        = 4,
	    .data_pins      = 4,
	    .data_offset    = 0,
	    //MT6620 use External IRQ, wifi uses high speed. here wifi manage his own suspend and resume, does not support hot plug
	    .flags          = MSDC_EXT_SDIO_IRQ | MSDC_HIGHSPEED,//MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE,
	    .request_sdio_eirq = mtk_wcn_cmb_sdio_request_eirq,
	    .enable_sdio_eirq  = mtk_wcn_cmb_sdio_enable_eirq,
	    .disable_sdio_eirq = mtk_wcn_cmb_sdio_disable_eirq,
	    .register_pm       = mtk_wcn_cmb_sdio_register_pm,
	};
    #else
struct msdc_hw msdc1_hw = {
    .clk_src        = 1,
    .cmd_edge       = MSDC_SMPL_FALLING,
    .data_edge      = MSDC_SMPL_FALLING,
    .clk_drv        = 0,
    .cmd_drv        = 0,
    .dat_drv        = 0,
    .data_pins      = 4,
    .data_offset    = 0,
//Ivan	
    .flags          = MSDC_SYS_SUSPEND | /*MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE|*/ MSDC_HIGHSPEED|MSDC_SPE,
};
    #endif
#endif
#if defined(CFG_DEV_MSDC2)
    #if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) && (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 2)
    /* MSDC2 settings for MT66xx combo connectivity chip */
	struct msdc_hw msdc2_hw = {
	    .clk_src        = 1,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .data_edge      = MSDC_SMPL_FALLING,
	    .clk_drv        = 4,
	    .cmd_drv        = 4,
	    .dat_drv        = 4,
	    .data_pins      = 4,
	    .data_offset    = 0,
	    //MT6620 use External IRQ, wifi uses high speed. here wifi manage his own suspend and resume, does not support hot plug
	    .flags          = MSDC_EXT_SDIO_IRQ | MSDC_HIGHSPEED,//MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE,
	    .request_sdio_eirq = mtk_wcn_cmb_sdio_request_eirq,
	    .enable_sdio_eirq  = mtk_wcn_cmb_sdio_enable_eirq,
	    .disable_sdio_eirq = mtk_wcn_cmb_sdio_disable_eirq,
	    .register_pm       = mtk_wcn_cmb_sdio_register_pm,
	};
    #else
	struct msdc_hw msdc2_hw = {
	    .clk_src        = 1,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .data_edge      = MSDC_SMPL_FALLING,
	    .clk_drv        = 0,
	    .cmd_drv        = 0,
	    .dat_drv        = 0,
	    .data_pins      = 4,
	    .data_offset    = 0,
	    .flags          = MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE,
	};
	#endif
#endif

#if defined(CFG_DEV_MSDC3)
	#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT) && (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 3)
    /* MSDC3 settings for MT66xx combo connectivity chip */
	struct msdc_hw msdc3_hw = {
	    .clk_src        = 1,
	    .cmd_edge       = MSDC_SMPL_FALLING,
	    .data_edge      = MSDC_SMPL_FALLING,
	    .clk_drv        = 4,
	    .cmd_drv        = 4,
	    .dat_drv        = 4,
	    .data_pins      = 4,
	    .data_offset    = 0,
	    //.flags          = MSDC_EXT_SDIO_IRQ | MSDC_HIGHSPEED,//| MSDC_HIGHSPEED
	    .flags = MSDC_EXT_SDIO_IRQ | MSDC_HIGHSPEED|MSDC_TABDRV|MSDC_INTERNAL_CLK,
	    .request_sdio_eirq = mtk_wcn_cmb_sdio_request_eirq,
	    .enable_sdio_eirq  = mtk_wcn_cmb_sdio_enable_eirq,
	    .disable_sdio_eirq = mtk_wcn_cmb_sdio_disable_eirq,
	    .register_pm       = mtk_wcn_cmb_sdio_register_pm,
	};
    #else
    struct msdc_hw msdc3_hw = {
	    .clk_src        = 1,
	    .cmd_edge       = MSDC_SMPL_FALLING,		//MSDC_SMPL_RISING,
	    .data_edge      = MSDC_SMPL_FALLING,		//MSDC_SMPL_RISING,
	    .clk_drv        = 4,
	    .cmd_drv        = 4,
	    .dat_drv        = 4,
	    .data_pins      = 4,
	    .data_offset    = 0,
	    .flags          = /*MSDC_SYS_SUSPEND | MSDC_WP_PIN_EN | MSDC_CD_PIN_EN | MSDC_REMOVABLE|*/ MSDC_HIGHSPEED,
	    .register_pm    = bcm_wifi_sdio_register_pm,
	};
	#endif
#endif

/* MT6575 NAND Driver */
#if defined(CONFIG_MTK_MTD_NAND)
struct mt6575_nand_host_hw mt6575_nand_hw = {
    .nfi_bus_width          = 8,
	.nfi_access_timing		= NFI_DEFAULT_ACCESS_TIMING,
	.nfi_cs_num				= NFI_CS_NUM,
	.nand_sec_size			= 512,
	.nand_sec_shift			= 9,
	.nand_ecc_size			= 2048,
	.nand_ecc_bytes			= 32,
	.nand_ecc_mode			= NAND_ECC_HW,
};
#endif
