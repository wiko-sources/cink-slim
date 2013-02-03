#ifndef _MT6577_PMIC_UBOOT_SW_H_
#define _MT6577_PMIC_UBOOT_SW_H_

//#define PMIC_SW_TEST_MODE_6326

#define AUXADC_BATTERY_VOLTAGE_CHANNEL  0
#define AUXADC_REF_CURRENT_CHANNEL     	1
#define AUXADC_CHARGER_VOLTAGE_CHANNEL  2
#define AUXADC_TEMPERATURE_CHANNEL     	3

#define VOLTAGE_FULL_RANGE 	1200
#define ADC_PRECISE 		1024 // 10 bits

//==============================================================================
// PMIC6329 BANK 0/1 address
//==============================================================================
#define mt6329_BANK0_SLAVE_ADDR_WRITE	0xC0
#define mt6329_BANK0_SLAVE_ADDR_Read	0xC1
#define mt6329_BANK1_SLAVE_ADDR_WRITE	0xC2
#define mt6329_BANK1_SLAVE_ADDR_Read	0xC3

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

//==============================================================================
// PMIC6329 Register Index - BANK1 - Remapping
//==============================================================================
#define BANK1_TEST_MODE			BANK1_TEST_CON0

#define BANK1_TOP2_CTRL			BANK1_TOP2_CON0

//Driver
#define BANK1_FLASH_CTRL		BANK1_FLASH_CON0
#define BANK1_KPLED_CTRL		BANK1_KPLED_CON0
#define BANK1_ISINKS_CTRL		BANK1_ISINKS_CON0

#define BANK1_BOOST_CTRL		BANK1_BOOST_CON0

#define BANK1_SPK_CTRL			BANK1_SPK_CON0

#define BANK1_ASW_CTRL			BANK1_ASW_CON0

#define BANK1_FGPLL_CTRL		BANK1_FGPLL_CON0

#define BANK1_FGADC_CTRL		BANK1_FGADC_CON0

#define BANK1_OTPC_CTRL			BANK1_OTPC_CON0

#define BANK1_I2C_CTRL			BANK1_I2C_CON0

//==============================================================================
// Common Offset
//==============================================================================
//STATUS
#define STATUS_CON0_OFFSET          0x00
#define STATUS_CON1_OFFSET          0x01
#define STATUS_CON2_OFFSET          0x02
#define STATUS_CON3_OFFSET          0x03

#define STATUS_PG_CON0_OFFSET       0x00

#define STATUS_CHR_CON0_OFFSET      0x00

#define STATUS_OC_CON0_OFFSET       0x00
#define STATUS_OC_CON1_OFFSET       0x01
#define STATUS_OC_CON2_OFFSET       0x02
#define STATUS_OC_CON3_OFFSET       0x03

//STARTUP
#define STARTUP_CON0_OFFSET         0x00
#define STARTUP_CON1_OFFSET         0x01
#define STARTUP_CON2_OFFSET         0x02
#define STARTUP_CON3_OFFSET         0x03
#define STARTUP_CON4_OFFSET         0x04
#define STARTUP_CON5_OFFSET         0x05
#define STARTUP_CON6_OFFSET         0x06
#define STARTUP_CON7_OFFSET         0x07

//INTTERRUPT
#define INTTERRUPT_CON0_OFFSET      0x00
#define INTTERRUPT_CON1_OFFSET      0x01
#define INTTERRUPT_CON2_OFFSET      0x02
#define INTTERRUPT_CON3_OFFSET      0x03
#define INTTERRUPT_CON4_OFFSET      0x04
#define INTTERRUPT_CON5_OFFSET      0x05

//CHARGER
#define CHR_CON0_OFFSET          0x00
#define CHR_CON1_OFFSET          0x01
#define CHR_CON2_OFFSET          0x02
#define CHR_CON3_OFFSET          0x03
#define CHR_CON4_OFFSET          0x04
#define CHR_CON5_OFFSET          0x05
#define CHR_CON6_OFFSET          0x06
#define CHR_CON7_OFFSET          0x07
#define CHR_CON8_OFFSET          0x08
#define CHR_CON9_OFFSET          0x09
#define CHR_CONA_OFFSET          0x0A
#define CHR_CONB_OFFSET          0x0B
#define CHR_CONC_OFFSET          0x0C
#define CHR_COND_OFFSET          0x0D
#define CHR_CONE_OFFSET          0x0E
#define CHR_CONF_OFFSET          0x0F
#define CHR_CON11_OFFSET         0x10
#define CHR_CON12_OFFSET         0x11
#define CHR_CON13_OFFSET         0x12
#define CHR_CON14_OFFSET         0x13
#define CHR_CON15_OFFSET         0x14
#define CHR_CON16_OFFSET         0x15
#define CHR_CON17_OFFSET         0x16
#define CHR_CON18_OFFSET         0x17
#define CHR_CON19_OFFSET         0x18
#define CHR_CON1A_OFFSET         0x19
#define CHR_CON1B_OFFSET         0x1A
#define CHR_CON1C_OFFSET         0x1B
#define CHR_CON1D_OFFSET         0x1C

//BUCK
#define BUCK_CON0_OFFSET          0x00
#define BUCK_CON1_OFFSET          0x01
#define BUCK_CON2_OFFSET          0x02
#define BUCK_CON3_OFFSET          0x03
#define BUCK_CON4_OFFSET          0x04
#define BUCK_CON5_OFFSET          0x05
#define BUCK_CON6_OFFSET          0x06
#define BUCK_CON7_OFFSET          0x07
#define BUCK_CON8_OFFSET          0x08
#define BUCK_CON9_OFFSET          0x09
#define BUCK_CONA_OFFSET          0x0A
#define BUCK_CONB_OFFSET          0x0B
#define BUCK_CONC_OFFSET          0x0C
#define BUCK_COND_OFFSET          0x0D
#define BUCK_CONE_OFFSET          0x0E
#define BUCK_CONF_OFFSET          0x0F

//LDO
#define LDO_CON0_OFFSET          0x00
#define LDO_CON1_OFFSET          0x01
#define LDO_CON2_OFFSET          0x02
#define LDO_CON3_OFFSET          0x03
#define LDO_CON4_OFFSET          0x04
#define LDO_CON5_OFFSET          0x05
#define LDO_CON6_OFFSET          0x06
#define LDO_CON7_OFFSET          0x07

//BUCK CALIBRATION
#define BUCK_K_CON0_OFFSET       0x00
#define BUCK_K_CON1_OFFSET       0x01
#define BUCK_K_CON2_OFFSET       0x02

//AUXADC
#define AUXADC_CON0_OFFSET          0x00
#define AUXADC_CON1_OFFSET          0x01
#define AUXADC_CON2_OFFSET          0x02
#define AUXADC_CON3_OFFSET          0x03
#define AUXADC_CON4_OFFSET          0x04
#define AUXADC_CON5_OFFSET          0x05
#define AUXADC_CON6_OFFSET          0x06
#define AUXADC_CON7_OFFSET          0x07
#define AUXADC_CON8_OFFSET          0x08
#define AUXADC_CON9_OFFSET          0x09
#define AUXADC_CON10_OFFSET         0x0A
#define AUXADC_CON11_OFFSET         0x0B
#define AUXADC_CON12_OFFSET         0x0C
#define AUXADC_CON13_OFFSET         0x0D
#define AUXADC_CON14_OFFSET         0x0E
#define AUXADC_CON15_OFFSET         0x0F
#define AUXADC_CON16_OFFSET         0x10
#define AUXADC_CON17_OFFSET         0x11
#define AUXADC_CON18_OFFSET         0x12
#define AUXADC_CON19_OFFSET         0x13
#define AUXADC_CON20_OFFSET         0x14
#define AUXADC_CON21_OFFSET         0x15
#define AUXADC_CON22_OFFSET         0x16
#define AUXADC_CON23_OFFSET         0x17
#define AUXADC_CON24_OFFSET         0x18
#define AUXADC_CON25_OFFSET         0x19
#define AUXADC_CON26_OFFSET         0x1A
#define AUXADC_CON27_OFFSET         0x1B
#define AUXADC_CON28_OFFSET         0x1C
#define AUXADC_CON29_OFFSET         0x1D

//TESTMODE
#define TESTMODE_CON0_OFFSET        0x00
#define TESTMODE_CON1_OFFSET        0x01
#define TESTMODE_CON2_OFFSET        0x02
#define TESTMODE_CON3_OFFSET        0x03
#define TESTMODE_CON4_OFFSET        0x04
#define TESTMODE_CON5_OFFSET        0x05
#define TESTMODE_CON6_OFFSET        0x06
#define TESTMODE_CON7_OFFSET        0x07
#define TESTMODE_CON8_OFFSET        0x08
#define TESTMODE_CON9_OFFSET        0x09
#define TESTMODE_CON10_OFFSET       0x0A
#define TESTMODE_CON11_OFFSET       0x0B
#define TESTMODE_CON12_OFFSET       0x0C
#define TESTMODE_CON13_OFFSET       0x0D

//TOP2
#define TOP2_CON0_OFFSET          	0x00
#define TOP2_CON1_OFFSET          	0x01
#define TOP2_CON2_OFFSET          	0x02
#define TOP2_CON3_OFFSET          	0x03
#define TOP2_CON4_OFFSET          	0x04

//FLASH
#define FLASH_CON0_OFFSET          	0x00
#define FLASH_CON1_OFFSET          	0x01
#define FLASH_CON2_OFFSET          	0x02
#define FLASH_CON3_OFFSET          	0x03
#define FLASH_CON4_OFFSET          	0x04

//KPLED
#define KPLED_CON0_OFFSET          	0x00
#define KPLED_CON1_OFFSET          	0x01
#define KPLED_CON2_OFFSET          	0x02
#define KPLED_CON3_OFFSET          	0x03
#define KPLED_CON4_OFFSET          	0x04

//ISINKS
#define ISINKS_CON0_OFFSET          0x00
#define ISINKS_CON1_OFFSET          0x01
#define ISINKS_CON2_OFFSET          0x02
#define ISINKS_CON3_OFFSET          0x03
#define ISINKS_CON4_OFFSET          0x04
#define ISINKS_CON5_OFFSET          0x05
#define ISINKS_CON6_OFFSET          0x06
#define ISINKS_CON7_OFFSET          0x07
#define ISINKS_CON8_OFFSET          0x08
#define ISINKS_CON9_OFFSET          0x09
#define ISINKS_CON10_OFFSET         0x0A
#define ISINKS_CON11_OFFSET         0x0B
#define ISINKS_CON12_OFFSET         0x0C
#define ISINKS_CON13_OFFSET         0x0D
#define ISINKS_CON14_OFFSET         0x0E
#define ISINKS_CON15_OFFSET         0x0F

//BOOST
#define BOOST_CON0_OFFSET          	0x00
#define BOOST_CON1_OFFSET          	0x01
#define BOOST_CON2_OFFSET          	0x02
#define BOOST_CON3_OFFSET          	0x03
#define BOOST_CON4_OFFSET          	0x04

//SPK
#define SPK_CON0_OFFSET          0x00
#define SPK_CON1_OFFSET          0x01
#define SPK_CON2_OFFSET          0x02
#define SPK_CON3_OFFSET          0x03
#define SPK_CON4_OFFSET          0x04
#define SPK_CON5_OFFSET          0x05
#define SPK_CON6_OFFSET          0x06
#define SPK_CON7_OFFSET          0x07
#define SPK_CON8_OFFSET          0x08
#define SPK_CON9_OFFSET          0x09
#define SPK_CON10_OFFSET         0x0A
#define SPK_CON11_OFFSET         0x0B
#define SPK_CON12_OFFSET         0x0C
#define SPK_CON13_OFFSET         0x0D
#define SPK_CON14_OFFSET         0x0E
#define SPK_CON15_OFFSET         0x0F
#define SPK_CON16_OFFSET         0x10
#define SPK_CON17_OFFSET         0x11
#define SPK_CON18_OFFSET         0x12
#define SPK_CON19_OFFSET         0x13

//ASW
#define ASW_CON0_OFFSET          0x00

//FGPLL
#define FGPLL_CON0_OFFSET          0x00
#define FGPLL_CON1_OFFSET          0x01
#define FGPLL_CON2_OFFSET          0x02
#define FGPLL_CON3_OFFSET          0x03
#define FGPLL_CON4_OFFSET          0x04
#define FGPLL_CON5_OFFSET          0x05
#define FGPLL_CON6_OFFSET          0x06

//FGADC
#define FGADC_CON0_OFFSET          0x00
#define FGADC_CON1_OFFSET          0x01
#define FGADC_CON2_OFFSET          0x02
#define FGADC_CON3_OFFSET          0x03
#define FGADC_CON4_OFFSET          0x04
#define FGADC_CON5_OFFSET          0x05
#define FGADC_CON6_OFFSET          0x06
#define FGADC_CON7_OFFSET          0x07
#define FGADC_CON8_OFFSET          0x08
#define FGADC_CON9_OFFSET          0x09
#define FGADC_CON10_OFFSET         0x0A
#define FGADC_CON11_OFFSET         0x0B
#define FGADC_CON12_OFFSET         0x0C
#define FGADC_CON13_OFFSET         0x0D
#define FGADC_CON14_OFFSET         0x0E
#define FGADC_CON15_OFFSET         0x0F
#define FGADC_CON16_OFFSET         0x10
#define FGADC_CON17_OFFSET         0x11
#define FGADC_CON18_OFFSET         0x12
#define FGADC_CON19_OFFSET         0x13
#define FGADC_CON20_OFFSET         0x14
#define FGADC_CON21_OFFSET         0x15
#define FGADC_CON22_OFFSET         0x16
#define FGADC_CON23_OFFSET         0x17
#define FGADC_CON24_OFFSET         0x18
#define FGADC_CON25_OFFSET         0x19
#define FGADC_CON26_OFFSET         0x1A
#define FGADC_CON27_OFFSET         0x1B
#define FGADC_CON28_OFFSET         0x1C
#define FGADC_CON29_OFFSET         0x1D
#define FGADC_CON30_OFFSET         0x1E

//OTPC
#define OTPC_CON0_OFFSET          0x00
#define OTPC_CON1_OFFSET          0x01
#define OTPC_CON2_OFFSET          0x02
#define OTPC_CON3_OFFSET          0x03
#define OTPC_CON4_OFFSET          0x04
#define OTPC_CON5_OFFSET          0x05
#define OTPC_CON6_OFFSET          0x06
#define OTPC_CON7_OFFSET          0x07
#define OTPC_CON8_OFFSET          0x08
#define OTPC_CON9_OFFSET          0x09
#define OTPC_CON10_OFFSET         0x0A
#define OTPC_CON11_OFFSET         0x0B
#define OTPC_CON12_OFFSET         0x0C
#define OTPC_CON13_OFFSET         0x0D
#define OTPC_CON14_OFFSET         0x0E
#define OTPC_CON15_OFFSET         0x0F
#define OTPC_CON16_OFFSET         0x10
#define OTPC_CON17_OFFSET         0x11
#define OTPC_CON18_OFFSET         0x12
#define OTPC_CON19_OFFSET         0x13
#define OTPC_CON20_OFFSET         0x14
#define OTPC_CON21_OFFSET         0x15

//I2C
#define I2C_CON0_OFFSET          0x00
#define I2C_CON1_OFFSET          0x01


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
// PMIC6329 Register Index - BANK1
//==============================================================================
#define BANK1_TEST_CON0       0x000
#define BANK1_TEST_CON1       0x001
#define BANK1_TEST_CON2       0x002
#define BANK1_TEST_CON3       0x003
#define BANK1_TEST_CON4       0x004
#define BANK1_TEST_CON5       0x005
#define BANK1_TEST_CON6       0x006
#define BANK1_TEST_CON7       0x007
#define BANK1_TEST_CON8       0x008
#define BANK1_TEST_CON9       0x009
#define BANK1_TEST_CON10      0x00A
#define BANK1_TEST_CON11      0x00B
#define BANK1_RST_CON0        0x00C
#define BANK1_RST_CON1        0x00D

#define BANK1_TOP2_CON0       0x014
#define BANK1_TOP2_CON1       0x015
#define BANK1_CC_CTL1         0x016
#define BANK1_INT2_RSV        0x017
#define BANK1_OC_PWMCTL1      0x018

#define BANK1_FLASH_CON0      0x022  
#define BANK1_FLASH_CON1      0x023  
#define BANK1_FLASH_CON2      0x024  
#define BANK1_FLASH_CON3      0x025  
#define BANK1_FLASH_CON4      0x026  
#define BANK1_KPLED_CON0      0x027  
#define BANK1_KPLED_CON1      0x028  
#define BANK1_KPLED_CON2      0x029  
#define BANK1_KPLED_CON3      0x02A  
#define BANK1_KPLED_CON4      0x02B  
#define BANK1_ISINKS_CON0     0x02C  
#define BANK1_ISINKS_CON1     0x02D  
#define BANK1_ISINKS_CON2     0x02E  
#define BANK1_ISINKS_CON3     0x02F  
#define BANK1_ISINKS_CON4     0x030  
#define BANK1_ISINKS_CON5     0x031  
#define BANK1_ISINKS_CON6     0x032  
#define BANK1_ISINKS_CON7     0x033  
#define BANK1_ISINKS_CON8     0x034  
#define BANK1_ISINKS_CON9     0x035  
#define BANK1_ISINKS_CON10    0x036  
#define BANK1_ISINKS_CON11    0x037  
#define BANK1_ISINKS_CON12    0x038  
#define BANK1_ISINKS_CON13    0x039  
#define BANK1_ISINKS_CON14    0x03A  
#define BANK1_ISINKS_CON15    0x03B  
#define BANK1_ISINKS_CON16    0x03C
   
#define BANK1_BOOST_CON0      0x03F
#define BANK1_BOOST_CON1      0x040
#define BANK1_BOOST_CON2      0x041
#define BANK1_BOOST_CON3      0x042
#define BANK1_BOOST_CON4      0x043
    
#define BANK1_SPK_CON0        0x046
#define BANK1_SPK_CON1        0x047
#define BANK1_SPK_CON2        0x048
#define BANK1_SPK_CON3        0x049
#define BANK1_SPK_CON4        0x04A
#define BANK1_SPK_CON5        0x04B
#define BANK1_SPK_CON6        0x04C
#define BANK1_SPK_CON7        0x04D
#define BANK1_SPK_CON8        0x04E
#define BANK1_SPK_CON9        0x04F
#define BANK1_SPK_CON10       0x050
#define BANK1_SPK_CON11       0x051
#define BANK1_SPK_CON12       0x052
#define BANK1_SPK_CON13       0x053
#define BANK1_SPK_CON14       0x054
#define BANK1_SPK_CON15       0x055
#define BANK1_SPK_CON16       0x056
#define BANK1_SPK_CON17       0x057
#define BANK1_SPK_CON18       0x058
#define BANK1_SPK_CON19       0x059
#define BANK1_SPK_CON20       0x05A
#define BANK1_SPK_CON21       0x05B
#define BANK1_SPK_CON22       0x05C
  
#define BANK1_ASW_CON0        0x05F

#define BANK1_FGPLL_CON0      0x060
#define BANK1_FGPLL_CON1      0x061
#define BANK1_FGPLL_CON2      0x062
#define BANK1_FGPLL_CON3      0x063
#define BANK1_FGPLL_CON4      0x064
#define BANK1_FGPLL_CON5      0x065
#define BANK1_FGPLL_CON6      0x066
    
#define BANK1_FGADC_CON0      0x069 
#define BANK1_FGADC_CON1      0x06A 
#define BANK1_FGADC_CON2      0x06B 
#define BANK1_FGADC_CON3      0x06C 
#define BANK1_FGADC_CON4      0x06D 
#define BANK1_FGADC_CON5      0x06E 
#define BANK1_FGADC_CON6      0x06F 
#define BANK1_FGADC_CON7      0x070 
#define BANK1_FGADC_CON8      0x071 
#define BANK1_FGADC_CON9      0x072 
#define BANK1_FGADC_CON10     0x073 
#define BANK1_FGADC_CON11     0x074 
#define BANK1_FGADC_CON12     0x075 
#define BANK1_FGADC_CON13     0x076 
#define BANK1_FGADC_CON14     0x077 
#define BANK1_FGADC_CON15     0x078 
#define BANK1_FGADC_CON16     0x079 
#define BANK1_FGADC_CON17     0x07A 
#define BANK1_FGADC_CON18     0x07B 
#define BANK1_FGADC_CON19     0x07C 
#define BANK1_FGADC_CON20     0x07D 
#define BANK1_FGADC_CON21     0x07E 
#define BANK1_FGADC_CON22     0x07F 
#define BANK1_FGADC_CON23     0x080 
#define BANK1_FGADC_CON24     0x081 
#define BANK1_FGADC_CON25     0x082 
#define BANK1_FGADC_CON26     0x083 
#define BANK1_FGADC_CON27     0x084 
#define BANK1_FGADC_CON28     0x085 
#define BANK1_FGADC_CON29     0x086 
#define BANK1_FGADC_CON30     0x087 
#define BANK1_FGADC_CON31     0x088 
#define BANK1_FGADC_CON32     0x089 

#define BANK1_OTPC_CON0       0x08C
#define BANK1_OTPC_CON1       0x08D
#define BANK1_OTPC_CON2       0x08E
#define BANK1_OTPC_CON3       0x08F
#define BANK1_OTPC_CON4       0x090
#define BANK1_OTPC_CON5       0x091
#define BANK1_OTPC_CON6       0x092
#define BANK1_OTPC_CON7       0x093
#define BANK1_OTPC_CON8       0x094
#define BANK1_OTPC_CON9       0x095
#define BANK1_OTPC_CON10      0x096
#define BANK1_OTPC_CON11      0x097
#define BANK1_OTPC_CON12      0x098
#define BANK1_OTPC_CON13      0x099
#define BANK1_OTPC_CON14      0x09A
#define BANK1_OTPC_CON15      0x09B
#define BANK1_OTPC_CON16      0x09C
#define BANK1_OTPC_CON17      0x09D
#define BANK1_OTPC_CON18      0x09E
#define BANK1_OTPC_CON19      0x09F
#define BANK1_OTPC_CON20      0x0A0
#define BANK1_OTPC_CON21      0x0A1
   
#define BANK1_I2C_CON0        0x0A4
#define BANK1_I2C_CON1        0x0A5

//==============================================================================
// Common HW mask and shift
//==============================================================================
//BUCK - CON0
#define BANK_0_RG_BUCK_CSL_MASK 0x3
#define BANK_0_RG_BUCK_CSL_SHIFT 0x6

#define BANK_0_RG_BUCK_CSR_MASK 0x3
#define BANK_0_RG_BUCK_CSR_SHIFT 0x4

#define BANK_0_RG_BUCK_CC_MASK 0x3
#define BANK_0_RG_BUCK_CC_SHIFT 0x2

#define BANK_0_RG_BUCK_RZSEL_MASK 0x3
#define BANK_0_RG_BUCK_RZSEL_SHIFT 0x0

//BUCK - CON1
#define BANK_0_RG_BUCK_SLP_MASK 0x3
#define BANK_0_RG_BUCK_SLP_SHIFT 0x6

#define BANK_0_RG_BUCK_ZX_OS_MASK 0x3
#define BANK_0_RG_BUCK_ZX_OS_SHIFT 0x4

#define BANK_0_RG_BUCK_SLEW_MASK 0x3
#define BANK_0_RG_BUCK_SLEW_SHIFT 0x2

#define BANK_0_RG_BUCK_SLEW_NMOS_MASK 0x3
#define BANK_0_RG_BUCK_SLEW_NMOS_SHIFT 0x0

//BUCK - CON2
#define BANK_0_RG_BUCK_AVP_OS_MASK 0x7
#define BANK_0_RG_BUCK_AVP_OS_SHIFT 0x4

#define BANK_0_RG_BUCK_AVP_EN_MASK 0x1
#define BANK_0_RG_BUCK_AVP_EN_SHIFT 0x1

#define BANK_0_RG_BUCK_MODESET_MASK 0x1
#define BANK_0_RG_BUCK_MODESET_SHIFT 0x0

//BUCK - CON3
#define BANK_0_QI_BUCK_EN_MASK 0x1
#define BANK_0_QI_BUCK_EN_SHIFT 0x7

#define BANK_0_RG_BUCK_ON_CTRL_MASK 0x1
#define BANK_0_RG_BUCK_ON_CTRL_SHIFT 0x1

#define BANK_0_RG_BUCK_EN_MASK 0x1
#define BANK_0_RG_BUCK_EN_SHIFT 0x0

//BUCK - CON4
#define BANK_0_RG_BUCK_STBTD_MASK 0x3
#define BANK_0_RG_BUCK_STBTD_SHIFT 0x6

#define BANK_0_RG_BUCK_BURST_MASK 0x3
#define BANK_0_RG_BUCK_BURST_SHIFT 0x4

#define BANK_0_RG_BUCK_VFBADJ_MASK 0x7
#define BANK_0_RG_BUCK_VFBADJ_SHIFT 0x1

#define BANK_0_RG_BUCK_OCFB_EN_MASK 0x1
#define BANK_0_RG_BUCK_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_BUCK_NDIS_EN_MASK 0x1
#define BANK_0_RG_BUCK_NDIS_EN_SHIFT 0x0

//BUCK - CON5
#define BANK_0_RG_BUCK_VOSEL_MASK 0x1F
#define BANK_0_RG_BUCK_VOSEL_SHIFT 0x0

//BUCK - CON6
#define BANK_0_BUCK_VOSEL_SRCLKEN0_MASK 0x1F
#define BANK_0_BUCK_VOSEL_SRCLKEN0_SHIFT 0x0

#define BANK_0_RG_BUCK_VOSEL_CON1_MASK 0x1F
#define BANK_0_RG_BUCK_VOSEL_CON1_SHIFT 0x0

#define BANK_0_RG_PASEL_SET0_MASK 0x1F
#define BANK_0_RG_PASEL_SET0_SHIFT 0x0

//BUCK - CON7
#define BANK_0_BUCK_VOSEL_SRCLKEN1_MASK 0x1F
#define BANK_0_BUCK_VOSEL_SRCLKEN1_SHIFT 0x0

#define BANK_0_RO_QI_BUCK_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_BUCK_OC_STATUS_SHIFT 0x0

#define BANK_0_RG_PASEL_SET1_MASK 0x1F
#define BANK_0_RG_PASEL_SET1_SHIFT 0x0

//BUCK - CON8
#define BANK_0_BUCK_VOSEL_DVS00_MASK 0x1F
#define BANK_0_BUCK_VOSEL_DVS00_SHIFT 0x0

#define BANK_0_RG_PASEL_SET2_MASK 0x1F
#define BANK_0_RG_PASEL_SET2_SHIFT 0x0

//BUCK - CON9
#define BANK_0_BUCK_VOSEL_DVS01_MASK 0x1F
#define BANK_0_BUCK_VOSEL_DVS01_SHIFT 0x0

#define BANK_0_RG_PASEL_SET3_MASK 0x1F
#define BANK_0_RG_PASEL_SET3_SHIFT 0x0

//BUCK - CONA
#define BANK_0_BUCK_VOSEL_DVS10_MASK 0x1F
#define BANK_0_BUCK_VOSEL_DVS10_SHIFT 0x0

#define BANK_0_RG_PASEL_SET4_MASK 0x1F
#define BANK_0_RG_PASEL_SET4_SHIFT 0x0

//BUCK - CONB
#define BANK_0_BUCK_VOSEL_DVS11_MASK 0x1F
#define BANK_0_BUCK_VOSEL_DVS11_SHIFT 0x0

#define BANK_0_RG_PASEL_SET5_MASK 0x1F
#define BANK_0_RG_PASEL_SET5_SHIFT 0x0

//BUCK - CONC
#define BANK_0_RG_BUCK_VOSEL_SFCHG_EN_MASK 0x1
#define BANK_0_RG_BUCK_VOSEL_SFCHG_EN_SHIFT 0x2

#define BANK_0_RG_BUCK_CTRL_MASK 0x3
#define BANK_0_RG_BUCK_CTRL_SHIFT 0x0

#define BANK_0_RG_PASEL_SET6_MASK 0x1F
#define BANK_0_RG_PASEL_SET6_SHIFT 0x0

//BUCK - COND
#define BANK_0_RG_BUCK_SMPS_TESTMODE_MASK 0x3F
#define BANK_0_RG_BUCK_SMPS_TESTMODE_SHIFT 0x0

#define BANK_0_RG_PASEL_SET7_MASK 0x1F
#define BANK_0_RG_PASEL_SET7_SHIFT 0x0

//BUCK - CONE

//BUCK - CONF
#define BANK_0_QI_BUCK_VOSEL_MASK 0x1F
#define BANK_0_QI_BUCK_VOSEL_SHIFT 0x0

//LDO - CON0
#define BANK_0_RG_LDO_CAL_MASK 0x1F // only VM12_INT need 0x1F, others is 0xF
#define BANK_0_RG_LDO_CAL_SHIFT 0x0

//LDO - CON1
#define BANK_0_QI_LDO_EN_MASK 0x1
#define BANK_0_QI_LDO_EN_SHIFT 0x7

#define BANK_0_RG_LDO_VOSEL_MASK 0x7
#define BANK_0_RG_LDO_VOSEL_SHIFT 0x4

#define BANK_0_LDO_ON_CTRL_MASK 0x1
#define BANK_0_LDO_ON_CTRL_SHIFT 0x1

#define BANK_0_LDO_EN_MASK 0x1
#define BANK_0_LDO_EN_SHIFT 0x0

//LDO - CON2
#define BANK_0_RG_LDO_STBTD_MASK 0x3
#define BANK_0_RG_LDO_STBTD_SHIFT 0x4

#define BANK_0_RG_LDO_THR_SHDN_EN_MASK 0x1
#define BANK_0_RG_LDO_THR_SHDN_EN_SHIFT 0x3

#define BANK_0_RG_LDO_STB_SEL_MASK 0x1
#define BANK_0_RG_LDO_STB_SEL_SHIFT 0x2

#define BANK_0_RG_LDO_OCFB_EN_MASK 0x1
#define BANK_0_RG_LDO_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_LDO_NDIS_EN_MASK 0x1
#define BANK_0_RG_LDO_NDIS_EN_SHIFT 0x0

//LDO - CON3
#define BANK_0_QI_LDO_MODE_MASK 0x1
#define BANK_0_QI_LDO_MODE_SHIFT 0x7

#define BANK_0_LDO_LP_SET_MASK 0x1
#define BANK_0_LDO_LP_SET_SHIFT 0x1

#define BANK_0_LDO_LP_SEL_MASK 0x1
#define BANK_0_LDO_LP_SEL_SHIFT 0x0

//==============================================================================
// PMIC6329 BANK 0
//==============================================================================
#define BANK_0_CID0_MASK 0xFF
#define BANK_0_CID0_SHIFT 0x0

#define BANK_0_CID1_MASK 0xFF
#define BANK_0_CID1_SHIFT 0x0

#define BANK_0_VCORE_STATUS_MASK 0x1
#define BANK_0_VCORE_STATUS_SHIFT 0x7

#define BANK_0_VPROC_STATUS_MASK 0x1
#define BANK_0_VPROC_STATUS_SHIFT 0x6

#define BANK_0_VIO18_STATUS_MASK 0x1
#define BANK_0_VIO18_STATUS_SHIFT 0x5

#define BANK_0_VRF18_STATUS_MASK 0x1
#define BANK_0_VRF18_STATUS_SHIFT 0x4

#define BANK_0_VPA_STATUS_MASK 0x1
#define BANK_0_VPA_STATUS_SHIFT 0x3

#define BANK_0_VRF_STATUS_MASK 0x1
#define BANK_0_VRF_STATUS_SHIFT 0x2

#define BANK_0_VTCXO_STATUS_MASK 0x1
#define BANK_0_VTCXO_STATUS_SHIFT 0x1

#define BANK_0_VA1_STATUS_MASK 0x1
#define BANK_0_VA1_STATUS_SHIFT 0x0

#define BANK_0_VCAMA_STATUS_MASK 0x1
#define BANK_0_VCAMA_STATUS_SHIFT 0x7

#define BANK_0_VM12_1_STATUS_MASK 0x1
#define BANK_0_VM12_1_STATUS_SHIFT 0x6

#define BANK_0_VM12_INT_STATUS_MASK 0x1
#define BANK_0_VM12_INT_STATUS_SHIFT 0x5

#define BANK_0_VIO28_STATUS_MASK 0x1
#define BANK_0_VIO28_STATUS_SHIFT 0x4

#define BANK_0_VSIM1_STATUS_MASK 0x1
#define BANK_0_VSIM1_STATUS_SHIFT 0x3

#define BANK_0_VSIM2_STATUS_MASK 0x1
#define BANK_0_VSIM2_STATUS_SHIFT 0x2

#define BANK_0_VUSB_STATUS_MASK 0x1
#define BANK_0_VUSB_STATUS_SHIFT 0x1

#define BANK_0_VCAMD_STATUS_MASK 0x1
#define BANK_0_VCAMD_STATUS_SHIFT 0x0

#define BANK_0_VCAM_IO_STATUS_MASK 0x1
#define BANK_0_VCAM_IO_STATUS_SHIFT 0x7

#define BANK_0_VCAM_AF_STATUS_MASK 0x1
#define BANK_0_VCAM_AF_STATUS_SHIFT 0x6

#define BANK_0_VMC_STATUS_MASK 0x1
#define BANK_0_VMC_STATUS_SHIFT 0x5

#define BANK_0_VMCH_STATUS_MASK 0x1
#define BANK_0_VMCH_STATUS_SHIFT 0x4

#define BANK_0_VGP_STATUS_MASK 0x1
#define BANK_0_VGP_STATUS_SHIFT 0x3

#define BANK_0_VGP2_STATUS_MASK 0x1
#define BANK_0_VGP2_STATUS_SHIFT 0x2

#define BANK_0_VM12_2_STATUS_MASK 0x1
#define BANK_0_VM12_2_STATUS_SHIFT 0x1

#define BANK_0_VIBR_STATUS_MASK 0x1
#define BANK_0_VIBR_STATUS_SHIFT 0x0

#define BANK_0_VA2_STATUS_MASK 0x1
#define BANK_0_VA2_STATUS_SHIFT 0x5

#define BANK_0_VRTC_STATUS_MASK 0x1
#define BANK_0_VRTC_STATUS_SHIFT 0x4

#define BANK_0_VTCXO_PG_STATUS_MASK 0x1
#define BANK_0_VTCXO_PG_STATUS_SHIFT 0x0

#define BANK_0_VCORE_PG_STATUS_MASK 0x1
#define BANK_0_VCORE_PG_STATUS_SHIFT 0x7

#define BANK_0_VPROC_PG_STATUS_MASK 0x1
#define BANK_0_VPROC_PG_STATUS_SHIFT 0x6

#define BANK_0_VIO18_PG_STATUS_MASK 0x1
#define BANK_0_VIO18_PG_STATUS_SHIFT 0x5

#define BANK_0_VIO28_PG_STATUS_MASK 0x1
#define BANK_0_VIO28_PG_STATUS_SHIFT 0x4

#define BANK_0_VA1_PG_STATUS_MASK 0x1
#define BANK_0_VA1_PG_STATUS_SHIFT 0x3

#define BANK_0_VM12_1_PG_STATUS_MASK 0x1
#define BANK_0_VM12_1_PG_STATUS_SHIFT 0x2

#define BANK_0_VM12_2_PG_STATUS_MASK 0x1
#define BANK_0_VM12_2_PG_STATUS_SHIFT 0x1

#define BANK_0_VM12_INT_PG_STATUS_MASK 0x1
#define BANK_0_VM12_INT_PG_STATUS_SHIFT 0x0

#define BANK_0_CV_MASK 0x1
#define BANK_0_CV_SHIFT 0x7

#define BANK_0_RO_BATON_UNDET_MASK 0x1
#define BANK_0_RO_BATON_UNDET_SHIFT 0x6

#define BANK_0_PCHR_CHRDET_MASK 0x1
#define BANK_0_PCHR_CHRDET_SHIFT 0x5

#define BANK_0_VBAT_OV_MASK 0x1
#define BANK_0_VBAT_OV_SHIFT 0x4

#define BANK_0_PWRKEY_DEB_MASK 0x1
#define BANK_0_PWRKEY_DEB_SHIFT 0x3

#define BANK_0_USBDL_MASK 0x1
#define BANK_0_USBDL_SHIFT 0x2

#define BANK_0_TEST_MODE_POR_MASK 0x1
#define BANK_0_TEST_MODE_POR_SHIFT 0x0

#define BANK_0_VCAMA_OC_STATUS_MASK 0x1
#define BANK_0_VCAMA_OC_STATUS_SHIFT 0x7

#define BANK_0_VM12_1_OC_STATUS_MASK 0x1
#define BANK_0_VM12_1_OC_STATUS_SHIFT 0x6

#define BANK_0_VM12_OC_STATUS_MASK 0x1
#define BANK_0_VM12_OC_STATUS_SHIFT 0x5

#define BANK_0_VIO28_OC_STATUS_MASK 0x1
#define BANK_0_VIO28_OC_STATUS_SHIFT 0x4

#define BANK_0_VSIM1_OC_STATUS_MASK 0x1
#define BANK_0_VSIM1_OC_STATUS_SHIFT 0x3

#define BANK_0_VSIM2_OC_STATUS_MASK 0x1
#define BANK_0_VSIM2_OC_STATUS_SHIFT 0x2

#define BANK_0_VUSB_OC_STATUS_MASK 0x1
#define BANK_0_VUSB_OC_STATUS_SHIFT 0x1

#define BANK_0_VCAMD_OC_STATUS_MASK 0x1
#define BANK_0_VCAMD_OC_STATUS_SHIFT 0x0

#define BANK_0_VCAM_IO_OC_STATUS_MASK 0x1
#define BANK_0_VCAM_IO_OC_STATUS_SHIFT 0x7

#define BANK_0_VCAM_AF_OC_STATUS_MASK 0x1
#define BANK_0_VCAM_AF_OC_STATUS_SHIFT 0x6

#define BANK_0_VMC_OC_STATUS_MASK 0x1
#define BANK_0_VMC_OC_STATUS_SHIFT 0x5

#define BANK_0_VMCH_OC_STATUS_MASK 0x1
#define BANK_0_VMCH_OC_STATUS_SHIFT 0x4

#define BANK_0_VGP_OC_STATUS_MASK 0x1
#define BANK_0_VGP_OC_STATUS_SHIFT 0x3

#define BANK_0_VGP2_OC_STATUS_MASK 0x1
#define BANK_0_VGP2_OC_STATUS_SHIFT 0x2

#define BANK_0_VM12_2_OC_STATUS_MASK 0x1
#define BANK_0_VM12_2_OC_STATUS_SHIFT 0x1

#define BANK_0_VIBR_OC_STATUS_MASK 0x1
#define BANK_0_VIBR_OC_STATUS_SHIFT 0x0

#define BANK_0_HOMEKEY_DEB_MASK 0x1
#define BANK_0_HOMEKEY_DEB_SHIFT 0x6

#define BANK_0_BOOST_OC_STATUS_MASK 0x1
#define BANK_0_BOOST_OC_STATUS_SHIFT 0x5

#define BANK_0_VA2_OC_STATUS_MASK 0x1
#define BANK_0_VA2_OC_STATUS_SHIFT 0x4

#define BANK_0_NI_SPK_OC_DET_D_R_MASK 0x1
#define BANK_0_NI_SPK_OC_DET_D_R_SHIFT 0x3

#define BANK_0_NI_SPK_OC_DET_D_L_MASK 0x1
#define BANK_0_NI_SPK_OC_DET_D_L_SHIFT 0x2

#define BANK_0_NI_SPK_OC_DET_AB_R_MASK 0x1
#define BANK_0_NI_SPK_OC_DET_AB_R_SHIFT 0x1

#define BANK_0_NI_SPK_OC_DET_AB_L_MASK 0x1
#define BANK_0_NI_SPK_OC_DET_AB_L_SHIFT 0x0

#define BANK_0_VCORE_OC_STATUS_MASK 0x1
#define BANK_0_VCORE_OC_STATUS_SHIFT 0x7

#define BANK_0_VPROC_OC_STATUS_MASK 0x1
#define BANK_0_VPROC_OC_STATUS_SHIFT 0x6

#define BANK_0_VIO18_OC_STATUS_MASK 0x1
#define BANK_0_VIO18_OC_STATUS_SHIFT 0x5

#define BANK_0_VRF18_OC_STATUS_MASK 0x1
#define BANK_0_VRF18_OC_STATUS_SHIFT 0x4

#define BANK_0_VPA_OC_STATUS_MASK 0x1
#define BANK_0_VPA_OC_STATUS_SHIFT 0x3

#define BANK_0_VRF_OC_STATUS_MASK 0x1
#define BANK_0_VRF_OC_STATUS_SHIFT 0x2

#define BANK_0_VTCXO_OC_STATUS_MASK 0x1
#define BANK_0_VTCXO_OC_STATUS_SHIFT 0x1

#define BANK_0_VA1_OC_STATUS_MASK 0x1
#define BANK_0_VA1_OC_STATUS_SHIFT 0x0

#define BANK_0_RG_THRDET_SEL_MASK 0x1
#define BANK_0_RG_THRDET_SEL_SHIFT 0x6

#define BANK_0_RG_THR_HWPDN_EN_MASK 0x1
#define BANK_0_RG_THR_HWPDN_EN_SHIFT 0x5

#define BANK_0_RG_STRUP_THR_SEL_MASK 0x3
#define BANK_0_RG_STRUP_THR_SEL_SHIFT 0x3

#define BANK_0_RG_THR_TMODE_MASK 0x1
#define BANK_0_RG_THR_TMODE_SHIFT 0x1

#define BANK_0_RG_FORCE_NON_THERMAL_MASK 0x1
#define BANK_0_RG_FORCE_NON_THERMAL_SHIFT 0x0

#define BANK_0_RG_VREF_BG_MASK 0x7
#define BANK_0_RG_VREF_BG_SHIFT 0x4

#define BANK_0_RG_STRUP_IREF_TRIM_MASK 0xF
#define BANK_0_RG_STRUP_IREF_TRIM_SHIFT 0x0

#define BANK_0_RG_BB_PROC_SEL_MASK 0x1
#define BANK_0_RG_BB_PROC_SEL_SHIFT 0x7

#define BANK_0_RG_STRUP_RST_DRVSEL_MASK 0x1
#define BANK_0_RG_STRUP_RST_DRVSEL_SHIFT 0x6

#define BANK_0_RG_PMU_LEV_UNGATE_MASK 0x1
#define BANK_0_RG_PMU_LEV_UNGATE_SHIFT 0x1

#define BANK_0_RG_USBDL_EN_MASK 0x1
#define BANK_0_RG_USBDL_EN_SHIFT 0x0

#define BANK_0_RG_PMU_RSV_MASK 0xF
#define BANK_0_RG_PMU_RSV_SHIFT 0x4

#define BANK_0_RG_BIAS_GEN_EN_FORCE_MASK 0x1
#define BANK_0_RG_BIAS_GEN_EN_FORCE_SHIFT 0x5

#define BANK_0_RG_STRUP_PWRON_FORCE_MASK 0x1
#define BANK_0_RG_STRUP_PWRON_FORCE_SHIFT 0x4

#define BANK_0_RG_STRUP_TEST_MASK 0x1
#define BANK_0_RG_STRUP_TEST_SHIFT 0x3

#define BANK_0_RG_STRUP_FT_CTRL_MASK 0x3
#define BANK_0_RG_STRUP_FT_CTRL_SHIFT 0x0

#define BANK_0_PMU_THR_STATUS_MASK 0x7
#define BANK_0_PMU_THR_STATUS_SHIFT 0x4

#define BANK_0_RG_THERMAL_TEST_MASK 0x3
#define BANK_0_RG_THERMAL_TEST_SHIFT 0x2

#define BANK_0_RG_VCORE_PG_ENB_MASK 0x1
#define BANK_0_RG_VCORE_PG_ENB_SHIFT 0x7

#define BANK_0_RG_VPROC_PG_ENB_MASK 0x1
#define BANK_0_RG_VPROC_PG_ENB_SHIFT 0x6

#define BANK_0_RG_VIO18_PG_ENB_MASK 0x1
#define BANK_0_RG_VIO18_PG_ENB_SHIFT 0x5

#define BANK_0_RG_VIO28_PG_ENB_MASK 0x1
#define BANK_0_RG_VIO28_PG_ENB_SHIFT 0x4

#define BANK_0_RG_VA1_PG_ENB_MASK 0x1
#define BANK_0_RG_VA1_PG_ENB_SHIFT 0x3

#define BANK_0_RG_VM12_1_PG_ENB_MASK 0x1
#define BANK_0_RG_VM12_1_PG_ENB_SHIFT 0x2

#define BANK_0_RG_VM12_2_PG_ENB_MASK 0x1
#define BANK_0_RG_VM12_2_PG_ENB_SHIFT 0x1

#define BANK_0_RG_VM12_INT_PG_ENB_MASK 0x1
#define BANK_0_RG_VM12_INT_PG_ENB_SHIFT 0x0

#define BANK_0_RG_STRUP_DIG_RSV_MASK 0xF
#define BANK_0_RG_STRUP_DIG_RSV_SHIFT 0x4

#define BANK_0_RG_VTCXO_PG_ENB_MASK 0x1
#define BANK_0_RG_VTCXO_PG_ENB_SHIFT 0x0

#define BANK_0_RG_OV_INT_EN_MASK 0x1
#define BANK_0_RG_OV_INT_EN_SHIFT 0x7

#define BANK_0_RG_CHRDET_INT_EN_MASK 0x1
#define BANK_0_RG_CHRDET_INT_EN_SHIFT 0x6

#define BANK_0_RG_BVALID_DET_INT_EN_MASK 0x1
#define BANK_0_RG_BVALID_DET_INT_EN_SHIFT 0x5

#define BANK_0_RG_VBATON_UNDET_INT_EN_MASK 0x1
#define BANK_0_RG_VBATON_UNDET_INT_EN_SHIFT 0x4

#define BANK_0_RG_THR_H_INT_EN_MASK 0x1
#define BANK_0_RG_THR_H_INT_EN_SHIFT 0x3

#define BANK_0_RG_THR_L_INT_EN_MASK 0x1
#define BANK_0_RG_THR_L_INT_EN_SHIFT 0x2

#define BANK_0_RG_PWRKEY_INT_EN_MASK 0x1
#define BANK_0_RG_PWRKEY_INT_EN_SHIFT 0x1

#define BANK_0_RG_WATCHDOG_INT_EN_MASK 0x1
#define BANK_0_RG_WATCHDOG_INT_EN_SHIFT 0x0

#define BANK_0_RG_FG_BAT_H_INT_EN_MASK 0x1
#define BANK_0_RG_FG_BAT_H_INT_EN_SHIFT 0x7

#define BANK_0_RG_FG_BAT_L_INT_EN_MASK 0x1
#define BANK_0_RG_FG_BAT_L_INT_EN_SHIFT 0x6

#define BANK_0_RG_HIGH_BAT_INT_EN_MASK 0x1
#define BANK_0_RG_HIGH_BAT_INT_EN_SHIFT 0x5

#define BANK_0_RG_LOW_BAT_INT_EN_MASK 0x1
#define BANK_0_RG_LOW_BAT_INT_EN_SHIFT 0x4

#define BANK_0_RG_SPKR_D_OC_INT_EN_MASK 0x1
#define BANK_0_RG_SPKR_D_OC_INT_EN_SHIFT 0x3

#define BANK_0_RG_SPKL_D_OC_INT_EN_MASK 0x1
#define BANK_0_RG_SPKL_D_OC_INT_EN_SHIFT 0x2

#define BANK_0_RG_SPKR_AB_OC_INT_EN_MASK 0x1
#define BANK_0_RG_SPKR_AB_OC_INT_EN_SHIFT 0x1

#define BANK_0_RG_SPKL_AB_OC_INT_EN_MASK 0x1
#define BANK_0_RG_SPKL_AB_OC_INT_EN_SHIFT 0x0

#define BANK_0_RG_HOMEKEY_INT_EN_MASK 0x1
#define BANK_0_RG_HOMEKEY_INT_EN_SHIFT 0x5

#define BANK_0_RG_VRF18_OC_INT_EN_MASK 0x1
#define BANK_0_RG_VRF18_OC_INT_EN_SHIFT 0x4

#define BANK_0_RG_VPA_OC_INT_EN_MASK 0x1
#define BANK_0_RG_VPA_OC_INT_EN_SHIFT 0x3

#define BANK_0_RG_LDO_OC_INT_EN_MASK 0x1
#define BANK_0_RG_LDO_OC_INT_EN_SHIFT 0x2

#define BANK_0_RG_OV_INT_STATUS_MASK 0x1
#define BANK_0_RG_OV_INT_STATUS_SHIFT 0x7

#define BANK_0_RG_CHRDET_INT_STATUS_MASK 0x1
#define BANK_0_RG_CHRDET_INT_STATUS_SHIFT 0x6

#define BANK_0_RG_BVALID_DET_INT_STATUS_MASK 0x1
#define BANK_0_RG_BVALID_DET_INT_STATUS_SHIFT 0x5

#define BANK_0_RG_VBATON_UNDET_INT_STATUS_MASK 0x1
#define BANK_0_RG_VBATON_UNDET_INT_STATUS_SHIFT 0x4

#define BANK_0_RG_THR_H_INT_STATUS_MASK 0x1
#define BANK_0_RG_THR_H_INT_STATUS_SHIFT 0x3

#define BANK_0_RG_THR_L_INT_STATUS_MASK 0x1
#define BANK_0_RG_THR_L_INT_STATUS_SHIFT 0x2

#define BANK_0_RG_PWRKEY_INT_STATUS_MASK 0x1
#define BANK_0_RG_PWRKEY_INT_STATUS_SHIFT 0x1

#define BANK_0_RG_WATCHDOG_INT_STATUS_MASK 0x1
#define BANK_0_RG_WATCHDOG_INT_STATUS_SHIFT 0x0

#define BANK_0_RG_FG_BAT_H_INT_STATUS_MASK 0x1
#define BANK_0_RG_FG_BAT_H_INT_STATUS_SHIFT 0x7

#define BANK_0_RG_FG_BAT_L_INT_STATUS_MASK 0x1
#define BANK_0_RG_FG_BAT_L_INT_STATUS_SHIFT 0x6

#define BANK_0_RG_HIGH_BAT_INT_STATUS_MASK 0x1
#define BANK_0_RG_HIGH_BAT_INT_STATUS_SHIFT 0x5

#define BANK_0_RG_LOW_BAT_INT_STATUS_MASK 0x1
#define BANK_0_RG_LOW_BAT_INT_STATUS_SHIFT 0x4

#define BANK_0_RG_SPKR_D_OC_INT_STATUS_MASK 0x1
#define BANK_0_RG_SPKR_D_OC_INT_STATUS_SHIFT 0x3

#define BANK_0_RG_SPKL_D_OC_INT_STATUS_MASK 0x1
#define BANK_0_RG_SPKL_D_OC_INT_STATUS_SHIFT 0x2

#define BANK_0_RG_SPKR_AB_OC_INT_STATUS_MASK 0x1
#define BANK_0_RG_SPKR_AB_OC_INT_STATUS_SHIFT 0x1

#define BANK_0_RG_SPKL_AB_OC_INT_STATUS_MASK 0x1
#define BANK_0_RG_SPKL_AB_OC_INT_STATUS_SHIFT 0x0

#define BANK_0_RG_HOMEKEY_INT_STATUS_MASK 0x1
#define BANK_0_RG_HOMEKEY_INT_STATUS_SHIFT 0x5

#define BANK_0_RG_VRF18_OC_INT_STATUS_MASK 0x1
#define BANK_0_RG_VRF18_OC_INT_STATUS_SHIFT 0x4

#define BANK_0_RG_VPA_OC_INT_STATUS_MASK 0x1
#define BANK_0_RG_VPA_OC_INT_STATUS_SHIFT 0x3

#define BANK_0_RG_LDO_OC_INT_STATUS_MASK 0x1
#define BANK_0_RG_LDO_OC_INT_STATUS_SHIFT 0x2

#define BANK_0_VCDT_HV_DET_MASK 0x1
#define BANK_0_VCDT_HV_DET_SHIFT 0x7

#define BANK_0_VCDT_LV_DET_MASK 0x1
#define BANK_0_VCDT_LV_DET_SHIFT 0x6

#define BANK_0_CHRDET_MASK 0x1
#define BANK_0_CHRDET_SHIFT 0x5

#define BANK_0_CHR_EN_MASK 0x1
#define BANK_0_CHR_EN_SHIFT 0x4

#define BANK_0_CSDAC_EN_MASK 0x1
#define BANK_0_CSDAC_EN_SHIFT 0x3

#define BANK_0_PCHR_AUTO_MASK 0x1
#define BANK_0_PCHR_AUTO_SHIFT 0x2

#define BANK_0_CHR_LDO_DET_MASK 0x1
#define BANK_0_CHR_LDO_DET_SHIFT 0x1

#define BANK_0_VCDT_HV_EN_MASK 0x1
#define BANK_0_VCDT_HV_EN_SHIFT 0x0

#define BANK_0_VCDT_HV_VTH_MASK 0xF
#define BANK_0_VCDT_HV_VTH_SHIFT 0x4

#define BANK_0_VCDT_LV_VTH_MASK 0xF
#define BANK_0_VCDT_LV_VTH_SHIFT 0x0

#define BANK_0_VBAT_CC_DET_MASK 0x1
#define BANK_0_VBAT_CC_DET_SHIFT 0x7

#define BANK_0_VBAT_CV_DET_MASK 0x1
#define BANK_0_VBAT_CV_DET_SHIFT 0x6

#define BANK_0_CS_DET_MASK 0x1
#define BANK_0_CS_DET_SHIFT 0x5

#define BANK_0_CS_EN_MASK 0x1
#define BANK_0_CS_EN_SHIFT 0x3

#define BANK_0_VBAT_CC_EN_MASK 0x1
#define BANK_0_VBAT_CC_EN_SHIFT 0x2

#define BANK_0_VBAT_CV_EN_MASK 0x1
#define BANK_0_VBAT_CV_EN_SHIFT 0x1

#define BANK_0_VBAT_CC_VTH_MASK 0x3
#define BANK_0_VBAT_CC_VTH_SHIFT 0x6

#define BANK_0_VBAT_CV_VTH_MASK 0x1F
#define BANK_0_VBAT_CV_VTH_SHIFT 0x0

#define BANK_0_CS_VTH_MASK 0xF
#define BANK_0_CS_VTH_SHIFT 0x0

#define BANK_0_TOLTC_MASK 0x7
#define BANK_0_TOLTC_SHIFT 0x4

#define BANK_0_TOHTC_MASK 0x7
#define BANK_0_TOHTC_SHIFT 0x0

#define BANK_0_VBAT_OV_DET_MASK 0x1
#define BANK_0_VBAT_OV_DET_SHIFT 0x6

#define BANK_0_VBAT_OV_DEG_MASK 0x1
#define BANK_0_VBAT_OV_DEG_SHIFT 0x5

#define BANK_0_VBAT_OV_VTH_MASK 0x3
#define BANK_0_VBAT_OV_VTH_SHIFT 0x1

#define BANK_0_VBAT_OV_EN_MASK 0x1
#define BANK_0_VBAT_OV_EN_SHIFT 0x0

#define BANK_0_BATON_UNDET_MASK 0x1
#define BANK_0_BATON_UNDET_SHIFT 0x7

#define BANK_0_BATON_HT_TRIM_MASK 0x7
#define BANK_0_BATON_HT_TRIM_SHIFT 0x4

#define BANK_0_BATON_HT_EN_MASK 0x1
#define BANK_0_BATON_HT_EN_SHIFT 0x2

#define BANK_0_BATON_EN_MASK 0x1
#define BANK_0_BATON_EN_SHIFT 0x1

#define BANK_0_CSDAC_DAT_H_MASK 0x3
#define BANK_0_CSDAC_DAT_H_SHIFT 0x0

#define BANK_0_CSDAC_DAT_L_MASK 0xFF
#define BANK_0_CSDAC_DAT_L_SHIFT 0x0

#define BANK_0_OTG_BVALID_MASK 0x1
#define BANK_0_OTG_BVALID_SHIFT 0x6

#define BANK_0_OTG_BVALID_EN_MASK 0x1
#define BANK_0_OTG_BVALID_EN_SHIFT 0x5

#define BANK_0_PCHR_FLAG_EN_MASK 0x1
#define BANK_0_PCHR_FLAG_EN_SHIFT 0x4

#define BANK_0_PCHR_FLAG_OUT_MASK 0xF
#define BANK_0_PCHR_FLAG_OUT_SHIFT 0x0

#define BANK_0_PCHR_FLAG_SEL_MASK 0x3F
#define BANK_0_PCHR_FLAG_SEL_SHIFT 0x0

#define BANK_0_PCHR_FT_CTRL_MASK 0x7
#define BANK_0_PCHR_FT_CTRL_SHIFT 0x4

#define BANK_0_PCHR_RST_MASK 0x1
#define BANK_0_PCHR_RST_SHIFT 0x2

#define BANK_0_CSDAC_TEST_MASK 0x1
#define BANK_0_CSDAC_TEST_SHIFT 0x1

#define BANK_0_PCHR_TEST_MASK 0x1
#define BANK_0_PCHR_TEST_SHIFT 0x0

#define BANK_0_CHRWDT_EN_MASK 0x1
#define BANK_0_CHRWDT_EN_SHIFT 0x4

#define BANK_0_CHRWDT_TD_MASK 0xF
#define BANK_0_CHRWDT_TD_SHIFT 0x0

#define BANK_0_PCHR_RV_MASK 0xFF
#define BANK_0_PCHR_RV_SHIFT 0x0

#define BANK_0_CHRWDT_OUT_MASK 0x1
#define BANK_0_CHRWDT_OUT_SHIFT 0x2

#define BANK_0_CHRWDT_FLAG_WR_MASK 0x1
#define BANK_0_CHRWDT_FLAG_WR_SHIFT 0x1

#define BANK_0_CHRWDT_INT_EN_MASK 0x1
#define BANK_0_CHRWDT_INT_EN_SHIFT 0x0

#define BANK_0_ADCIN_VCHR_EN_MASK 0x1
#define BANK_0_ADCIN_VCHR_EN_SHIFT 0x6

#define BANK_0_ADCIN_VSEN_EN_MASK 0x1
#define BANK_0_ADCIN_VSEN_EN_SHIFT 0x5

#define BANK_0_ADCIN_VBAT_EN_MASK 0x1
#define BANK_0_ADCIN_VBAT_EN_SHIFT 0x4

#define BANK_0_USBDL_SET_MASK 0x1
#define BANK_0_USBDL_SET_SHIFT 0x3

#define BANK_0_USBDL_RST_MASK 0x1
#define BANK_0_USBDL_RST_SHIFT 0x2

#define BANK_0_UVLO_VTHL_MASK 0x3
#define BANK_0_UVLO_VTHL_SHIFT 0x0

#define BANK_0_BGR_UNCHOP_MASK 0x1
#define BANK_0_BGR_UNCHOP_SHIFT 0x5

#define BANK_0_BGR_UNCHOP_PH_MASK 0x1
#define BANK_0_BGR_UNCHOP_PH_SHIFT 0x4

#define BANK_0_BGR_RSEL_MASK 0x7
#define BANK_0_BGR_RSEL_SHIFT 0x0

#define BANK_0_BC11_CMP_OUT_MASK 0x1
#define BANK_0_BC11_CMP_OUT_SHIFT 0x7

#define BANK_0_RG_BC11_VSRC_EN_MASK 0x3
#define BANK_0_RG_BC11_VSRC_EN_SHIFT 0x2

#define BANK_0_BC11_RST_MASK 0x1
#define BANK_0_BC11_RST_SHIFT 0x1

#define BANK_0_BC11_BB_CTRL_MASK 0x1
#define BANK_0_BC11_BB_CTRL_SHIFT 0x0

#define BANK_0_BC11_BIAS_EN_MASK 0x1
#define BANK_0_BC11_BIAS_EN_SHIFT 0x7

#define BANK_0_BC11_IPU_EN_MASK 0x3
#define BANK_0_BC11_IPU_EN_SHIFT 0x5

#define BANK_0_BC11_IPD_EN_MASK 0x3
#define BANK_0_BC11_IPD_EN_SHIFT 0x3

#define BANK_0_BC11_CMP_EN_MASK 0x3
#define BANK_0_BC11_CMP_EN_SHIFT 0x1

#define BANK_0_BC11_VREF_VTH_MASK 0x1
#define BANK_0_BC11_VREF_VTH_SHIFT 0x0

#define BANK_0_CSDAC_STP_DEC_MASK 0x7
#define BANK_0_CSDAC_STP_DEC_SHIFT 0x4

#define BANK_0_CSDAC_STP_INC_MASK 0x7
#define BANK_0_CSDAC_STP_INC_SHIFT 0x0

#define BANK_0_CSDAC_STP_MASK 0x7
#define BANK_0_CSDAC_STP_SHIFT 0x4

#define BANK_0_CSDAC_DLY_MASK 0x7
#define BANK_0_CSDAC_DLY_SHIFT 0x0

#define BANK_0_RG_CHRIND_DIMMING_MASK 0x1
#define BANK_0_RG_CHRIND_DIMMING_SHIFT 0x7

#define BANK_0_RG_CHRIND_ON_MASK 0x1
#define BANK_0_RG_CHRIND_ON_SHIFT 0x6

#define BANK_0_RG_LOW_ICH_DB_MASK 0x3F
#define BANK_0_RG_LOW_ICH_DB_SHIFT 0x0

#define BANK_0_RG_ULC_DET_EN_MASK 0x1
#define BANK_0_RG_ULC_DET_EN_SHIFT 0x7

#define BANK_0_RG_HWCV_EN_MASK 0x1
#define BANK_0_RG_HWCV_EN_SHIFT 0x6

#define BANK_0_BATON_TDET_EN_MASK 0x1
#define BANK_0_BATON_TDET_EN_SHIFT 0x5

#define BANK_0_TRACKING_EN_MASK 0x1
#define BANK_0_TRACKING_EN_SHIFT 0x4

#define BANK_0_CSDAC_MODE_MASK 0x1
#define BANK_0_CSDAC_MODE_SHIFT 0x2

#define BANK_0_VCDT_MODE_MASK 0x1
#define BANK_0_VCDT_MODE_SHIFT 0x1

#define BANK_0_CV_MODE_MASK 0x1
#define BANK_0_CV_MODE_SHIFT 0x0

#define BANK_0_RG_ICHRG_TRIM_MASK 0xF
#define BANK_0_RG_ICHRG_TRIM_SHIFT 0x4

#define BANK_0_RG_BGR_TRIM_EN_MASK 0x1
#define BANK_0_RG_BGR_TRIM_EN_SHIFT 0x0

#define BANK_0_RG_BGR_TRIM_MASK 0x1F
#define BANK_0_RG_BGR_TRIM_SHIFT 0x0

#define BANK_0_RG_OVP_TRIM_MASK 0xF
#define BANK_0_RG_OVP_TRIM_SHIFT 0x0

#define BANK_0_CHR_RSV0_MASK 0x1
#define BANK_0_CHR_RSV0_SHIFT 0x7

#define BANK_0_RG_BGR_TEST_RSTB_MASK 0x1
#define BANK_0_RG_BGR_TEST_RSTB_SHIFT 0x6

#define BANK_0_RG_BGR_TEST_EN_MASK 0x1
#define BANK_0_RG_BGR_TEST_EN_SHIFT 0x5

#define BANK_0_RG_CHR_OSC_TRIM_MASK 0x1F
#define BANK_0_RG_CHR_OSC_TRIM_SHIFT 0x0

#define BANK_0_CHR_RSV1_MASK 0xFF
#define BANK_0_CHR_RSV1_SHIFT 0x0

#define BANK_0_RG_VPROC_CSL_MASK 0x3
#define BANK_0_RG_VPROC_CSL_SHIFT 0x6

#define BANK_0_RG_VPROC_CSR_MASK 0x3
#define BANK_0_RG_VPROC_CSR_SHIFT 0x4

#define BANK_0_RG_VPROC_CC_MASK 0x3
#define BANK_0_RG_VPROC_CC_SHIFT 0x2

#define BANK_0_RG_VPROC_RZSEL_MASK 0x3
#define BANK_0_RG_VPROC_RZSEL_SHIFT 0x0

#define BANK_0_RG_VPROC_SLP_MASK 0x3
#define BANK_0_RG_VPROC_SLP_SHIFT 0x6

#define BANK_0_RG_VPROC_ZX_OS_MASK 0x3
#define BANK_0_RG_VPROC_ZX_OS_SHIFT 0x4

#define BANK_0_RG_VPROC_SLEW_MASK 0x3
#define BANK_0_RG_VPROC_SLEW_SHIFT 0x2

#define BANK_0_RG_VPROC_SLEW_NMOS_MASK 0x3
#define BANK_0_RG_VPROC_SLEW_NMOS_SHIFT 0x0

#define BANK_0_RG_VPROC_AVP_OS_MASK 0x7
#define BANK_0_RG_VPROC_AVP_OS_SHIFT 0x4

#define BANK_0_RG_VPROC_AVP_EN_MASK 0x1
#define BANK_0_RG_VPROC_AVP_EN_SHIFT 0x1

#define BANK_0_RG_VPROC_MODESET_MASK 0x1
#define BANK_0_RG_VPROC_MODESET_SHIFT 0x0

#define BANK_0_QI_VPROC_EN_MASK 0x1
#define BANK_0_QI_VPROC_EN_SHIFT 0x7

#define BANK_0_RG_VPROC_EN_MASK 0x1
#define BANK_0_RG_VPROC_EN_SHIFT 0x0

#define BANK_0_RG_VPROC_BURST_MASK 0x3
#define BANK_0_RG_VPROC_BURST_SHIFT 0x4

#define BANK_0_RG_VPROC_VFBADJ_MASK 0x7
#define BANK_0_RG_VPROC_VFBADJ_SHIFT 0x1

#define BANK_0_RG_VPROC_NDIS_EN_MASK 0x1
#define BANK_0_RG_VPROC_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VPROC_VOSEL_MASK 0x1F
#define BANK_0_RG_VPROC_VOSEL_SHIFT 0x0

#define BANK_0_VPROC_VOSEL_SRCLKEN0_MASK 0x1F
#define BANK_0_VPROC_VOSEL_SRCLKEN0_SHIFT 0x0

#define BANK_0_VPROC_VOSEL_SRCLKEN1_MASK 0x1F
#define BANK_0_VPROC_VOSEL_SRCLKEN1_SHIFT 0x0

#define BANK_0_VPROC_VOSEL_DVS00_MASK 0x1F
#define BANK_0_VPROC_VOSEL_DVS00_SHIFT 0x0

#define BANK_0_VPROC_VOSEL_DVS01_MASK 0x1F
#define BANK_0_VPROC_VOSEL_DVS01_SHIFT 0x0

#define BANK_0_VPROC_VOSEL_DVS10_MASK 0x1F
#define BANK_0_VPROC_VOSEL_DVS10_SHIFT 0x0

#define BANK_0_VPROC_VOSEL_DVS11_MASK 0x1F
#define BANK_0_VPROC_VOSEL_DVS11_SHIFT 0x0

#define BANK_0_RG_VPROC_RSV_MASK 0xF
#define BANK_0_RG_VPROC_RSV_SHIFT 0x4

#define BANK_0_RG_VPROC_VOSEL_SFCHG_EN_MASK 0x1
#define BANK_0_RG_VPROC_VOSEL_SFCHG_EN_SHIFT 0x2

#define BANK_0_RG_VPROC_CTRL_MASK 0x3
#define BANK_0_RG_VPROC_CTRL_SHIFT 0x0

#define BANK_0_RG_SMPS_TESTMODE_MASK 0x3F
#define BANK_0_RG_SMPS_TESTMODE_SHIFT 0x0

#define BANK_0_RG_SMPS_RSV_MASK 0xFF
#define BANK_0_RG_SMPS_RSV_SHIFT 0x0

#define BANK_0_QI_VPROC_VOSEL_MASK 0x1F
#define BANK_0_QI_VPROC_VOSEL_SHIFT 0x0

#define BANK_0_RG_BUCK_RSV_MASK 0xFF
#define BANK_0_RG_BUCK_RSV_SHIFT 0x0

#define BANK_0_RG_VCORE_CSL_MASK 0x3
#define BANK_0_RG_VCORE_CSL_SHIFT 0x6

#define BANK_0_RG_VCORE_CSR_MASK 0x3
#define BANK_0_RG_VCORE_CSR_SHIFT 0x4

#define BANK_0_RG_VCORE_CC_MASK 0x3
#define BANK_0_RG_VCORE_CC_SHIFT 0x2

#define BANK_0_RG_VCORE_RZSEL_MASK 0x3
#define BANK_0_RG_VCORE_RZSEL_SHIFT 0x0

#define BANK_0_RG_VCORE_SLP_MASK 0x3
#define BANK_0_RG_VCORE_SLP_SHIFT 0x6

#define BANK_0_RG_VCORE_ZX_OS_MASK 0x3
#define BANK_0_RG_VCORE_ZX_OS_SHIFT 0x4

#define BANK_0_RG_VCORE_SLEW_MASK 0x3
#define BANK_0_RG_VCORE_SLEW_SHIFT 0x2

#define BANK_0_RG_VCORE_SLEW_NMOS_MASK 0x3
#define BANK_0_RG_VCORE_SLEW_NMOS_SHIFT 0x0

#define BANK_0_RG_VCORE_AVP_OS_MASK 0x7
#define BANK_0_RG_VCORE_AVP_OS_SHIFT 0x4

#define BANK_0_RG_VCORE_AVP_EN_MASK 0x1
#define BANK_0_RG_VCORE_AVP_EN_SHIFT 0x1

#define BANK_0_RG_VCORE_MODESET_MASK 0x1
#define BANK_0_RG_VCORE_MODESET_SHIFT 0x0

#define BANK_0_QI_VCORE_EN_MASK 0x1
#define BANK_0_QI_VCORE_EN_SHIFT 0x7

#define BANK_0_RG_VCORE_EN_MASK 0x1
#define BANK_0_RG_VCORE_EN_SHIFT 0x0

#define BANK_0_RG_VCORE_BURST_MASK 0x3
#define BANK_0_RG_VCORE_BURST_SHIFT 0x4

#define BANK_0_RG_VCORE_VFBADJ_MASK 0x7
#define BANK_0_RG_VCORE_VFBADJ_SHIFT 0x1

#define BANK_0_RG_VCORE_NDIS_EN_MASK 0x1
#define BANK_0_RG_VCORE_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VCORE_VOSEL_MASK 0x1F
#define BANK_0_RG_VCORE_VOSEL_SHIFT 0x0

#define BANK_0_RG_VCORE_VOSEL_CON1_MASK 0x1F
#define BANK_0_RG_VCORE_VOSEL_CON1_SHIFT 0x0

#define BANK_0_RG_VCORE_RSV_MASK 0xF
#define BANK_0_RG_VCORE_RSV_SHIFT 0x4

#define BANK_0_RG_VCORE_VOSEL_SFCHG_EN_MASK 0x1
#define BANK_0_RG_VCORE_VOSEL_SFCHG_EN_SHIFT 0x2

#define BANK_0_RG_VCORE_CTRL_MASK 0x1
#define BANK_0_RG_VCORE_CTRL_SHIFT 0x0

#define BANK_0_QI_VCORE_VOSEL_MASK 0x1F
#define BANK_0_QI_VCORE_VOSEL_SHIFT 0x0

#define BANK_0_RG_VRF18_CSL_MASK 0x3
#define BANK_0_RG_VRF18_CSL_SHIFT 0x6

#define BANK_0_RG_VRF18_CSR_MASK 0x3
#define BANK_0_RG_VRF18_CSR_SHIFT 0x4

#define BANK_0_RG_VRF18_CC_MASK 0x3
#define BANK_0_RG_VRF18_CC_SHIFT 0x2

#define BANK_0_RG_VRF18_RZSEL_MASK 0x3
#define BANK_0_RG_VRF18_RZSEL_SHIFT 0x0

#define BANK_0_RG_VRF18_SLP_MASK 0x3
#define BANK_0_RG_VRF18_SLP_SHIFT 0x6

#define BANK_0_RG_VRF18_ZX_OS_MASK 0x3
#define BANK_0_RG_VRF18_ZX_OS_SHIFT 0x4

#define BANK_0_RG_VRF18_SLEW_MASK 0x3
#define BANK_0_RG_VRF18_SLEW_SHIFT 0x2

#define BANK_0_RG_VRF18_SLEW_NMOS_MASK 0x3
#define BANK_0_RG_VRF18_SLEW_NMOS_SHIFT 0x0

#define BANK_0_RG_VRF18_AVP_EN_MASK 0x1
#define BANK_0_RG_VRF18_AVP_EN_SHIFT 0x1

#define BANK_0_RG_VRF18_MODESET_MASK 0x1
#define BANK_0_RG_VRF18_MODESET_SHIFT 0x0

#define BANK_0_QI_VRF18_EN_MASK 0x1
#define BANK_0_QI_VRF18_EN_SHIFT 0x7

#define BANK_0_RG_VRF18_ON_CTRL_MASK 0x1
#define BANK_0_RG_VRF18_ON_CTRL_SHIFT 0x1

#define BANK_0_RG_VRF18_EN_MASK 0x1
#define BANK_0_RG_VRF18_EN_SHIFT 0x0

#define BANK_0_RG_VRF18_STBTD_MASK 0x3
#define BANK_0_RG_VRF18_STBTD_SHIFT 0x6

#define BANK_0_RG_VRF18_BURST_MASK 0x3
#define BANK_0_RG_VRF18_BURST_SHIFT 0x4

#define BANK_0_RG_VRF18_OCFB_EN_MASK 0x1
#define BANK_0_RG_VRF18_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VRF18_NDIS_EN_MASK 0x1
#define BANK_0_RG_VRF18_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VRF18_VOSEL_MASK 0x1F
#define BANK_0_RG_VRF18_VOSEL_SHIFT 0x0

#define BANK_0_RG_VRF18_RSV_MASK 0xF
#define BANK_0_RG_VRF18_RSV_SHIFT 0x0

#define BANK_0_RO_QI_VRF18_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VRF18_OC_STATUS_SHIFT 0x0

#define BANK_0_RG_VIO18_CSL_MASK 0x3
#define BANK_0_RG_VIO18_CSL_SHIFT 0x6

#define BANK_0_RG_VIO18_CSR_MASK 0x3
#define BANK_0_RG_VIO18_CSR_SHIFT 0x4

#define BANK_0_RG_VIO18_CC_MASK 0x3
#define BANK_0_RG_VIO18_CC_SHIFT 0x2

#define BANK_0_RG_VIO18_RZSEL_MASK 0x3
#define BANK_0_RG_VIO18_RZSEL_SHIFT 0x0

#define BANK_0_RG_VIO18_SLP_MASK 0x3
#define BANK_0_RG_VIO18_SLP_SHIFT 0x6

#define BANK_0_RG_VIO18_ZX_OS_MASK 0x3
#define BANK_0_RG_VIO18_ZX_OS_SHIFT 0x4

#define BANK_0_RG_VIO18_SLEW_MASK 0x3
#define BANK_0_RG_VIO18_SLEW_SHIFT 0x2

#define BANK_0_RG_VIO18_SELW_NMOS_MASK 0x3
#define BANK_0_RG_VIO18_SELW_NMOS_SHIFT 0x0

#define BANK_0_RG_VIO18_AVP_EN_MASK 0x1
#define BANK_0_RG_VIO18_AVP_EN_SHIFT 0x1

#define BANK_0_RG_VIO18_MODESET_MASK 0x1
#define BANK_0_RG_VIO18_MODESET_SHIFT 0x0

#define BANK_0_NI_VIO18_EN_MASK 0x1
#define BANK_0_NI_VIO18_EN_SHIFT 0x7

#define BANK_0_RG_VIO18_EN_MASK 0x1
#define BANK_0_RG_VIO18_EN_SHIFT 0x0

#define BANK_0_RG_VIO18_BURST_MASK 0x3
#define BANK_0_RG_VIO18_BURST_SHIFT 0x4

#define BANK_0_RG_VIO18_NDIS_EN_MASK 0x1
#define BANK_0_RG_VIO18_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VIO18_VOSEL_MASK 0x1F
#define BANK_0_RG_VIO18_VOSEL_SHIFT 0x0

#define BANK_0_RG_VIO18_RSV_MASK 0xF
#define BANK_0_RG_VIO18_RSV_SHIFT 0x4

#define BANK_0_RG_VPA_CSL_MASK 0x3
#define BANK_0_RG_VPA_CSL_SHIFT 0x6

#define BANK_0_RG_VPA_CSR_MASK 0x3
#define BANK_0_RG_VPA_CSR_SHIFT 0x4

#define BANK_0_RG_VPA_CC_MASK 0x3
#define BANK_0_RG_VPA_CC_SHIFT 0x2

#define BANK_0_RG_VPA_RZSEL_MASK 0x3
#define BANK_0_RG_VPA_RZSEL_SHIFT 0x0

#define BANK_0_RG_VPA_SLP_MASK 0x3
#define BANK_0_RG_VPA_SLP_SHIFT 0x6

#define BANK_0_RG_VPA_ZX_OS_MASK 0x3
#define BANK_0_RG_VPA_ZX_OS_SHIFT 0x4

#define BANK_0_RG_VPA_SLEW_MASK 0x3
#define BANK_0_RG_VPA_SLEW_SHIFT 0x2

#define BANK_0_RG_VPA_SLEW_NMOS_MASK 0x3
#define BANK_0_RG_VPA_SLEW_NMOS_SHIFT 0x0

#define BANK_0_RG_VPA_AVP_EN_MASK 0x1
#define BANK_0_RG_VPA_AVP_EN_SHIFT 0x1

#define BANK_0_RG_VPA_MODESET_MASK 0x1
#define BANK_0_RG_VPA_MODESET_SHIFT 0x0

#define BANK_0_RG_VPA_EN_MASK 0x1
#define BANK_0_RG_VPA_EN_SHIFT 0x0

#define BANK_0_RG_VPA_BURST_MASK 0x3
#define BANK_0_RG_VPA_BURST_SHIFT 0x4

#define BANK_0_RG_VPA_NDIS_EN_MASK 0x1
#define BANK_0_RG_VPA_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VPA_VOSEL_MASK 0x1F
#define BANK_0_RG_VPA_VOSEL_SHIFT 0x0

#define BANK_0_RG_PASEL_SET0_MASK 0x1F
#define BANK_0_RG_PASEL_SET0_SHIFT 0x0

#define BANK_0_RG_PASEL_SET1_MASK 0x1F
#define BANK_0_RG_PASEL_SET1_SHIFT 0x0

#define BANK_0_RG_PASEL_SET2_MASK 0x1F
#define BANK_0_RG_PASEL_SET2_SHIFT 0x0

#define BANK_0_RG_PASEL_SET3_MASK 0x1F
#define BANK_0_RG_PASEL_SET3_SHIFT 0x0

#define BANK_0_RG_PASEL_SET4_MASK 0x1F
#define BANK_0_RG_PASEL_SET4_SHIFT 0x0

#define BANK_0_RG_PASEL_SET5_MASK 0x1F
#define BANK_0_RG_PASEL_SET5_SHIFT 0x0

#define BANK_0_RG_PASEL_SET6_MASK 0x1F
#define BANK_0_RG_PASEL_SET6_SHIFT 0x0

#define BANK_0_RG_PASEL_SET7_MASK 0x1F
#define BANK_0_RG_PASEL_SET7_SHIFT 0x0

#define BANK_0_RG_VPA_RSV_MASK 0xF
#define BANK_0_RG_VPA_RSV_SHIFT 0x4

#define BANK_0_RG_VPA_CTRL_MASK 0x1
#define BANK_0_RG_VPA_CTRL_SHIFT 0x0

#define BANK_0_QI_VPA_VOSEL_MASK 0x1F
#define BANK_0_QI_VPA_VOSEL_SHIFT 0x0

#define BANK_0_RG_VM12_1_CAL_MASK 0xF
#define BANK_0_RG_VM12_1_CAL_SHIFT 0x0

#define BANK_0_QI_VM12_1_EN_MASK 0x1
#define BANK_0_QI_VM12_1_EN_SHIFT 0x7

#define BANK_0_VM12_1_EN_MASK 0x1
#define BANK_0_VM12_1_EN_SHIFT 0x0

#define BANK_0_RG_VM12_1_STBTD_MASK 0x3
#define BANK_0_RG_VM12_1_STBTD_SHIFT 0x4

#define BANK_0_RG_VM12_1_OCFB_EN_MASK 0x1
#define BANK_0_RG_VM12_1_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VM12_1_NDIS_EN_MASK 0x1
#define BANK_0_RG_VM12_1_NDIS_EN_SHIFT 0x0

#define BANK_0_QI_VM12_1_MODE_MASK 0x1
#define BANK_0_QI_VM12_1_MODE_SHIFT 0x7

#define BANK_0_VM12_1_LP_SET_MASK 0x1
#define BANK_0_VM12_1_LP_SET_SHIFT 0x1

#define BANK_0_VM12_1_LP_SEL_MASK 0x1
#define BANK_0_VM12_1_LP_SEL_SHIFT 0x0

#define BANK_0_RG_VM12_2_CAL_MASK 0xF
#define BANK_0_RG_VM12_2_CAL_SHIFT 0x0

#define BANK_0_QI_VM12_2_EN_MASK 0x1
#define BANK_0_QI_VM12_2_EN_SHIFT 0x7

#define BANK_0_VM12_2_EN_MASK 0x1
#define BANK_0_VM12_2_EN_SHIFT 0x0

#define BANK_0_RG_VM12_2_STBTD_MASK 0x3
#define BANK_0_RG_VM12_2_STBTD_SHIFT 0x4

#define BANK_0_RG_VM12_2_OCFB_EN_MASK 0x1
#define BANK_0_RG_VM12_2_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VM12_2_NDIS_EN_MASK 0x1
#define BANK_0_RG_VM12_2_NDIS_EN_SHIFT 0x0

#define BANK_0_QI_VM12_2_MODE_MASK 0x1
#define BANK_0_QI_VM12_2_MODE_SHIFT 0x7

#define BANK_0_VM12_2_LP_SET_MASK 0x1
#define BANK_0_VM12_2_LP_SET_SHIFT 0x1

#define BANK_0_VM12_2_LP_SEL_MASK 0x1
#define BANK_0_VM12_2_LP_SEL_SHIFT 0x0

#define BANK_0_RG_VM12_INT_CAL_MASK 0x1F
#define BANK_0_RG_VM12_INT_CAL_SHIFT 0x0

#define BANK_0_VM12_INT_SLEEP_MASK 0x1F
#define BANK_0_VM12_INT_SLEEP_SHIFT 0x0

#define BANK_0_VM12_INT_LOW_BOUND_MASK 0x1F
#define BANK_0_VM12_INT_LOW_BOUND_SHIFT 0x0

#define BANK_0_QI_VM12_INT_EN_MASK 0x1
#define BANK_0_QI_VM12_INT_EN_SHIFT 0x7

#define BANK_0_VM12_INT_EN_MASK 0x1
#define BANK_0_VM12_INT_EN_SHIFT 0x0

#define BANK_0_RG_VM12_INT_STBTD_MASK 0x3
#define BANK_0_RG_VM12_INT_STBTD_SHIFT 0x4

#define BANK_0_RG_VM12_INT_OCFB_EN_MASK 0x1
#define BANK_0_RG_VM12_INT_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VM12_INT_NDIS_EN_MASK 0x1
#define BANK_0_RG_VM12_INT_NDIS_EN_SHIFT 0x0

#define BANK_0_VM12_INT_CTRL_SEL_MASK 0x1
#define BANK_0_VM12_INT_CTRL_SEL_SHIFT 0x4

#define BANK_0_RG_VM12_INT_TRIM_MASK 0x7
#define BANK_0_RG_VM12_INT_TRIM_SHIFT 0x1

#define BANK_0_RG_VM12_INT_CAL_SFCHG_EN_MASK 0x1
#define BANK_0_RG_VM12_INT_CAL_SFCHG_EN_SHIFT 0x0

#define BANK_0_QI_VM12_INT_MODE_MASK 0x1
#define BANK_0_QI_VM12_INT_MODE_SHIFT 0x7

#define BANK_0_VM12_INT_LP_SET_MASK 0x1
#define BANK_0_VM12_INT_LP_SET_SHIFT 0x1

#define BANK_0_VM12_INT_LP_SEL_MASK 0x1
#define BANK_0_VM12_INT_LP_SEL_SHIFT 0x0

#define BANK_0_RG_VIO28_CAL_MASK 0xF
#define BANK_0_RG_VIO28_CAL_SHIFT 0x0

#define BANK_0_QI_VIO28_EN_MASK 0x1
#define BANK_0_QI_VIO28_EN_SHIFT 0x7

#define BANK_0_VIO28_EN_MASK 0x1
#define BANK_0_VIO28_EN_SHIFT 0x0

#define BANK_0_RG_VIO28_STBTD_MASK 0x3
#define BANK_0_RG_VIO28_STBTD_SHIFT 0x4

#define BANK_0_RG_VIO28_OCFB_EN_MASK 0x1
#define BANK_0_RG_VIO28_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VIO28_NDIS_EN_MASK 0x1
#define BANK_0_RG_VIO28_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VSIM_CAL_MASK 0xF
#define BANK_0_RG_VSIM_CAL_SHIFT 0x0

#define BANK_0_RG_VSIM_VOSEL_MASK 0x1
#define BANK_0_RG_VSIM_VOSEL_SHIFT 0x4

#define BANK_0_RG_VSIM_EN_MASK 0x1
#define BANK_0_RG_VSIM_EN_SHIFT 0x0

#define BANK_0_RG_VSIM_STBTD_MASK 0x3
#define BANK_0_RG_VSIM_STBTD_SHIFT 0x4

#define BANK_0_RG_VSIM_OCFB_EN_MASK 0x1
#define BANK_0_RG_VSIM_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VSIM_NDIS_EN_MASK 0x1
#define BANK_0_RG_VSIM_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VSIM2_CAL_MASK 0xF
#define BANK_0_RG_VSIM2_CAL_SHIFT 0x0

#define BANK_0_RG_VSIM2_VOSEL_MASK 0x7
#define BANK_0_RG_VSIM2_VOSEL_SHIFT 0x4

#define BANK_0_RG_VSIM2_EN_MASK 0x1
#define BANK_0_RG_VSIM2_EN_SHIFT 0x0

#define BANK_0_RG_VSIM2_STBTD_MASK 0x3
#define BANK_0_RG_VSIM2_STBTD_SHIFT 0x4

#define BANK_0_RG_VSIM2_OCFB_EN_MASK 0x1
#define BANK_0_RG_VSIM2_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VSIM2_NDIS_EN_MASK 0x1
#define BANK_0_RG_VSIM2_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VUSB_CAL_MASK 0xF
#define BANK_0_RG_VUSB_CAL_SHIFT 0x0

#define BANK_0_QI_VUSB_EN_MASK 0x1
#define BANK_0_QI_VUSB_EN_SHIFT 0x7

#define BANK_0_RG_VUSB_EN_MASK 0x1
#define BANK_0_RG_VUSB_EN_SHIFT 0x0

#define BANK_0_RG_VUSB_STBTD_MASK 0x3
#define BANK_0_RG_VUSB_STBTD_SHIFT 0x4

#define BANK_0_RG_VUSB_OCFB_EN_MASK 0x1
#define BANK_0_RG_VUSB_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VUSB_NDIS_EN_MASK 0x1
#define BANK_0_RG_VUSB_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VCAMD_CAL_MASK 0xF
#define BANK_0_RG_VCAMD_CAL_SHIFT 0x0

#define BANK_0_RG_VCAMD_VOSEL_MASK 0x7
#define BANK_0_RG_VCAMD_VOSEL_SHIFT 0x4

#define BANK_0_RG_VCAMD_EN_MASK 0x1
#define BANK_0_RG_VCAMD_EN_SHIFT 0x0

#define BANK_0_RG_VCAMD_STBTD_MASK 0x3
#define BANK_0_RG_VCAMD_STBTD_SHIFT 0x4

#define BANK_0_RG_VCAMD_OCFB_EN_MASK 0x1
#define BANK_0_RG_VCAMD_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VCAMD_NDIS_EN_MASK 0x1
#define BANK_0_RG_VCAMD_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VCAM_IO_CAL_MASK 0xF
#define BANK_0_RG_VCAM_IO_CAL_SHIFT 0x0

#define BANK_0_RG_VCAM_IO_VOSEL_MASK 0x7
#define BANK_0_RG_VCAM_IO_VOSEL_SHIFT 0x4

#define BANK_0_RG_VCAM_IO_EN_MASK 0x1
#define BANK_0_RG_VCAM_IO_EN_SHIFT 0x0

#define BANK_0_RG_VCAM_IO_STBTD_MASK 0x3
#define BANK_0_RG_VCAM_IO_STBTD_SHIFT 0x4

#define BANK_0_RG_VCAM_IO_OCFB_EN_MASK 0x1
#define BANK_0_RG_VCAM_IO_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VCAM_IO_NDIS_EN_MASK 0x1
#define BANK_0_RG_VCAM_IO_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VCAM_AF_CAL_MASK 0xF
#define BANK_0_RG_VCAM_AF_CAL_SHIFT 0x0

#define BANK_0_RG_VCAM_AF_VOSEL_MASK 0x7
#define BANK_0_RG_VCAM_AF_VOSEL_SHIFT 0x4

#define BANK_0_RG_VCAM_AF_EN_MASK 0x1
#define BANK_0_RG_VCAM_AF_EN_SHIFT 0x0

#define BANK_0_RG_VCAM_AF_STBTD_MASK 0x3
#define BANK_0_RG_VCAM_AF_STBTD_SHIFT 0x4

#define BANK_0_RG_VCAM_AF_OCFB_EN_MASK 0x1
#define BANK_0_RG_VCAM_AF_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VCAM_AF_NDIS_EN_MASK 0x1
#define BANK_0_RG_VCAM_AF_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VMC_CAL_MASK 0xF
#define BANK_0_RG_VMC_CAL_SHIFT 0x0

#define BANK_0_QI_VMC_EN_MASK 0x1
#define BANK_0_QI_VMC_EN_SHIFT 0x7

#define BANK_0_RG_VMC_VOSEL_MASK 0x7
#define BANK_0_RG_VMC_VOSEL_SHIFT 0x4

#define BANK_0_RG_VMC_EN_MASK 0x1
#define BANK_0_RG_VMC_EN_SHIFT 0x0

#define BANK_0_RG_VMC_STBTD_MASK 0x3
#define BANK_0_RG_VMC_STBTD_SHIFT 0x4

#define BANK_0_RG_VMC_OCFB_EN_MASK 0x1
#define BANK_0_RG_VMC_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VMC_NDIS_EN_MASK 0x1
#define BANK_0_RG_VMC_NDIS_EN_SHIFT 0x0

#define BANK_0_QI_VMC_MODE_MASK 0x1
#define BANK_0_QI_VMC_MODE_SHIFT 0x7

#define BANK_0_VMC_LP_MODE_SET_MASK 0x1
#define BANK_0_VMC_LP_MODE_SET_SHIFT 0x1

#define BANK_0_VMC_LP_SEL_MASK 0x1
#define BANK_0_VMC_LP_SEL_SHIFT 0x0

#define BANK_0_RG_VMCH_CAL_MASK 0xF
#define BANK_0_RG_VMCH_CAL_SHIFT 0x0

#define BANK_0_QI_VMCH_EN_MASK 0x1
#define BANK_0_QI_VMCH_EN_SHIFT 0x7

#define BANK_0_RG_VMCH_VOSEL_MASK 0x7
#define BANK_0_RG_VMCH_VOSEL_SHIFT 0x4

#define BANK_0_RG_VMCH_EN_MASK 0x1
#define BANK_0_RG_VMCH_EN_SHIFT 0x0

#define BANK_0_RG_VMCH_STBTD_MASK 0x3
#define BANK_0_RG_VMCH_STBTD_SHIFT 0x4

#define BANK_0_RG_VMCH_OCFB_EN_MASK 0x1
#define BANK_0_RG_VMCH_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VMCH_NDIS_EN_MASK 0x1
#define BANK_0_RG_VMCH_NDIS_EN_SHIFT 0x0

#define BANK_0_QI_VMCH_MODE_MASK 0x1
#define BANK_0_QI_VMCH_MODE_SHIFT 0x7

#define BANK_0_VMCH_LP_MODE_SET_MASK 0x1
#define BANK_0_VMCH_LP_MODE_SET_SHIFT 0x1

#define BANK_0_VMCH_LP_SEL_MASK 0x1
#define BANK_0_VMCH_LP_SEL_SHIFT 0x0

#define BANK_0_RG_VGP_CAL_MASK 0xF
#define BANK_0_RG_VGP_CAL_SHIFT 0x0

#define BANK_0_RG_VGP_VOSEL_MASK 0x7
#define BANK_0_RG_VGP_VOSEL_SHIFT 0x4

#define BANK_0_RG_VGP_EN_MASK 0x1
#define BANK_0_RG_VGP_EN_SHIFT 0x0

#define BANK_0_RG_VGP_STBTD_MASK 0x3
#define BANK_0_RG_VGP_STBTD_SHIFT 0x4

#define BANK_0_RG_VGP_OCFB_EN_MASK 0x1
#define BANK_0_RG_VGP_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VGP_NDIS_EN_MASK 0x1
#define BANK_0_RG_VGP_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VGP2_CAL_MASK 0xF
#define BANK_0_RG_VGP2_CAL_SHIFT 0x0

#define BANK_0_RG_VGP2_VOSEL_MASK 0x7
#define BANK_0_RG_VGP2_VOSEL_SHIFT 0x4

#define BANK_0_RG_VGP2_EN_MASK 0x1
#define BANK_0_RG_VGP2_EN_SHIFT 0x0

#define BANK_0_RG_VGP2_STBTD_MASK 0x3
#define BANK_0_RG_VGP2_STBTD_SHIFT 0x4

#define BANK_0_RG_VGP2_OCFB_EN_MASK 0x1
#define BANK_0_RG_VGP2_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VGP2_NDIS_EN_MASK 0x1
#define BANK_0_RG_VGP2_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VIBR_CAL_MASK 0xF
#define BANK_0_RG_VIBR_CAL_SHIFT 0x0

#define BANK_0_RG_VIBR_VOSEL_MASK 0x7
#define BANK_0_RG_VIBR_VOSEL_SHIFT 0x4

#define BANK_0_RG_VIBR_EN_MASK 0x1
#define BANK_0_RG_VIBR_EN_SHIFT 0x0

#define BANK_0_RG_VIBR_STBTD_MASK 0x3
#define BANK_0_RG_VIBR_STBTD_SHIFT 0x4

#define BANK_0_RG_VIBR_THR_SHDN_EN_MASK 0x1
#define BANK_0_RG_VIBR_THR_SHDN_EN_SHIFT 0x3

#define BANK_0_RG_VIBR_STB_SEL_MASK 0x1
#define BANK_0_RG_VIBR_STB_SEL_SHIFT 0x2

#define BANK_0_RG_VIBR_OCFB_EN_MASK 0x1
#define BANK_0_RG_VIBR_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VIBR_NDIS_EN_MASK 0x1
#define BANK_0_RG_VIBR_NDIS_EN_SHIFT 0x0

#define BANK_0_RO_QI_VM12_1_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VM12_1_OC_STATUS_SHIFT 0x0

#define BANK_0_RO_QI_VM12_2_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VM12_2_OC_STATUS_SHIFT 0x1

#define BANK_0_RO_QI_VM12_INT_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VM12_INT_OC_STATUS_SHIFT 0x2

#define BANK_0_RO_QI_VIO28_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VIO28_OC_STATUS_SHIFT 0x3

#define BANK_0_RO_QI_VSIM_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VSIM_OC_STATUS_SHIFT 0x4

#define BANK_0_RO_QI_VSIM2_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VSIM2_OC_STATUS_SHIFT 0x5

#define BANK_0_RO_QI_VUSB_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VUSB_OC_STATUS_SHIFT 0x6

#define BANK_0_RO_QI_VCAMD_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VCAMD_OC_STATUS_SHIFT 0x0

#define BANK_0_RO_QI_VCAM_IO_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VCAM_IO_OC_STATUS_SHIFT 0x1

#define BANK_0_RO_QI_VCAM_AF_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VCAM_AF_OC_STATUS_SHIFT 0x2

#define BANK_0_RO_QI_VMC_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VMC_OC_STATUS_SHIFT 0x3

#define BANK_0_RO_QI_VMCH_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VMCH_OC_STATUS_SHIFT 0x4

#define BANK_0_RO_QI_VGP_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VGP_OC_STATUS_SHIFT 0x5

#define BANK_0_RO_QI_VGP2_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VGP2_OC_STATUS_SHIFT 0x6

#define BANK_0_RO_QI_VIBR_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VIBR_OC_STATUS_SHIFT 0x7

#define BANK_0_QI_VM12_INT_CAL_MASK 0x1F
#define BANK_0_QI_VM12_INT_CAL_SHIFT 0x0

#define BANK_0_RG_DIGLDO_RSV_MASK 0xF
#define BANK_0_RG_DIGLDO_RSV_SHIFT 0x0

#define BANK_0_RG_VRF_CAL_MASK 0xF
#define BANK_0_RG_VRF_CAL_SHIFT 0x0

#define BANK_0_QI_VRF_EN_MASK 0x1
#define BANK_0_QI_VRF_EN_SHIFT 0x7

#define BANK_0_VRF_ON_CTRL_MASK 0x1
#define BANK_0_VRF_ON_CTRL_SHIFT 0x1

#define BANK_0_RG_VRF_EN_MASK 0x1
#define BANK_0_RG_VRF_EN_SHIFT 0x0

#define BANK_0_RG_VRF_STBTD_MASK 0x3
#define BANK_0_RG_VRF_STBTD_SHIFT 0x4

#define BANK_0_RG_VRF_OCFB_EN_MASK 0x1
#define BANK_0_RG_VRF_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VRF_NDIS_EN_MASK 0x1
#define BANK_0_RG_VRF_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VTCXO_CAL_MASK 0xF
#define BANK_0_RG_VTCXO_CAL_SHIFT 0x0

#define BANK_0_QI_VTCXO_EN_MASK 0x1
#define BANK_0_QI_VTCXO_EN_SHIFT 0x7

#define BANK_0_VTCXO_ON_CTRL_MASK 0x1
#define BANK_0_VTCXO_ON_CTRL_SHIFT 0x1

#define BANK_0_RG_VTCXO_EN_MASK 0x1
#define BANK_0_RG_VTCXO_EN_SHIFT 0x0

#define BANK_0_RG_VTCXO_STBTD_MASK 0x3
#define BANK_0_RG_VTCXO_STBTD_SHIFT 0x4

#define BANK_0_RG_VTCXO_OCFB_EN_MASK 0x1
#define BANK_0_RG_VTCXO_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VTCXO_NDIS_EN_MASK 0x1
#define BANK_0_RG_VTCXO_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VA1_CAL_MASK 0xF
#define BANK_0_RG_VA1_CAL_SHIFT 0x0

#define BANK_0_QI_VA1_EN_MASK 0x1
#define BANK_0_QI_VA1_EN_SHIFT 0x7

#define BANK_0_RG_VA1_VOSEL_MASK 0x3
#define BANK_0_RG_VA1_VOSEL_SHIFT 0x4

#define BANK_0_RG_VA1_EN_MASK 0x1
#define BANK_0_RG_VA1_EN_SHIFT 0x0

#define BANK_0_RG_VA1_STBTD_MASK 0x3
#define BANK_0_RG_VA1_STBTD_SHIFT 0x4

#define BANK_0_RG_VA1_OCFB_EN_MASK 0x1
#define BANK_0_RG_VA1_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VA1_NDIS_EN_MASK 0x1
#define BANK_0_RG_VA1_NDIS_EN_SHIFT 0x0

#define BANK_0_QI_VA1_MODE_MASK 0x1
#define BANK_0_QI_VA1_MODE_SHIFT 0x7

#define BANK_0_VA1_LP_SET_MASK 0x1
#define BANK_0_VA1_LP_SET_SHIFT 0x1

#define BANK_0_VA1_LP_SEL_MASK 0x1
#define BANK_0_VA1_LP_SEL_SHIFT 0x0

#define BANK_0_RG_VA2_CAL_MASK 0xF
#define BANK_0_RG_VA2_CAL_SHIFT 0x0

#define BANK_0_QI_VA2_EN_MASK 0x1
#define BANK_0_QI_VA2_EN_SHIFT 0x7

#define BANK_0_RG_VA2_VOSEL_MASK 0x1
#define BANK_0_RG_VA2_VOSEL_SHIFT 0x4

#define BANK_0_RG_VA2_EN_MASK 0x1
#define BANK_0_RG_VA2_EN_SHIFT 0x0

#define BANK_0_RG_VA2_STBTD_MASK 0x3
#define BANK_0_RG_VA2_STBTD_SHIFT 0x4

#define BANK_0_RG_VA2_OCFB_EN_MASK 0x1
#define BANK_0_RG_VA2_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VA2_NDIS_EN_MASK 0x1
#define BANK_0_RG_VA2_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VCAMA_CAL_MASK 0xF
#define BANK_0_RG_VCAMA_CAL_SHIFT 0x0

#define BANK_0_RG_VCAMA_VOSEL_MASK 0x3
#define BANK_0_RG_VCAMA_VOSEL_SHIFT 0x4

#define BANK_0_RG_VCAMA_EN_MASK 0x1
#define BANK_0_RG_VCAMA_EN_SHIFT 0x0

#define BANK_0_RG_VCAMA_STBTD_MASK 0x3
#define BANK_0_RG_VCAMA_STBTD_SHIFT 0x4

#define BANK_0_RG_VCAMA_OCFB_EN_MASK 0x1
#define BANK_0_RG_VCAMA_OCFB_EN_SHIFT 0x1

#define BANK_0_RG_VCAMA_NDIS_EN_MASK 0x1
#define BANK_0_RG_VCAMA_NDIS_EN_SHIFT 0x0

#define BANK_0_RG_VCAMA_FBSEL_MASK 0x3
#define BANK_0_RG_VCAMA_FBSEL_SHIFT 0x0

#define BANK_0_RG_VRTC_VOSEL_MASK 0x3
#define BANK_0_RG_VRTC_VOSEL_SHIFT 0x0

#define BANK_0_QI_VRTC_EN_MASK 0x1
#define BANK_0_QI_VRTC_EN_SHIFT 0x7

#define BANK_0_VRTC_EN_MASK 0x1
#define BANK_0_VRTC_EN_SHIFT 0x0

#define BANK_0_RO_QI_VRF_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VRF_OC_STATUS_SHIFT 0x0

#define BANK_0_RO_QI_VTCXO_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VTCXO_OC_STATUS_SHIFT 0x1

#define BANK_0_RO_QI_VA1_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VA1_OC_STATUS_SHIFT 0x2

#define BANK_0_RO_QI_VA2_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VA2_OC_STATUS_SHIFT 0x3

#define BANK_0_RO_QI_VCAMA_OC_STATUS_MASK 0x1
#define BANK_0_RO_QI_VCAMA_OC_STATUS_SHIFT 0x4

#define BANK_0_RG_ANALDO_RSV_MASK 0xF
#define BANK_0_RG_ANALDO_RSV_SHIFT 0x0

#define BANK_0_rg_auto_k_MASK 0x1
#define BANK_0_rg_auto_k_SHIFT 0x6

#define BANK_0_rg_k_src_sel_MASK 0x1
#define BANK_0_rg_k_src_sel_SHIFT 0x5

#define BANK_0_rg_k_start_manual_MASK 0x1
#define BANK_0_rg_k_start_manual_SHIFT 0x4

#define BANK_0_rg_k_once_MASK 0x1
#define BANK_0_rg_k_once_SHIFT 0x3

#define BANK_0_rg_k_once_en_MASK 0x1
#define BANK_0_rg_k_once_en_SHIFT 0x2

#define BANK_0_rg_k_map_sel_MASK 0x1
#define BANK_0_rg_k_map_sel_SHIFT 0x1

#define BANK_0_rg_k_rst_done_MASK 0x1
#define BANK_0_rg_k_rst_done_SHIFT 0x0

#define BANK_0_rg_k_control_smps_MASK 0xF
#define BANK_0_rg_k_control_smps_SHIFT 0x0

#define BANK_0_k_control_MASK 0x1F
#define BANK_0_k_control_SHIFT 0x3

#define BANK_0_k_done_MASK 0x1
#define BANK_0_k_done_SHIFT 0x1

#define BANK_0_k_result_MASK 0x1
#define BANK_0_k_result_SHIFT 0x0

#define BANK_0_RG_ADC_OUT_C0_7_0_MASK 0xFF
#define BANK_0_RG_ADC_OUT_C0_7_0_SHIFT 0x0

#define BANK_0_RG_ADC_OUT_C0_9_8_MASK 0x3
#define BANK_0_RG_ADC_OUT_C0_9_8_SHIFT 0x0

#define BANK_0_RG_ADC_RDY_C0_MASK 0x1
#define BANK_0_RG_ADC_RDY_C0_SHIFT 0x7

#define BANK_0_RG_ADC_OUT_C1_7_0_MASK 0xFF
#define BANK_0_RG_ADC_OUT_C1_7_0_SHIFT 0x0

#define BANK_0_RG_ADC_OUT_C1_9_8_MASK 0x3
#define BANK_0_RG_ADC_OUT_C1_9_8_SHIFT 0x0

#define BANK_0_RG_ADC_RDY_C1_MASK 0x1
#define BANK_0_RG_ADC_RDY_C1_SHIFT 0x7

#define BANK_0_RG_ADC_OUT_C2_7_0_MASK 0xFF
#define BANK_0_RG_ADC_OUT_C2_7_0_SHIFT 0x0

#define BANK_0_RG_ADC_OUT_C2_9_8_MASK 0x3
#define BANK_0_RG_ADC_OUT_C2_9_8_SHIFT 0x0

#define BANK_0_RG_ADC_RDY_C2_MASK 0x1
#define BANK_0_RG_ADC_RDY_C2_SHIFT 0x7

#define BANK_0_RG_ADC_OUT_C3_7_0_MASK 0xFF
#define BANK_0_RG_ADC_OUT_C3_7_0_SHIFT 0x0

#define BANK_0_RG_ADC_OUT_C3_9_8_MASK 0x3
#define BANK_0_RG_ADC_OUT_C3_9_8_SHIFT 0x0

#define BANK_0_RG_ADC_RDY_C3_MASK 0x1
#define BANK_0_RG_ADC_RDY_C3_SHIFT 0x7

#define BANK_0_RG_ADC_OUT_WAKEUP_7_0_MASK 0xFF
#define BANK_0_RG_ADC_OUT_WAKEUP_7_0_SHIFT 0x0

#define BANK_0_RG_ADC_OUT_WAKEUP_9_8_MASK 0x3
#define BANK_0_RG_ADC_OUT_WAKEUP_9_8_SHIFT 0x0

#define BANK_0_RG_ADC_RDY_WAKEUP_MASK 0x1
#define BANK_0_RG_ADC_RDY_WAKEUP_SHIFT 0x7

#define BANK_0_RG_ADC_OUT_LBAT_7_0_MASK 0xFF
#define BANK_0_RG_ADC_OUT_LBAT_7_0_SHIFT 0x0

#define BANK_0_RG_ADC_OUT_LBAT_9_8_MASK 0x3
#define BANK_0_RG_ADC_OUT_LBAT_9_8_SHIFT 0x0

#define BANK_0_RG_ADC_RDY_LBAT_MASK 0x1
#define BANK_0_RG_ADC_RDY_LBAT_SHIFT 0x7

#define BANK_0_RG_ADC_OUT_TRIM_7_0_MASK 0xFF
#define BANK_0_RG_ADC_OUT_TRIM_7_0_SHIFT 0x0

#define BANK_0_RG_ADC_OUT_TRIM_9_8_MASK 0x3
#define BANK_0_RG_ADC_OUT_TRIM_9_8_SHIFT 0x0

#define BANK_0_RG_ADC_RDY_TRIM_MASK 0x1
#define BANK_0_RG_ADC_RDY_TRIM_SHIFT 0x7

#define BANK_0_RG_AUXADC_START_MASK 0x1
#define BANK_0_RG_AUXADC_START_SHIFT 0x0

#define BANK_0_RG_AUXADC_BIST_ENB_MASK 0x1
#define BANK_0_RG_AUXADC_BIST_ENB_SHIFT 0x1

#define BANK_0_RG_ADC_TRIM_COMP_MASK 0x1
#define BANK_0_RG_ADC_TRIM_COMP_SHIFT 0x2

#define BANK_0_RG_AUXADC_CHSEL_MASK 0xF
#define BANK_0_RG_AUXADC_CHSEL_SHIFT 0x4

#define BANK_0_RG_SPL_NUM_MASK 0xF
#define BANK_0_RG_SPL_NUM_SHIFT 0x4

#define BANK_0_RG_AVG_NUM_MASK 0x3
#define BANK_0_RG_AVG_NUM_SHIFT 0x2

#define BANK_0_RG_BUF_PWD_B_MASK 0x1
#define BANK_0_RG_BUF_PWD_B_SHIFT 0x1

#define BANK_0_RG_ADC_PWD_B_MASK 0x1
#define BANK_0_RG_ADC_PWD_B_SHIFT 0x0

#define BANK_0_RG_LBAT_DEBT_MAX_MASK 0xFF
#define BANK_0_RG_LBAT_DEBT_MAX_SHIFT 0x0

#define BANK_0_RG_LBAT_DEBT_MIN_MASK 0xFF
#define BANK_0_RG_LBAT_DEBT_MIN_SHIFT 0x0

#define BANK_0_RG_LBAT_DET_PRD_7_0_MASK 0xFF
#define BANK_0_RG_LBAT_DET_PRD_7_0_SHIFT 0x0

#define BANK_0_RG_LBAT_DET_PRD_15_8_MASK 0xFF
#define BANK_0_RG_LBAT_DET_PRD_15_8_SHIFT 0x0

#define BANK_0_RG_LBAT_DET_PRD_19_16_MASK 0xF
#define BANK_0_RG_LBAT_DET_PRD_19_16_SHIFT 0x0

#define BANK_0_RG_LBAT_VOLT_MAX_7_0_MASK 0xFF
#define BANK_0_RG_LBAT_VOLT_MAX_7_0_SHIFT 0x0

#define BANK_0_RG_LBAT_VOLT_MAX_9_8_MASK 0x3
#define BANK_0_RG_LBAT_VOLT_MAX_9_8_SHIFT 0x0

#define BANK_0_RG_LBAT_IRQ_EN_MAX_MASK 0x1
#define BANK_0_RG_LBAT_IRQ_EN_MAX_SHIFT 0x4

#define BANK_0_RG_LBAT_EN_MAX_MASK 0x1
#define BANK_0_RG_LBAT_EN_MAX_SHIFT 0x7

#define BANK_0_RG_LBAT_VOLT_MIN_7_0_MASK 0xFF
#define BANK_0_RG_LBAT_VOLT_MIN_7_0_SHIFT 0x0

#define BANK_0_RG_LBAT_VOLT_MIN_9_8_MASK 0x3
#define BANK_0_RG_LBAT_VOLT_MIN_9_8_SHIFT 0x0

#define BANK_0_RG_LBAT_IRQ_EN_MIN_MASK 0x1
#define BANK_0_RG_LBAT_IRQ_EN_MIN_SHIFT 0x4

#define BANK_0_RG_LBAT_EN_MIN_MASK 0x1
#define BANK_0_RG_LBAT_EN_MIN_SHIFT 0x7

#define BANK_0_RG_LBAT_MAX_IRQ_B_MASK 0x1
#define BANK_0_RG_LBAT_MAX_IRQ_B_SHIFT 0x7

#define BANK_0_RG_LBAT_MIN_IRQ_B_MASK 0x1
#define BANK_0_RG_LBAT_MIN_IRQ_B_SHIFT 0x6

#define BANK_0_RG_DA_DAC_7_0_MASK 0xFF
#define BANK_0_RG_DA_DAC_7_0_SHIFT 0x0

#define BANK_0_RG_DA_DAC_9_8_MASK 0x3
#define BANK_0_RG_DA_DAC_9_8_SHIFT 0x0

#define BANK_0_RG_NI_COMP_MASK 0x1
#define BANK_0_RG_NI_COMP_SHIFT 0x7

#define BANK_0_RG_AUXADC_RSV_MASK 0x7
#define BANK_0_RG_AUXADC_RSV_SHIFT 0x5

#define BANK_0_RG_DA_DAC_SEL_MASK 0x1
#define BANK_0_RG_DA_DAC_SEL_SHIFT 0x4

#define BANK_0_RG_AUX_OUT_SEL_MASK 0x1
#define BANK_0_RG_AUX_OUT_SEL_SHIFT 0x3

#define BANK_0_RG_ARB_PRIO_2_MASK 0x1
#define BANK_0_RG_ARB_PRIO_2_SHIFT 0x2

#define BANK_0_RG_ARB_PRIO_1_MASK 0x1
#define BANK_0_RG_ARB_PRIO_1_SHIFT 0x1

#define BANK_0_RG_ARB_PRIO_0_MASK 0x1
#define BANK_0_RG_ARB_PRIO_0_SHIFT 0x0

#define BANK_0_RG_AUXADC_CALI_MASK 0x3
#define BANK_0_RG_AUXADC_CALI_SHIFT 0x4

#define BANK_0_RG_BUF_CALI_MASK 0x3
#define BANK_0_RG_BUF_CALI_SHIFT 0x0

//==============================================================================
// PMIC6329 BANK 1
//==============================================================================
#define BANK_1_RG_MON_GRP_SEL_MASK 0xF
#define BANK_1_RG_MON_GRP_SEL_SHIFT 0x0

#define BANK_1_RG_MON_FLAG_SEL_MASK 0xFF
#define BANK_1_RG_MON_FLAG_SEL_SHIFT 0x0

#define BANK_1_RG_IO_PASEL0_SEL_MASK 0xF
#define BANK_1_RG_IO_PASEL0_SEL_SHIFT 0x4

#define BANK_1_RG_IO_PASEL1_SEL_MASK 0xF
#define BANK_1_RG_IO_PASEL1_SEL_SHIFT 0x0

#define BANK_1_RG_IO_PASEL2_SEL_MASK 0xF
#define BANK_1_RG_IO_PASEL2_SEL_SHIFT 0x4

#define BANK_1_RG_IO_INT_SEL_MASK 0xF
#define BANK_1_RG_IO_INT_SEL_SHIFT 0x0

#define BANK_1_RG_DIO_SMT_MASK 0x1
#define BANK_1_RG_DIO_SMT_SHIFT 0x5

#define BANK_1_RG_DIO_E2_MASK 0x1
#define BANK_1_RG_DIO_E2_SHIFT 0x4

#define BANK_1_RG_DIO_E4_MASK 0x1
#define BANK_1_RG_DIO_E4_SHIFT 0x3

#define BANK_1_RG_DIO_SR_MASK 0x1
#define BANK_1_RG_DIO_SR_SHIFT 0x2

#define BANK_1_RG_SCK_PU_MASK 0x1
#define BANK_1_RG_SCK_PU_SHIFT 0x1

#define BANK_1_RG_SCK_PD_MASK 0x1
#define BANK_1_RG_SCK_PD_SHIFT 0x0

#define BANK_1_RG_SDA_E2_MASK 0x1
#define BANK_1_RG_SDA_E2_SHIFT 0x7

#define BANK_1_RG_SDA_E4_MASK 0x1
#define BANK_1_RG_SDA_E4_SHIFT 0x6

#define BANK_1_RG_SDA_PU_MASK 0x1
#define BANK_1_RG_SDA_PU_SHIFT 0x5

#define BANK_1_RG_SDA_PD_MASK 0x1
#define BANK_1_RG_SDA_PD_SHIFT 0x4

#define BANK_1_RG_INT_E2_MASK 0x1
#define BANK_1_RG_INT_E2_SHIFT 0x3

#define BANK_1_RG_INT_E4_MASK 0x1
#define BANK_1_RG_INT_E4_SHIFT 0x2

#define BANK_1_RG_INT_PU_MASK 0x1
#define BANK_1_RG_INT_PU_SHIFT 0x1

#define BANK_1_RG_INT_PD_MASK 0x1
#define BANK_1_RG_INT_PD_SHIFT 0x0

#define BANK_1_RG_DVS_PU_MASK 0x1
#define BANK_1_RG_DVS_PU_SHIFT 0x5

#define BANK_1_RG_DVS_PD_MASK 0x1
#define BANK_1_RG_DVS_PD_SHIFT 0x4

#define BANK_1_RG_PASEL_PU_MASK 0x1
#define BANK_1_RG_PASEL_PU_SHIFT 0x3

#define BANK_1_RG_PASEL_PD_MASK 0x1
#define BANK_1_RG_PASEL_PD_SHIFT 0x2

#define BANK_1_RG_SYSRSTB_PU_MASK 0x1
#define BANK_1_RG_SYSRSTB_PU_SHIFT 0x1

#define BANK_1_RG_SYSRSTB_PD_MASK 0x1
#define BANK_1_RG_SYSRSTB_PD_SHIFT 0x0

#define BANK_1_RG_SRCLKEN_PU_MASK 0x1
#define BANK_1_RG_SRCLKEN_PU_SHIFT 0x3

#define BANK_1_RG_SRCLKEN_PD_MASK 0x1
#define BANK_1_RG_SRCLKEN_PD_SHIFT 0x2

#define BANK_1_RG_BL_PWM_PU_MASK 0x1
#define BANK_1_RG_BL_PWM_PU_SHIFT 0x1

#define BANK_1_RG_BL_PWM_PD_MASK 0x1
#define BANK_1_RG_BL_PWM_PD_SHIFT 0x0

#define BANK_1_RG_TEST_RSV_MASK 0x1
#define BANK_1_RG_TEST_RSV_SHIFT 0x3

#define BANK_1_RG_TEST_STRUP_MASK 0x1
#define BANK_1_RG_TEST_STRUP_SHIFT 0x2

#define BANK_1_RG_OTP_W_MODE_MASK 0x1
#define BANK_1_RG_OTP_W_MODE_SHIFT 0x1

#define BANK_1_RG_NANDTREE_MODE_MASK 0x1
#define BANK_1_RG_NANDTREE_MODE_SHIFT 0x0

#define BANK_1_RG_TEST_AUXADC_MASK 0x1
#define BANK_1_RG_TEST_AUXADC_SHIFT 0x7

#define BANK_1_RG_TEST_FGPLL_MASK 0x1
#define BANK_1_RG_TEST_FGPLL_SHIFT 0x6

#define BANK_1_RG_TEST_FG1_MASK 0x1
#define BANK_1_RG_TEST_FG1_SHIFT 0x5

#define BANK_1_RG_TEST_FG2_MASK 0x1
#define BANK_1_RG_TEST_FG2_SHIFT 0x4

#define BANK_1_RG_TEST_IO_FG_SEL_MASK 0x1
#define BANK_1_RG_TEST_IO_FG_SEL_SHIFT 0x3

#define BANK_1_RG_TEST_CLASSD_MASK 0x1
#define BANK_1_RG_TEST_CLASSD_SHIFT 0x2

#define BANK_1_RG_TEST_DRIVER_MASK 0x1
#define BANK_1_RG_TEST_DRIVER_SHIFT 0x1

#define BANK_1_RG_TEST_BOOST_MASK 0x1
#define BANK_1_RG_TEST_BOOST_SHIFT 0x0

#define BANK_1_RO_MON_MASK 0xFF
#define BANK_1_RO_MON_SHIFT 0x0

#define BANK_1_RO_DVS1_IN_MASK 0x1
#define BANK_1_RO_DVS1_IN_SHIFT 0x7

#define BANK_1_RO_DVS2_IN_MASK 0x1
#define BANK_1_RO_DVS2_IN_SHIFT 0x6

#define BANK_1_RO_PASEL0_IN_MASK 0x1
#define BANK_1_RO_PASEL0_IN_SHIFT 0x5

#define BANK_1_RO_PASEL1_IN_MASK 0x1
#define BANK_1_RO_PASEL1_IN_SHIFT 0x4

#define BANK_1_RO_PASEL2_IN_MASK 0x1
#define BANK_1_RO_PASEL2_IN_SHIFT 0x3

#define BANK_1_RO_SYSRSTB_IN_MASK 0x1
#define BANK_1_RO_SYSRSTB_IN_SHIFT 0x2

#define BANK_1_RO_SRCLKEN_IN_MASK 0x1
#define BANK_1_RO_SRCLKEN_IN_SHIFT 0x1

#define BANK_1_RO_BLPWM_IN_MASK 0x1
#define BANK_1_RO_BLPWM_IN_SHIFT 0x0

#define BANK_1_RG_PWRKEY_RST_EN_MASK 0x1
#define BANK_1_RG_PWRKEY_RST_EN_SHIFT 0x4

#define BANK_1_RG_HOMEKEY_RST_EN_MASK 0x1
#define BANK_1_RG_HOMEKEY_RST_EN_SHIFT 0x3

#define BANK_1_RG_PWRKEY_RST_TD_MASK 0x3
#define BANK_1_RG_PWRKEY_RST_TD_SHIFT 0x1

#define BANK_1_RG_PWRRST_TMR_DIS_MASK 0x1
#define BANK_1_RG_PWRRST_TMR_DIS_SHIFT 0x0

#define BANK_1_RG_RST_PART_SEL_MASK 0x1
#define BANK_1_RG_RST_PART_SEL_SHIFT 0x7

#define BANK_1_RG_OTP_MAN_RST_MASK 0x1
#define BANK_1_RG_OTP_MAN_RST_SHIFT 0x5

#define BANK_1_RG_PCHR_MAN_RST_EN_MASK 0x1
#define BANK_1_RG_PCHR_MAN_RST_EN_SHIFT 0x4

#define BANK_1_RG_PCHR_MAN_RST_MASK 0x1
#define BANK_1_RG_PCHR_MAN_RST_SHIFT 0x3

#define BANK_1_RG_STRUP_MAN_RST_EN_MASK 0x1
#define BANK_1_RG_STRUP_MAN_RST_EN_SHIFT 0x2

#define BANK_1_RG_SIF_TST_CK_DIS_MASK 0x1
#define BANK_1_RG_SIF_TST_CK_DIS_SHIFT 0x1

#define BANK_1_RG_SYSRSTB_EN_MASK 0x1
#define BANK_1_RG_SYSRSTB_EN_SHIFT 0x0

#define BANK_1_RG_75K_EXT_SEL_MASK 0x1
#define BANK_1_RG_75K_EXT_SEL_SHIFT 0x7

#define BANK_1_RG_FG_TST_CK_SEL_MASK 0x1
#define BANK_1_RG_FG_TST_CK_SEL_SHIFT 0x6

#define BANK_1_RG_CHR1M_TST_CK_SEL_MASK 0x1
#define BANK_1_RG_CHR1M_TST_CK_SEL_SHIFT 0x5

#define BANK_1_RG_CLK_TST_MASK 0x1
#define BANK_1_RG_CLK_TST_SHIFT 0x4

#define BANK_1_RG_AUXADC_CK_RST_MASK 0x1
#define BANK_1_RG_AUXADC_CK_RST_SHIFT 0x3

#define BANK_1_RG_AUXADC_CK_SEL_MASK 0x7
#define BANK_1_RG_AUXADC_CK_SEL_SHIFT 0x0

#define BANK_1_RG_FGADC_CK_PDN_MASK 0x1
#define BANK_1_RG_FGADC_CK_PDN_SHIFT 0x4

#define BANK_1_RG_OTPC_CK_PDN_MASK 0x1
#define BANK_1_RG_OTPC_CK_PDN_SHIFT 0x3

#define BANK_1_RG_BST_DRV_CK_PDN_MASK 0x1
#define BANK_1_RG_BST_DRV_CK_PDN_SHIFT 0x2

#define BANK_1_RG_SPK_CK_PDN_MASK 0x1
#define BANK_1_RG_SPK_CK_PDN_SHIFT 0x1

#define BANK_1_RG_PWMOC_CK_PDN_MASK 0x1
#define BANK_1_RG_PWMOC_CK_PDN_SHIFT 0x0

#define BANK_1_OC_GEAR_BVALID_DET_MASK 0x3
#define BANK_1_OC_GEAR_BVALID_DET_SHIFT 0x6

#define BANK_1_OC_GEAR_VBATON_UNDET_MASK 0x3
#define BANK_1_OC_GEAR_VBATON_UNDET_SHIFT 0x4

#define BANK_1_OC_GEAR_LDO_MASK 0x3
#define BANK_1_OC_GEAR_LDO_SHIFT 0x0

#define BANK_1_RG_INT_RSV_MASK 0x1F
#define BANK_1_RG_INT_RSV_SHIFT 0x3

#define BANK_1_POLARITY_BVALID_DET_MASK 0x1
#define BANK_1_POLARITY_BVALID_DET_SHIFT 0x2

#define BANK_1_POLARITY_VBATON_UNDET_MASK 0x1
#define BANK_1_POLARITY_VBATON_UNDET_SHIFT 0x1

#define BANK_1_POLARITY_MASK 0x1
#define BANK_1_POLARITY_SHIFT 0x0

#define BANK_1_VPA_OC_WND_MASK 0x3
#define BANK_1_VPA_OC_WND_SHIFT 0x6

#define BANK_1_VPA_OC_THD_MASK 0x3
#define BANK_1_VPA_OC_THD_SHIFT 0x4

#define BANK_1_VRF18_OC_WND_MASK 0x3
#define BANK_1_VRF18_OC_WND_SHIFT 0x2

#define BANK_1_VRF18_OC_THD_MASK 0x3
#define BANK_1_VRF18_OC_THD_SHIFT 0x0

#define BANK_1_FLASH_RSV0_MASK 0x7
#define BANK_1_FLASH_RSV0_SHIFT 0x5

#define BANK_1_FLASH_DIM_DUTY_MASK 0x1F
#define BANK_1_FLASH_DIM_DUTY_SHIFT 0x0

#define BANK_1_FLASH_THER_SHDN_EN_MASK 0x1
#define BANK_1_FLASH_THER_SHDN_EN_SHIFT 0x1

#define BANK_1_FLASH_EN_MASK 0x1
#define BANK_1_FLASH_EN_SHIFT 0x0

#define BANK_1_FLASH_DIM_DIV_MASK 0xFF
#define BANK_1_FLASH_DIM_DIV_SHIFT 0x0

#define BANK_1_FLASH_RSV1_MASK 0x1F
#define BANK_1_FLASH_RSV1_SHIFT 0x3

#define BANK_1_FLASH_SEL_MASK 0x7
#define BANK_1_FLASH_SEL_SHIFT 0x0

#define BANK_1_FLASH_SFSTREN_MASK 0x1
#define BANK_1_FLASH_SFSTREN_SHIFT 0x7

#define BANK_1_FLASH_SFSTR_MASK 0x3
#define BANK_1_FLASH_SFSTR_SHIFT 0x4

#define BANK_1_FLASH_MODE_MASK 0x1
#define BANK_1_FLASH_MODE_SHIFT 0x0

#define BANK_1_KPLED_RSV0_MASK 0x7
#define BANK_1_KPLED_RSV0_SHIFT 0x5

#define BANK_1_KPLED_DIM_DUTY_MASK 0x1F
#define BANK_1_KPLED_DIM_DUTY_SHIFT 0x0

#define BANK_1_KPLED_THER_SHDN_EN_MASK 0x1
#define BANK_1_KPLED_THER_SHDN_EN_SHIFT 0x1

#define BANK_1_KPLED_EN_MASK 0x1
#define BANK_1_KPLED_EN_SHIFT 0x0

#define BANK_1_KPLED_DIM_DIV_MASK 0xFF
#define BANK_1_KPLED_DIM_DIV_SHIFT 0x0

#define BANK_1_KPLED_RSV1_MASK 0x1F
#define BANK_1_KPLED_RSV1_SHIFT 0x3

#define BANK_1_KPLED_SEL_MASK 0x7
#define BANK_1_KPLED_SEL_SHIFT 0x0

#define BANK_1_KPLED_SFSTREN_MASK 0x1
#define BANK_1_KPLED_SFSTREN_SHIFT 0x7

#define BANK_1_KPLED_SFSTR_MASK 0x3
#define BANK_1_KPLED_SFSTR_SHIFT 0x4

#define BANK_1_KPLED_MODE_MASK 0x1
#define BANK_1_KPLED_MODE_SHIFT 0x0

#define BANK_1_ISINK_RSV0_MASK 0x7
#define BANK_1_ISINK_RSV0_SHIFT 0x5

#define BANK_1_ISINK_DIM0_DUTY_MASK 0xF
#define BANK_1_ISINK_DIM0_DUTY_SHIFT 0x0

#define BANK_1_ISINK_DIM0_FSEL_MASK 0x1F
#define BANK_1_ISINK_DIM0_FSEL_SHIFT 0x0

#define BANK_1_ISINK_RSV1_MASK 0x7
#define BANK_1_ISINK_RSV1_SHIFT 0x5

#define BANK_1_ISINK_DIM1_DUTY_MASK 0x1F
#define BANK_1_ISINK_DIM1_DUTY_SHIFT 0x0

#define BANK_1_ISINK_DIM1_FSEL_MASK 0x1F
#define BANK_1_ISINK_DIM1_FSEL_SHIFT 0x0

#define BANK_1_ISINK_RSV2_MASK 0x7
#define BANK_1_ISINK_RSV2_SHIFT 0x5

#define BANK_1_ISINK_DIM2_DUTY_MASK 0x1F
#define BANK_1_ISINK_DIM2_DUTY_SHIFT 0x0

#define BANK_1_ISINK_DIM2_FSEL_MASK 0x1F
#define BANK_1_ISINK_DIM2_FSEL_SHIFT 0x0

#define BANK_1_ISINK_RSV3_MASK 0x3
#define BANK_1_ISINK_RSV3_SHIFT 0x6

#define BANK_1_ISINKS_CH5_EN_MASK 0x1
#define BANK_1_ISINKS_CH5_EN_SHIFT 0x5

#define BANK_1_ISINKS_CH4_EN_MASK 0x1
#define BANK_1_ISINKS_CH4_EN_SHIFT 0x4

#define BANK_1_ISINKS_CH3_EN_MASK 0x1
#define BANK_1_ISINKS_CH3_EN_SHIFT 0x3

#define BANK_1_ISINKS_CH2_EN_MASK 0x1
#define BANK_1_ISINKS_CH2_EN_SHIFT 0x2

#define BANK_1_ISINKS_CH1_EN_MASK 0x1
#define BANK_1_ISINKS_CH1_EN_SHIFT 0x1

#define BANK_1_ISINKS_CH0_EN_MASK 0x1
#define BANK_1_ISINKS_CH0_EN_SHIFT 0x0

#define BANK_1_ISINK_RSV4_MASK 0x3
#define BANK_1_ISINK_RSV4_SHIFT 0x6

#define BANK_1_ISINKS_CH5_CABC_EN_MASK 0x1
#define BANK_1_ISINKS_CH5_CABC_EN_SHIFT 0x5

#define BANK_1_ISINKS_CH4_CABC_EN_MASK 0x1
#define BANK_1_ISINKS_CH4_CABC_EN_SHIFT 0x4

#define BANK_1_ISINKS_CH3_CABC_EN_MASK 0x1
#define BANK_1_ISINKS_CH3_CABC_EN_SHIFT 0x3

#define BANK_1_ISINKS_CH2_CABC_EN_MASK 0x1
#define BANK_1_ISINKS_CH2_CABC_EN_SHIFT 0x2

#define BANK_1_ISINKS_CH1_CABC_EN_MASK 0x1
#define BANK_1_ISINKS_CH1_CABC_EN_SHIFT 0x1

#define BANK_1_ISINKS_CH0_CABC_EN_MASK 0x1
#define BANK_1_ISINKS_CH0_CABC_EN_SHIFT 0x0

#define BANK_1_ISINKS_CH0_STEP_MASK 0x7
#define BANK_1_ISINKS_CH0_STEP_SHIFT 0x4

#define BANK_1_ISINKS_CH0_MODE_MASK 0x3
#define BANK_1_ISINKS_CH0_MODE_SHIFT 0x0

#define BANK_1_ISINKS_CH1_STEP_MASK 0x7
#define BANK_1_ISINKS_CH1_STEP_SHIFT 0x4

#define BANK_1_ISINKS_CH1_MODE_MASK 0x3
#define BANK_1_ISINKS_CH1_MODE_SHIFT 0x0

#define BANK_1_ISINKS_CH2_STEP_MASK 0x7
#define BANK_1_ISINKS_CH2_STEP_SHIFT 0x4

#define BANK_1_ISINKS_CH2_MODE_MASK 0x3
#define BANK_1_ISINKS_CH2_MODE_SHIFT 0x0

#define BANK_1_ISINKS_CH3_STEP_MASK 0x7
#define BANK_1_ISINKS_CH3_STEP_SHIFT 0x4

#define BANK_1_ISINKS_CH3_MODE_MASK 0x3
#define BANK_1_ISINKS_CH3_MODE_SHIFT 0x0

#define BANK_1_ISINKS_CH4_STEP_MASK 0x7
#define BANK_1_ISINKS_CH4_STEP_SHIFT 0x4

#define BANK_1_ISINKS_CH4_MODE_MASK 0x3
#define BANK_1_ISINKS_CH4_MODE_SHIFT 0x0

#define BANK_1_ISINKS_CH5_STEP_MASK 0x7
#define BANK_1_ISINKS_CH5_STEP_SHIFT 0x4

#define BANK_1_ISINKS_CH5_MODE_MASK 0x3
#define BANK_1_ISINKS_CH5_MODE_SHIFT 0x0

#define BANK_1_IBIAS_TRIM_EN_MASK 0x1
#define BANK_1_IBIAS_TRIM_EN_SHIFT 0x6

#define BANK_1_ISINKS_VREF_CAL_MASK 0x1F
#define BANK_1_ISINKS_VREF_CAL_SHIFT 0x0

#define BANK_1_ISINK_RSV5_MASK 0xF
#define BANK_1_ISINK_RSV5_SHIFT 0x4

#define BANK_1_ISINKS_RSV_MASK 0xF
#define BANK_1_ISINKS_RSV_SHIFT 0x0

#define BANK_1_DRV_RSV0_MASK 0xFF
#define BANK_1_DRV_RSV0_SHIFT 0x0

#define BANK_1_BOOST_ISINK_HW_SEL_MASK 0x1
#define BANK_1_BOOST_ISINK_HW_SEL_SHIFT 0x7

#define BANK_1_BOOST_MODE_MASK 0x3
#define BANK_1_BOOST_MODE_SHIFT 0x4

#define BANK_1_BOOST_CABC_EN_MASK 0x1
#define BANK_1_BOOST_CABC_EN_SHIFT 0x2

#define BANK_1_BOOST_EN_MASK 0x1
#define BANK_1_BOOST_EN_SHIFT 0x0

#define BANK_1_BOOST_SR_NMOS_MASK 0x3
#define BANK_1_BOOST_SR_NMOS_SHIFT 0x6

#define BANK_1_BOOST_VRSEL_MASK 0xF
#define BANK_1_BOOST_VRSEL_SHIFT 0x0

#define BANK_1_BOOST_RC_MASK 0x3
#define BANK_1_BOOST_RC_SHIFT 0x4

#define BANK_1_BOOST_CS_MASK 0x3
#define BANK_1_BOOST_CS_SHIFT 0x2

#define BANK_1_BOOST_CC_MASK 0x3
#define BANK_1_BOOST_CC_SHIFT 0x0

#define BANK_1_BOOST_SLP_MASK 0x3
#define BANK_1_BOOST_SLP_SHIFT 0x2

#define BANK_1_BOOST_CL_MASK 0x3
#define BANK_1_BOOST_CL_SHIFT 0x0

#define BANK_1_BOOST_SS_MASK 0x7
#define BANK_1_BOOST_SS_SHIFT 0x4

#define BANK_1_BOOST_RSV_MASK 0xF
#define BANK_1_BOOST_RSV_SHIFT 0x0

#define BANK_1_SPK_THER_SHDN_L_EN_MASK 0x1
#define BANK_1_SPK_THER_SHDN_L_EN_SHIFT 0x6

#define BANK_1_SPK_TRIM_EN_L_MASK 0x1
#define BANK_1_SPK_TRIM_EN_L_SHIFT 0x4

#define BANK_1_SPKMODE_L_MASK 0x1
#define BANK_1_SPKMODE_L_SHIFT 0x2

#define BANK_1_SPK_EN_L_MASK 0x1
#define BANK_1_SPK_EN_L_SHIFT 0x0

#define BANK_1_SPK_OC_EN_L_MASK 0x1
#define BANK_1_SPK_OC_EN_L_SHIFT 0x7

#define BANK_1_SPKAB_OC_EN_L_MASK 0x1
#define BANK_1_SPKAB_OC_EN_L_SHIFT 0x6

#define BANK_1_SPK_OC_SHDN_DL_MASK 0x1
#define BANK_1_SPK_OC_SHDN_DL_SHIFT 0x4

#define BANK_1_SPK_VOL_L_MASK 0x7
#define BANK_1_SPK_VOL_L_SHIFT 0x0

#define BANK_1_SPK_RSV0_MASK 0x3F
#define BANK_1_SPK_RSV0_SHIFT 0x2

#define BANK_1_SPK_AB_OC_L_DEG_MASK 0x1
#define BANK_1_SPK_AB_OC_L_DEG_SHIFT 0x1

#define BANK_1_SPK_D_OC_L_DEG_MASK 0x1
#define BANK_1_SPK_D_OC_L_DEG_SHIFT 0x0

#define BANK_1_SPK_OFFSET_L_OV_MASK 0x1
#define BANK_1_SPK_OFFSET_L_OV_SHIFT 0x7

#define BANK_1_SPK_OFFSET_L_MODE_MASK 0x1
#define BANK_1_SPK_OFFSET_L_MODE_SHIFT 0x6

#define BANK_1_SPK_LEAD_L_SW_MASK 0x1
#define BANK_1_SPK_LEAD_L_SW_SHIFT 0x5

#define BANK_1_SPK_OFFSET_L_SW_MASK 0x1F
#define BANK_1_SPK_OFFSET_L_SW_SHIFT 0x0

#define BANK_1_SPK_TRIM_DONE_L_MASK 0x1
#define BANK_1_SPK_TRIM_DONE_L_SHIFT 0x7

#define BANK_1_SPK_LEAD_L_FLAG_MASK 0x1
#define BANK_1_SPK_LEAD_L_FLAG_SHIFT 0x6

#define BANK_1_SPK_LEAD_L_FLAG_DEG_MASK 0x1
#define BANK_1_SPK_LEAD_L_FLAG_DEG_SHIFT 0x5

#define BANK_1_SPK_OFFSET_L_MASK 0x1F
#define BANK_1_SPK_OFFSET_L_SHIFT 0x0

#define BANK_1_SPK_RSV1_MASK 0x1
#define BANK_1_SPK_RSV1_SHIFT 0x7

#define BANK_1_SPKRCV_EN_L_MASK 0x1
#define BANK_1_SPKRCV_EN_L_SHIFT 0x6

#define BANK_1_SPKAB_OBIAS_L_MASK 0x3
#define BANK_1_SPKAB_OBIAS_L_SHIFT 0x4

#define BANK_1_SPK_SLEW_L_MASK 0x3
#define BANK_1_SPK_SLEW_L_SHIFT 0x2

#define BANK_1_SPK_FORCE_EN_L_MASK 0x1
#define BANK_1_SPK_FORCE_EN_L_SHIFT 0x1

#define BANK_1_SPK_INTG_RST_L_MASK 0x1
#define BANK_1_SPK_INTG_RST_L_SHIFT 0x0

#define BANK_1_SPK_THER_SHDN_R_EN_MASK 0x1
#define BANK_1_SPK_THER_SHDN_R_EN_SHIFT 0x6

#define BANK_1_SPK_TRIM_EN_R_MASK 0x1
#define BANK_1_SPK_TRIM_EN_R_SHIFT 0x4

#define BANK_1_SPKMODE_R_MASK 0x1
#define BANK_1_SPKMODE_R_SHIFT 0x2

#define BANK_1_SPK_EN_R_MASK 0x1
#define BANK_1_SPK_EN_R_SHIFT 0x0

#define BANK_1_SPK_OC_EN_R_MASK 0x1
#define BANK_1_SPK_OC_EN_R_SHIFT 0x7

#define BANK_1_SPKAB_OC_EN_R_MASK 0x1
#define BANK_1_SPKAB_OC_EN_R_SHIFT 0x6

#define BANK_1_SPK_OC_SHDN_DR_MASK 0x1
#define BANK_1_SPK_OC_SHDN_DR_SHIFT 0x4

#define BANK_1_SPK_VOL_R_MASK 0x7
#define BANK_1_SPK_VOL_R_SHIFT 0x0

#define BANK_1_SPK_RSV2_MASK 0x3F
#define BANK_1_SPK_RSV2_SHIFT 0x2

#define BANK_1_SPK_AB_OC_R_DEG_MASK 0x1
#define BANK_1_SPK_AB_OC_R_DEG_SHIFT 0x1

#define BANK_1_SPK_D_OC_R_DEG_MASK 0x1
#define BANK_1_SPK_D_OC_R_DEG_SHIFT 0x0

#define BANK_1_SPK_OFFSET_R_OV_MASK 0x1
#define BANK_1_SPK_OFFSET_R_OV_SHIFT 0x7

#define BANK_1_SPK_OFFSET_R_MODE_MASK 0x1
#define BANK_1_SPK_OFFSET_R_MODE_SHIFT 0x6

#define BANK_1_SPK_LEAD_R_SW_MASK 0x1
#define BANK_1_SPK_LEAD_R_SW_SHIFT 0x5

#define BANK_1_SPK_OFFSET_R_SW_MASK 0x1F
#define BANK_1_SPK_OFFSET_R_SW_SHIFT 0x0

#define BANK_1_SPK_TRIM_DONE_R_MASK 0x1
#define BANK_1_SPK_TRIM_DONE_R_SHIFT 0x7

#define BANK_1_SPK_LEAD_R_FLAG_MASK 0x1
#define BANK_1_SPK_LEAD_R_FLAG_SHIFT 0x6

#define BANK_1_SPK_LEAD_R_FLAG_DEG_MASK 0x1
#define BANK_1_SPK_LEAD_R_FLAG_DEG_SHIFT 0x5

#define BANK_1_SPK_OFFSET_R_MASK 0x1F
#define BANK_1_SPK_OFFSET_R_SHIFT 0x0

#define BANK_1_SPK_RSV3_MASK 0x1
#define BANK_1_SPK_RSV3_SHIFT 0x7

#define BANK_1_SPKRCV_EN_R_MASK 0x1
#define BANK_1_SPKRCV_EN_R_SHIFT 0x6

#define BANK_1_SPKAB_OBIAS_R_MASK 0x3
#define BANK_1_SPKAB_OBIAS_R_SHIFT 0x4

#define BANK_1_SPK_SLEW_R_MASK 0x3
#define BANK_1_SPK_SLEW_R_SHIFT 0x2

#define BANK_1_SPK_FORCE_EN_R_MASK 0x1
#define BANK_1_SPK_FORCE_EN_R_SHIFT 0x1

#define BANK_1_SPK_INTG_RST_R_MASK 0x1
#define BANK_1_SPK_INTG_RST_R_SHIFT 0x0

#define BANK_1_SPK_OC_AUTOFF_MASK 0x1
#define BANK_1_SPK_OC_AUTOFF_SHIFT 0x3

#define BANK_1_SPK_OC_DGLH_MASK 0x3
#define BANK_1_SPK_OC_DGLH_SHIFT 0x1

#define BANK_1_SPK_OCTH_D_MASK 0x1
#define BANK_1_SPK_OCTH_D_SHIFT 0x0

#define BANK_1_SPK_OC_WND_MASK 0x3
#define BANK_1_SPK_OC_WND_SHIFT 0x4

#define BANK_1_SPK_OC_THD_MASK 0x3
#define BANK_1_SPK_OC_THD_SHIFT 0x0

#define BANK_1_SPK_TRIM_DIV_MASK 0x3
#define BANK_1_SPK_TRIM_DIV_SHIFT 0x4

#define BANK_1_SPK_TRIM_DEG_MASK 0x3
#define BANK_1_SPK_TRIM_DEG_SHIFT 0x0

#define BANK_1_SPKAB_OBIAS_MASK 0x3
#define BANK_1_SPKAB_OBIAS_SHIFT 0x6

#define BANK_1_SPKAB_FB_ATT_MASK 0x3
#define BANK_1_SPKAB_FB_ATT_SHIFT 0x2

#define BANK_1_SPKAB_OVDRV_MASK 0x1
#define BANK_1_SPKAB_OVDRV_SHIFT 0x0

#define BANK_1_SPK_FBRC_EN_MASK 0x1
#define BANK_1_SPK_FBRC_EN_SHIFT 0x6

#define BANK_1_SPK_IBIAS_SEL_MASK 0x3
#define BANK_1_SPK_IBIAS_SEL_SHIFT 0x4

#define BANK_1_SPK_VCM_IBSEL_MASK 0x1
#define BANK_1_SPK_VCM_IBSEL_SHIFT 0x3

#define BANK_1_SPK_VCM_SEL_MASK 0x1
#define BANK_1_SPK_VCM_SEL_SHIFT 0x2

#define BANK_1_SPK_EN_VIEW_CLK_MASK 0x1
#define BANK_1_SPK_EN_VIEW_CLK_SHIFT 0x1

#define BANK_1_SPK_EN_VIEW_VCM_MASK 0x1
#define BANK_1_SPK_EN_VIEW_VCM_SHIFT 0x0

#define BANK_1_SPK_CCODE_MASK 0xF
#define BANK_1_SPK_CCODE_SHIFT 0x4

#define BANK_1_SPK_BTL_SET_MASK 0x3
#define BANK_1_SPK_BTL_SET_SHIFT 0x0

#define BANK_1_SPK_RSV_MASK 0xF
#define BANK_1_SPK_RSV_SHIFT 0x0

#define BANK_1_SPK_TD1_MASK 0x3
#define BANK_1_SPK_TD1_SHIFT 0x4

#define BANK_1_SPK_TD2_MASK 0x3
#define BANK_1_SPK_TD2_SHIFT 0x0

#define BANK_1_SPK_DEPOP_EN_L_SW_MASK 0x1
#define BANK_1_SPK_DEPOP_EN_L_SW_SHIFT 0x7

#define BANK_1_SPK_DEPOP_EN_R_SW_MASK 0x1
#define BANK_1_SPK_DEPOP_EN_R_SW_SHIFT 0x6

#define BANK_1_SPKMODE_L_SW_MASK 0x1
#define BANK_1_SPKMODE_L_SW_SHIFT 0x5

#define BANK_1_SPKMODE_R_SW_MASK 0x1
#define BANK_1_SPKMODE_R_SW_SHIFT 0x4

#define BANK_1_SPK_RST_L_SW_MASK 0x1
#define BANK_1_SPK_RST_L_SW_SHIFT 0x3

#define BANK_1_SPK_RST_R_SW_MASK 0x1
#define BANK_1_SPK_RST_R_SW_SHIFT 0x2

#define BANK_1_SPK_EN_MODE_MASK 0x1
#define BANK_1_SPK_EN_MODE_SHIFT 0x0

#define BANK_1_SPK_TRIM_EN_L_SW_MASK 0x1
#define BANK_1_SPK_TRIM_EN_L_SW_SHIFT 0x7

#define BANK_1_SPK_TRIM_EN_R_SW_MASK 0x1
#define BANK_1_SPK_TRIM_EN_R_SW_SHIFT 0x6

#define BANK_1_SPK_OUTSTG_EN_L_SW_MASK 0x1
#define BANK_1_SPK_OUTSTG_EN_L_SW_SHIFT 0x5

#define BANK_1_SPK_OUTSTG_EN_R_SW_MASK 0x1
#define BANK_1_SPK_OUTSTG_EN_R_SW_SHIFT 0x4

#define BANK_1_SPK_EN_L_SW_MASK 0x1
#define BANK_1_SPK_EN_L_SW_SHIFT 0x3

#define BANK_1_SPK_EN_R_SW_MASK 0x1
#define BANK_1_SPK_EN_R_SW_SHIFT 0x2

#define BANK_1_SPK_VCM_FAST_SW_MASK 0x1
#define BANK_1_SPK_VCM_FAST_SW_SHIFT 0x0

#define BANK_1_SPK_TRIM_STOP_L_SW_MASK 0x1
#define BANK_1_SPK_TRIM_STOP_L_SW_SHIFT 0x1

#define BANK_1_SPK_TRIM_STOP_R_SW_MASK 0x1
#define BANK_1_SPK_TRIM_STOP_R_SW_SHIFT 0x0

#define BANK_1_RG_ANA_SW_SEL_MASK 0x1
#define BANK_1_RG_ANA_SW_SEL_SHIFT 0x0

#define BANK_1_FGPLL_PDIV1_MASK 0xF
#define BANK_1_FGPLL_PDIV1_SHIFT 0x4

#define BANK_1_FGPLL_PDIV1_EN_MASK 0x1
#define BANK_1_FGPLL_PDIV1_EN_SHIFT 0x3

#define BANK_1_FGPLL_BS_RST_MASK 0x1
#define BANK_1_FGPLL_BS_RST_SHIFT 0x1

#define BANK_1_FGPLL_EN_MASK 0x1
#define BANK_1_FGPLL_EN_SHIFT 0x0

#define BANK_1_FGPLL_DIV1_MASK 0x3F
#define BANK_1_FGPLL_DIV1_SHIFT 0x0

#define BANK_1_FGPLL_BC_MASK 0x3
#define BANK_1_FGPLL_BC_SHIFT 0x4

#define BANK_1_FGPLL_BP_MASK 0x3
#define BANK_1_FGPLL_BP_SHIFT 0x2

#define BANK_1_FGPLL_BR_MASK 0x3
#define BANK_1_FGPLL_BR_SHIFT 0x0

#define BANK_1_FGPLL_CDIV_MASK 0x7
#define BANK_1_FGPLL_CDIV_SHIFT 0x5

#define BANK_1_FGPLL_VCOBAND_MASK 0x7
#define BANK_1_FGPLL_VCOBAND_SHIFT 0x2

#define BANK_1_FGPLL_CKO_SEL_MASK 0x3
#define BANK_1_FGPLL_CKO_SEL_SHIFT 0x0

#define BANK_1_FGPLL_IBSEL_MASK 0x3
#define BANK_1_FGPLL_IBSEL_SHIFT 0x6

#define BANK_1_FGPLL_RLATCH_EN_MASK 0x1
#define BANK_1_FGPLL_RLATCH_EN_SHIFT 0x5

#define BANK_1_FGPLL_CKDRV_EN_MASK 0x1
#define BANK_1_FGPLL_CKDRV_EN_SHIFT 0x4

#define BANK_1_FGPLL_VCT_EN_MASK 0x1
#define BANK_1_FGPLL_VCT_EN_SHIFT 0x3

#define BANK_1_FGPLL_CKT_SEL_MASK 0x3
#define BANK_1_FGPLL_CKT_SEL_SHIFT 0x1

#define BANK_1_FGPLL_CKT_EN_MASK 0x1
#define BANK_1_FGPLL_CKT_EN_SHIFT 0x0

#define BANK_1_FGPLL_RSVA_MASK 0xFF
#define BANK_1_FGPLL_RSVA_SHIFT 0x0

#define BANK_1_FGPLL_RSVB_MASK 0xFF
#define BANK_1_FGPLL_RSVB_SHIFT 0x0

#define BANK_1_FG_CLKSRC_MASK 0x1
#define BANK_1_FG_CLKSRC_SHIFT 0x7

#define BANK_1_FG_AUTOCALRATE_MASK 0x7
#define BANK_1_FG_AUTOCALRATE_SHIFT 0x4

#define BANK_1_FG_CAL_MASK 0x3
#define BANK_1_FG_CAL_SHIFT 0x2

#define BANK_1_FG_VMODE_MASK 0x1
#define BANK_1_FG_VMODE_SHIFT 0x1

#define BANK_1_FG_ON_MASK 0x1
#define BANK_1_FG_ON_SHIFT 0x0

#define BANK_1_FG_SW_RSTCLR_MASK 0x1
#define BANK_1_FG_SW_RSTCLR_SHIFT 0x7

#define BANK_1_FG_CHARGE_RST_MASK 0x1
#define BANK_1_FG_CHARGE_RST_SHIFT 0x6

#define BANK_1_FG_TIME_RST_MASK 0x1
#define BANK_1_FG_TIME_RST_SHIFT 0x5

#define BANK_1_FG_OFFSET_RST_MASK 0x1
#define BANK_1_FG_OFFSET_RST_SHIFT 0x4

#define BANK_1_FG_SW_CLEAR_MASK 0x1
#define BANK_1_FG_SW_CLEAR_SHIFT 0x3

#define BANK_1_FG_LATCHDATA_ST_MASK 0x1
#define BANK_1_FG_LATCHDATA_ST_SHIFT 0x2

#define BANK_1_FG_SW_READ_PRE_MASK 0x1
#define BANK_1_FG_SW_READ_PRE_SHIFT 0x1

#define BANK_1_FG_SW_CR_MASK 0x1
#define BANK_1_FG_SW_CR_SHIFT 0x0

#define BANK_1_FG_CAR_35_32_MASK 0xF
#define BANK_1_FG_CAR_35_32_SHIFT 0x0

#define BANK_1_FG_CAR_31_24_MASK 0xFF
#define BANK_1_FG_CAR_31_24_SHIFT 0x0

#define BANK_1_FG_CAR_23_16_MASK 0xFF
#define BANK_1_FG_CAR_23_16_SHIFT 0x0

#define BANK_1_FG_CAR_15_08_MASK 0xFF
#define BANK_1_FG_CAR_15_08_SHIFT 0x0

#define BANK_1_FG_CAR_07_00_MASK 0xFF
#define BANK_1_FG_CAR_07_00_SHIFT 0x0

#define BANK_1_FG_NTER_29_24_MASK 0x3F
#define BANK_1_FG_NTER_29_24_SHIFT 0x0

#define BANK_1_FG_NTER_23_16_MASK 0xFF
#define BANK_1_FG_NTER_23_16_SHIFT 0x0

#define BANK_1_FG_NTER_15_08_MASK 0xFF
#define BANK_1_FG_NTER_15_08_SHIFT 0x0

#define BANK_1_FG_NTER_07_00_MASK 0xFF
#define BANK_1_FG_NTER_07_00_SHIFT 0x0

#define BANK_1_FG_BLTR_15_08_MASK 0xFF
#define BANK_1_FG_BLTR_15_08_SHIFT 0x0

#define BANK_1_FG_BLTR_07_00_MASK 0xFF
#define BANK_1_FG_BLTR_07_00_SHIFT 0x0

#define BANK_1_FG_BFTR_15_08_MASK 0xFF
#define BANK_1_FG_BFTR_15_08_SHIFT 0x0

#define BANK_1_FG_BFTR_07_00_MASK 0xFF
#define BANK_1_FG_BFTR_07_00_SHIFT 0x0

#define BANK_1_FG_CURRENT_OUT_15_08_MASK 0xFF
#define BANK_1_FG_CURRENT_OUT_15_08_SHIFT 0x0

#define BANK_1_FG_CURRENT_OUT_07_00_MASK 0xFF
#define BANK_1_FG_CURRENT_OUT_07_00_SHIFT 0x0

#define BANK_1_FG_ADJUST_OFFSET_VALUE_15_08_MASK 0xFF
#define BANK_1_FG_ADJUST_OFFSET_VALUE_15_08_SHIFT 0x0

#define BANK_1_FG_ADJUST_OFFSET_VALUE_07_00_MASK 0xFF
#define BANK_1_FG_ADJUST_OFFSET_VALUE_07_00_SHIFT 0x0

#define BANK_1_FG_OFFSET_15_08_MASK 0xFF
#define BANK_1_FG_OFFSET_15_08_SHIFT 0x0

#define BANK_1_FG_OFFSET_07_00_MASK 0xFF
#define BANK_1_FG_OFFSET_07_00_SHIFT 0x0

#define BANK_1_FG_RSV0_MASK 0xF
#define BANK_1_FG_RSV0_SHIFT 0x4

#define BANK_1_FG_ANALOGTEST_MASK 0xF
#define BANK_1_FG_ANALOGTEST_SHIFT 0x0

#define BANK_1_FG_SPARE_MASK 0xFF
#define BANK_1_FG_SPARE_SHIFT 0x0

#define BANK_1_FG_BLTR_BFTR_EN_MASK 0x1
#define BANK_1_FG_BLTR_BFTR_EN_SHIFT 0x7

#define BANK_1_FG_ADC_AUTORST_MASK 0x1
#define BANK_1_FG_ADC_AUTORST_SHIFT 0x6

#define BANK_1_FG_ADJ_OFFSET_EN_MASK 0x1
#define BANK_1_FG_ADJ_OFFSET_EN_SHIFT 0x4

#define BANK_1_FG_OSR_MASK 0x7
#define BANK_1_FG_OSR_SHIFT 0x0

#define BANK_1_VOL_OSR_MASK 0x7
#define BANK_1_VOL_OSR_SHIFT 0x0

#define BANK_1_FG_ADC_RSTDETECT_MASK 0x1
#define BANK_1_FG_ADC_RSTDETECT_SHIFT 0x7

#define BANK_1_FG_H_INT_STS_MASK 0x1
#define BANK_1_FG_H_INT_STS_SHIFT 0x5

#define BANK_1_FG_L_INT_STS_MASK 0x1
#define BANK_1_FG_L_INT_STS_SHIFT 0x4

#define BANK_1_VOL_FIR1BYPASS_MASK 0x1
#define BANK_1_VOL_FIR1BYPASS_SHIFT 0x2

#define BANK_1_FG_FIR2BYPASS_MASK 0x1
#define BANK_1_FG_FIR2BYPASS_SHIFT 0x1

#define BANK_1_FG_FIR1BYPASS_MASK 0x1
#define BANK_1_FG_FIR1BYPASS_SHIFT 0x0

#define BANK_1_VOL_CURRENT_OUT_15_08_MASK 0xFF
#define BANK_1_VOL_CURRENT_OUT_15_08_SHIFT 0x0

#define BANK_1_VOL_CURRENT_OUT_07_00_MASK 0xFF
#define BANK_1_VOL_CURRENT_OUT_07_00_SHIFT 0x0

#define BANK_1_FG_CIC2_15_08_MASK 0xFF
#define BANK_1_FG_CIC2_15_08_SHIFT 0x0

#define BANK_1_FG_CIC2_07_00_MASK 0xFF
#define BANK_1_FG_CIC2_07_00_SHIFT 0x0

#define BANK_1_FG_RSV1_MASK 0x7
#define BANK_1_FG_RSV1_SHIFT 0x5

#define BANK_1_FG_VMODE_SW_MASK 0x1
#define BANK_1_FG_VMODE_SW_SHIFT 0x4

#define BANK_1_FG_FGADC_EN_SW_MASK 0x1
#define BANK_1_FG_FGADC_EN_SW_SHIFT 0x3

#define BANK_1_FG_FGCAL_EN_SW_MASK 0x1
#define BANK_1_FG_FGCAL_EN_SW_SHIFT 0x2

#define BANK_1_FG_RST_SW_MASK 0x1
#define BANK_1_FG_RST_SW_SHIFT 0x1

#define BANK_1_FG_MODE_MASK 0x1
#define BANK_1_FG_MODE_SHIFT 0x0

#define BANK_1_FG_MON_MASK 0xFF
#define BANK_1_FG_MON_SHIFT 0x0

#define BANK_1_FG_RSV2_MASK 0xFF
#define BANK_1_FG_RSV2_SHIFT 0x0

#define BANK_1_RG_OTP_PDO_7_0_MASK 0xFF
#define BANK_1_RG_OTP_PDO_7_0_SHIFT 0x0

#define BANK_1_RG_OTP_PDO_15_8_MASK 0xFF
#define BANK_1_RG_OTP_PDO_15_8_SHIFT 0x0

#define BANK_1_RG_OTP_PDO_23_16_MASK 0xFF
#define BANK_1_RG_OTP_PDO_23_16_SHIFT 0x0

#define BANK_1_RG_OTP_PDO_31_24_MASK 0xFF
#define BANK_1_RG_OTP_PDO_31_24_SHIFT 0x0

#define BANK_1_RG_OTP_PDO_39_32_MASK 0xFF
#define BANK_1_RG_OTP_PDO_39_32_SHIFT 0x0

#define BANK_1_RG_OTP_PDO_47_40_MASK 0xFF
#define BANK_1_RG_OTP_PDO_47_40_SHIFT 0x0

#define BANK_1_RG_OTP_PDO_55_48_MASK 0xFF
#define BANK_1_RG_OTP_PDO_55_48_SHIFT 0x0

#define BANK_1_RG_OTP_PDO_63_56_MASK 0xFF
#define BANK_1_RG_OTP_PDO_63_56_SHIFT 0x0

#define BANK_1_RG_OTP_PDIN_MASK 0xFF
#define BANK_1_RG_OTP_PDIN_SHIFT 0x0

#define BANK_1_RG_OTP_PA_MASK 0x3
#define BANK_1_RG_OTP_PA_SHIFT 0x6

#define BANK_1_RG_OTP_PTM_MASK 0x3
#define BANK_1_RG_OTP_PTM_SHIFT 0x4

#define BANK_1_RG_OTP_PWE_MASK 0x3
#define BANK_1_RG_OTP_PWE_SHIFT 0x2

#define BANK_1_RG_OTP_PPROG_MASK 0x1
#define BANK_1_RG_OTP_PPROG_SHIFT 0x1

#define BANK_1_RG_OTP_READ_MASK 0x1
#define BANK_1_RG_OTP_READ_SHIFT 0x0

#define BANK_1_RG_OTP_READ_PRD_MASK 0x3
#define BANK_1_RG_OTP_READ_PRD_SHIFT 0x6

#define BANK_1_RG_OTP_TEST_SEL_MASK 0x7
#define BANK_1_RG_OTP_TEST_SEL_SHIFT 0x5

#define BANK_1_RG_OTP_SKIP_OUT_MASK 0x1
#define BANK_1_RG_OTP_SKIP_OUT_SHIFT 0x4

#define BANK_1_RG_OTP_OUT_SEL_MASK 0x3
#define BANK_1_RG_OTP_OUT_SEL_SHIFT 0x2

#define BANK_1_RG_OTP_RSV_MASK 0x3
#define BANK_1_RG_OTP_RSV_SHIFT 0x0

#define BANK_1_RG_OTP_VAL_7_0_MASK 0xFF
#define BANK_1_RG_OTP_VAL_7_0_SHIFT 0x0

#define BANK_1_RG_OTP_VAL_15_8_MASK 0xFF
#define BANK_1_RG_OTP_VAL_15_8_SHIFT 0x0

#define BANK_1_RG_OTP_VAL_23_16_MASK 0xFF
#define BANK_1_RG_OTP_VAL_23_16_SHIFT 0x0

#define BANK_1_RG_OTP_VAL_31_24_MASK 0xFF
#define BANK_1_RG_OTP_VAL_31_24_SHIFT 0x0

#define BANK_1_RG_OTP_VAL_39_32_MASK 0xFF
#define BANK_1_RG_OTP_VAL_39_32_SHIFT 0x0

#define BANK_1_RG_OTP_VAL_47_40_MASK 0xFF
#define BANK_1_RG_OTP_VAL_47_40_SHIFT 0x0

#define BANK_1_RG_OTP_VAL_55_48_MASK 0xFF
#define BANK_1_RG_OTP_VAL_55_48_SHIFT 0x0

#define BANK_1_RG_OTP_VAL_63_56_MASK 0xFF
#define BANK_1_RG_OTP_VAL_63_56_SHIFT 0x0

#define BANK_1_RG_OTP_BUSY_MASK 0x1
#define BANK_1_RG_OTP_BUSY_SHIFT 0x7

#define BANK_1_RG_OTP_VLD_MASK 0x1
#define BANK_1_RG_OTP_VLD_SHIFT 0x6

#define BANK_1_RG_OTP_READ_RDY_BYPASS_MASK 0x1
#define BANK_1_RG_OTP_READ_RDY_BYPASS_SHIFT 0x3

#define BANK_1_RG_OTP_W_LOCK_MASK 0x1
#define BANK_1_RG_OTP_W_LOCK_SHIFT 0x7

#define BANK_1_RG_OTP_W_LOCK_KEY_TOG_MASK 0x1
#define BANK_1_RG_OTP_W_LOCK_KEY_TOG_SHIFT 0x4

#define BANK_1_RG_OTP_W_LOCK_KEY_MASK 0xF
#define BANK_1_RG_OTP_W_LOCK_KEY_SHIFT 0x0

#define BANK_1_SCL_DE_MASK 0x1F
#define BANK_1_SCL_DE_SHIFT 0x0

#define BANK_1_SDIN_DE_MASK 0x1F
#define BANK_1_SDIN_DE_SHIFT 0x0

//==============================================================================
// PMIC6329 Status Code
//==============================================================================
#define PMIC_TEST_PASS               0x0000
#define PMIC_TEST_FAIL               0xB001
#define PMIC_EXCEED_I2C_FIFO_LENGTH  0xB002
#define PMIC_CHRDET_EXIST            0xB003
#define PMIC_CHRDET_NOT_EXIST        0xB004

//==============================================================================
// PMIC6329 Exported Function
//==============================================================================
extern U32 pmic6329_IsUsbCableIn (void);
extern U32 pmic6329_i2c_read (U8 chip, U8 *cmdBuffer, int cmdBufferLen, U8 *dataBuffer, int dataBufferLen);
extern U32 pmic6329_i2c_write (U8 chip, U8 *cmdBuffer, int cmdBufferLen, U8 *dataBuffer, int dataBufferLen);
extern U32 pmic6329_init (void);
extern U32 pmic6329_int_ctrl_enable (U8 chip, U8 ctrl_type, U8 sel, kal_bool enable);
extern kal_uint32 upmu_get_reg_value(kal_uint32 reg);

extern kal_uint32 pmic_read_interface (kal_uint8 RegNum, kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT);
extern kal_uint32 pmic_config_interface (kal_uint8 RegNum, kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT);
extern kal_uint32 pmic_bank1_read_interface (kal_uint8 RegNum, kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT);
extern kal_uint32 pmic_bank1_config_interface (kal_uint8 RegNum, kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT);

//==============================================================================
// STATUS APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_status_vcore(void);
extern kal_uint32 upmu_status_vproc(void);
extern kal_uint32 upmu_status_vio18(void);
extern kal_uint32 upmu_status_vrf18(void);
extern kal_uint32 upmu_status_vpa(void);
extern kal_uint32 upmu_status_vrf(void);
extern kal_uint32 upmu_status_vtcxo(void);
extern kal_uint32 upmu_status_va1(void);
//CON 1
extern kal_uint32 upmu_status_vcama(void);
extern kal_uint32 upmu_status_vm12_1(void);
extern kal_uint32 upmu_status_vm12_int(void);
extern kal_uint32 upmu_status_vio28(void);
extern kal_uint32 upmu_status_vsim1(void);
extern kal_uint32 upmu_status_vsim2(void);
extern kal_uint32 upmu_status_vusb(void);
extern kal_uint32 upmu_status_vcamd(void);
//CON 2
extern kal_uint32 upmu_status_vcam_io(void);
extern kal_uint32 upmu_status_vcam_af(void);
extern kal_uint32 upmu_status_vmc(void);
extern kal_uint32 upmu_status_vmch(void);
extern kal_uint32 upmu_status_vgp(void);
extern kal_uint32 upmu_status_vgp2(void);
extern kal_uint32 upmu_status_vm12_2(void);
extern kal_uint32 upmu_status_vibr(void);
//CON 3
extern kal_uint32 upmu_status_va2(void);
extern kal_uint32 upmu_status_vrtc(void);
extern kal_uint32 upmu_status_vtcxo_pg(void);

//==============================================================================
// Power Good Status APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_status_vcore_pg(void);
extern kal_uint32 upmu_status_vproc_pg(void);
extern kal_uint32 upmu_status_vio18_pg(void);
extern kal_uint32 upmu_status_vio28_pg(void);
extern kal_uint32 upmu_status_va1_pg(void);
extern kal_uint32 upmu_status_vm12_1_pg(void);
extern kal_uint32 upmu_status_vm12_2_pg(void);
extern kal_uint32 upmu_status_vm12_int_pg(void);

//==============================================================================
// Charger Status APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_status_chr_cv(void);
extern kal_uint32 upmu_status_chr_ro_baton_undet(void);
extern kal_uint32 upmu_status_chr_pchr_chrdet(void);
extern kal_uint32 upmu_status_chr_vbat_ov(void);
extern kal_uint32 upmu_status_chr_pwrkey_deb(void);
extern kal_uint32 upmu_status_chr_usb_dl(void);
extern kal_uint32 upmu_status_chr_test_mode_por(void);

//==============================================================================
// OC Status APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_status_oc_vcama(void);
extern kal_uint32 upmu_status_oc_vm12_1(void);
extern kal_uint32 upmu_status_oc_vm12_int(void);
extern kal_uint32 upmu_status_oc_vio28(void);
extern kal_uint32 upmu_status_oc_vsim1(void);
extern kal_uint32 upmu_status_oc_vsim2(void);
extern kal_uint32 upmu_status_oc_vusb(void);
extern kal_uint32 upmu_status_oc_vcamad(void);
//CON 1
extern kal_uint32 upmu_status_oc_vcam_io(void);
extern kal_uint32 upmu_status_oc_vcam_af(void);
extern kal_uint32 upmu_status_oc_vmc(void);
extern kal_uint32 upmu_status_oc_vmch(void);
extern kal_uint32 upmu_status_oc_vgp(void);
extern kal_uint32 upmu_status_oc_vgp2(void);
extern kal_uint32 upmu_status_oc_vm12_2(void);
extern kal_uint32 upmu_status_oc_vibr(void);
//CON 2
extern kal_uint32 upmu_status_oc_homekey_deb(void);
extern kal_uint32 upmu_status_oc_boost(void);
extern kal_uint32 upmu_status_oc_va2(void);
extern kal_uint32 upmu_status_oc_ni_spk_det_d_r(void);
extern kal_uint32 upmu_status_oc_ni_spk_det_d_l(void);
extern kal_uint32 upmu_status_oc_ni_spk_det_ab_r(void);
extern kal_uint32 upmu_status_oc_ni_spk_det_ab_l(void);
//CON 3
extern kal_uint32 upmu_status_oc_vcore(void);
extern kal_uint32 upmu_status_oc_vproc(void);
extern kal_uint32 upmu_status_oc_vio18(void);
extern kal_uint32 upmu_status_oc_vrf18(void);
extern kal_uint32 upmu_status_oc_vpa(void);
extern kal_uint32 upmu_status_oc_vrf(void);
extern kal_uint32 upmu_status_oc_vtcxo(void);
extern kal_uint32 upmu_status_oc_va1(void);

//==============================================================================
// STARTUP APIs
//==============================================================================
//CON 0
extern void upmu_strup_thrdet_sel(kal_uint32 val);
extern void upmu_strup_thr_hwpdn_en(kal_uint32 val);
extern void upmu_strup_thr_sel(kal_uint32 val);
extern void upmu_strup_thr_test_mode(kal_uint32 val);
extern void upmu_strup_thr_force_non_thermal(kal_uint32 val);
//CON 1
extern void upmu_strup_vref_bg(kal_uint32 val);
extern void upmu_strup_iref_trim(kal_uint32 val);
//CON 2
extern void upmu_strup_bb_proc_sel(kal_uint32 val);
extern void upmu_strup_rst_drvsel(kal_uint32 val);
extern void upmu_strup_pmu_lev_ungate(kal_uint32 val);
extern void upmu_strup_usbdl_en(kal_uint32 val);
//CON 3
extern void upmu_strup_pmu_rsv(kal_uint32 val);
//CON 4
extern void upmu_strup_bias_gen_en_force(kal_uint32 val);
extern void upmu_strup_pwron_force(kal_uint32 val);
extern void upmu_strup_test_mode(kal_uint32 val);
extern void upmu_strup_ft_ctrl_test_mode(kal_uint32 val);
//CON 5
extern kal_uint32 upmu_strup_pmu_thr_status(void);
extern void upmu_strup_thermal_test_mode(kal_uint32 val);
//CON 6
extern void upmu_strup_vcore_pg_enb(kal_uint32 val);
extern void upmu_strup_vproc_pg_enb(kal_uint32 val);
extern void upmu_strup_vio18_pg_enb(kal_uint32 val);
extern void upmu_strup_vio28_pg_enb(kal_uint32 val);
extern void upmu_strup_va1_pg_enb(kal_uint32 val);
extern void upmu_strup_vm12_1_pg_enb(kal_uint32 val);
extern void upmu_strup_vm12_2_pg_enb(kal_uint32 val);
extern void upmu_strup_vm12_int_pg_enb(kal_uint32 val);
//CON 7
extern void upmu_strup_dig_rsv(kal_uint32 val);
extern void upmu_strup_vtcxo_pg_enb(kal_uint32 val);

//==============================================================================
// Interrupt control APIs
//==============================================================================
//CON 0
extern void upmu_interrupt_ov_en(kal_uint32 val);
extern void upmu_interrupt_chrdet_int_en(kal_uint32 val);
extern void upmu_interrupt_bvalid_det_int_en(kal_uint32 val);
extern void upmu_interrupt_vbaton_undet_int_en(kal_uint32 val);
extern void upmu_interrupt_thr_h_int_en(kal_uint32 val);
extern void upmu_interrupt_thr_l_int_en(kal_uint32 val);
extern void upmu_interrupt_pwrkey_int_en(kal_uint32 val);
extern void upmu_interrupt_watchdog_int_en(kal_uint32 val);
//CON 1
extern void upmu_interrupt_fg_bat_h_int_en(kal_uint32 val);
extern void upmu_interrupt_fg_bat_l_int_en(kal_uint32 val);
extern void upmu_interrupt_high_bat_int_en(kal_uint32 val);
extern void upmu_interrupt_low_bat_int_en(kal_uint32 val);
extern void upmu_interrupt_spkr_d_oc_int_en(kal_uint32 val);
extern void upmu_interrupt_spkl_d_oc_int_en(kal_uint32 val);
extern void upmu_interrupt_spkr_ab_oc_int_en(kal_uint32 val);
extern void upmu_interrupt_spkl_ab_oc_int_en(kal_uint32 val);
//CON 2
extern void upmu_interrupt_homekey_int_en(kal_uint32 val);
extern void upmu_interrupt_vrf18_oc_int_en(kal_uint32 val);
extern void upmu_interrupt_vpa_oc_int_en(kal_uint32 val);
extern void upmu_interrupt_ldo_oc_int_en(kal_uint32 val);
//CON 3
extern kal_uint32 upmu_interrupt_get_ov_status(void);
extern kal_uint32 upmu_interrupt_get_chrdet_int_status(void);
extern kal_uint32 upmu_interrupt_get_bvalid_det_int_status(void);
extern kal_uint32 upmu_interrupt_get_vbaton_undet_int_status(void);
extern kal_uint32 upmu_interrupt_get_thr_h_int_status(void);
extern kal_uint32 upmu_interrupt_get_thr_l_int_status(void);
extern kal_uint32 upmu_interrupt_get_pwrkey_int_status(void);
extern kal_uint32 upmu_interrupt_get_watchdog_int_status(void);
//CON 4
extern kal_uint32 upmu_interrupt_get_fg_bat_h_int_status(void);
extern kal_uint32 upmu_interrupt_get_fg_bat_l_int_status(void);
extern kal_uint32 upmu_interrupt_get_high_bat_int_status(void);
extern kal_uint32 upmu_interrupt_get_low_bat_int_status(void);
extern kal_uint32 upmu_interrupt_get_spkr_d_oc_int_status(void);
extern kal_uint32 upmu_interrupt_get_spkl_d_oc_int_status(void);
extern kal_uint32 upmu_interrupt_get_spkr_ab_oc_int_status(void);
extern kal_uint32 upmu_interrupt_get_spkl_ab_oc_int_status(void);
//CON 5
extern kal_uint32 upmu_interrupt_get_homekey_int_status(void);
extern kal_uint32 upmu_interrupt_get_vrf18_oc_int_status(void);
extern kal_uint32 upmu_interrupt_get_vpa_oc_int_status(void);
extern kal_uint32 upmu_interrupt_get_ldo_oc_int_status(void);

//==============================================================================
// Charger APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_chr_get_vcdt_hv_det(void);
extern kal_uint32 upmu_chr_get_vcdt_lv_det(void);
extern kal_uint32 upmu_chr_get_chrdet(void);
extern void upmu_chr_enable(kal_uint32 val);
extern void upmu_chr_csdac_enable(kal_uint32 val);
extern void upmu_chr_hw_auto_enable(kal_uint32 val);
extern kal_uint32 upmu_chr_get_ldo_det(void);
extern void upmu_chr_vcdt_hv_enable(kal_uint32 val);
//CON1
extern void upmu_chr_vcdt_hv_vth(kal_uint32 val);
extern void upmu_chr_vcdt_lv_vth(kal_uint32 val);
//CON2
extern kal_uint32 upmu_chr_get_vbat_cc_det(void);
extern kal_uint32 upmu_chr_get_vbat_cv_det(void);
extern kal_uint32 upmu_chr_get_cs_det(void);
extern void upmu_chr_cs_en(kal_uint32 val);
extern void upmu_chr_vbat_cc_en(kal_uint32 val);
extern void upmu_chr_vbat_cv_en(kal_uint32 val);
//CON3
extern void upmu_chr_vbat_cc_vth(kal_uint32 val);
extern void upmu_chr_vbat_cv_vth(kal_uint32 val);
//CON4
extern void upmu_chr_vbat_cs_vth(kal_uint32 val);
//CON5
extern void upmu_chr_toltc(kal_uint32 val);
extern void upmu_chr_tohtc(kal_uint32 val);
//CON6
extern kal_uint32 upmu_chr_get_vbat_ov_det(void);
extern void upmu_chr_vbat_ov_deg(kal_uint32 val);
extern void upmu_chr_vbat_ov_vth(kal_uint32 val);
extern void upmu_chr_vbat_ov_en(kal_uint32 val);
//CON7
extern kal_uint32 upmu_chr_get_baton_undet(void);
extern void upmu_chr_baton_ht_trim(kal_uint32 val);
extern void upmu_chr_baton_ht_en(kal_uint32 val);
extern void upmu_chr_baton_en(kal_uint32 val);
//CON8
extern void upmu_chr_csdac_dat_h(kal_uint32 val);
//CON9
extern void upmu_chr_csdac_dat_l(kal_uint32 val);
//CONA
extern kal_uint32 upmu_chr_get_otg_bvalid(void);
extern void upmu_chr_otg_bvalid_en(kal_uint32 val);
extern void upmu_chr_pchr_flag_en(kal_uint32 val);
extern void upmu_chr_pchr_flag_out(kal_uint32 val);
//CONB
extern void upmu_chr_pchr_flag_sel(kal_uint32 val);
//CONC
extern void upmu_chr_pchr_ft_ctrl(kal_uint32 val);
extern void upmu_chr_pchr_rst(kal_uint32 val);
extern void upmu_chr_csdac_test_mode(kal_uint32 val);
extern void upmu_chr_pchr_test_mode(kal_uint32 val);
//COND
extern void upmu_chr_chrwdt_en(kal_uint32 val);
extern void upmu_chr_chrwdt_td(kal_uint32 val);
//CONE
extern void upmu_chr_pchr_rv(kal_uint32 val);
//CONF
extern kal_uint32 upmu_chr_get_chrwdt_out(void);
extern void upmu_chr_chrwdt_flag_wr(kal_uint32 val);
extern void upmu_chr_chrwdt_int_en(kal_uint32 val);
//CON11
extern void upmu_chr_adcin_vchr_en(kal_uint32 val);
extern void upmu_chr_adcin_vsen_en(kal_uint32 val);
extern void upmu_chr_adcin_vbat_en(kal_uint32 val);
extern void upmu_chr_usbdl_set(kal_uint32 val);
extern void upmu_chr_usbdl_rst(kal_uint32 val);
extern void upmu_chr_uvlo_vthl(kal_uint32 val);
//CON12
extern void upmu_chr_bgr_unchop(kal_uint32 val);
extern void upmu_chr_bgr_unchop_ph(kal_uint32 val);
extern void upmu_chr_bgr_rsel(kal_uint32 val);
//CON13
extern kal_uint32 upmu_chr_get_bc11_cmp_out(void);
extern void upmu_chr_bc11_vsrc_en(kal_uint32 val);
extern void upmu_chr_bc11_rst(kal_uint32 val);
extern void upmu_chr_bc11_bb_ctrl(kal_uint32 val);
//CON14
extern void upmu_chr_bc11_bias_en(kal_uint32 val);
extern void upmu_chr_bc11_ipu_en(kal_uint32 val);
extern void upmu_chr_bc11_ipd_en(kal_uint32 val);
extern void upmu_chr_bc11_cmp_en(kal_uint32 val);
extern void upmu_chr_bc11_vref_vth(kal_uint32 val);
//CON15
extern void upmu_chr_csdac_stp_dec(kal_uint32 val);
extern void upmu_chr_csdac_stp_inc(kal_uint32 val);
//CON16
extern void upmu_chr_csdac_stp(kal_uint32 val);
extern void upmu_chr_csdac_dly(kal_uint32 val);
//CON17
extern void upmu_chr_chrind_dimming(kal_uint32 val);
extern void upmu_chr_chrind_on(kal_uint32 val);
extern void upmu_chr_low_ich_db(kal_uint32 val);
//CON18
extern void upmu_chr_ulc_det_en(kal_uint32 val);
extern void upmu_chr_hw_cv_en(kal_uint32 val);
extern void upmu_chr_baton_tdet_en(kal_uint32 val);
extern void upmu_chr_tracking_en(kal_uint32 val);
extern void upmu_chr_csdac_mode(kal_uint32 val);
extern void upmu_chr_vcdt_mode(kal_uint32 val);
extern void upmu_chr_cv_mode(kal_uint32 val);
//CON19
extern void upmu_chr_ichrg_trim(kal_uint32 val);
extern void upmu_chr_bgr_trim_en(kal_uint32 val);
//CON1A
extern void upmu_chr_bgr_trim(kal_uint32 val);
//CON1B
extern void upmu_chr_ovp_trim(kal_uint32 val);
//CON1C
extern void upmu_chr_rsv0(kal_uint32 val);
extern void upmu_chr_bgr_test_rstb(kal_uint32 val);
extern void upmu_chr_bgr_test_en(kal_uint32 val);
extern void upmu_chr_osc_trim(kal_uint32 val);
//CON1D
extern void upmu_chr_rsv1(kal_uint32 val);

//==============================================================================
// BUCK CALIBRATION APIs
//==============================================================================
//CON 0
extern void upmu_buck_calibration_auto_k(kal_uint32 val);
extern void upmu_buck_calibration_k_src_sel(kal_uint32 val);
extern void upmu_buck_calibration_k_start_manual(kal_uint32 val);
extern void upmu_buck_calibration_k_once(kal_uint32 val);
extern void upmu_buck_calibration_k_once_en(kal_uint32 val);
extern void upmu_buck_calibration_k_map_sel(kal_uint32 val);
extern void upmu_buck_calibration_k_rst_done(kal_uint32 val);
//CON 1
extern void upmu_buck_calibration_k_control_smps(kal_uint32 val);
//CON 2
extern kal_uint32 upmu_buck_calibration_get_k_control(void);
extern kal_uint32 upmu_buck_calibration_get_k_done(void);
extern kal_uint32 upmu_buck_calibration_get_k_result(void);

//==============================================================================
// AUXADC APIs
//==============================================================================
//CON 0
extern kal_uint32 upmu_auxadc_get_ch0_data_7_0(void);
//CON 1
extern kal_uint32 upmu_auxadc_get_ch0_ready(void);
extern kal_uint32 upmu_auxadc_get_ch0_data_9_8(void);
//CON 2
extern kal_uint32 upmu_auxadc_get_ch1_data_7_0(void);
//CON 3
extern kal_uint32 upmu_auxadc_get_ch1_ready(void);
extern kal_uint32 upmu_auxadc_get_ch1_data_9_8(void);
//CON 4
extern kal_uint32 upmu_auxadc_get_ch2_data_7_0(void);
//CON 5
extern kal_uint32 upmu_auxadc_get_ch2_ready(void);
extern kal_uint32 upmu_auxadc_get_ch2_data_9_8(void);
//CON 6
extern kal_uint32 upmu_auxadc_get_ch3_data_7_0(void);
//CON 7
extern kal_uint32 upmu_auxadc_get_ch3_ready(void);
extern kal_uint32 upmu_auxadc_get_ch3_data_9_8(void);
//CON 8
extern kal_uint32 upmu_auxadc_get_wakeup_date_7_0(void);
//CON 9
extern kal_uint32 upmu_auxadc_get_wakeup_date_ready(void);
extern kal_uint32 upmu_auxadc_get_wakeup_date_9_8(void);
//CON 10
extern kal_uint32 upmu_auxadc_get_low_battery_data_7_0(void);
//CON 11
extern kal_uint32 upmu_auxadc_get_low_battery_data_ready(void);
extern kal_uint32 upmu_auxadc_get_low_battery_data_9_8(void);
//CON 12
extern kal_uint32 upmu_auxadc_get_trimming_data_7_0(void);
//CON 13
extern kal_uint32 upmu_auxadc_get_trimming_data_ready(void);
extern kal_uint32 upmu_auxadc_get_trimming_data_9_8(void);
//CON 14
extern void upmu_auxadc_ch_sel(kal_uint32 val);
extern void upmu_auxadc_trim_comp(kal_uint32 val);
extern void upmu_auxadc_bist_enb(kal_uint32 val);
extern void upmu_auxadc_start(kal_uint32 val);
//CON 15
extern void upmu_auxadc_spl_num(kal_uint32 val);
extern void upmu_auxadc_avg_num(kal_uint32 val);
extern void upmu_auxadc_buf_pwd_b(kal_uint32 val);
extern void upmu_auxadc_adc_pwd_b(kal_uint32 val);
//CON 16
extern void upmu_auxadc_lowbat_debtime_max(kal_uint32 val);
//CON 17
extern void upmu_auxadc_lowbat_debtime_min(kal_uint32 val);
//CON 18
extern void upmu_auxadc_lowbat_period_7_0(kal_uint32 val);
//CON 19
extern void upmu_auxadc_lowbat_period_15_8(kal_uint32 val);
//CON 20
extern void upmu_auxadc_lowbat_period_19_16(kal_uint32 val);
//CON 21
extern void upmu_auxadc_lowbat_set_max_voltage_7_0(kal_uint32 val);
//CON 22
extern void upmu_auxadc_lowbat_set_max_voltage_9_8(kal_uint32 val);
extern void upmu_auxadc_lowbat_en_max_volt(kal_uint32 val);
extern void upmu_auxadc_lowbat_irq_en_max_volt(kal_uint32 val);
//CON 23
extern void upmu_auxadc_lowbat_set_min_voltage_7_0(kal_uint32 val);
//CON 24
extern void upmu_auxadc_lowbat_set_min_voltage_9_8(kal_uint32 val);
extern void upmu_auxadc_lowbat_en_min_volt(kal_uint32 val);
extern void upmu_auxadc_lowbat_irq_en_min_volt(kal_uint32 val);
//CON 25
extern kal_uint32 upmu_auxadc_lowbat_max_irq_status(void);
extern kal_uint32 upmu_auxadc_lowbat_min_irq_status(void);
//CON 26
extern void upmu_auxadc_da_dac_value_7_0(kal_uint32 val);
//CON 27
extern void upmu_auxadc_da_dac_value_9_8(kal_uint32 val);
extern kal_uint32 upmu_auxadc_ni_comp(void);
//CON 28
extern void upmu_auxadc_da_dac_sel(kal_uint32 val);
extern void upmu_auxadc_aux_out_sel(kal_uint32 val);
extern void upmu_auxadc_arb_prio_2(kal_uint32 val);
extern void upmu_auxadc_arb_prio_1(kal_uint32 val);
extern void upmu_auxadc_arb_prio_0(kal_uint32 val);
//CON 29
extern void upmu_auxadc_cali(kal_uint32 val);
extern void upmu_buf_cali(kal_uint32 val);

//==============================================================================
// TEST MODE APIs
//==============================================================================
//TEST MODE - CON 0
extern void upmu_testmode_mon_grp_sel(kal_uint32 val);
//TEST MODE - CON 1
extern void upmu_testmode_mon_flag_sel(kal_uint32 val);
//TEST MODE - CON 2
extern void upmu_testmode_io_pasel0_sel(kal_uint32 val);
extern void upmu_testmode_io_pasel1_sel(kal_uint32 val);
//TEST MODE - CON 3
extern void upmu_testmode_io_pasel2_sel(kal_uint32 val);
extern void upmu_testmode_io_int_sel(kal_uint32 val);
//TEST MODE - CON 4
extern void upmu_testmode_sck_pu(kal_uint32 val);
extern void upmu_testmode_sck_pd(kal_uint32 val);
extern void upmu_testmode_dio_smt(kal_uint32 val);
extern void upmu_testmode_dio_e2(kal_uint32 val);
extern void upmu_testmode_dio_e4(kal_uint32 val);
extern void upmu_testmode_dio_sr(kal_uint32 val);
//TEST MODE - CON 5
extern void upmu_testmode_sda_e2(kal_uint32 val);
extern void upmu_testmode_sda_e4(kal_uint32 val);
extern void upmu_testmode_sda_pu(kal_uint32 val);
extern void upmu_testmode_sda_pd(kal_uint32 val);
extern void upmu_testmode_int_e2(kal_uint32 val);
extern void upmu_testmode_int_e4(kal_uint32 val);
extern void upmu_testmode_int_pu(kal_uint32 val);
extern void upmu_testmode_int_pd(kal_uint32 val);
//TEST MODE - CON 6
extern void upmu_testmode_dvs_pu(kal_uint32 val);
extern void upmu_testmode_dvs_pd(kal_uint32 val);
extern void upmu_testmode_pasel_pu(kal_uint32 val);
extern void upmu_testmode_pasel_pd(kal_uint32 val);
extern void upmu_testmode_sysrstb_pu(kal_uint32 val);
extern void upmu_testmode_sysrstb_pd(kal_uint32 val);
//TEST MODE - CON 7
extern void upmu_testmode_srclken_pu(kal_uint32 val);
extern void upmu_testmode_srclken_pd(kal_uint32 val);
extern void upmu_testmode_bl_pwm_pu(kal_uint32 val);
extern void upmu_testmode_bl_pwm_pd(kal_uint32 val);
//TEST MODE - CON 8
extern void upmu_testmode_test_rsv(kal_uint32 val);
extern void upmu_testmode_test_strup(kal_uint32 val);
extern void upmu_testmode_otp_w_mode(kal_uint32 val);
extern void upmu_testmode_nandtree_mode(kal_uint32 val);
//TEST MODE - CON 9
extern void upmu_testmode_test_auxadc(kal_uint32 val);
extern void upmu_testmode_test_fgpll(kal_uint32 val);
extern void upmu_testmode_test_fg1(kal_uint32 val);
extern void upmu_testmode_test_fg2(kal_uint32 val);
extern void upmu_testmode_test_io_fg_sel(kal_uint32 val);
extern void upmu_testmode_test_classd(kal_uint32 val);
extern void upmu_testmode_test_driver(kal_uint32 val);
extern void upmu_testmode_test_boost(kal_uint32 val);
//TEST MODE - CON 10
extern kal_uint32 upmu_testmode_mon_flag_status(void);
//TEST MODE - CON 11
extern kal_uint32 upmu_testmode_dvs1_in_status(void);
extern kal_uint32 upmu_testmode_dvs2_in_status(void);
extern kal_uint32 upmu_testmode_pasel0_in_status(void);
extern kal_uint32 upmu_testmode_pasel1_in_status(void);
extern kal_uint32 upmu_testmode_pasel2_in_status(void);
extern kal_uint32 upmu_testmode_sysrstb_in_status(void);
extern kal_uint32 upmu_testmode_srclken_in_status(void);
extern kal_uint32 upmu_testmode_blpwm_in_status(void);
//TEST MODE - CON 12
extern void upmu_testmode_pwrkey_rst_en(kal_uint32 val);
extern void upmu_testmode_homekey_rst_en(kal_uint32 val);
extern void upmu_testmode_pwrkey_rst_td(kal_uint32 val);
extern void upmu_testmode_pwrrst_tmr_dis(kal_uint32 val);
//TEST MODE - CON 13
extern void upmu_testmode_rst_part_sel(kal_uint32 val);
extern void upmu_testmode_otp_man_rst(kal_uint32 val);
extern void upmu_testmode_pchr_man_rst_en(kal_uint32 val);
extern void upmu_testmode_pchr_man_rst(kal_uint32 val);
extern void upmu_testmode_strup_man_rst_en(kal_uint32 val);
extern void upmu_testmode_sif_tst_ck_dis(kal_uint32 val);
extern void upmu_testmode_sysrstb_en(kal_uint32 val);

//==============================================================================
// TOP2 APIs
//==============================================================================
//TOP2 - CON 0
extern void upmu_top2_75k_ext_sel(kal_uint32 val);
extern void upmu_top2_fg_tst_ck_sel(kal_uint32 val);
extern void upmu_top2_chr1m_tst_ck_sel(kal_uint32 val);
extern void upmu_top2_clk_tst(kal_uint32 val);
extern void upmu_top2_auxadc_ck_rst(kal_uint32 val);
extern void upmu_top2_auxadc_ck_sel(kal_uint32 val);
//TOP2 - CON 1
extern void upmu_top2_fgadc_ck_pdn(kal_uint32 val);
extern void upmu_top2_otpc_ck_pdn(kal_uint32 val);
extern void upmu_top2_bst_drv_ck_pdn(kal_uint32 val);
extern void upmu_top2_spk_ck_pdn(kal_uint32 val);
extern void upmu_top2_pwmoc_ck_pdn(kal_uint32 val);
//TOP2 - CON 2
extern void upmu_top2_oc_gear_bvalid_det(kal_uint32 val);
extern void upmu_top2_oc_gear_vbaton_undet(kal_uint32 val);
extern void upmu_top2_oc_gear_ldo(kal_uint32 val);
//TOP2 - CON 3
extern void upmu_top2_int_rsv(kal_uint32 val);
extern void upmu_top2_polarity_bvalid_det(kal_uint32 val);
extern void upmu_top2_polarity_vbaton_undet(kal_uint32 val);
extern void upmu_top2_polarity_select(kal_uint32 val);
//TOP2 - CON 4
extern void upmu_top2_vpa_oc_wnd(kal_uint32 val);
extern void upmu_top2_vpa_oc_thd(kal_uint32 val);
extern void upmu_top2_vrf18_oc_wnd(kal_uint32 val);
extern void upmu_top2_vrf18_oc_thd(kal_uint32 val);

//==============================================================================
// FLASH APIs
//==============================================================================
//FLASH - CON 0
extern void upmu_flash_dim_duty(kal_uint32 val);
//FLASH - CON 1
extern void upmu_flash_ther_shdn_en(kal_uint32 val);
extern void upmu_flash_en(kal_uint32 val);
//FLASH - CON 2
extern void upmu_flash_dim_div(kal_uint32 val);
//FLASH - CON 3
extern void upmu_flash_step_sel(kal_uint32 val);
//FLASH - CON 4
extern void upmu_flash_sfstren(kal_uint32 val);
extern void upmu_flash_sfstr_time(kal_uint32 val);
extern void upmu_flash_mode_select(kal_uint32 val);

//==============================================================================
// KPLED APIs
//==============================================================================
//KPLED - CON 0
extern void upmu_kpled_dim_duty(kal_uint32 val);
//KPLED - CON 1
extern void upmu_kpled_ther_shdn_en(kal_uint32 val);
extern void upmu_kpled_en(kal_uint32 val);
//KPLED - CON 2
extern void upmu_kpled_dim_div(kal_uint32 val);
//KPLED - CON 3
extern void upmu_kpled_step_sel(kal_uint32 val);
//KPLED - CON 4
extern void upmu_kpled_sfstren(kal_uint32 val);
extern void upmu_kpled_sfstr_time(kal_uint32 val);
extern void upmu_kpled_mode_select(kal_uint32 val);

//==============================================================================
// ISINKS APIs
//==============================================================================
//ISINKS - CON 0
extern void upmu_isinks_dim0_duty(kal_uint32 val);
//ISINKS - CON 1
extern void upmu_isinks_dim0_fsel(kal_uint32 val);
//ISINKS - CON 2
extern void upmu_isinks_dim1_duty(kal_uint32 val);
//ISINKS - CON 3
extern void upmu_isinks_dim1_fsel(kal_uint32 val);
//ISINKS - CON 4
extern void upmu_isinks_dim2_duty(kal_uint32 val);
//ISINKS - CON 5
extern void upmu_isinks_dim2_fsel(kal_uint32 val);
//ISINKS - CON 6
extern void upmu_isinks_ch5_en(kal_uint32 val);
extern void upmu_isinks_ch4_en(kal_uint32 val);
extern void upmu_isinks_ch3_en(kal_uint32 val);
extern void upmu_isinks_ch2_en(kal_uint32 val);
extern void upmu_isinks_ch1_en(kal_uint32 val);
extern void upmu_isinks_ch0_en(kal_uint32 val);
//ISINKS - CON 7
extern void upmu_isinks_ch5_cabc_en(kal_uint32 val);
extern void upmu_isinks_ch4_cabc_en(kal_uint32 val);
extern void upmu_isinks_ch3_cabc_en(kal_uint32 val);
extern void upmu_isinks_ch2_cabc_en(kal_uint32 val);
extern void upmu_isinks_ch1_cabc_en(kal_uint32 val);
extern void upmu_isinks_ch0_cabc_en(kal_uint32 val);
//ISINKS - CON 8
extern void upmu_isinks_ch0_step(kal_uint32 val);
extern void upmu_isinks_ch0_mode(kal_uint32 val);
//ISINKS - CON 9
extern void upmu_isinks_ch1_step(kal_uint32 val);
extern void upmu_isinks_ch1_mode(kal_uint32 val);
//ISINKS - CON 10
extern void upmu_isinks_ch2_step(kal_uint32 val);
extern void upmu_isinks_ch2_mode(kal_uint32 val);
//ISINKS - CON 11
extern void upmu_isinks_ch3_step(kal_uint32 val);
extern void upmu_isinks_ch3_mode(kal_uint32 val);
//ISINKS - CON 12
extern void upmu_isinks_ch4_step(kal_uint32 val);
extern void upmu_isinks_ch4_mode(kal_uint32 val);
//ISINKS - CON 13
extern void upmu_isinks_ch5_step(kal_uint32 val);
extern void upmu_isinks_ch5_mode(kal_uint32 val);
//ISINKS - CON 14
extern void upmu_isinks_ibias_trim_en(kal_uint32 val);
extern void upmu_isinks_vref_cal(kal_uint32 val);

//==============================================================================
// BOOST APIs
//==============================================================================
//BOOST - CON 0
extern void upmu_boost_isink_hw_sel(kal_uint32 val);
extern void upmu_boost_mode(kal_uint32 val);
extern void upmu_boost_cabc_en(kal_uint32 val);
extern void upmu_boost_en(kal_uint32 val);
//BOOST - CON 1
extern void upmu_boost_sr_nmos(kal_uint32 val);
extern void upmu_boost_vrsel(kal_uint32 val);
//BOOST - CON 2
extern void upmu_boost_rc(kal_uint32 val);
extern void upmu_boost_cs(kal_uint32 val);
extern void upmu_boost_cc(kal_uint32 val);
//BOOST - CON 3
extern void upmu_boost_slp(kal_uint32 val);
extern void upmu_boost_cl(kal_uint32 val);
//BOOST - CON 4
extern void upmu_boost_ss(kal_uint32 val);
extern void upmu_boost_rsv(kal_uint32 val);

//==============================================================================
// SPK APIs
//==============================================================================
//SPK - CON 0
extern void upmu_spk_ther_shdn_l_en(kal_uint32 val);
extern void upmu_spk_trim_en_l(kal_uint32 val);
extern void upmu_spk_mode_l(kal_uint32 val);
extern void upmu_spk_en_l(kal_uint32 val);
//SPK - CON 1
extern void upmu_spk_oc_en_l(kal_uint32 val);
extern void upmu_spkab_oc_en_l(kal_uint32 val);
extern void upmu_spk_oc_shdn_dl(kal_uint32 val);
extern void upmu_spk_vol_l(kal_uint32 val);
//SPK - CON 2
extern void upmu_spk_ab_oc_l_deg(kal_uint32 val);
extern void upmu_spk_d_oc_l_deg(kal_uint32 val);
//SPK - CON 3
extern void upmu_spk_offset_l_ov(kal_uint32 val);
extern void upmu_spk_offset_l_mode(kal_uint32 val);
extern void upmu_spk_lead_l_sw(kal_uint32 val);
extern void upmu_spk_offset_l_sw(kal_uint32 val);
//SPK - CON 4
extern kal_uint32 upmu_spk_trim_done_l(void);
extern kal_uint32 upmu_spk_lead_l_flag(void);
extern kal_uint32 upmu_spk_lead_l_flag_deg(void);
extern kal_uint32 upmu_spk_offset_l(void);
//SPK - CON 5
extern void upmu_spkrcv_en_l(kal_uint32 val);
extern void upmu_spkab_obias_l(kal_uint32 val);
extern void upmu_spk_slew_l(kal_uint32 val);
extern void upmu_spk_force_en_l(kal_uint32 val);
extern void upmu_spk_intg_rst_l(kal_uint32 val);
//SPK - CON 6
extern void upmu_spk_ther_shdn_r_en(kal_uint32 val);
extern void upmu_spk_trim_en_r(kal_uint32 val);
extern void upmu_spk_mode_r(kal_uint32 val);
extern void upmu_spk_en_r(kal_uint32 val);
//SPK - CON 7
extern void upmu_spk_oc_en_r(kal_uint32 val);
extern void upmu_spkab_oc_en_r(kal_uint32 val);
extern void upmu_spk_oc_shdn_dr(kal_uint32 val);
extern void upmu_spk_vol_r(kal_uint32 val);	
//SPK - CON 8
extern void upmu_spk_ab_oc_r_deg(kal_uint32 val);
extern void upmu_spk_d_oc_r_deg(kal_uint32 val);
//SPK - CON 9
extern void upmu_spk_offset_r_ov(kal_uint32 val);
extern void upmu_spk_offset_r_mode(kal_uint32 val);
extern void upmu_spk_lead_r_sw(kal_uint32 val);
extern void upmu_spk_offset_r_sw(kal_uint32 val);
//SPK - CON 10
extern kal_uint32 upmu_spk_trim_done_r(void);
extern kal_uint32 upmu_spk_lead_r_flag(void);
extern kal_uint32 upmu_spk_lead_r_flag_deg(void);
extern kal_uint32 upmu_spk_offset_r(void);
//SPK - CON 11
extern void upmu_spkrcv_en_r(kal_uint32 val);
extern void upmu_spkab_obias_r(kal_uint32 val);
extern void upmu_spk_slew_r(kal_uint32 val);
extern void upmu_spk_force_en_r(kal_uint32 val);
extern void upmu_spk_intg_rst_r(kal_uint32 val);
//SPK - CON 12
extern void upmu_spk_oc_autoff(kal_uint32 val);
extern void upmu_spk_oc_dglh(kal_uint32 val);
extern void upmu_spk_octh_d(kal_uint32 val);
//SPK - CON 13
extern void upmu_spk_oc_wnd(kal_uint32 val);
extern void upmu_spk_oc_thd(kal_uint32 val);
//SPK - CON 14
extern void upmu_spk_trim_div(kal_uint32 val);
extern void upmu_spk_trim_deg(kal_uint32 val);
//SPK - CON 15
extern void upmu_spkab_fb_att(kal_uint32 val);
extern void upmu_spkab_ovdrv(kal_uint32 val);
//SPK - CON 16
extern void upmu_spk_fbrc_en(kal_uint32 val);
extern void upmu_spk_ibias_sel(kal_uint32 val);
extern void upmu_spk_vcm_ibsel(kal_uint32 val);
extern void upmu_spk_vcm_sel(kal_uint32 val);
extern void upmu_spk_en_view_clk(kal_uint32 val);
extern void upmu_spk_en_view_vcm(kal_uint32 val);
//SPK - CON 17
extern void upmu_spk_ccode(kal_uint32 val);
extern void upmu_spk_btl_set(kal_uint32 val);
//SPK - CON 18
extern void upmu_spk_rsv(kal_uint32 val);
//SPK - CON 19
extern void upmu_spk_td1(kal_uint32 val);
extern void upmu_spk_td2(kal_uint32 val);

//==============================================================================
// ASW APIs
//==============================================================================
//ASW - CON 0
extern void upmu_asw_ana_sw_sel(kal_uint32 val);

//==============================================================================
// FGPLL APIs
//==============================================================================
//FGPLL - CON 0
extern void upmu_fgpll_pdiv1(kal_uint32 val);
extern void upmu_fgpll_pdiv1_en(kal_uint32 val);
extern void upmu_fgpll_pllbs_rst(kal_uint32 val);
extern void upmu_fgpll_en(kal_uint32 val);
//FGPLL - CON 1
extern void upmu_fgpll_div1(kal_uint32 val);
//FGPLL - CON 2
extern void upmu_fgpll_bc(kal_uint32 val);
extern void upmu_fgpll_bp(kal_uint32 val);
extern void upmu_fgpll_br(kal_uint32 val);
//FGPLL - CON 3
extern void upmu_fgpll_cdiv(kal_uint32 val);
extern void upmu_fgpll_vcoband(kal_uint32 val);
extern void upmu_fgpll_cko_sel(kal_uint32 val);
//FGPLL - CON 4
extern void upmu_fgpll_ibsel(kal_uint32 val);
extern void upmu_fgpll_rlatch_en(kal_uint32 val);
extern void upmu_fgpll_ckdrv_en(kal_uint32 val);
extern void upmu_fgpll_vct_en(kal_uint32 val);
extern void upmu_fgpll_ckt_sel(kal_uint32 val);
extern void upmu_fgpll_ckt_en(kal_uint32 val);
//FGPLL - CON 5
extern void upmu_fgpll_rsva(kal_uint32 val);
//FGPLL - CON 6
extern void upmu_fgpll_rsvb(kal_uint32 val);

//==============================================================================
// FGADC APIs
//==============================================================================
//FGADC - CON 0
extern void upmu_fgadc_clksrc(kal_uint32 val);
extern void upmu_fgadc_autocalrate(kal_uint32 val);
extern void upmu_fgadc_cal(kal_uint32 val);
extern void upmu_fgadc_vmode(kal_uint32 val);
extern void upmu_fgadc_on(kal_uint32 val);
//FGADC - CON 1
extern void upmu_fgadc_sw_rst_clr(kal_uint32 val);
extern void upmu_fgadc_charge_rst(kal_uint32 val);
extern void upmu_fgadc_time_rst(kal_uint32 val);
extern void upmu_fgadc_offset_rst(kal_uint32 val);
extern void upmu_fgadc_sw_clear(kal_uint32 val);
extern void upmu_fgadc_latchdata_st(kal_uint32 val);
extern void upmu_fgadc_sw_read_pre(kal_uint32 val);
extern void upmu_fgadc_sw_cr(kal_uint32 val);
//FGADC - CON 2
extern kal_uint32 upmu_fgadc_car_35_32(void);
//FGADC - CON 3
extern kal_uint32 upmu_fgadc_car_31_24(void);
//FGADC - CON 4
extern kal_uint32 upmu_fgadc_car_23_16(void);
//FGADC - CON 5
extern kal_uint32 upmu_fgadc_car_15_08(void);
//FGADC - CON 6
extern kal_uint32 upmu_fgadc_car_07_00(void);
//FGADC - CON 7
extern kal_uint32 upmu_fgadc_nter_29_24(void);
//FGADC - CON 8
extern kal_uint32 upmu_fgadc_nter_23_16(void);
//FGADC - CON 9
extern kal_uint32 upmu_fgadc_nter_15_08(void);
//FGADC - CON 10
extern kal_uint32 upmu_fgadc_nter_07_00(void);
//FGADC - CON 11
extern void upmu_fgadc_bltr_15_08(kal_uint32 val);
//FGADC - CON 12
extern void upmu_fgadc_bltr_07_00(kal_uint32 val);
//FGADC - CON 13
extern void upmu_fgadc_bftr_15_08(kal_uint32 val);
//FGADC - CON 14
extern void upmu_fgadc_bftr_07_00(kal_uint32 val);
//FGADC - CON 15
extern kal_uint32 upmu_fgadc_current_out_15_08(void);
//FGADC - CON 16
extern kal_uint32 upmu_fgadc_current_out_07_00(void);
//FGADC - CON 17
extern void upmu_fgadc_adjust_offset_value_15_08(kal_uint32 val);
//FGADC - CON 18
extern void upmu_fgadc_adjust_offset_value_07_00(kal_uint32 val);
//FGADC - CON 19
extern kal_uint32 upmu_fgadc_offset_15_08(void);
//FGADC - CON 20
extern kal_uint32 upmu_fgadc_offset_07_00(void);
//FGADC - CON 21
extern void upmu_fgadc_analogtest(kal_uint32 val);
//FGADC - CON 22
extern void upmu_fgadc_spare(kal_uint32 val);
//FGADC - CON 23
extern void upmu_fgadc_bltr_bftr_en(kal_uint32 val);
extern void upmu_fgadc_adc_autorst(kal_uint32 val);
extern void upmu_fgadc_adj_offset_en(kal_uint32 val);
extern void upmu_fgadc_osr(kal_uint32 val);
//FGADC - CON 24
extern void upmu_fgadc_vol_osr(kal_uint32 val);
//FGADC - CON 25
extern kal_uint32 upmu_fgadc_adc_rstdetect(void);
extern kal_uint32 upmu_fgadc_h_int_sts(void);
extern kal_uint32 upmu_fgadc_l_int_sts(void);
extern void upmu_fgadc_vol_fir1bypass(kal_uint32 val);
extern void upmu_fgadc_fg_fir2bypass(kal_uint32 val);
extern void upmu_fgadc_fg_fir1bypass(kal_uint32 val);
//FGADC - CON 26
extern kal_uint32 upmu_fgadc_vol_current_out_15_08(void);
//FGADC - CON 27
extern kal_uint32 upmu_fgadc_vol_current_out_07_00(void);
//FGADC - CON 28
extern kal_uint32 upmu_fgadc_cic2_15_08(void);
//FGADC - CON 29
extern kal_uint32 upmu_fgadc_cic2_07_00(void);
//FGADC - CON 30
extern void upmu_fgadc_vmode_sw(kal_uint32 val);
extern void upmu_fgadc_fgadc_en_sw(kal_uint32 val);
extern void upmu_fgadc_fgcal_en_sw(kal_uint32 val);
extern void upmu_fgadc_rst_sw(kal_uint32 val);
extern void upmu_fgadc_mode(kal_uint32 val);

//==============================================================================
// OTPC APIs
//==============================================================================
//OTPC - CON 0
extern kal_uint32 upmu_otpc_otp_pdo_7_0(void);
//OTPC - CON 1
extern kal_uint32 upmu_otpc_otp_pdo_15_8(void);
//OTPC - CON 2
extern kal_uint32 upmu_otpc_otp_pdo_23_16(void);
//OTPC - CON 3
extern kal_uint32 upmu_otpc_otp_pdo_31_24(void);
//OTPC - CON 4
extern kal_uint32 upmu_otpc_otp_pdo_39_32(void);
//OTPC - CON 5
extern kal_uint32 upmu_otpc_otp_pdo_47_40(void);
//OTPC - CON 6
extern kal_uint32 upmu_otpc_otp_pdo_55_48(void);
//OTPC - CON 7
extern kal_uint32 upmu_otpc_otp_pdo_63_56(void);
//OTPC - CON 8
extern void upmu_otpc_otp_pdin(kal_uint32 val);
//OTPC - CON 9
extern void upmu_otpc_otp_pa(kal_uint32 val);
extern void upmu_otpc_otp_ptm(kal_uint32 val);
extern void upmu_otpc_otp_pwe(kal_uint32 val);
extern void upmu_otpc_otp_pprog(kal_uint32 val);
extern void upmu_otpc_otp_read(kal_uint32 val);
//OTPC - CON 10
extern void upmu_otpc_otp_read_prd(kal_uint32 val);
//OTPC - CON 11
extern void upmu_otpc_otp_test_sel(kal_uint32 val);
extern void upmu_otpc_otp_skip_out(kal_uint32 val);
extern void upmu_otpc_otp_out_sel(kal_uint32 val);
extern void upmu_otpc_otp_rsv(kal_uint32 val);
//OTPC - CON 12
extern void upmu_otpc_otp_val_7_0(kal_uint32 val);
//OTPC - CON 13
extern void upmu_otpc_otp_val_15_8(kal_uint32 val);
//OTPC - CON 14
extern void upmu_otpc_otp_val_23_16(kal_uint32 val);
//OTPC - CON 15
extern void upmu_otpc_otp_val_31_24(kal_uint32 val);
//OTPC - CON 16
extern void upmu_otpc_otp_val_39_32(kal_uint32 val);
//OTPC - CON 17
extern void upmu_otpc_otp_val_47_40(kal_uint32 val);
//OTPC - CON 18
extern void upmu_otpc_otp_val_55_48(kal_uint32 val);
//OTPC - CON 19
extern void upmu_otpc_otp_val_63_56(kal_uint32 val);
//OTPC - CON 20
extern kal_uint32 upmu_otpc_otp_busy(void);
extern kal_uint32 upmu_otpc_otp_vld(void);
extern void upmu_otpc_otp_read_rdy_bypass(kal_uint32 val);
//OTPC - CON 21
extern kal_uint32 upmu_otpc_otp_w_lock(void);
extern void upmu_otpc_otp_w_lock_key_tog(kal_uint32 val);
extern void upmu_otpc_otp_w_lock_key(kal_uint32 val);

//==============================================================================
// I2C APIs
//==============================================================================
//I2C - CON 0
extern void upmu_i2c_scl_deglitch(kal_uint32 val);
//I2C - CON 1
extern void upmu_i2c_sdin_deglitch(kal_uint32 val);


#ifdef PMIC_SW_TEST_MODE_6326
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

#define PMIC_REG_UNM            0x97
#endif

#define PMIC6329_E1_CID_CODE    0x0029
#define PMIC6329_E2_CID_CODE    0x1029

typedef enum {
    CHARGER_UNKNOWN = 0,
    STANDARD_HOST,          // USB : 450mA
    CHARGING_HOST,
    NONSTANDARD_CHARGER,    // AC : 450mA~1A 
    STANDARD_CHARGER,       // AC : ~1A
} CHARGER_TYPE;

#endif // _MT6577_PMIC_UBOOT_SW_H_

