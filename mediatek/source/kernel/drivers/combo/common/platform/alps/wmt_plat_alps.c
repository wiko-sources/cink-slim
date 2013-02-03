/*! \file
    \brief  Declaration of library functions

    Any definitions in this file will be shared among GLUE Layer and internal Driver Stack.
*/

/*******************************************************************************
* Copyright (c) 2009 MediaTek Inc.
*
* All rights reserved. Copying, compilation, modification, distribution
* or any other use whatsoever of this material is strictly prohibited
* except in accordance with a Software License Agreement with
* MediaTek Inc.
********************************************************************************
*/

/*******************************************************************************
* LEGAL DISCLAIMER
*
* BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND
* AGREES THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK
* SOFTWARE") RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE
* PROVIDED TO BUYER ON AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY
* DISCLAIMS ANY AND ALL WARRANTIES, EXPRESS OR IMPLIED, INCLUDING BUT NOT
* LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE OR NONINFRINGEMENT. NEITHER DOES MEDIATEK PROVIDE
* ANY WARRANTY WHATSOEVER WITH RESPECT TO THE SOFTWARE OF ANY THIRD PARTY
* WHICH MAY BE USED BY, INCORPORATED IN, OR SUPPLIED WITH THE MEDIATEK
* SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH THIRD PARTY FOR ANY
* WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE
* FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S SPECIFICATION OR TO
* CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
* BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
* LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL
* BE, AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT
* ISSUE, OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY
* BUYER TO MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
* THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
* WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT
* OF LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING
* THEREOF AND RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN
* FRANCISCO, CA, UNDER THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE
* (ICC).
********************************************************************************
*/


/*******************************************************************************
*                         C O M P I L E R   F L A G S
********************************************************************************
*/

/*******************************************************************************
*                                 M A C R O S
********************************************************************************
*/


#if CONFIG_HAS_WAKELOCK
#include <linux/wakelock.h>
#define CFG_WMT_WAKELOCK_SUPPORT 1
#endif

#ifdef DFT_TAG
#undef DFT_TAG
#endif
#define DFT_TAG         "[WMT-PLAT]"


/*******************************************************************************
*                    E X T E R N A L   R E F E R E N C E S
********************************************************************************
*/

/* ALPS header files */
#include <mach/mtk_rtc.h>
#include <cust_gpio_usage.h>
#include <cust_eint.h>
/* MT6573 header files 
#include <mach/mt6573_gpio.h>
#include <mach/mt6573_eint.h>
#include <mach/mt6573_boot.h>
#include <mach/mt6573_pll.h>
#include <mach/mt6573_devs.h>
*/

/* MT6575 header files */
#if defined(CONFIG_ARCH_MT6575)
#include <mach/eint.h>
#include <mach/mtk_rtc.h>
#include <mach/mt6575_gpio.h>
#include <cust_gpio_usage.h>
#include <mach/mt6575_boot.h>
#elif defined(CONFIG_ARCH_MT6577)
#include <mach/eint.h>
#include <mach/mtk_rtc.h>
#include <mach/mt_gpio.h>
#include <cust_gpio_usage.h>
#include <mach/mt6577_boot.h>
#endif
/* ALPS and COMBO header files */
#include <mach/mtk_wcn_cmb_stub.h>

/* MTK_WCN_COMBO header files */
#include "wmt_plat.h"
#include "wmt_dev.h"
#include "wmt_lib.h"
#include "mtk_wcn_cmb_hw.h"

/*******************************************************************************
*                              C O N S T A N T S
********************************************************************************
*/




/*******************************************************************************
*                             D A T A   T Y P E S
********************************************************************************
*/

/*******************************************************************************
*                  F U N C T I O N   D E C L A R A T I O N S
********************************************************************************
*/

static VOID wmt_plat_func_ctrl (UINT32 type, UINT32 on);
static VOID wmt_plat_bgf_eirq_cb (VOID);

static INT32 wmt_plat_ldo_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_pmu_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_rtc_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_rst_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_bgf_eint_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_wifi_eint_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_all_eint_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_uart_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_pcm_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_i2s_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_sdio_pin_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_gps_sync_ctrl (ENUM_PIN_STATE state);
static INT32 wmt_plat_gps_lna_ctrl (ENUM_PIN_STATE state);

static INT32 wmt_plat_dump_pin_conf (VOID);



/*******************************************************************************
*                            P U B L I C   D A T A
********************************************************************************
*/

/*******************************************************************************
*                           P R I V A T E   D A T A
********************************************************************************
*/




#if CFG_WMT_WAKELOCK_SUPPORT
static OSAL_SLEEPABLE_LOCK gOsSLock;
static struct wake_lock wmtWakeLock;
#endif

const static fp_set_pin gfp_set_pin_table[] =
{
    [PIN_LDO] = wmt_plat_ldo_ctrl,
    [PIN_PMU] = wmt_plat_pmu_ctrl,
    [PIN_RTC] = wmt_plat_rtc_ctrl,
    [PIN_RST] = wmt_plat_rst_ctrl,
    [PIN_BGF_EINT] = wmt_plat_bgf_eint_ctrl,
    [PIN_WIFI_EINT] = wmt_plat_wifi_eint_ctrl,
    [PIN_ALL_EINT] = wmt_plat_all_eint_ctrl,
    [PIN_UART_GRP] = wmt_plat_uart_ctrl,
    [PIN_PCM_GRP] = wmt_plat_pcm_ctrl,
    [PIN_I2S_GRP] = wmt_plat_i2s_ctrl,
    [PIN_SDIO_GRP] = wmt_plat_sdio_pin_ctrl,
    [PIN_GPS_SYNC] = wmt_plat_gps_sync_ctrl,
    [PIN_GPS_LNA] = wmt_plat_gps_lna_ctrl,
    
};

/*******************************************************************************
*                              F U N C T I O N S
********************************************************************************
*/

/*!
 * \brief audio control callback function for CMB_STUB on ALPS
 *
 * A platform function required for dynamic binding with CMB_STUB on ALPS.
 *
 * \param state desired audio interface state to use
 * \param flag audio interface control options
 *
 * \retval 0 operation success
 * \retval -1 invalid parameters
 * \retval < 0 error for operation fail
 */
INT32 wmt_plat_audio_ctrl (CMB_STUB_AIF_X state, CMB_STUB_AIF_CTRL ctrl)
{
    INT32 iRet;
    UINT32 pinShare;

    /* input sanity check */
    if ( (CMB_STUB_AIF_MAX <= state)
        || (CMB_STUB_AIF_CTRL_MAX <= ctrl) ) {
        return -1;
    }

    if (get_chip_eco_ver() == CHIP_E1) {
        // TODO: [FixMe][GeorgeKuo] how about MT6575? The following is applied to MT6573E1 only!!
        pinShare = 1;
        WMT_INFO_FUNC( "ALPS MT6573 CHIP_E1 PCM/I2S pin share\n");
    }
    else{ //E1 later
        pinShare = 0;
        WMT_INFO_FUNC( "PCM/I2S pin seperate\n");
    }

    iRet = 0;

    /* set host side first */
    switch (state) {
    case CMB_STUB_AIF_0:
        /* BT_PCM_OFF & FM line in/out */
        iRet += wmt_plat_gpio_ctrl(PIN_PCM_GRP, PIN_STA_DEINIT);
        iRet += wmt_plat_gpio_ctrl(PIN_I2S_GRP, PIN_STA_DEINIT);
        break;

    case CMB_STUB_AIF_1:
        iRet += wmt_plat_gpio_ctrl(PIN_PCM_GRP, PIN_STA_INIT);
        iRet += wmt_plat_gpio_ctrl(PIN_I2S_GRP, PIN_STA_DEINIT);
        break;

    case CMB_STUB_AIF_2:
        iRet += wmt_plat_gpio_ctrl(PIN_PCM_GRP, PIN_STA_DEINIT);
        iRet += wmt_plat_gpio_ctrl(PIN_I2S_GRP, PIN_STA_INIT);
        break;

    case CMB_STUB_AIF_3:
        iRet += wmt_plat_gpio_ctrl(PIN_PCM_GRP, PIN_STA_INIT);
        iRet += wmt_plat_gpio_ctrl(PIN_I2S_GRP, PIN_STA_INIT);
        break;

    default:
        /* FIXME: move to cust folder? */
        WMT_ERR_FUNC("invalid state [%d]\n", state);
        return -1;
        break;
    }

    if (CMB_STUB_AIF_CTRL_EN == ctrl) {
        WMT_INFO_FUNC("call chip aif setting \n");
        /* need to control chip side GPIO */
        iRet += wmt_lib_set_aif(state, (pinShare) ? MTK_WCN_BOOL_TRUE : MTK_WCN_BOOL_FALSE);
    }
    else {
        WMT_INFO_FUNC("skip chip aif setting \n");
    }

    return iRet;

}

static VOID wmt_plat_func_ctrl (UINT32 type, UINT32 on)
{
    if (on) {
        mtk_wcn_wmt_func_on((ENUM_WMTDRV_TYPE_T)type);
    }
    else {
        mtk_wcn_wmt_func_off((ENUM_WMTDRV_TYPE_T)type);
    }
    return;
}

static VOID
wmt_plat_bgf_eirq_cb (VOID)
{
#if CFG_WMT_PS_SUPPORT
    /**/
//#error "need to disable EINT here"
    wmt_lib_ps_irq_cb();

#else
    return;
#endif

}

INT32
wmt_plat_init (P_PWR_SEQ_TIME pPwrSeqTime)
{
    CMB_STUB_CB stub_cb;
    /*PWR_SEQ_TIME pwr_seq_time;*/
    INT32 iret;

    stub_cb.aif_ctrl_cb = wmt_plat_audio_ctrl;
    stub_cb.func_ctrl_cb = wmt_plat_func_ctrl;
    stub_cb.size = sizeof(stub_cb);

    /* register to cmb_stub */
    iret = mtk_wcn_cmb_stub_reg(&stub_cb);

    /* init cmb_hw */
    iret += mtk_wcn_cmb_hw_init(pPwrSeqTime);

    /*init wmt function ctrl wakelock if wake lock is supported by host platform*/
    #ifdef CFG_WMT_WAKELOCK_SUPPORT
    wake_lock_init(&wmtWakeLock, WAKE_LOCK_SUSPEND, "wmtFuncCtrl");
    osal_sleepable_lock_init(&gOsSLock);
    #endif
    
    WMT_DBG_FUNC("WMT-PLAT: ALPS platform init (%d)\n", iret);

    return 0;
}


INT32
wmt_plat_deinit (VOID)
{
    INT32 iret;

    /* 1. de-init cmb_hw */
    iret = mtk_wcn_cmb_hw_deinit();
    /* 2. unreg to cmb_stub */
    iret += mtk_wcn_cmb_stub_unreg();
    /*3. wmt wakelock deinit*/
    #ifdef CFG_WMT_WAKELOCK_SUPPORT
    wake_lock_destroy(&wmtWakeLock);
    osal_sleepable_lock_deinit(&gOsSLock);
    WMT_DBG_FUNC("destroy wmtWakeLock\n");
    #endif
    WMT_DBG_FUNC("WMT-PLAT: ALPS platform init (%d)\n", iret);

    return 0;
}

INT32 wmt_plat_sdio_ctrl (WMT_SDIO_SLOT_NUM sdioPortType, ENUM_FUNC_STATE on)
{
    return board_sdio_ctrl(sdioPortType, (FUNC_OFF == on) ? 0 : 1);
}

INT32
wmt_plat_irq_ctrl (
    ENUM_FUNC_STATE state
    )
{
    return -1;
}


static INT32
wmt_plat_dump_pin_conf (VOID)
{
    WMT_INFO_FUNC( "[WMT-PLAT]=>dump wmt pin configuration start<=\n");

    #ifdef GPIO_COMBO_6620_LDO_EN_PIN
        WMT_INFO_FUNC( "LDO(GPIO%d)\n", GPIO_COMBO_6620_LDO_EN_PIN);
    #else
        WMT_INFO_FUNC( "LDO(not defined)\n");
    #endif

    #ifdef GPIO_COMBO_PMU_EN_PIN
        WMT_INFO_FUNC( "PMU(GPIO%d)\n", GPIO_COMBO_PMU_EN_PIN);
    #else
        WMT_INFO_FUNC( "PMU(not defined)\n");
    #endif

    #ifdef GPIO_COMBO_PMUV28_EN_PIN
        WMT_INFO_FUNC( "PMUV28(GPIO%d)\n", GPIO_COMBO_PMUV28_EN_PIN);
    #else
        WMT_INFO_FUNC( "PMUV28(not defined)\n");
    #endif

    #ifdef GPIO_COMBO_RST_PIN
        WMT_INFO_FUNC( "RST(GPIO%d)\n", GPIO_COMBO_RST_PIN);
    #else
        WMT_INFO_FUNC( "RST(not defined)\n");
    #endif

    #ifdef GPIO_COMBO_BGF_EINT_PIN
        WMT_INFO_FUNC( "BGF_EINT(GPIO%d)\n", GPIO_COMBO_BGF_EINT_PIN);
    #else
        WMT_INFO_FUNC( "BGF_EINT(not defined)\n");
    #endif

    #ifdef CUST_EINT_COMBO_BGF_NUM
        WMT_INFO_FUNC( "BGF_EINT_NUM(%d)\n", CUST_EINT_COMBO_BGF_NUM);
    #else
        WMT_INFO_FUNC( "BGF_EINT_NUM(not defined)\n");
    #endif

    #ifdef GPIO_WIFI_EINT_PIN
        WMT_INFO_FUNC( "WIFI_EINT(GPIO%d)\n", GPIO_WIFI_EINT_PIN);
    #else
        WMT_INFO_FUNC( "WIFI_EINT(not defined)\n");
    #endif

    #ifdef CUST_EINT_WIFI_NUM
        WMT_INFO_FUNC( "WIFI_EINT_NUM(%d)\n", CUST_EINT_WIFI_NUM);
    #else
        WMT_INFO_FUNC( "WIFI_EINT_NUM(not defined)\n");
    #endif

    #ifdef GPIO_UART_URXD3_PIN
        WMT_INFO_FUNC( "UART_RX(GPIO%d)\n", GPIO_UART_URXD3_PIN);
    #else
        WMT_INFO_FUNC( "UART_RX(not defined)\n");
    #endif
    #ifdef GPIO_UART_UTXD3_PIN
        WMT_INFO_FUNC( "UART_TX(GPIO%d)\n", GPIO_UART_UTXD3_PIN);
    #else
        WMT_INFO_FUNC( "UART_TX(not defined)\n");
    #endif
    #ifdef GPIO_PCM_DAICLK_PIN
        WMT_INFO_FUNC( "DAICLK(GPIO%d)\n", GPIO_PCM_DAICLK_PIN);
    #else
        WMT_INFO_FUNC( "DAICLK(not defined)\n");
    #endif
    #ifdef GPIO_PCM_DAIPCMOUT_PIN
        WMT_INFO_FUNC( "PCMOUT(GPIO%d)\n", GPIO_PCM_DAIPCMOUT_PIN);
    #else
        WMT_INFO_FUNC( "PCMOUT(not defined)\n");
    #endif
    #ifdef GPIO_PCM_DAIPCMIN_PIN
        WMT_INFO_FUNC( "PCMIN(GPIO%d)\n", GPIO_PCM_DAIPCMIN_PIN);
    #else
        WMT_INFO_FUNC( "PCMIN(not defined)\n");
    #endif
    #ifdef GPIO_PCM_DAISYNC_PIN
        WMT_INFO_FUNC( "PCMSYNC(GPIO%d)\n", GPIO_PCM_DAISYNC_PIN);
    #else
        WMT_INFO_FUNC( "PCMSYNC(not defined)\n");
    #endif
    #if defined(FM_DIGITAL_INPUT) || defined(FM_DIGITAL_OUTPUT)
        #ifdef GPIO_COMBO_I2S_CK_PIN
            WMT_INFO_FUNC( "I2S_CK(GPIO%d)\n", GPIO_COMBO_I2S_CK_PIN);
        #else
            WMT_INFO_FUNC( "I2S_CK(not defined)\n");
        #endif
        #ifdef GPIO_COMBO_I2S_WS_PIN
            WMT_INFO_FUNC( "I2S_WS(GPIO%d)\n", GPIO_COMBO_I2S_WS_PIN);
        #else
            WMT_INFO_FUNC( "I2S_WS(not defined)\n");
        #endif
        #ifdef GPIO_COMBO_I2S_DAT_PIN
            WMT_INFO_FUNC( "I2S_DAT(GPIO%d)\n", GPIO_COMBO_I2S_DAT_PIN);
        #else
            WMT_INFO_FUNC( "I2S_DAT(not defined)\n");
        #endif
    #else    //FM_ANALOG_INPUT || FM_ANALOG_OUTPUT
        WMT_INFO_FUNC( "FM digital mode is not set, no need for I2S GPIOs\n");
    #endif     
    #ifdef GPIO_GPS_SYNC_PIN
                WMT_INFO_FUNC( "GPS_SYNC(GPIO%d)\n", GPIO_GPS_SYNC_PIN);
    #else
                WMT_INFO_FUNC( "GPS_SYNC(not defined)\n");
    #endif

    #ifdef GPIO_GPS_LNA_PIN
                WMT_INFO_FUNC( "GPS_LNA(GPIO%d)\n", GPIO_GPS_LNA_PIN);
    #else
                WMT_INFO_FUNC( "GPS_LNA(not defined)\n");
    #endif
    WMT_INFO_FUNC( "[WMT-PLAT]=>dump wmt pin configuration emds<=\n");
    return 0;
}


INT32 wmt_plat_pwr_ctrl (
    ENUM_FUNC_STATE state
    )
{
    INT32 ret = -1;

    switch (state) {
    case FUNC_ON:
        // TODO:[ChangeFeature][George] always output this or by request throuth /proc or sysfs?
        wmt_plat_dump_pin_conf();
        ret = mtk_wcn_cmb_hw_pwr_on();
        break;

    case FUNC_OFF:
        ret = mtk_wcn_cmb_hw_pwr_off();
        break;

    case FUNC_RST:
        ret = mtk_wcn_cmb_hw_rst();
        break;

    default:
        WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) in pwr_ctrl\n", state);
        break;
    }

    return ret;
}

INT32 wmt_plat_ps_ctrl (ENUM_FUNC_STATE state)
{
    return -1;
}

INT32
wmt_plat_eirq_ctrl (
    ENUM_PIN_ID id,
    ENUM_PIN_STATE state
    )
{
    INT32 iret;

    // TODO: [ChangeFeature][GeorgeKuo]: use another function to handle this, as done in gpio_ctrls

    if ( (PIN_STA_INIT != state )
        && (PIN_STA_DEINIT != state )
        && (PIN_STA_EINT_EN != state )
        && (PIN_STA_EINT_DIS != state ) ) {
        WMT_WARN_FUNC("WMT-PLAT:invalid PIN_STATE(%d) in eirq_ctrl for PIN(%d)\n", state, id);
        return -1;
    }

    iret = -2;
    switch (id) {
    case PIN_BGF_EINT:
#ifdef GPIO_COMBO_BGF_EINT_PIN
        if (PIN_STA_INIT == state) {
            mt65xx_eint_set_sens(CUST_EINT_COMBO_BGF_NUM, CUST_EINT_COMBO_BGF_SENSITIVE);
            mt65xx_eint_set_hw_debounce(CUST_EINT_COMBO_BGF_NUM, CUST_EINT_COMBO_BGF_DEBOUNCE_CN);
            mt65xx_eint_registration(CUST_EINT_COMBO_BGF_NUM,
                CUST_EINT_COMBO_BGF_DEBOUNCE_EN,
                CUST_EINT_COMBO_BGF_POLARITY,
                wmt_plat_bgf_eirq_cb,
                0);
            mt65xx_eint_mask(CUST_EINT_COMBO_BGF_NUM); /*2*/
        }
        else if (PIN_STA_EINT_EN == state) {
             mt65xx_eint_unmask(CUST_EINT_COMBO_BGF_NUM);
             WMT_DBG_FUNC("WMT-PLAT:BGFInt (en) \n");
        }
        else if (PIN_STA_EINT_DIS == state) {
            mt65xx_eint_mask(CUST_EINT_COMBO_BGF_NUM);
            WMT_DBG_FUNC("WMT-PLAT:BGFInt (dis) \n");
        }
        else {
            mt65xx_eint_mask(CUST_EINT_COMBO_BGF_NUM);
            /* de-init: nothing to do in ALPS, such as un-registration... */
        }
#else
        WMT_INFO_FUNC("WMT-PLAT:BGF EINT not defined\n", state);
#endif
        iret = 0;
        break;

    case PIN_ALL_EINT:
#ifdef GPIO_COMBO_ALL_EINT_PIN
        if (PIN_STA_INIT == state) {
            #if 0
            mt65xx_eint_set_sens(CUST_EINT_COMBO_ALL_NUM, CUST_EINT_COMBO_ALL_SENSITIVE);
            mt65xx_eint_set_hw_debounce(CUST_EINT_COMBO_ALL_NUM, CUST_EINT_COMBO_ALL_DEBOUNCE_CN);
            mt65xx_eint_registration(CUST_EINT_COMBO_ALL_NUM,
                CUST_EINT_COMBO_ALL_DEBOUNCE_EN,
                CUST_EINT_COMBO_ALL_POLARITY,
                combo_bgf_eirq_handler,
                0);
            #endif
            mt65xx_eint_mask(CUST_EINT_COMBO_ALL_NUM); /*2*/
            WMT_DBG_FUNC("WMT-PLAT:ALLInt (INIT but not used yet) \n");
        }
        else if (PIN_STA_EINT_EN == state) {
             /*mt65xx_eint_unmask(CUST_EINT_COMBO_ALL_NUM);*/
             WMT_DBG_FUNC("WMT-PLAT:ALLInt (EN but not used yet) \n");
        }
        else if (PIN_STA_EINT_DIS == state) {
            mt65xx_eint_mask(CUST_EINT_COMBO_ALL_NUM);
            WMT_DBG_FUNC("WMT-PLAT:ALLInt (DIS but not used yet) \n");
        }
        else {
            mt65xx_eint_mask(CUST_EINT_COMBO_ALL_NUM);
            WMT_DBG_FUNC("WMT-PLAT:ALLInt (DEINIT but not used yet) \n");
            /* de-init: nothing to do in ALPS, such as un-registration... */
        }
#else
        WMT_INFO_FUNC("WMT-PLAT:ALL EINT not defined\n", state);
#endif
        iret = 0;
        break;

    default:
        WMT_WARN_FUNC("WMT-PLAT:unsupported EIRQ(PIN_ID:%d) in eirq_ctrl\n", id);
        iret = -1;
        break;
    }

    return iret;
}

INT32 wmt_plat_gpio_ctrl (
    ENUM_PIN_ID id,
    ENUM_PIN_STATE state
    )
{
    if ( (PIN_ID_MAX > id)
        && (PIN_STA_MAX > state) ) {

        // TODO: [FixMe][GeorgeKuo] do sanity check to const function table when init and skip checking here
        if (gfp_set_pin_table[id]) {
            return (*(gfp_set_pin_table[id]))(state); /* .handler */
        }
        else {
            WMT_WARN_FUNC("WMT-PLAT: null fp for gpio_ctrl(%d)\n", id);
            return -2;
        }
    }
    return -1;
}

INT32
wmt_plat_ldo_ctrl (
    ENUM_PIN_STATE state
    )
{
#ifdef GPIO_COMBO_6620_LDO_EN_PIN
    switch(state)
    {
    case PIN_STA_INIT:
        /*set to gpio output low, disable pull*/
        mt_set_gpio_pull_enable(GPIO_COMBO_6620_LDO_EN_PIN, GPIO_PULL_DISABLE);
        mt_set_gpio_dir(GPIO_COMBO_6620_LDO_EN_PIN, GPIO_DIR_OUT);
        mt_set_gpio_mode(GPIO_COMBO_6620_LDO_EN_PIN, GPIO_MODE_GPIO);
        mt_set_gpio_out(GPIO_COMBO_6620_LDO_EN_PIN, GPIO_OUT_ZERO);
        WMT_DBG_FUNC("WMT-PLAT:LDO init (out 0) \n");
        break;

    case PIN_STA_OUT_H:
        mt_set_gpio_out(GPIO_COMBO_6620_LDO_EN_PIN, GPIO_OUT_ONE);
        WMT_DBG_FUNC("WMT-PLAT:LDO (out 1) \n");
        break;

    case PIN_STA_OUT_L:
        mt_set_gpio_out(GPIO_COMBO_6620_LDO_EN_PIN, GPIO_OUT_ZERO);
        WMT_DBG_FUNC("WMT-PLAT:LDO (out 0) \n");
        break;

    case PIN_STA_IN_L:
    case PIN_STA_DEINIT:
        /*set to gpio input low, pull down enable*/
        mt_set_gpio_mode(GPIO_COMBO_6620_LDO_EN_PIN, GPIO_COMBO_6620_LDO_EN_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_COMBO_6620_LDO_EN_PIN, GPIO_DIR_IN);
        mt_set_gpio_pull_select(GPIO_COMBO_6620_LDO_EN_PIN, GPIO_PULL_DOWN);
        mt_set_gpio_pull_enable(GPIO_COMBO_6620_LDO_EN_PIN, GPIO_PULL_ENABLE);
        WMT_DBG_FUNC("WMT-PLAT:LDO deinit (in pd) \n");
        break;

    default:
        WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) on LDO\n", state);
        break;
    }
#else
    WMT_INFO_FUNC("WMT-PLAT:LDO is not used\n");
#endif
    return 0;
}

INT32
wmt_plat_pmu_ctrl (
    ENUM_PIN_STATE state
    )
{
    switch(state)
    {
    case PIN_STA_INIT:
        /*set to gpio output low, disable pull*/
        mt_set_gpio_pull_enable(GPIO_COMBO_PMU_EN_PIN, GPIO_PULL_DISABLE);
        mt_set_gpio_dir(GPIO_COMBO_PMU_EN_PIN, GPIO_DIR_OUT);
        mt_set_gpio_mode(GPIO_COMBO_PMU_EN_PIN, GPIO_MODE_GPIO);
        mt_set_gpio_out(GPIO_COMBO_PMU_EN_PIN, GPIO_OUT_ZERO);
#ifdef GPIO_COMBO_PMUV28_EN_PIN
        mt_set_gpio_pull_enable(GPIO_COMBO_PMUV28_EN_PIN, GPIO_PULL_DISABLE);
        mt_set_gpio_dir(GPIO_COMBO_PMUV28_EN_PIN, GPIO_DIR_OUT);
        mt_set_gpio_mode(GPIO_COMBO_PMUV28_EN_PIN, GPIO_MODE_GPIO);
        mt_set_gpio_out(GPIO_COMBO_PMUV28_EN_PIN, GPIO_OUT_ZERO);
#endif
        WMT_DBG_FUNC("WMT-PLAT:PMU init (out 0) \n");
        break;

    case PIN_STA_OUT_H:
        mt_set_gpio_out(GPIO_COMBO_PMU_EN_PIN, GPIO_OUT_ONE);
#ifdef GPIO_COMBO_PMUV28_EN_PIN
        mt_set_gpio_out(GPIO_COMBO_PMUV28_EN_PIN, GPIO_OUT_ONE);
#endif
        WMT_DBG_FUNC("WMT-PLAT:PMU (out 1) \n");
        break;

    case PIN_STA_OUT_L:
        mt_set_gpio_out(GPIO_COMBO_PMU_EN_PIN, GPIO_OUT_ZERO);
#ifdef GPIO_COMBO_PMUV28_EN_PIN
        mt_set_gpio_out(GPIO_COMBO_PMUV28_EN_PIN, GPIO_OUT_ZERO);
#endif
        WMT_DBG_FUNC("WMT-PLAT:PMU (out 0) \n");
        break;

    case PIN_STA_IN_L:
    case PIN_STA_DEINIT:
        /*set to gpio input low, pull down enable*/
        mt_set_gpio_mode(GPIO_COMBO_PMU_EN_PIN, GPIO_COMBO_PMU_EN_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_COMBO_PMU_EN_PIN, GPIO_DIR_IN);
        mt_set_gpio_pull_select(GPIO_COMBO_PMU_EN_PIN, GPIO_PULL_DOWN);
        mt_set_gpio_pull_enable(GPIO_COMBO_PMU_EN_PIN, GPIO_PULL_ENABLE);
#ifdef GPIO_COMBO_PMUV28_EN_PIN
        mt_set_gpio_mode(GPIO_COMBO_PMUV28_EN_PIN, GPIO_COMBO_PMUV28_EN_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_COMBO_PMUV28_EN_PIN, GPIO_DIR_IN);
        mt_set_gpio_pull_select(GPIO_COMBO_PMUV28_EN_PIN, GPIO_PULL_DOWN);
        mt_set_gpio_pull_enable(GPIO_COMBO_PMUV28_EN_PIN, GPIO_PULL_ENABLE);
#endif
        WMT_DBG_FUNC("WMT-PLAT:PMU deinit (in pd) \n");
        break;

    default:
        WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) on PMU\n", state);
        break;
    }

    return 0;
}

INT32
wmt_plat_rtc_ctrl (
    ENUM_PIN_STATE state
    )
{
    switch(state)
    {
    case PIN_STA_INIT:
        rtc_gpio_enable_32k(RTC_GPIO_USER_GPS);
        WMT_DBG_FUNC("WMT-PLAT:RTC init \n");
        break;

    default:
        WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) on RTC\n", state);
        break;
    }
    return 0;
}


INT32
wmt_plat_rst_ctrl (
    ENUM_PIN_STATE state
    )
{
    switch(state)
    {
        case PIN_STA_INIT:
            /*set to gpio output low, disable pull*/
            mt_set_gpio_pull_enable(GPIO_COMBO_RST_PIN, GPIO_PULL_DISABLE);
            mt_set_gpio_dir(GPIO_COMBO_RST_PIN, GPIO_DIR_OUT);
            mt_set_gpio_mode(GPIO_COMBO_RST_PIN, GPIO_MODE_GPIO);
            mt_set_gpio_out(GPIO_COMBO_RST_PIN, GPIO_OUT_ZERO);
            WMT_DBG_FUNC("WMT-PLAT:RST init (out 0) \n");
            break;

        case PIN_STA_OUT_H:
            mt_set_gpio_out(GPIO_COMBO_RST_PIN, GPIO_OUT_ONE);
            WMT_DBG_FUNC("WMT-PLAT:RST (out 1) \n");
            break;

        case PIN_STA_OUT_L:
            mt_set_gpio_out(GPIO_COMBO_RST_PIN, GPIO_OUT_ZERO);
            WMT_DBG_FUNC("WMT-PLAT:RST (out 0) \n");
            break;

        case PIN_STA_IN_L:
        case PIN_STA_DEINIT:
            /*set to gpio input low, pull down enable*/
            mt_set_gpio_mode(GPIO_COMBO_RST_PIN, GPIO_COMBO_RST_PIN_M_GPIO);
            mt_set_gpio_dir(GPIO_COMBO_RST_PIN, GPIO_DIR_IN);
            mt_set_gpio_pull_select(GPIO_COMBO_RST_PIN, GPIO_PULL_DOWN);
            mt_set_gpio_pull_enable(GPIO_COMBO_RST_PIN, GPIO_PULL_ENABLE);
            WMT_DBG_FUNC("WMT-PLAT:RST deinit (in pd) \n");
            break;

        default:
            WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) on RST\n", state);
            break;
    }

    return 0;
}

INT32
wmt_plat_bgf_eint_ctrl (
    ENUM_PIN_STATE state
    )
{
#ifdef GPIO_COMBO_BGF_EINT_PIN
    switch(state)
    {
        case PIN_STA_INIT:
            /*set to gpio input low, pull down enable*/
            mt_set_gpio_mode(GPIO_COMBO_BGF_EINT_PIN, GPIO_COMBO_BGF_EINT_PIN_M_GPIO);
            mt_set_gpio_dir(GPIO_COMBO_BGF_EINT_PIN, GPIO_DIR_IN);
            mt_set_gpio_pull_select(GPIO_COMBO_BGF_EINT_PIN, GPIO_PULL_DOWN);
            mt_set_gpio_pull_enable(GPIO_COMBO_BGF_EINT_PIN, GPIO_PULL_ENABLE);
            WMT_DBG_FUNC("WMT-PLAT:BGFInt init(in pd) \n");
            break;

        case PIN_STA_MUX:
            mt_set_gpio_mode(GPIO_COMBO_BGF_EINT_PIN, GPIO_COMBO_BGF_EINT_PIN_M_GPIO);
            mt_set_gpio_pull_enable(GPIO_COMBO_BGF_EINT_PIN, GPIO_PULL_ENABLE);
            mt_set_gpio_pull_select(GPIO_COMBO_BGF_EINT_PIN, GPIO_PULL_UP);
            mt_set_gpio_mode(GPIO_COMBO_BGF_EINT_PIN, GPIO_COMBO_BGF_EINT_PIN_M_EINT);
            WMT_DBG_FUNC("WMT-PLAT:BGFInt mux (eint) \n");
            break;

        case PIN_STA_IN_L:
        case PIN_STA_DEINIT:
            /*set to gpio input low, pull down enable*/
            mt_set_gpio_mode(GPIO_COMBO_BGF_EINT_PIN, GPIO_COMBO_BGF_EINT_PIN_M_GPIO);
            mt_set_gpio_dir(GPIO_COMBO_BGF_EINT_PIN, GPIO_DIR_IN);
            mt_set_gpio_pull_select(GPIO_COMBO_BGF_EINT_PIN, GPIO_PULL_DOWN);
            mt_set_gpio_pull_enable(GPIO_COMBO_BGF_EINT_PIN, GPIO_PULL_ENABLE);
            WMT_DBG_FUNC("WMT-PLAT:BGFInt deinit(in pd) \n");
            break;

        default:
            WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) on BGF EINT\n", state);
            break;
    }
#else
    WMT_INFO_FUNC("WMT-PLAT:BGF EINT not defined\n", state);
#endif
    return 0;
}


INT32 wmt_plat_wifi_eint_ctrl(ENUM_PIN_STATE state)
{
#if 0 /*def GPIO_WIFI_EINT_PIN*/
    switch(state)
    {
        case PIN_STA_INIT:
            mt_set_gpio_pull_enable(GPIO_WIFI_EINT_PIN, GPIO_PULL_DISABLE);
            mt_set_gpio_dir(GPIO_WIFI_EINT_PIN, GPIO_DIR_OUT);
            mt_set_gpio_mode(GPIO_WIFI_EINT_PIN, GPIO_MODE_GPIO);
            mt_set_gpio_out(GPIO_WIFI_EINT_PIN, GPIO_OUT_ONE);
            break;
        case PIN_STA_MUX:
            mt_set_gpio_mode(GPIO_WIFI_EINT_PIN, GPIO_WIFI_EINT_PIN_M_GPIO);
            mt_set_gpio_pull_enable(GPIO_WIFI_EINT_PIN, GPIO_PULL_ENABLE);
            mt_set_gpio_pull_select(GPIO_WIFI_EINT_PIN, GPIO_PULL_UP);
            mt_set_gpio_mode(GPIO_WIFI_EINT_PIN, GPIO_WIFI_EINT_PIN_M_EINT);

            break;
        case PIN_STA_EINT_EN:
            mt65xx_eint_unmask(CUST_EINT_WIFI_NUM);
            break;
        case PIN_STA_EINT_DIS:
            mt65xx_eint_mask(CUST_EINT_WIFI_NUM);
            break;
        case PIN_STA_IN_L:
        case PIN_STA_DEINIT:
            /*set to gpio input low, pull down enable*/
            mt_set_gpio_mode(GPIO_WIFI_EINT_PIN, GPIO_COMBO_BGF_EINT_PIN_M_GPIO);
            mt_set_gpio_dir(GPIO_WIFI_EINT_PIN, GPIO_DIR_IN);
            mt_set_gpio_pull_select(GPIO_WIFI_EINT_PIN, GPIO_PULL_DOWN);
            mt_set_gpio_pull_enable(GPIO_WIFI_EINT_PIN, GPIO_PULL_ENABLE);
            break;
        default:
            WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) on WIFI EINT\n", state);
            break;
    }
#else
    WMT_INFO_FUNC("WMT-PLAT:WIFI EINT is controlled by MSDC driver \n");
#endif
    return 0;
}


INT32
wmt_plat_all_eint_ctrl (
    ENUM_PIN_STATE state
    )
{
#ifdef GPIO_COMBO_ALL_EINT_PIN
    switch(state)
    {
        case PIN_STA_INIT:
            mt_set_gpio_mode(GPIO_COMBO_ALL_EINT_PIN, GPIO_COMBO_ALL_EINT_PIN_M_GPIO);
            mt_set_gpio_dir(GPIO_COMBO_ALL_EINT_PIN, GPIO_DIR_IN);
            mt_set_gpio_pull_select(GPIO_COMBO_ALL_EINT_PIN, GPIO_PULL_DOWN);
            mt_set_gpio_pull_enable(GPIO_COMBO_ALL_EINT_PIN, GPIO_PULL_ENABLE);
            WMT_DBG_FUNC("WMT-PLAT:ALLInt init(in pd) \n");
            break;

        case PIN_STA_MUX:
            mt_set_gpio_mode(GPIO_COMBO_ALL_EINT_PIN, GPIO_COMBO_ALL_EINT_PIN_M_GPIO);
            mt_set_gpio_pull_enable(GPIO_COMBO_ALL_EINT_PIN, GPIO_PULL_ENABLE);
            mt_set_gpio_pull_select(GPIO_COMBO_ALL_EINT_PIN, GPIO_PULL_UP);
            mt_set_gpio_mode(GPIO_COMBO_ALL_EINT_PIN, GPIO_COMBO_ALL_EINT_PIN_M_EINT);
            break;

        case PIN_STA_IN_L:
        case PIN_STA_DEINIT:
            /*set to gpio input low, pull down enable*/
            mt_set_gpio_mode(GPIO_COMBO_ALL_EINT_PIN, GPIO_COMBO_ALL_EINT_PIN_M_GPIO);
            mt_set_gpio_dir(GPIO_COMBO_ALL_EINT_PIN, GPIO_DIR_IN);
            mt_set_gpio_pull_select(GPIO_COMBO_ALL_EINT_PIN, GPIO_PULL_DOWN);
            mt_set_gpio_pull_enable(GPIO_COMBO_ALL_EINT_PIN, GPIO_PULL_ENABLE);
            break;

        default:
            WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) on ALL EINT\n", state);
            break;
    }
#else
    WMT_INFO_FUNC("WMT-PLAT:ALL EINT not defined\n");
#endif
    return 0;
}

INT32 wmt_plat_uart_ctrl(ENUM_PIN_STATE state)
{
    switch(state)
    {
    case PIN_STA_MUX:
    case PIN_STA_INIT:
        mt_set_gpio_mode(GPIO_UART_URXD3_PIN, GPIO_UART_URXD3_PIN_M_URXD);
        mt_set_gpio_mode(GPIO_UART_UTXD3_PIN, GPIO_UART_UTXD3_PIN_M_UTXD);
        WMT_DBG_FUNC("WMT-PLAT:UART init (mode_01, uart) \n");
        break;
    case PIN_STA_IN_L:
    case PIN_STA_DEINIT:
        mt_set_gpio_mode(GPIO_UART_URXD3_PIN, GPIO_UART_URXD3_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_UART_URXD3_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_UART_URXD3_PIN, GPIO_OUT_ZERO);

        mt_set_gpio_mode(GPIO_UART_UTXD3_PIN, GPIO_UART_UTXD3_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_UART_UTXD3_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_UART_UTXD3_PIN, GPIO_OUT_ZERO);
        WMT_DBG_FUNC("WMT-PLAT:UART deinit (out 0) \n");
        break;

    default:
        WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) on UART Group\n", state);
        break;
    }

    return 0;
}


INT32 wmt_plat_pcm_ctrl(ENUM_PIN_STATE state)
{

    switch(state)
    {
    case PIN_STA_MUX:
    case PIN_STA_INIT:
        mt_set_gpio_mode(GPIO_PCM_DAICLK_PIN, GPIO_PCM_DAICLK_PIN_M_CLK);
        mt_set_gpio_mode(GPIO_PCM_DAIPCMOUT_PIN, GPIO_PCM_DAIPCMOUT_PIN_M_DAIPCMOUT);
        mt_set_gpio_mode(GPIO_PCM_DAIPCMIN_PIN, GPIO_PCM_DAIPCMIN_PIN_M_DAIPCMIN);
        mt_set_gpio_mode(GPIO_PCM_DAISYNC_PIN, GPIO_PCM_DAISYNC_PIN_M_BTSYNC);
        WMT_DBG_FUNC("WMT-PLAT:PCM init (pcm) \n");
        break;

    case PIN_STA_IN_L:
    case PIN_STA_DEINIT:
        mt_set_gpio_mode(GPIO_PCM_DAICLK_PIN, GPIO_PCM_DAICLK_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_PCM_DAICLK_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_PCM_DAICLK_PIN, GPIO_OUT_ZERO);

        mt_set_gpio_mode(GPIO_PCM_DAIPCMOUT_PIN, GPIO_PCM_DAIPCMOUT_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_PCM_DAIPCMOUT_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_PCM_DAIPCMOUT_PIN, GPIO_OUT_ZERO);

        mt_set_gpio_mode(GPIO_PCM_DAIPCMIN_PIN, GPIO_PCM_DAIPCMIN_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_PCM_DAIPCMIN_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_PCM_DAIPCMIN_PIN, GPIO_OUT_ZERO);

        mt_set_gpio_mode(GPIO_PCM_DAISYNC_PIN, GPIO_PCM_DAISYNC_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_PCM_DAISYNC_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_PCM_DAISYNC_PIN, GPIO_OUT_ZERO);
        WMT_DBG_FUNC("WMT-PLAT:PCM deinit (out 0) \n");
        break;

    default:
        WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) on PCM Group\n", state);
        break;
    }
    return 0;
}


INT32 wmt_plat_i2s_ctrl(ENUM_PIN_STATE state)
{
    // TODO: [NewFeature][GeorgeKuo]: GPIO_I2Sx is changed according to different project.
    // TODO: provide a translation table in board_custom.h for different ALPS project customization.
#if defined(FM_DIGITAL_INPUT) || defined(FM_DIGITAL_OUTPUT)
    #if defined (GPIO_COMBO_I2S_CK_PIN)
    switch(state)
    {
    case PIN_STA_INIT:
    case PIN_STA_MUX:
            mt_set_gpio_mode(GPIO_COMBO_I2S_CK_PIN, GPIO_COMBO_I2S_CK_PIN_M_I2S0_CK);
            mt_set_gpio_mode(GPIO_COMBO_I2S_WS_PIN, GPIO_COMBO_I2S_WS_PIN_M_I2S0_WS);
            mt_set_gpio_mode(GPIO_COMBO_I2S_DAT_PIN, GPIO_COMBO_I2S_DAT_PIN_M_I2S0_DAT);
            WMT_DBG_FUNC("WMT-PLAT:I2S init (I2S0 system) \n");
        break;
    case PIN_STA_IN_L:
    case PIN_STA_DEINIT:
            mt_set_gpio_mode(GPIO_COMBO_I2S_CK_PIN, GPIO_COMBO_I2S_CK_PIN_M_GPIO);
            mt_set_gpio_dir(GPIO_COMBO_I2S_CK_PIN, GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_COMBO_I2S_CK_PIN, GPIO_OUT_ZERO);

            mt_set_gpio_mode(GPIO_COMBO_I2S_WS_PIN, GPIO_COMBO_I2S_WS_PIN_M_GPIO);
            mt_set_gpio_dir(GPIO_COMBO_I2S_WS_PIN, GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_COMBO_I2S_WS_PIN, GPIO_OUT_ZERO);

            mt_set_gpio_mode(GPIO_COMBO_I2S_DAT_PIN, GPIO_COMBO_I2S_DAT_PIN_M_GPIO);
            mt_set_gpio_dir(GPIO_COMBO_I2S_DAT_PIN, GPIO_DIR_OUT);
            mt_set_gpio_out(GPIO_COMBO_I2S_DAT_PIN, GPIO_OUT_ZERO);
            WMT_DBG_FUNC("WMT-PLAT:I2S deinit (out 0) \n");
        break;
    default:
        WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) on I2S Group\n", state);
        break;
    }
    #else
    WMT_ERR_FUNC( "[MT6620]Error:FM digital mode set, but no I2S GPIOs defined\n");
    #endif
#else
        WMT_INFO_FUNC( "[MT6620]warnning:FM digital mode is not set, no I2S GPIO settings should be modified by combo driver\n");
#endif

    return 0;
}

INT32
wmt_plat_sdio_pin_ctrl (
    ENUM_PIN_STATE state
    )
{
#if 0
    switch (state) {
    case PIN_STA_INIT:
    case PIN_STA_MUX:
#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
    #if (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 1)
        // TODO: [FixMe][GeorgeKuo]: below are used for MT6573 only! Find a better way to do ALPS customization for different platform.
        //WMT_INFO_FUNC( "[mt6620] pull up sd1 bus(gpio62~68)\n");
        mt_set_gpio_pull_enable(GPIO62, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO62, GPIO_PULL_UP);
        mt_set_gpio_pull_enable(GPIO63, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO63, GPIO_PULL_UP);
        mt_set_gpio_pull_enable(GPIO64, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO64, GPIO_PULL_UP);
        mt_set_gpio_pull_enable(GPIO65, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO65, GPIO_PULL_UP);
        mt_set_gpio_pull_enable(GPIO66, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO66, GPIO_PULL_UP);
        mt_set_gpio_pull_enable(GPIO67, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO67, GPIO_PULL_UP);
        mt_set_gpio_pull_enable(GPIO68, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO68, GPIO_PULL_UP);
        WMT_DBG_FUNC("WMT-PLAT:SDIO init (pu) \n");
    #elif (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 2)
        #error "fix sdio2 gpio settings"
    #endif
#else
    #error "CONFIG_MTK_WCN_CMB_SDIO_SLOT undefined!!!"
#endif

        break;

    case PIN_STA_DEINIT:
#if defined(CONFIG_MTK_WCN_CMB_SDIO_SLOT)
    #if (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 1)
        // TODO: [FixMe][GeorgeKuo]: below are used for MT6573 only! Find a better way to do ALPS customization for different platform.
        //WMT_INFO_FUNC( "[mt6620] pull down sd1 bus(gpio62~68)\n");
        mt_set_gpio_pull_select(GPIO62, GPIO_PULL_DOWN);
        mt_set_gpio_pull_enable(GPIO62, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO63, GPIO_PULL_DOWN);
        mt_set_gpio_pull_enable(GPIO63, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO64, GPIO_PULL_DOWN);
        mt_set_gpio_pull_enable(GPIO64, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO65, GPIO_PULL_DOWN);
        mt_set_gpio_pull_enable(GPIO65, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO66, GPIO_PULL_DOWN);
        mt_set_gpio_pull_enable(GPIO66, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO67, GPIO_PULL_DOWN);
        mt_set_gpio_pull_enable(GPIO67, GPIO_PULL_ENABLE);
        mt_set_gpio_pull_select(GPIO68, GPIO_PULL_DOWN);
        mt_set_gpio_pull_enable(GPIO68, GPIO_PULL_ENABLE);
        WMT_DBG_FUNC("WMT-PLAT:SDIO deinit (pd) \n");
    #elif (CONFIG_MTK_WCN_CMB_SDIO_SLOT == 2)
        #error "fix sdio2 gpio settings"
    #endif
#else
    #error "CONFIG_MTK_WCN_CMB_SDIO_SLOT undefined!!!"
#endif
        break;

    default:
        WMT_WARN_FUNC("WMT-PLAT:Warnning, invalid state(%d) on SDIO Group\n", state);
        break;
    }
#endif
    return 0;
}

static INT32
wmt_plat_gps_sync_ctrl (
    ENUM_PIN_STATE state
    )
{
#ifdef GPIO_GPS_SYNC_PIN
    switch (state) {
    case PIN_STA_INIT:
    case PIN_STA_DEINIT:
        mt_set_gpio_mode(GPIO_GPS_SYNC_PIN, GPIO_GPS_SYNC_PIN_M_GPIO);
        mt_set_gpio_dir(GPIO_GPS_SYNC_PIN, GPIO_DIR_OUT);
        mt_set_gpio_out(GPIO_GPS_SYNC_PIN, GPIO_OUT_ZERO);
        break;

    case PIN_STA_MUX:
        mt_set_gpio_mode(GPIO_GPS_SYNC_PIN, GPIO_GPS_SYNC_PIN_M_GPS_SYNC);
        break;

    default:
        break;
    }
#endif
    return 0;
}


static INT32
wmt_plat_gps_lna_ctrl (
        ENUM_PIN_STATE state
        )
{
#ifdef GPIO_GPS_LNA_PIN
    switch (state) {
    case PIN_STA_INIT:
    case PIN_STA_DEINIT:
        mt_set_gpio_pull_enable(GPIO_GPS_LNA_PIN, GPIO_PULL_DISABLE);
        mt_set_gpio_dir(GPIO_GPS_LNA_PIN, GPIO_DIR_OUT);
        mt_set_gpio_mode(GPIO_GPS_LNA_PIN, GPIO_GPS_LNA_PIN_M_GPIO);
        mt_set_gpio_out(GPIO_GPS_LNA_PIN, GPIO_OUT_ZERO);
        break;
    case PIN_STA_OUT_H:
        mt_set_gpio_out(GPIO_GPS_LNA_PIN, GPIO_OUT_ONE);
        break;
    case PIN_STA_OUT_L:
        mt_set_gpio_out(GPIO_GPS_LNA_PIN, GPIO_OUT_ZERO);
        break;

    default:
        WMT_WARN_FUNC("%d mode not defined for  gps lna pin !!!\n", state);
        break;
    }
    return 0;
#else
    WMT_WARN_FUNC("host gps lna pin not defined!!!\n")
    return 0;
#endif
}



INT32 wmt_plat_wake_lock_ctrl(ENUM_WL_OP opId)
{
#ifdef CFG_WMT_WAKELOCK_SUPPORT
    static INT32 counter = 0;
    
    
    osal_lock_sleepable_lock( &gOsSLock);
    if (WL_OP_GET == opId)
    {
        ++counter;
    }else if (WL_OP_PUT == opId)
    {
        --counter;
    }
    osal_unlock_sleepable_lock( &gOsSLock);
    if (WL_OP_GET == opId && counter == 1)
    {
        wake_lock(&wmtWakeLock);
        WMT_DBG_FUNC("WMT-PLAT: after wake_lock(%d), counter(%d)\n", wake_lock_active(&wmtWakeLock), counter);    
        
    }
    else if (WL_OP_PUT == opId && counter == 0)
    {
        wake_unlock(&wmtWakeLock);
        WMT_DBG_FUNC("WMT-PLAT: after wake_unlock(%d), counter(%d)\n", wake_lock_active(&wmtWakeLock), counter); 
    }
    else
    {
        WMT_WARN_FUNC("WMT-PLAT: wakelock status(%d), counter(%d)\n", wake_lock_active(&wmtWakeLock), counter); 
    }
    return 0;
#else
    WMT_WARN_FUNC("WMT-PLAT: host awake function is not supported.");
    return 0;

#endif
}

