/*****************************************************************************
 *
 * Filename:
 * ---------
 *    pmic)mt6329_hw.h
 *
 * Project:
 * --------
 *   Android_Software
 *
 * Description:
 * ------------
 *   This file is intended for PMIC 6329 driver.
 *
 * Author:
 * -------
 * -------
 *
 ****************************************************************************/

#ifndef _MT6577_PMIC_HW_BANK0_H_
#define _MT6577_PMIC_HW_BANK0_H_

//==============================================================================
// PMIC6329 Register Index - BANK0
//==============================================================================
#define BANK0_CID0            0x000 		
#define BANK0_CID1            0x001 	

#define BANK0_STATUS0         0x002 		
#define BANK0_STATUS1         0x003 		
#define BANK0_STATUS2         0x004 		
#define BANK0_STATUS3         0x005 		
#define BANK0_PGSTATUS0       0x006 		
#define BANK0_CHRSTATUS       0x009 		
#define BANK0_OCSTATUS0       0x00A 		
#define BANK0_OCSTATUS1       0x00B 		
#define BANK0_OCSTATUS2       0x00C 		
#define BANK0_OCSTATUS3       0x00D
 		
#define BANK0_STRUP_CTRL1     0x00E 
#define BANK0_STRUP_CTRL2     0x00F 
#define BANK0_STRUP_CTRL3     0x010 
#define BANK0_STRUP_RSV1      0x011 
#define BANK0_STRUP_TST_CTL   0x012 
#define BANK0_STRUP_THR_CTL   0x013 
#define BANK0_STRUP_VPG_EN1   0x014 
#define BANK0_STRUP_VPG_EN2   0x015 

#define BANK0_INT_CON0        0x016 
#define BANK0_INT_CON1        0x017  
#define BANK0_INT_CON2        0x018  
#define BANK0_INT_STATUS5     0x019  
#define BANK0_INT_STATUS6     0x01A  
#define BANK0_INT_STATUS7     0x01B 
 
#define BANK0_CHR_CON0        0x021  
#define BANK0_CHR_CON1        0x022  
#define BANK0_CHR_CON2        0x023  
#define BANK0_CHR_CON3        0x024  
#define BANK0_CHR_CON4        0x025  
#define BANK0_CHR_CON5        0x026  
#define BANK0_CHR_CON6        0x027  
#define BANK0_CHR_CON7        0x028  
#define BANK0_CHR_CON8        0x029  
#define BANK0_CHR_CON9        0x02A  
#define BANK0_CHR_CONA        0x02B  
#define BANK0_CHR_CONB        0x02C  
#define BANK0_CHR_CONC        0x02D  
#define BANK0_CHR_COND        0x02E  
#define BANK0_CHR_CONE        0x02F  
#define BANK0_CHR_CONF        0x030  
#define BANK0_CHR_CON11       0x031  
#define BANK0_CHR_CON12       0x032  
#define BANK0_CHR_CON13       0x033  
#define BANK0_CHR_CON14       0x034  
#define BANK0_CHR_CON15       0x035  
#define BANK0_CHR_CON16       0x036  
#define BANK0_CHR_CON17       0x037  
#define BANK0_CHR_CON18       0x038  
#define BANK0_CHR_CON19       0x039  
#define BANK0_CHR_CON1A       0x03A  
#define BANK0_CHR_CON1B       0x03B  
#define BANK0_CHR_CON1C       0x03C  
#define BANK0_CHR_CON1D       0x03D 

#define BANK0_VPROC_CON0      0x03F 
#define BANK0_VPROC_CON1      0x040 
#define BANK0_VPROC_CON2      0x041 
#define BANK0_VPROC_CON3      0x042 
#define BANK0_VPROC_CON4      0x043 
#define BANK0_VPROC_CON5      0x044 
#define BANK0_VPROC_CON6      0x045 
#define BANK0_VPROC_CON7      0x046 
#define BANK0_VPROC_CON8      0x047 
#define BANK0_VPROC_CON9      0x048 
#define BANK0_VPROC_CONA      0x049 
#define BANK0_VPROC_CONB      0x04A 
#define BANK0_VPROC_CONC      0x04B 
#define BANK0_VPROC_COND      0x04C 
#define BANK0_VPROC_CONE      0x04D 
#define BANK0_VPROC_CONF      0x04E 
#define BANK0_BUCK_RSV        0x04F 
                               
#define BANK0_VCORE_CON0      0x052 
#define BANK0_VCORE_CON1      0x053 
#define BANK0_VCORE_CON2      0x054 
#define BANK0_VCORE_CON3      0x055 
#define BANK0_VCORE_CON4      0x056 
#define BANK0_VCORE_CON5      0x057 
#define BANK0_VCORE_CON6      0x058 
#define BANK0_VCORE_CON7      0x059 
#define BANK0_VCORE_CON8      0x05A 

#define BANK0_VRF18_CON0      0x05D           
#define BANK0_VRF18_CON1      0x05E           
#define BANK0_VRF18_CON2      0x05F           
#define BANK0_VRF18_CON3      0x060           
#define BANK0_VRF18_CON4      0x061           
#define BANK0_VRF18_CON5      0x062           
#define BANK0_VRF18_CON6      0x063           
#define BANK0_VRF18_CON7      0x064
                 
#define BANK0_VM_CON0         0x067        
#define BANK0_VM_CON1         0x068        
#define BANK0_VM_CON2         0x069        
#define BANK0_VM_CON3         0x06A        
#define BANK0_VM_CON4         0x06B        
#define BANK0_VM_CON5         0x06C        
#define BANK0_VM_CON6         0x06D
                
#define BANK0_VPA_CON0        0x070         
#define BANK0_VPA_CON1        0x071         
#define BANK0_VPA_CON2        0x072         
#define BANK0_VPA_CON3        0x073         
#define BANK0_VPA_CON4        0x074         
#define BANK0_VPA_CON5        0x075         
#define BANK0_VPA_CON6        0x076         
#define BANK0_VPA_CON7        0x077         
#define BANK0_VPA_CON8        0x078         
#define BANK0_VPA_CON9        0x079         
#define BANK0_VPA_CONA        0x07A         
#define BANK0_VPA_CONB        0x07B         
#define BANK0_VPA_CONC        0x07C         
#define BANK0_VPA_COND        0x07D                     
#define BANK0_VPA_CONE        0x07E                     
#define BANK0_VPA_CONF        0x07F
                           
#define BANK0_DIGLDO_CON0     0x082                         
#define BANK0_DIGLDO_CON1     0x083                         
#define BANK0_DIGLDO_CON2     0x084                         
#define BANK0_DIGLDO_CON3     0x085                         
#define BANK0_DIGLDO_CON4     0x086                         
#define BANK0_DIGLDO_CON5     0x087             
#define BANK0_DIGLDO_CON6     0x088             
#define BANK0_DIGLDO_CON7     0x089             
#define BANK0_DIGLDO_CON8     0x08A             
#define BANK0_DIGLDO_CON9     0x08B             
#define BANK0_DIGLDO_CONA     0x08C             
#define BANK0_DIGLDO_CONB     0x08D             
#define BANK0_DIGLDO_CONC     0x08E             
#define BANK0_DIGLDO_COND     0x08F             
#define BANK0_DIGLDO_CONE     0x090             
#define BANK0_DIGLDO_CONF     0x091             
#define BANK0_DIGLDO_CON10    0x092             
#define BANK0_DIGLDO_CON11    0x093             
#define BANK0_DIGLDO_CON12    0x094             
#define BANK0_DIGLDO_CON13    0x095             
#define BANK0_DIGLDO_CON14    0x096             
#define BANK0_DIGLDO_CON15    0x097             
#define BANK0_DIGLDO_CON16    0x098             
#define BANK0_DIGLDO_CON17    0x099             
#define BANK0_DIGLDO_CON18    0x09A             
#define BANK0_DIGLDO_CON19    0x09B             
#define BANK0_DIGLDO_CON1A    0x09C             
#define BANK0_DIGLDO_CON1B    0x09D             
#define BANK0_DIGLDO_CON1C    0x09E             
#define BANK0_DIGLDO_CON1D    0x09F             
#define BANK0_DIGLDO_CON1E    0x0A0             
#define BANK0_DIGLDO_CON1F    0x0A1             
#define BANK0_DIGLDO_CON20    0x0A2             
#define BANK0_DIGLDO_CON21    0x0A3             
#define BANK0_DIGLDO_CON22    0x0A4             
#define BANK0_DIGLDO_CON23    0x0A5             
#define BANK0_DIGLDO_CON24    0x0A6             
#define BANK0_DIGLDO_CON25    0x0A7             
#define BANK0_DIGLDO_CON26    0x0A8             
#define BANK0_DIGLDO_CON27    0x0A9             
#define BANK0_DIGLDO_CON28    0x0AA             
#define BANK0_DIGLDO_CON29    0x0AB             
#define BANK0_DIGLDO_CON2A    0x0AC             
#define BANK0_DIGLDO_CON2B    0x0AD             
#define BANK0_DIGLDO_CON2C    0x0AE             
#define BANK0_DIGLDO_CON2D    0x0AF             
#define BANK0_DIGLDO_CON2E    0x0B0             
#define BANK0_DIGLDO_CON2F    0x0B1             
#define BANK0_DIGLDO_CON30    0x0B2             
#define BANK0_DIGLDO_CON31    0x0B3             
#define BANK0_DIGLDO_CON32    0x0B4             
#define BANK0_DIGLDO_CON33    0x0B5             
#define BANK0_DIGLDO_CON34    0x0B6             
#define BANK0_DIGLDO_CON35    0x0B7             
#define BANK0_DIGLDO_CON36    0x0B8             
#define BANK0_DIGLDO_CON37    0x0B9             
#define BANK0_DIGLDO_RSV      0x0BA
                 
#define BANK0_ANALDO_CON0     0x0BD             
#define BANK0_ANALDO_CON1     0x0BE             
#define BANK0_ANALDO_CON2     0x0BF             
#define BANK0_ANALDO_CON3     0x0C0             
#define BANK0_ANALDO_CON4     0x0C1             
#define BANK0_ANALDO_CON5     0x0C2             
#define BANK0_ANALDO_CON6     0x0C3             
#define BANK0_ANALDO_CON7     0x0C4             
#define BANK0_ANALDO_CON8     0x0C5             
#define BANK0_ANALDO_CON9     0x0C6             
#define BANK0_ANALDO_CONA     0x0C7             
#define BANK0_ANALDO_CONB     0x0C8             
#define BANK0_ANALDO_CONC     0x0C9             
#define BANK0_ANALDO_COND     0x0CA             
#define BANK0_ANALDO_CONE     0x0CB             
#define BANK0_ANALDO_CONF     0x0CC             
#define BANK0_ANALDO_CON10    0x0CD             
#define BANK0_ANALDO_CON11    0x0CE             
#define BANK0_ANALDO_CON12    0x0CF             
#define BANK0_ANALDO_CON13    0x0D0             
#define BANK0_ANALDO_RSV      0x0D1
             
#define BANK0_BUCK_K_CON0     0x0D5            
#define BANK0_BUCK_K_CON1     0x0D6            
#define BANK0_BUCK_K_CON2     0x0D7
                
#define BANK0_AUXADC_CON0     0x0DA             
#define BANK0_AUXADC_CON1     0x0DB             
#define BANK0_AUXADC_CON2     0x0DC             
#define BANK0_AUXADC_CON3     0x0DD             
#define BANK0_AUXADC_CON4     0x0DE             
#define BANK0_AUXADC_CON5     0x0DF             
#define BANK0_AUXADC_CON6     0x0E0             
#define BANK0_AUXADC_CON7     0x0E1             
#define BANK0_AUXADC_CON8     0x0E2             
#define BANK0_AUXADC_CON9     0x0E3             
#define BANK0_AUXADC_CON10    0x0E4             
#define BANK0_AUXADC_CON11    0x0E5             
#define BANK0_AUXADC_CON12    0x0E6             
#define BANK0_AUXADC_CON13    0x0E7             
#define BANK0_AUXADC_CON14    0x0E8             
#define BANK0_AUXADC_CON15    0x0E9             
#define BANK0_AUXADC_CON16    0x0EA             
#define BANK0_AUXADC_CON17    0x0EB             
#define BANK0_AUXADC_CON18    0x0EC             
#define BANK0_AUXADC_CON19    0x0ED             
#define BANK0_AUXADC_CON20    0x0EE             
#define BANK0_AUXADC_CON21    0x0EF             
#define BANK0_AUXADC_CON22    0x0F0             
#define BANK0_AUXADC_CON23    0x0F1             
#define BANK0_AUXADC_CON24    0x0F2             
#define BANK0_AUXADC_CON25    0x0F3             
#define BANK0_AUXADC_CON26    0x0F4
#define BANK0_AUXADC_CON27    0x0F5
#define BANK0_AUXADC_CON28    0x0F6
#define BANK0_AUXADC_CON29    0x0F7

//==============================================================================
// PMIC6329 Register Index - BANK0 - Remapping
//==============================================================================
#define BANK0_STATUS			BANK0_STATUS0
#define BANK0_STATUS_PG			BANK0_PGSTATUS0
#define BANK0_STATUS_CHR		BANK0_CHRSTATUS
#define BANK0_STATUS_OC			BANK0_OCSTATUS0

#define BANK0_STARTUP			BANK0_STRUP_CTRL1

#define BANK0_INTERRUPT			BANK0_INT_CON0

#define BANK0_CHARGER			BANK0_CHR_CON0

#define BANK0_LDO_VM12_1		BANK0_DIGLDO_CON0
#define BANK0_LDO_VM12_2		BANK0_DIGLDO_CON4
#define BANK0_LDO_VM12_INT		BANK0_DIGLDO_CON8
#define BANK0_LDO_VIO28			BANK0_DIGLDO_CONF
#define BANK0_LDO_VSIM			BANK0_DIGLDO_CON12
#define BANK0_LDO_VSIM2			BANK0_DIGLDO_CON15
#define BANK0_LDO_VUSB			BANK0_DIGLDO_CON18
#define BANK0_LDO_VCAMD			BANK0_DIGLDO_CON1B
#define BANK0_LDO_VCAM_IO		BANK0_DIGLDO_CON1E
#define BANK0_LDO_VCAM_AF		BANK0_DIGLDO_CON21
#define BANK0_LDO_VMC			BANK0_DIGLDO_CON24
#define BANK0_LDO_VMCH			BANK0_DIGLDO_CON28
#define BANK0_LDO_VGP			BANK0_DIGLDO_CON2C
#define BANK0_LDO_VGP2			BANK0_DIGLDO_CON2F
#define BANK0_LDO_VIBR			BANK0_DIGLDO_CON32

#define BANK0_LDO_VRF			BANK0_ANALDO_CON0
#define BANK0_LDO_VTCXO			BANK0_ANALDO_CON3
#define BANK0_LDO_VA1			BANK0_ANALDO_CON6
#define BANK0_LDO_VA2			BANK0_ANALDO_CONA
#define BANK0_LDO_VCAMA			BANK0_ANALDO_COND
#define BANK0_LDO_VRTC			BANK0_ANALDO_CON11

#define BANK0_BUCK_CALIBRATION	BANK0_BUCK_K_CON0

#define BANK0_AUXADC			BANK0_AUXADC_CON0

#if 1
//==============================================================================
// Test for PMIC6326 
//==============================================================================
typedef enum{  
    PMIC_INIT_CTRL_1            = 0x89,  
    PMIC_INIT_CTRL_2            = 0x8A,  
    PMIC_INIT_CTRL_3            = 0x8B
} PMIC_INIT_CTRL;

// (0x89) INT CTRL 1
typedef enum
{
    INT_EN_VCORE2_OC            = 0x01,
    INT_EN_VPA_OC               = 0x02,
    INT_EN_BOOST1_OC            = 0x04,
    INT_EN_BOOST2_OC            = 0x08,
    INT_EN_SPKL_OC              = 0x10,
    INT_EN_SPKR_OC              = 0x20,
    INT_EN_V3GTX_OC             = 0x40,
    INT_EN_V3GRX_OC             = 0x80,
    INT_EN_0X89_ALL             = 0xFF,
    INT1_EN_ALL                 = 0xFF
}int_ctrl_1_enum;

// (0x8A) INT CTRL 2
typedef enum
{
    INT_EN_RF_OC                = 0x01,
    INT_EN_VTCXO_OC             = 0x02,
    INT_EN_VCAMA_OC             = 0x04,
    INT_EN_VWIFI3V3_OC          = 0x08,
    INT_EN_VWIFI2V8_OC          = 0x10,
    INT_EN_VSIM_OC              = 0x20,
    INT_EN_VBT_OC               = 0x40,
    INT_EN_VCAMD_OC             = 0x80,
    INT_EN_0X8A_ALL             = 0xFF,
    INT2_EN_ALL                 = 0xFF
}int_ctrl_2_enum;

// (0x8B) INT CTRL 3
typedef enum
{
    INT_EN_VSDIO_OC             = 0x01,
    INT_EN_VGP_OC               = 0x02,
    INT_EN_VUSB_OC              = 0x04,
    INT_EN_CHRDET               = 0x08,
    INT_EN_OVP                  = 0x10,
    INT_EN_WATCHDOG             = 0x20,
    INT_EN_PWRKEY               = 0x40,
    INT_EN_0X8B_ALL             = 0x7F,
    INT3_EN_ALL                 = 0x7F
}int_ctrl_3_enum;

#define PMIC_SLAVE_ADDR         0xC0
#define PMIC_STATUS_REG         0x09

#define CHRDET_MASK             0x1
#define CHRDET_SHIFT            0x5

#define PMIC6326_E1_CID_CODE    0x0000
#define PMIC6326_E2_CID_CODE    0x0009
#define PMIC6326_E3_CID_CODE    0x000A
#define PMIC6326_E4_CID_CODE    0x000B

#define CID_1_REG_INDEX         0x0
#define CID_2_REG_INDEX         0x1

#define PMIC_REG_NUM            0x97

#define PMIC6326_ECO_1_VERSION		0x01
#define PMIC6326_ECO_2_VERSION		0x02
#define PMIC6326_ECO_3_VERSION		0x03
#define PMIC6326_ECO_4_VERSION		0x04

#endif

#endif // _MT6577_PMIC_HW_BANK0_H_

