#include <asm/io.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/xlog.h>

#include <asm/uaccess.h>

#include <mach/mt6577_typedefs.h>
#include <mach/pmic_mt6329_hw_bank1.h>
#include <mach/pmic_mt6329_sw_bank1.h>
#include <mach/pmic_mt6329_hw.h>
#include <mach/pmic_mt6329_sw.h>
#include <mach/upmu_common_sw.h>
#include <mach/upmu_hw.h>
#include <mach/mt6577_pmic_feature_api.h>

int gDebugLog=1;

/****************************************************************
 * GLOBAL DEFINATION
 ****************************************************************/

/****************************************************************
 * FUNCTION DEFINATIONS
 *******************************************************************/

kal_uint32 get_pmic6329_chip_version (void)
{
   	kal_uint32 eco_version = 0;
    kal_uint32 tmp32;
	//kal_uint32 ret=0;

    /* Low part of CID */
    tmp32=upmu_get_cid0();
    eco_version |= tmp32;
    /* High part of CID */
    tmp32=upmu_get_cid1();
    eco_version |= (tmp32 << 8);

    return eco_version;
}

//--------------------------------------------------------------
//LDO for low power mode
//--------------------------------------------------------------
void hwSetLowPowerMode_VM12_1(kal_uint32 modeSet, kal_uint32 modeSel)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSetLowPowerMode_VM12_1]...........\r\n");
	}

	upmu_ldo_lp_set(LDO_VM12_1, modeSet);
	upmu_ldo_lp_sel(LDO_VM12_1, modeSel);
}

void hwSetLowPowerMode_VM12_2(kal_uint32 modeSet, kal_uint32 modeSel)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSetLowPowerMode_VM12_2]...........\r\n");
	}

	upmu_ldo_lp_set(LDO_VM12_2, modeSet);
	upmu_ldo_lp_sel(LDO_VM12_2, modeSel);
}

void hwSetLowPowerMode_VM12_INT(kal_uint32 modeSet, kal_uint32 modeSel)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSetLowPowerMode_VM12_INT]...........\r\n");
	}

	upmu_ldo_lp_set(LDO_VM12_INT, modeSet);
	upmu_ldo_lp_sel(LDO_VM12_INT, modeSel);
}

void hwSetLowPowerMode_VMC(kal_uint32 modeSet, kal_uint32 modeSel)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSetLowPowerMode_VMC]...........\r\n");
	}

	upmu_ldo_lp_set(LDO_VMC, modeSet);
	upmu_ldo_lp_sel(LDO_VMC, modeSel);
}

void hwSetLowPowerMode_VMCH(kal_uint32 modeSet, kal_uint32 modeSel)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSetLowPowerMode_VMCH]...........\r\n");
	}

	upmu_ldo_lp_set(LDO_VMCH, modeSet);
	upmu_ldo_lp_sel(LDO_VMCH, modeSel);
}

void hwSetLowPowerMode_VRF28(kal_uint32 modeSel)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSetLowPowerMode_VRF28]...........\r\n");
	}

	upmu_ldo_on_ctrl(LDO_VRF, modeSel);
}

void hwSetLowPowerMode_VTCXO(kal_uint32 modeSel)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSetLowPowerMode_VTCXO]...........\r\n");
	}

	upmu_ldo_on_ctrl(LDO_VTCXO, modeSel);
}

void hwSetLowPowerMode_VA1(kal_uint32 modeSet, kal_uint32 modeSel)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSetLowPowerMode_VA1]...........\r\n");
	}

	upmu_ldo_lp_set(LDO_VA1, modeSet);
	upmu_ldo_lp_sel(LDO_VA1, modeSel);
}

//--------------------------------------------------------------
//Backlight
//--------------------------------------------------------------
void hwPWMsetting(MT65XX_PMIC_PWM_NUMBER PWMmode, kal_uint32 duty, kal_uint32 freqSelect)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwPWMsetting]...........\r\n");
	}

	if (PWMmode == PMIC_PWM_0)
	{
		upmu_isinks_dim0_duty(duty);
		upmu_isinks_dim0_fsel(freqSelect);
	}
	else if (PWMmode == PMIC_PWM_1)
	{
		upmu_isinks_dim1_duty(duty);
		upmu_isinks_dim1_fsel(freqSelect);
	}
	else if (PWMmode == PMIC_PWM_2)
	{
		upmu_isinks_dim2_duty(duty);
		upmu_isinks_dim2_fsel(freqSelect);
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwPWMsetting] Error PWMmode number\r\n");
	}
}

void hwBacklightBoostTuning(kal_uint32 MODE, kal_uint32 VRSEL, kal_uint32 CABC_EN)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightBoostTuning]...........\r\n");
	}

	upmu_boost_isink_hw_sel(0x1);
	upmu_boost_mode(MODE);
	upmu_boost_vrsel(VRSEL);
	upmu_boost_cabc_en(CABC_EN);
}

void hwBacklightBoostTurnOn(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightBoostTurnOn]...........\r\n");
	}

	upmu_top2_bst_drv_ck_pdn(0x0);
	upmu_boost_en(0x1);
	upmu_isinks_ch5_en(0x1);
	upmu_isinks_ch4_en(0x1);
	upmu_isinks_ch0_en(0x1);
}

void hwBacklightBoostTurnOff(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightBoostTurnOff]...........\r\n");
	}

	upmu_boost_en(0x0);
	upmu_top2_bst_drv_ck_pdn(0x1);
	upmu_isinks_ch5_en(0x0);
	upmu_isinks_ch4_en(0x0);
	upmu_isinks_ch0_en(0x0);
}

void hwBacklightISINKTuning(kal_uint32 CH_NUM, kal_uint32 MODE, kal_uint32 STEP, kal_uint32 CABC_EN)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightISINKTuning]...........\r\n");
	}

	if (CH_NUM == 0)
	{
		upmu_isinks_ch0_mode(MODE);
		upmu_isinks_ch0_step(STEP);
		upmu_isinks_ch0_cabc_en(CABC_EN);
	}
	else if (CH_NUM == 1)
	{
		upmu_boost_isink_hw_sel(0x0);
		upmu_isinks_ch1_mode(MODE);
		upmu_isinks_ch1_step(STEP);
		upmu_isinks_ch1_cabc_en(CABC_EN);
	}
	else if (CH_NUM == 2)
	{
		upmu_boost_isink_hw_sel(0x0);
		upmu_isinks_ch2_mode(MODE);
		upmu_isinks_ch2_step(STEP);
		upmu_isinks_ch2_cabc_en(CABC_EN);
	}
	else if (CH_NUM == 3)
	{
		upmu_boost_isink_hw_sel(0x0);
		upmu_isinks_ch3_mode(MODE);
		upmu_isinks_ch3_step(STEP);
		upmu_isinks_ch3_cabc_en(CABC_EN);
	}
	else if (CH_NUM == 4)
	{
		upmu_isinks_ch4_mode(MODE);
		upmu_isinks_ch4_step(STEP);
		upmu_isinks_ch4_cabc_en(CABC_EN);
	}
	else if (CH_NUM == 5)
	{
		upmu_isinks_ch5_mode(MODE);
		upmu_isinks_ch5_step(STEP);
		upmu_isinks_ch5_cabc_en(CABC_EN);
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwPWMsetting] Error channel number\r\n");
	}

}

void hwBacklightISINKTurnOn(kal_uint32 CH_NUM)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightISINKTurnOn]...........\r\n");
	}

	if (CH_NUM == 0)
	{
		upmu_isinks_ch0_en(0x1);
	}
	else if (CH_NUM == 1)
	{
		upmu_isinks_ch1_en(0x1);
	}
	else if (CH_NUM == 2)
	{
		upmu_isinks_ch2_en(0x1);
	}
	else if (CH_NUM == 3)
	{
		upmu_isinks_ch3_en(0x1);
	}
	else if (CH_NUM == 4)
	{
		upmu_isinks_ch4_en(0x1);
	}
	else if (CH_NUM == 5)
	{
		upmu_isinks_ch5_en(0x1);
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwPWMsetting] Error channel number\r\n");
	}

}

void hwBacklightISINKTurnOff(kal_uint32 CH_NUM)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightISINKTurnOff]...........\r\n");
	}

	if (CH_NUM == 0)
	{
		upmu_isinks_ch0_en(0x0);
	}
	else if (CH_NUM == 1)
	{
		upmu_isinks_ch1_en(0x0);
	}
	else if (CH_NUM == 2)
	{
		upmu_isinks_ch2_en(0x0);
	}
	else if (CH_NUM == 3)
	{
		upmu_isinks_ch3_en(0x0);
	}
	else if (CH_NUM == 4)
	{
		upmu_isinks_ch4_en(0x0);
	}
	else if (CH_NUM == 5)
	{
		upmu_isinks_ch5_en(0x0);
	}
	else
	{
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwPWMsetting] Error channel number\r\n");
	}

}

void hwBacklightFlashlightTuning(kal_uint32 duty, kal_uint32 mode)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightFlashlightTuning]...........\r\n");
	}

	upmu_flash_dim_duty(duty);
	upmu_flash_mode_select(mode);
}

void hwBacklightFlashlightTuning_v2(kal_uint32 mode, kal_uint32 duty, kal_uint32 div)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightFlashlightTuning_v2]...........\r\n");
	}

	upmu_flash_mode_select(mode);
	upmu_flash_dim_duty(duty);
	upmu_flash_dim_div(div);
}

void hwBacklightFlashlightTurnOn(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightFlashlightTurnOn]...........\r\n");
	}

	upmu_flash_en(0x1);
}

void hwBacklightFlashlightTurnOff(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightFlashlightTurnOff]...........\r\n");
	}

	upmu_flash_en(0x0);
}

void hwBacklightKPLEDTuning(kal_uint32 duty, kal_uint32 mode)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightKPLEDTuning]...........\r\n");
	}

	upmu_kpled_dim_duty(duty);
	upmu_kpled_mode_select(mode);
}

void hwBacklightKPLEDTuning_v2(kal_uint32 mode, kal_uint32 duty, kal_uint32 div)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightKPLEDTuning_v2]...........\r\n");
	}

	upmu_kpled_mode_select(mode);
	upmu_kpled_dim_duty(duty);
	upmu_kpled_dim_div(div);
}

void hwBacklightKPLEDTurnOn(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightKPLEDTurnOn]...........\r\n");
	}

	upmu_kpled_en(0x1);
}

void hwBacklightKPLEDTurnOff(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwBacklightKPLEDTurnOff]...........\r\n");
	}

	upmu_kpled_en(0x0);
}

//--------------------------------------------------------------
//Speaker
//--------------------------------------------------------------
void hwSPKClassDInit(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKClassDInit]...........\r\n");
	}

	upmu_spk_mode_l(0x0);
	upmu_spk_mode_r(0x0);
}

void hwSPKClassDTurnOn(kal_uint32 EN_L, kal_uint32 EN_R)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKClassDTurnOn]...........\r\n");
	}

	upmu_top2_spk_ck_pdn(0x0);
    if(EN_L>0)
	    upmu_spk_en_l(0x1);
    if(EN_R>0)
	    upmu_spk_en_r(0x1);
}

void hwSPKClassDTurnOff(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKClassDTurnOff]...........\r\n");
	}

	upmu_spk_en_l(0x0);
	upmu_spk_en_r(0x0);
	upmu_top2_spk_ck_pdn(0x1);
}

void hwSPKClassDVolumeControl(kal_uint32 VOL_L, kal_uint32 VOL_R)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKClassDVolumeControl]...........\r\n");
	}

	upmu_spk_vol_l(VOL_L);
	upmu_spk_vol_r(VOL_R);
}

void hwSPKClassABInit(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKClassABInit]...........\r\n");
	}

	upmu_spk_mode_l(0x1);
	upmu_spk_mode_r(0x1);
}

void hwSPKClassABTurnOn(kal_uint32 EN_L, kal_uint32 EN_R)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKClassABTurnOn]...........\r\n");
	}

	upmu_top2_spk_ck_pdn(0x0);
    if(EN_L>0)
	    upmu_spk_en_l(0x1);
    if(EN_R>0)
	    upmu_spk_en_r(0x1);
}

void hwSPKClassABTurnOff(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKClassABTurnOff]...........\r\n");
	}

	upmu_spk_en_l(0x0);
	upmu_spk_en_r(0x0);
	upmu_top2_spk_ck_pdn(0x1);
}

void hwSPKClassABVolumeControl(kal_uint32 VOL_L, kal_uint32 VOL_R)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKClassABVolumeControl]...........\r\n");
	}

	upmu_spk_vol_l(VOL_L);
	upmu_spk_vol_r(VOL_R);
}

void hwSPKClassABVoiceBypassAnalogGain(kal_uint32 VOL_L)
{
    if(gDebugLog==1){
    	xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKClassABVoiceBypassAnalogGain]...........\r\n");
    }

    upmu_spkab_fb_att(VOL_L);
}

void hw2in1SpeakerSetting(kal_uint32 spksrc_en_l, kal_uint32 spksrc_en_r)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hw2in1SpeakerSetting]...........\r\n");
	}

	upmu_spkrcv_en_l(spksrc_en_l);
	upmu_spkrcv_en_r(spksrc_en_r);
}

void hwSPKClassABAnalogSwitchSelect(kal_uint32 CH_NUM)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKClassABAnalogSwitchSelect]...........\r\n");
	}

	upmu_asw_ana_sw_sel(CH_NUM);
}

void hwSPKOCControl(kal_uint32 CFG_L, kal_uint32 CFG_R)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKClassABOCControl]...........\r\n");
	}

	upmu_spk_oc_en_l((CFG_L>>7)&0x1);
	upmu_spkab_oc_en_l((CFG_L>>6)&0x1);
    upmu_spk_oc_shdn_dl((CFG_L>>4)&0x1);

	upmu_spk_oc_en_r((CFG_R>>7)&0x1);
	upmu_spkab_oc_en_r((CFG_R>>6)&0x1);
	upmu_spk_oc_shdn_dr((CFG_R>>4)&0x1);
}

kal_uint32 hwSPKOCStatus(void)
{
    kal_uint32 ret = 0;
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwSPKOCStatus]...........\r\n");
	}
    ret = upmu_spk_ab_oc_l_deg();
    ret = (ret<<1) + upmu_spk_d_oc_l_deg();
	ret = (ret<<1) + upmu_spk_ab_oc_r_deg();
	ret = (ret<<1) + upmu_spk_d_oc_r_deg();
	return ret;
}

void hwGasGaugeTurnOn(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwGasGaugeTurnOn]...........\r\n");
	}

	upmu_ldo_en(LDO_VA2, 0x1);
	upmu_top2_fgadc_ck_pdn(0x0);
	upmu_fgadc_on(0x1);
}

void hwGasGaugeTurnOff(void)
{
	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwGasGaugeTurnOff]...........\r\n");
	}

	upmu_fgadc_on(0x0);
	upmu_top2_fgadc_ck_pdn(0x1);

}

//==============================================================================
// Exported Function for E2 chip
//==============================================================================
void hwE2_VA2_lp_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VA2_lp_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0xD1,0,0x1,0x3);
}

void hwE2_VA2_lp_ctrl_by_sw(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VA2_lp_ctrl_by_sw]...........\r\n");
	}

	ret=pmic_config_interface(0xD1,0,0x1,0x2);
}

void hwE2_VM12_1_lp_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VM12_1_lp_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0x85,0,0x1,0x1);
	ret=pmic_config_interface(0x85,1,0x1,0x0);
}

void hwE2_VM12_1_lp_ctrl_by_sw(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VM12_1_lp_ctrl_by_sw]...........\r\n");
	}

	ret=pmic_config_interface(0x85,val,0x1,0x1);
	ret=pmic_config_interface(0x85,0,0x1,0x0);
}

void hwE2_VM12_2_vosel(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VM12_2_vosel]...........\r\n");
	}

	ret=pmic_config_interface(0xBA,val,0x3,0x2);
}

void hwE2_VM12_2_lp_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VM12_2_lp_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0x89,0,0x1,0x1);
	ret=pmic_config_interface(0x89,1,0x1,0x0);
}

void hwE2_VM12_2_lp_ctrl_by_sw(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VM12_2_lp_ctrl_by_sw]...........\r\n");
	}

	ret=pmic_config_interface(0x89,val,0x1,0x1);
	ret=pmic_config_interface(0x89,0,0x1,0x0);
}

void hwE2_VM12_2_sleep_volt(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VM12_2_sleep_volt]...........\r\n");
	}

	ret=pmic_config_interface(0xBA,val,0x3,0x0);
}

void hwE2_VM12_2_mode_sel(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VM12_2_mode_sel]...........\r\n");
	}

	ret=pmic_config_interface(0x89,val,0x1,0x1);
}

void hwE2_VM12_INT_lp_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VM12_INT_lp_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0x90,0,0x1,0x1);
	ret=pmic_config_interface(0x90,1,0x1,0x0);
}

void hwE2_VM12_INT_lp_ctrl_by_sw(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VM12_INT_lp_ctrl_by_sw]...........\r\n");
	}

	ret=pmic_config_interface(0x90,val,0x1,0x1);
	ret=pmic_config_interface(0x90,0,0x1,0x0);
}

void hwE2_VMC_lp_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VMC_lp_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0xA9,0,0x1,0x1);
	ret=pmic_config_interface(0xA9,1,0x1,0x0);
}

void hwE2_VMC_lp_ctrl_by_sw(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VMC_lp_ctrl_by_sw]...........\r\n");
	}

	ret=pmic_config_interface(0xA9,val,0x1,0x1);
	ret=pmic_config_interface(0xA9,0,0x1,0x0);
}

void hwE2_VMCH_lp_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VMCH_lp_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0xAD,0,0x1,0x1);
	ret=pmic_config_interface(0xAD,1,0x1,0x0);
}

void hwE2_VMCH_lp_ctrl_by_sw(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VMCH_lp_ctrl_by_sw]...........\r\n");
	}

	ret=pmic_config_interface(0xAD,val,0x1,0x1);
	ret=pmic_config_interface(0xAD,0,0x1,0x0);
}

void hwE2_VA1_lp_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VA1_lp_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0xC6,0,0x1,0x1);
	ret=pmic_config_interface(0xC6,1,0x1,0x0);
}

void hwE2_VA1_lp_ctrl_by_sw(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VA1_lp_ctrl_by_sw]...........\r\n");
	}

	ret=pmic_config_interface(0xC6,val,0x1,0x1);
	ret=pmic_config_interface(0xC6,0,0x1,0x0);
}

void hwE2_VRF_lp_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VRF_lp_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0xBE,1,0x1,0x1);
}

void hwE2_VTCXO_lp_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VTCXO_lp_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0xC1,1,0x1,0x1);
}

void hwE2_VCORE_lp_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VCORE_lp_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0x59,1,0x1,0x0);
}

void hwE2_VCORE_vosel_con1(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VCORE_vosel_con1]...........\r\n");
	}

	ret=pmic_config_interface(0x58,val,0x1F,0x0);
}

void hwE2_VRF18_ON_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VRF18_ON_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0x60,1,0x1,0x1);
}

void hwE2_VRF18_ON_ctrl_by_sw(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VRF18_ON_ctrl_by_sw]...........\r\n");
	}

	ret=pmic_config_interface(0x60,0,0x1,0x1);
	ret=pmic_config_interface(0x60,1,0x1,0x0);
}

void hwE2_VRF18_OFF_ctrl_by_srclken(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VRF18_OFF_ctrl_by_srclken]...........\r\n");
	}

	ret=pmic_config_interface(0x60,1,0x1,0x1);
}

void hwE2_VRF18_OFF_ctrl_by_sw(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_VRF18_OFF_ctrl_by_sw]...........\r\n");
	}

	ret=pmic_config_interface(0x60,0,0x1,0x1);
	ret=pmic_config_interface(0x60,0,0x1,0x0);
}

void hwE2_buck_2m_sel_en(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_buck_2m_sel_en]...........\r\n");
	}

	ret=pmic_config_interface(0x4F,val,0x1,0x6);
}

void hwE2_spk_set_receiver_mode(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_spk_set_receiver_mode]...........\r\n");
	}

	ret=pmic_bank1_config_interface(0x4B,1,0x1,0x6);
	ret=pmic_bank1_config_interface(0x51,1,0x1,0x6);
}

void hwE2_spk_set_spk_mode(void)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_spk_set_spk_mode]...........\r\n");
	}

	ret=pmic_bank1_config_interface(0x4B,0,0x1,0x6);
	ret=pmic_bank1_config_interface(0x51,0,0x1,0x6);
}

void hwE2_hw_reset_pwrkey_int_sel(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_hw_reset_pwrkey_int_sel]...........\r\n");
	}

	ret=pmic_bank1_config_interface(0x17,val,0x1,0x4);
}

void hwE2_hw_reset_homekey_int_sel(kal_uint32 val)
{
	kal_uint32 ret=0;

	if(gDebugLog==1){
		xlog_printk(ANDROID_LOG_DEBUG, "Power/PMIC", "[hwE2_hw_reset_homekey_int_sel]...........\r\n");
	}

	ret=pmic_bank1_config_interface(0x17,val,0x1,0x3);
}


