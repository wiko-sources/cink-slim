/*******************************************************************************
 *
 * Filename:
 * ---------
 *   auddrv_register.h
 *
 * Project:
 * --------
 *   Android
 *
 * Description:
 * ------------
 *   Audio Register Setting
 *
 * Author:
 * -------
 *   Stan Huang (mtk01728)
 *
 *
 *------------------------------------------------------------------------------
 * $Revision$
 * $Modtime:$
 * $Log:$
 *
 *
 *
 *******************************************************************************/

#ifndef _AUDDRV_REGISTER_H
#define _AUDDRV_REGISTER_H

/*****************************************************************************
*                E X T E R N A L   R E F E R E N C E S
******************************************************************************
*/

#include <mach/mt6577_reg_base.h>
//#include "../power/pmu6575_hw.h"

/*****************************************************************************
*                     C O M P I L E R   F L A G S
******************************************************************************
*/



/*****************************************************************************
*                         D A T A   T Y P E S
******************************************************************************
*/


/*****************************************************************************
*                          C O N S T A N T S
******************************************************************************
*/
#define MASK_ALL          (0xFFFFFFFF)
#define AFE_REGION        (0x3CC)

#ifndef AFE_BASE
#define AFE_BASE           AUDIO_BASE
#endif
/*
#ifndef AFE_DATA_BASE
#define AFE_DATA_BASE      (AUDIO_BASE + 0x0800)
#endif
*/
#define IRQ1_MCU_CLR				(1<<0)
#define IRQ2_MCU_CLR				(1<<1)
#define IRQ_MCU_DAI_SET_CLR	(1<<2)
#define IRQ_MCU_DAI_RST_CLR	(1<<3)

// AudioSys AFE base address
#ifndef AUDIO_AFE_REGS_BASE
#define AUDIO_AFE_REGS_BASE             AUDIO_BASE
#endif
/*
//Audio Slave Mode Buffer
#define AFE_DL1_SYSRAM_BASE (AFE_DATA_BASE + 0x0000)
#define AFE_DL1_SYSRAM_END  (AFE_DATA_BASE + 0x2700)
#define AFE_VUL_SYSRAM_BASE (AFE_DATA_BASE + 0x2700)
#define AFE_VUL_SYSRAM_END  (AFE_DATA_BASE + 0x2880)
#define AFE_DAI_SYSRAM_BASE (AFE_DATA_BASE + 0x2880)
#define AFE_DAI_SYSRAM_END  (AFE_DATA_BASE + 0x2940)
*/
// Analog AFE Base Address
#define ANA_MASK_ALL          (0xffffffff)
/*
#ifndef MIXEDSYS0_BASE
#define MIXEDSYS0_BASE          APMIXED_BASE
#endif

#ifndef AP_MDSYS_BASE
#define AP_MDSYS_BASE          ABB_MDSYS_BASE
#endif
*/
/*
#ifndef MIXEDSYS1_BASE
#define MIXEDSYS1_BASE          (0xF702F000)
#endif
*/
/*****************************************************************************
*           A U D I O S Y S      R E G I S T E R     D E F I N I T I O N
******************************************************************************
*/
#if defined(AUDIO_TOP_CON0)
#undef AUDIO_TOP_CON0
#endif
#define AUDIO_TOP_CON0    (0x0000)
#define AFE_DAC_CON0      (0x0010)
#define AFE_DAC_CON1      (0x0014)
#define AFE_I2S_IN_CON    (0x0018)
#define AFE_FOC_CON       (0x0170)
#define AFE_DAIBT_CON     (0x001c)
#define AFE_CONN0         (0x0020)
#define AFE_CONN1         (0x0024)
#define AFE_CONN2         (0x0028)
#define AFE_CONN3         (0x002C)
#define AFE_CONN4         (0x0030)
#define AFE_I2S_OUT_CON   (0x0034)
#define AFE_DL1_BASE      (0x0040)
#define AFE_DL1_CUR       (0x0044)
#define AFE_DL1_END       (0x0048)
#define AFE_DL2_BASE      (0x0050)
#define AFE_DL2_CUR       (0x0054)
#define AFE_DL2_END       (0x0058)
#define AFE_I2S_BASE      (0x0060)
#define AFE_I2S_CUR       (0x0064)
#define AFE_I2S_END       (0x0068)
#define AFE_AWB_BASE      (0x0070)
#define AFE_AWB_CUR       (0x0074)
#define AFE_AWB_END       (0x0078)
#define AFE_VUL_CUR       (0x0084)
#define AFE_DAI_CUR       (0x0094)

#define AFE_DL_SRC1_1     (0x0100) //reserved
#define AFE_DL_SRC1_2     (0x0104) //reserved
#define AFE_DL_SRC2_1     (0x0108)
#define AFE_DL_SRC2_2     (0x010C)
#define AFE_DL_SDM_CON0   (0x0110)
#define AFE_UL_SRC_0      (0x0114)
#define AFE_UL_SRC_1      (0x0118)

#define AFE_IRQ_CON       (0x03A0)
#define AFE_IR_STATUS     (0x03A4)
#define AFE_IR_CLR        (0x03A8)
#define AFE_IRQ_CNT1      (0x03AC)
#define AFE_IRQ_CNT2      (0x03B0)
#define AFE_IRQ_MON       (0x03B8)

// IRQ for Modem part
#define AFE_MODEM_IRQ_CON       (0x00A0)
#define AFE_MODEM_IR_STATUS     (0x00A4)
#define AFE_MODEM_IR_CLR        (0x00A8)
#define AFE_MODEM_IRQ_CNT1      (0x00AC)
#define AFE_MODEM_IRQ_CNT2      (0x00B0)


//Register : AGC
#define AFE_UL_AGC0        (0x020c)
#define AFE_UL_AGC1        (0x0120)
#define AFE_UL_AGC2        (0x0124)
#define AFE_UL_AGC3        (0x0128)
#define AFE_UL_AGC4        (0x012C)
#define AFE_UL_AGC5        (0x0130)
#define AFE_UL_AGC6        (0x0134)
#define AFE_UL_AGC7        (0x0138)
#define AFE_UL_AGC8        (0x013C)
#define AFE_UL_AGC9        (0x0140)
#define AFE_UL_AGC10       (0x0144)
#define AFE_UL_AGC11       (0x0148)
#define AFE_UL_AGC12       (0x014C)
#define AFE_UL_AGC13       (0x0150)
#define AFE_UL_AGC14       (0x0154)
#define AFE_UL_AGC15       (0x0158)
#define AFE_UL_AGC16       (0x015C)
#define AFE_UL_AGC17       (0x0160)
#define AFE_UL_AGC18       (0x0164)

#define AFE_SDM_GAIN_STAGE (0x0168) //[31]: 0, [5:0]: gain :default:0x10 -6dB

//SIDETONE
#define AFE_SIDETONE_CON0  (0x01E0)
#define AFE_SIDETONE_CON1  (0x01E4)

//Register : TOP CONTROL
#define AFE_TOP_CONTROL_0  (0x0200)


#define BIT_00	0x00000001        /* ---- ---- ---- ---- ---- ---- ---- ---1 */
#define BIT_01	0x00000002        /* ---- ---- ---- ---- ---- ---- ---- --1- */
#define BIT_02	0x00000004        /* ---- ---- ---- ---- ---- ---- ---- -1-- */
#define BIT_03	0x00000008        /* ---- ---- ---- ---- ---- ---- ---- 1--- */
#define BIT_04	0x00000010        /* ---- ---- ---- ---- ---- ---- ---1 ---- */
#define BIT_05	0x00000020        /* ---- ---- ---- ---- ---- ---- --1- ---- */
#define BIT_06	0x00000040        /* ---- ---- ---- ---- ---- ---- -1-- ---- */
#define BIT_07	0x00000080        /* ---- ---- ---- ---- ---- ---- 1--- ---- */
#define BIT_08	0x00000100        /* ---- ---- ---- ---- ---- ---1 ---- ---- */
#define BIT_09	0x00000200        /* ---- ---- ---- ---- ---- --1- ---- ---- */
#define BIT_10	0x00000400        /* ---- ---- ---- ---- ---- -1-- ---- ---- */
#define BIT_11	0x00000800        /* ---- ---- ---- ---- ---- 1--- ---- ---- */
#define BIT_12	0x00001000        /* ---- ---- ---- ---- ---1 ---- ---- ---- */
#define BIT_13	0x00002000        /* ---- ---- ---- ---- --1- ---- ---- ---- */
#define BIT_14	0x00004000        /* ---- ---- ---- ---- -1-- ---- ---- ---- */
#define BIT_15	0x00008000        /* ---- ---- ---- ---- 1--- ---- ---- ---- */
#define BIT_16	0x00010000        /* ---- ---- ---- ---1 ---- ---- ---- ---- */
#define BIT_17	0x00020000        /* ---- ---- ---- --1- ---- ---- ---- ---- */
#define BIT_18	0x00040000        /* ---- ---- ---- -1-- ---- ---- ---- ---- */
#define BIT_19	0x00080000        /* ---- ---- ---- 1--- ---- ---- ---- ---- */
#define BIT_20	0x00100000        /* ---- ---- ---1 ---- ---- ---- ---- ---- */
#define BIT_21	0x00200000        /* ---- ---- --1- ---- ---- ---- ---- ---- */
#define BIT_22	0x00400000        /* ---- ---- -1-- ---- ---- ---- ---- ---- */
#define BIT_23	0x00800000        /* ---- ---- 1--- ---- ---- ---- ---- ---- */
#define BIT_24	0x01000000        /* ---- ---1 ---- ---- ---- ---- ---- ---- */
#define BIT_25	0x02000000        /* ---- --1- ---- ---- ---- ---- ---- ---- */
#define BIT_26	0x04000000        /* ---- -1-- ---- ---- ---- ---- ---- ---- */
#define BIT_27	0x08000000        /* ---- 1--- ---- ---- ---- ---- ---- ---- */
#define BIT_28	0x10000000        /* ---1 ---- ---- ---- ---- ---- ---- ---- */
#define BIT_29	0x20000000        /* --1- ---- ---- ---- ---- ---- ---- ---- */
#define BIT_30	0x40000000        /* -1-- ---- ---- ---- ---- ---- ---- ---- */
#define BIT_31	0x80000000        /* 1--- ---- ---- ---- ---- ---- ---- ---- */


/*****************************************************************************
*           A N A L O G A F E      R E G I S T E R     D E F I N I T I O N
******************************************************************************
*/
// AP_MDSYS
#define WR_PATH0         (ABB_MDSYS_BASE+0x0100)
#define WR_PATH1         (ABB_MDSYS_BASE+0x0104)
#define WR_PATH2         (ABB_MDSYS_BASE+0x0108)
#define ABIST_MON_CON0   (ABB_MDSYS_BASE+0x0220)
#define ABIST_MON_CON1   (ABB_MDSYS_BASE+0x0224)
#define ABIST_MON_CON2   (ABB_MDSYS_BASE+0x0228)
#define ABIST_MON_CON3   (ABB_MDSYS_BASE+0x022C)
#define ABIST_MON_CON4   (ABB_MDSYS_BASE+0x0230)
#define ABIST_MON_CON5   (ABB_MDSYS_BASE+0x0234)
#define ABIST_MON_CON6   (ABB_MDSYS_BASE+0x0238)
#define ABIST_MON_CON7   (ABB_MDSYS_BASE+0x023C)
#define ABIST_MON_CON8   (ABB_MDSYS_BASE+0x0240)
#define AUDIO_CON0       (ABB_MDSYS_BASE+0x0300)
#define AUDIO_CON1       (ABB_MDSYS_BASE+0x0304)
#define AUDIO_CON2       (ABB_MDSYS_BASE+0x0308)
#define AUDIO_CON3       (ABB_MDSYS_BASE+0x030C)
#define AUDIO_CON4       (ABB_MDSYS_BASE+0x0310)
#define AUDIO_CON5       (ABB_MDSYS_BASE+0x0314)
#define AUDIO_CON6       (ABB_MDSYS_BASE+0x0318)
#define AUDIO_CON7       (ABB_MDSYS_BASE+0x031C)
#define AUDIO_CON8       (ABB_MDSYS_BASE+0x0320)
#define AUDIO_CON9       (ABB_MDSYS_BASE+0x0324)
#define AUDIO_CON10      (ABB_MDSYS_BASE+0x0328)
#define AUDIO_CON11      (ABB_MDSYS_BASE+0x032C)
#define AUDIO_CON12      (ABB_MDSYS_BASE+0x0330)
#define AUDIO_CON13      (ABB_MDSYS_BASE+0x0334)
#define AUDIO_CON14      (ABB_MDSYS_BASE+0x0338)
#define AUDIO_CON15      (ABB_MDSYS_BASE+0x033C)
#define AUDIO_CON16      (ABB_MDSYS_BASE+0x0340)
#define AUDIO_CON17      (ABB_MDSYS_BASE+0x0344)
#define AUDIO_CON20      (ABB_MDSYS_BASE+0x0380)
#define AUDIO_CON21      (ABB_MDSYS_BASE+0x0384)
#define AUDIO_CON22      (ABB_MDSYS_BASE+0x0388)
#define AUDIO_CON23      (ABB_MDSYS_BASE+0x038C)
#define AUDIO_CON24      (ABB_MDSYS_BASE+0x0390)
#define AUDIO_CON25      (ABB_MDSYS_BASE+0x0394)
#define AUDIO_CON26      (ABB_MDSYS_BASE+0x0398)
#define AUDIO_CON27      (ABB_MDSYS_BASE+0x039C)
#define AUDIO_CON28      (ABB_MDSYS_BASE+0x03A0)
#define AUDIO_CON29      (ABB_MDSYS_BASE+0x03A4)
#define AUDIO_CON30      (ABB_MDSYS_BASE+0x03A8)
#define AUDIO_CON31      (ABB_MDSYS_BASE+0x03AC)
#define AUDIO_CON32      (ABB_MDSYS_BASE+0x03B0)
#define AUDIO_CON33      (ABB_MDSYS_BASE+0x03B4)
#define AUDIO_CON34      (ABB_MDSYS_BASE+0x03B8)
#define AUDIO_CON35      (ABB_MDSYS_BASE+0x03BC)

#define AUDIO_NCP0        (ABB_MDSYS_BASE+0x0400)
#define AUDIO_NCP1        (ABB_MDSYS_BASE+0x0404)
#define AUDIO_LDO0        (ABB_MDSYS_BASE+0x0440)
#define AUDIO_LDO1        (ABB_MDSYS_BASE+0x0444)
#define AUDIO_LDO2        (ABB_MDSYS_BASE+0x0448)
#define AUDIO_LDO3        (ABB_MDSYS_BASE+0x044C)
#define AUDIO_GLB0        (ABB_MDSYS_BASE+0x0480)
#define AUDIO_GLB1        (ABB_MDSYS_BASE+0x0484)
#define AUDIO_GLB2        (ABB_MDSYS_BASE+0x0488)
#define AUDIO_REG0        (ABB_MDSYS_BASE+0x04C0)
#define AUDIO_REG1        (ABB_MDSYS_BASE+0x04C4)

#define BBRX_CON0         (ABB_MDSYS_BASE+0x0A00)
#define BBRX_CON1         (ABB_MDSYS_BASE+0x0A04)
#define BBRX_CON2         (ABB_MDSYS_BASE+0x0A08)
#define BBRX_CON3         (ABB_MDSYS_BASE+0x0A0C)
#define BBRX_CON4         (ABB_MDSYS_BASE+0x0A10)
#define BBRX_CON5         (ABB_MDSYS_BASE+0x0A14)
#define BBRX_CON6         (ABB_MDSYS_BASE+0x0A18)
#define BBRX_CON7         (ABB_MDSYS_BASE+0x0A1C)
#define BBRX_CON8         (ABB_MDSYS_BASE+0x0A20)
#define BBRX_CON9         (ABB_MDSYS_BASE+0x0A24)
#define BBRX_CON10        (ABB_MDSYS_BASE+0x0A28)
#define BBRX_CON11        (ABB_MDSYS_BASE+0x0A2C)
#define BBRX_CON12        (ABB_MDSYS_BASE+0x0A30)
#define BBRX_CON13        (ABB_MDSYS_BASE+0x0A34)
#define BBRX_CON14        (ABB_MDSYS_BASE+0x0A38)
#define BBRX_CON15        (ABB_MDSYS_BASE+0x0A3C)
#define BBRX_CON16        (ABB_MDSYS_BASE+0x0A40)
#define BBRX_CON17        (ABB_MDSYS_BASE+0x0A44)

#define BBTX_CON0         (ABB_MDSYS_BASE+0x0A80)
#define BBTX_CON1         (ABB_MDSYS_BASE+0x0A84)
#define BBTX_CON2         (ABB_MDSYS_BASE+0x0A88)
#define BBTX_CON3         (ABB_MDSYS_BASE+0x0A8C)
#define BBTX_CON4         (ABB_MDSYS_BASE+0x0A90)
#define BBTX_CON5         (ABB_MDSYS_BASE+0x0A94)
#define BBTX_CON6         (ABB_MDSYS_BASE+0x0A98)
#define BBTX_CON7         (ABB_MDSYS_BASE+0x0A9C)
#define BBTX_CON9         (ABB_MDSYS_BASE+0x0AA4)
#define BBTX_CON10        (ABB_MDSYS_BASE+0x0AA8)
#define BBTX_CON11        (ABB_MDSYS_BASE+0x0AAC)
#define BBTX_CON12        (ABB_MDSYS_BASE+0x0AB0)
#define BBTX_CON13        (ABB_MDSYS_BASE+0x0AB4)
#define BBTX_CON14        (ABB_MDSYS_BASE+0x0AB8)
#define BBTX_CON15        (ABB_MDSYS_BASE+0x0ABC)
#define BBTX_CON16        (ABB_MDSYS_BASE+0x0AC0)

#define APC_CON0          (ABB_MDSYS_BASE+0x0C00)
#define APC_CON1          (ABB_MDSYS_BASE+0x0C04)
#define VBIAS_CON0        (ABB_MDSYS_BASE+0x0C40)
#define VBIAS_CON1        (ABB_MDSYS_BASE+0x0C44)
#define AFC_CON0          (ABB_MDSYS_BASE+0x0CC0)
#define AFC_CON1          (ABB_MDSYS_BASE+0x0CC4)
#define AFC_CON2          (ABB_MDSYS_BASE+0x0CC8)

#define BBTX_CON17        (ABB_MDSYS_BASE+0x0F00)
#define BBTX_CON18        (ABB_MDSYS_BASE+0x0F04)


/*****************************************************************************
*                        F U N C T I O N   D E F I N I T I O N
******************************************************************************
*/

void Afe_Set_Reg(kal_uint32 offset,kal_uint32 value,kal_uint32 mask);
kal_uint32 Afe_Get_Reg(kal_uint32 offset);

void   Ana_Set_Reg(kal_uint32 offset,kal_uint32 value,kal_uint32 mask);
kal_uint32 Ana_Get_Reg(kal_uint32 offset);

void Afe_Enable_Memory_Power(void);
void Afe_Disable_Memory_Power(void);

void Ana_Alloc_Backup_Memory(void);
void Ana_Free_Backup_Memory(void);
void Ana_Init_Backup_Memory(void);
kal_uint32 Ana_Check_Backup_Memory(kal_uint32 offset);
kal_uint32 Ana_Get_Backup_Memory(kal_uint32 u4Addr);
void Ana_Recover_AudioPlayback(void);
void Ana_Recover_AudioRecord(void);
#endif


