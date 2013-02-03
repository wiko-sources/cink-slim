/*****************************************************************************
 *
 * Filename:
 * ---------
 *    pmic_mt6329_sw_bank1.h
 *
 * Project:
 * --------
 *   Android_Software
 *
 * Description:
 * ------------
 *   This file is intended for PMU 6329 bank 1 driver.
 *
 * Author:
 * -------
 * -------
 *
 ****************************************************************************/

#ifndef _MT6577_PMIC_SW_BANK1_H_
#define _MT6577_PMIC_SW_BANK1_H_

//#include "upmu_sw.h"

//==============================================================================
// The CHIP SPEC of each block
//==============================================================================

//==============================================================================
// PMIC6329 Status Code
//==============================================================================
#define PMIC_BANK1_TEST_PASS               0x0000
#define PMIC_BANK1_TEST_FAIL               0xB001
#define PMIC_BANK1_EXCEED_I2C_FIFO_LENGTH  0xB002
#define PMIC_BANK1_CHRDET_EXIST            0xB003
#define PMIC_BANK1_CHRDET_NOT_EXIST        0xB004

//==============================================================================
// PMIC6329 Exported Function
//==============================================================================
extern kal_uint32 pmic_bank1_read_interface (kal_uint8 RegNum, kal_uint8 *val, kal_uint8 MASK, kal_uint8 SHIFT);
extern kal_uint32 pmic_bank1_config_interface (kal_uint8 RegNum, kal_uint8 val, kal_uint8 MASK, kal_uint8 SHIFT);
extern kal_uint32 pmic_bank1_int_en_interface (kal_uint8 RegNum, kal_uint8 sel, kal_bool enable);

extern int mt6329_bank1_multi_read_byte(u8 cmd, u8 *returnData, int len);
extern int mt6329_bank1_multi_write_byte(u8 *cmd, u8 *writeData, int len);

#endif // _MT6577_PMIC_SW_BANK1_H_

