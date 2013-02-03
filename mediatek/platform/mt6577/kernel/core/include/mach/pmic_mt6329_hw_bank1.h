/*****************************************************************************
 *
 * Filename:
 * ---------
 *    pmic)mt6329_hw_bank1.h
 *
 * Project:
 * --------
 *   Android_Software
 *
 * Description:
 * ------------
 *   This file is intended for PMIC 6329 bank 1 driver.
 *
 * Author:
 * -------
 * -------
 *
 ****************************************************************************/

#ifndef _MT6577_PMIC_HW_BANK1_H_
#define _MT6577_PMIC_HW_BANK1_H_

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

#endif // _MT6577_PMIC_HW_BANK1_H_

