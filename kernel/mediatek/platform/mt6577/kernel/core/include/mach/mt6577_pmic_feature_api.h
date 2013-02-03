#include <mach/mt6577_typedefs.h>

//////////////////////////////////////////////////////////////////////////////////////////
// ECO CODE
//////////////////////////////////////////////////////////////////////////////////////////
#define PMIC6329_E1_CODE    0x0029
#define PMIC6329_E2_CODE    0x1029

typedef enum{  
    PMIC_PWM_0 = 0,  
    PMIC_PWM_1 = 1,  
    PMIC_PWM_2 = 2
} MT65XX_PMIC_PWM_NUMBER;

//==============================================================================
// Exported Function 
//==============================================================================

extern kal_uint32 get_pmic6329_chip_version (void);

//LDO for low power
extern void hwSetLowPowerMode_VM12_1(kal_uint32 modeSet, kal_uint32 modeSel);
extern void hwSetLowPowerMode_VM12_2(kal_uint32 modeSet, kal_uint32 modeSel);
extern void hwSetLowPowerMode_VM12_INT(kal_uint32 modeSet, kal_uint32 modeSel);
extern void hwSetLowPowerMode_VMC(kal_uint32 modeSet, kal_uint32 modeSel);
extern void hwSetLowPowerMode_VMCH(kal_uint32 modeSet, kal_uint32 modeSel);
extern void hwSetLowPowerMode_VRF28(kal_uint32 modeSel);
extern void hwSetLowPowerMode_VTCXO(kal_uint32 modeSel);
extern void hwSetLowPowerMode_VA1(kal_uint32 modeSet, kal_uint32 modeSel);

//Backlight
extern void hwPWMsetting(MT65XX_PMIC_PWM_NUMBER PWMmode, kal_uint32 duty, kal_uint32 freqSelect);
//Backlight - BOOST
extern void hwBacklightBoostTuning(kal_uint32 MODE, kal_uint32 VRSEL, kal_uint32 CABC_EN);
extern void hwBacklightBoostTurnOn(void);
extern void hwBacklightBoostTurnOff(void);
//Backlight - ISINK
extern void hwBacklightISINKTuning(kal_uint32 CH_NUM, kal_uint32 MODE, kal_uint32 STEP, kal_uint32 CABC_EN);
extern void hwBacklightISINKTurnOn(kal_uint32 CH_NUM);
extern void hwBacklightISINKTurnOff(kal_uint32 CH_NUM);

//Flash
extern void hwBacklightFlashlightTuning(kal_uint32 duty, kal_uint32 mode);
extern void hwBacklightFlashlightTuning_v2(kal_uint32 mode, kal_uint32 duty, kal_uint32 div);
extern void hwBacklightFlashlightTurnOn(void);
extern void hwBacklightFlashlightTurnOff(void);

//KPLED
extern void hwBacklightKPLEDTuning(kal_uint32 duty, kal_uint32 mode);
extern void hwBacklightKPLEDTuning_v2(kal_uint32 mode, kal_uint32 duty, kal_uint32 div);
extern void hwBacklightKPLEDTurnOn(void);
extern void hwBacklightKPLEDTurnOff(void);

//Speaker 
//Class D
extern void hwSPKClassDInit(void);
extern void hwSPKClassDTurnOn(kal_uint32 EN_L, kal_uint32 EN_R);
extern void hwSPKClassDTurnOff(void);
extern void hwSPKClassDVolumeControl(kal_uint32 VOL_L, kal_uint32 VOL_R);
//Class AB
extern void hwSPKClassABInit(void);
extern void hwSPKClassABTurnOn(kal_uint32 EN_L, kal_uint32 EN_R);
extern void hwSPKClassABTurnOff(void);
extern void hwSPKClassABVolumeControl(kal_uint32 VOL_L, kal_uint32 VOL_R);
extern void hwSPKClassABVoiceBypassAnalogGain(kal_uint32 VOL_L);
//SPK - 2 in 1 speaker
extern void hw2in1SpeakerSetting(kal_uint32 spksrc_en_l, kal_uint32 spksrc_en_r);
//ASW
extern void hwSPKClassABAnalogSwitchSelect(kal_uint32 CH_NUM);
//SPK OC
extern void hwSPKOCControl(kal_uint32 CFG_L, kal_uint32 CFG_R);
extern kal_uint32 hwSPKOCStatus(void);

//Gas Gauge
extern void hwGasGaugeTurnOn(void);
extern void hwGasGaugeTurnOff(void);

//==============================================================================
// Exported Function for E2 chip
//==============================================================================
extern void hwE2_VA2_lp_ctrl_by_srclken(void);
extern void hwE2_VA2_lp_ctrl_by_sw(void);
extern void hwE2_VM12_1_lp_ctrl_by_srclken(void);
extern void hwE2_VM12_1_lp_ctrl_by_sw(kal_uint32 val);
extern void hwE2_VM12_2_vosel(kal_uint32 val);
extern void hwE2_VM12_2_lp_ctrl_by_srclken(void);
extern void hwE2_VM12_2_lp_ctrl_by_sw(kal_uint32 val);
extern void hwE2_VM12_2_sleep_volt(kal_uint32 val);
extern void hwE2_VM12_2_mode_sel(kal_uint32 val);
extern void hwE2_VM12_INT_lp_ctrl_by_srclken(void);
extern void hwE2_VM12_INT_lp_ctrl_by_sw(kal_uint32 val);
extern void hwE2_VMC_lp_ctrl_by_srclken(void);
extern void hwE2_VMC_lp_ctrl_by_sw(kal_uint32 val);
extern void hwE2_VMCH_lp_ctrl_by_srclken(void);
extern void hwE2_VMCH_lp_ctrl_by_sw(kal_uint32 val);
extern void hwE2_VA1_lp_ctrl_by_srclken(void);
extern void hwE2_VA1_lp_ctrl_by_sw(kal_uint32 val);
extern void hwE2_VRF_lp_ctrl_by_srclken(void);
extern void hwE2_VTCXO_lp_ctrl_by_srclken(void);
extern void hwE2_VCORE_lp_ctrl_by_srclken(void);
extern void hwE2_VCORE_vosel_con1(kal_uint32 val);
extern void hwE2_VRF18_ON_ctrl_by_srclken(void);
extern void hwE2_VRF18_ON_ctrl_by_sw(void);
extern void hwE2_VRF18_OFF_ctrl_by_srclken(void);
extern void hwE2_VRF18_OFF_ctrl_by_sw(void);
extern void hwE2_buck_2m_sel_en(kal_uint32 val);
extern void hwE2_spk_set_receiver_mode(void);
extern void hwE2_spk_set_spk_mode(void);
extern void hwE2_hw_reset_pwrkey_int_sel(kal_uint32 val);
extern void hwE2_hw_reset_homekey_int_sel(kal_uint32 val);

