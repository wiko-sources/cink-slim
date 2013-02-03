/*******************************************************************************
 *
 * Filename:
 * ---------
 *   auddrv_afe_reg.c
 *
 * Project:
 * --------
 *   MT6573 Android
 *
 * Description:
 * ------------
 *   AudioSys AFE HW function
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
 *******************************************************************************/

/*****************************************************************************
*                     C O M P I L E R   F L A G S
******************************************************************************
*/

/*****************************************************************************
*                E X T E R N A L   R E F E R E N C E S
******************************************************************************
*/
#include <mach/mt6577_typedefs.h>
#include <linux/spinlock.h>

#include <linux/kernel.h>
#include <linux/io.h>
#include <asm/system.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/xlog.h>
#include <mach/mt6577_sc.h>

#include "mach/sync_write.h"
#include "auddrv_register.h"
#include "auddrv_def.h"

/*****************************************************************************
*                          C O N S T A N T S
******************************************************************************
*/
#define ANA_BACKUP_MEM_SIZE   (0x1000)
/*****************************************************************************
*                         D A T A   T Y P E S
******************************************************************************
*/
static kal_uint8 *p_gAnaBackReg = NULL;
/*****************************************************************************
*                     E X T E R N   F U N C T I O N
******************************************************************************
*/
extern void AudDrv_ANA_Clk_On(void);
extern void AudDrv_ANA_Clk_Off(void);

/*****************************************************************************
*                         LOCAL FUNCTION
******************************************************************************
*/
void Ana_Backup_Reg(kal_uint32 u4Addr, kal_uint32 u4Val);
void Ana_Resume_Reg(void);

#define ANA_INIT_REG(addr) Ana_Backup_Reg(addr,Ana_Get_Reg(addr))
/*****************************************************************************
*                        F U N C T I O N   D E F I N I T I O N
******************************************************************************
*/
void Afe_Set_Reg(kal_uint32 offset, kal_uint32 value , kal_uint32 mask)
{
   volatile kal_uint32 address = (AFE_BASE+offset);
   volatile kal_uint32 *AFE_Register = (volatile kal_uint32*)address;

   volatile kal_uint32 val_tmp;
   PRINTK_AFE_REG("Afe_Set_Reg offset=%x, value=%x, mask=%x \n",offset,value,mask);
   val_tmp = READ_REGISTER_UINT32(AFE_Register);
   val_tmp &= (~mask);
	/*
   mt65xx_reg_sync_writel(val_tmp,AFE_Register);
   val_tmp = READ_REGISTER_UINT32(AFE_Register);
   	*/
   val_tmp |= (value&mask);
   mt65xx_reg_sync_writel(val_tmp,AFE_Register);
/*
   *AFE_Register &= (~mask);
   dsb();
   *AFE_Register |= (value&mask);
   dsb();
*/
}

kal_uint32 Afe_Get_Reg(kal_uint32 offset)
{
   volatile kal_uint32 *value;
   volatile kal_uint32 address = (AFE_BASE+offset);
   value = (volatile kal_uint32 *)(address);

   PRINTK_AFE_REG("Afe_Get_Reg offset=%x, value=%d \n",offset,*value);
   return *value;
}

void Ana_Set_Reg(kal_uint32 offset, kal_uint32 value, kal_uint32 mask)
{
   volatile kal_uint32 address = (offset);
   volatile kal_uint32 *Analog_Register = (volatile kal_uint32 *)address;
   volatile kal_uint32 val_tmp;

   AudDrv_ANA_Clk_On();
   val_tmp = READ_REGISTER_UINT32(Analog_Register);
   val_tmp &= (~mask);
   val_tmp |= (value&mask);
   mt65xx_reg_sync_writel(val_tmp,Analog_Register);

   //Back Ana Reg
   Ana_Backup_Reg(address,val_tmp);
   if(Ana_Check_Backup_Memory(offset)==0){
       xlog_printk(ANDROID_LOG_INFO, "Sound","Fail to backup Ana Register @Offset=0x%x\n",offset);
   }
   AudDrv_ANA_Clk_Off();
/*
   *Analog_Register &= (~mask);
   dsb();
   *Analog_Register |= (value&mask);
   dsb();
*/
}

kal_uint32 Ana_Get_Reg(kal_uint32 offset)
{
   volatile kal_uint32 *value , ret =0;
   UINT32 address = (offset);
   AudDrv_ANA_Clk_On();
   value = (volatile kal_uint32  *)(address);
   ret = *value;
   AudDrv_ANA_Clk_Off();
   return ret;
}

// TBD...
void Afe_Enable_Memory_Power()
{
   // No need to enable memory power
   return;
}

// TBD...
void Afe_Disable_Memory_Power()
{
   // No need to disable memory power
   return;
}

void Ana_Alloc_Backup_Memory(void)
{
    if (p_gAnaBackReg == NULL)
    {
        p_gAnaBackReg = (kal_uint8 *)kmalloc(ANA_BACKUP_MEM_SIZE,GFP_KERNEL);
    }
    else{
        xlog_printk(ANDROID_LOG_INFO, "Sound","Ana_Alloc_Backup_Memory failed!\n");
    }
}

void Ana_Free_Backup_Memory(void)
{
    if (p_gAnaBackReg != NULL)
    {
        kfree((void*)p_gAnaBackReg);
        p_gAnaBackReg = NULL;
    }
    else{
        xlog_printk(ANDROID_LOG_INFO, "Sound","Ana_Free_Backup_Memory failed!\n");
    }
}

void Ana_Init_Backup_Memory(void)
{
    kal_uint32 u4addr = 0;
    if (p_gAnaBackReg != NULL)
    {
        memset(p_gAnaBackReg,0,ANA_BACKUP_MEM_SIZE);
        for (u4addr=WR_PATH0;u4addr<=BBTX_CON18;u4addr+=4){
            ANA_INIT_REG(u4addr);
        }
    }
    else{
        xlog_printk(ANDROID_LOG_INFO, "Sound","init_ANA_Backup_Memory failed!\n");
    }
}

void Ana_Backup_Reg(kal_uint32 u4Addr/*Kernel VA*/, kal_uint32 u4Val)
{
    volatile kal_uint32 *value;
    kal_uint32 offset = 0;
    PRINTK_ANA_REG("Ana_Backup_Reg Addr=%x Val=%x\n",u4Addr,u4Val);
    if (u4Addr >= ABB_MDSYS_BASE){
        offset =  u4Addr - ABB_MDSYS_BASE;
        value = (volatile kal_uint32  *)(p_gAnaBackReg+offset);
        *value = u4Val;
    }else{
        xlog_printk(ANDROID_LOG_INFO, "Sound","Ana_Backup_Reg failed!\n");
    }
}

kal_uint32 Ana_Get_Backup_Memory(kal_uint32 u4Addr)
{
   volatile kal_uint32 *value;
   kal_uint32 offset = 0;
   PRINTK_ANA_REG("Ana_Get_Backup_Memory Addr=%x \n",u4Addr);
   if (u4Addr >= ABB_MDSYS_BASE){
       offset =  u4Addr - ABB_MDSYS_BASE;
       value = (volatile kal_uint32  *)(p_gAnaBackReg+offset);
       return *value;
   }else{
       xlog_printk(ANDROID_LOG_INFO, "Sound","Ana_Backup_Reg failed!\n");
       return (0xc5c5c5c5);
   }
}


kal_uint32 Ana_Check_Backup_Memory(kal_uint32 offset)
{
    kal_uint32 val_1=0, val_2=0, ret_value = 0;
    val_1 = Ana_Get_Reg(offset);
    val_2 = Ana_Get_Backup_Memory(offset);

    PRINTK_ANA_REG("Ana_Check_Backup_Memory ANA=0x%x, BACKUP=0x%x \n", val_1, val_2);

    if(val_1 == val_2)
        ret_value = 1;
    else
        ret_value = 0;

    return (ret_value);
}
void Ana_Recover_Reg(kal_uint32 offset)
{
    kal_uint32 value = 0;
    value = Ana_Get_Backup_Memory(offset);
    Ana_Set_Reg(offset, value, 0xffffffff);
}
void Ana_Recover_AudioPlayback(void)
{
    kal_uint32 u4AUD_CON9 = 0;
    PRINTK_ANA_REG("Ana_Recover_AudioPlayback\n");
    u4AUD_CON9 = Ana_Get_Backup_Memory(AUDIO_CON9);
    Ana_Recover_Reg(AUDIO_NCP0);
    Ana_Recover_Reg(AUDIO_NCP1);
    Ana_Recover_Reg(AUDIO_LDO0);
    Ana_Recover_Reg(AUDIO_LDO1);
    Ana_Recover_Reg(AUDIO_LDO2);
    Ana_Recover_Reg(AUDIO_GLB0);
    Ana_Recover_Reg(AUDIO_GLB1);
    Ana_Recover_Reg(AUDIO_REG1);

    Ana_Recover_Reg(AUDIO_CON0);//set line-in gain and select ZCD source
    Ana_Recover_Reg(AUDIO_CON1);
    Ana_Recover_Reg(AUDIO_CON2);
    Ana_Recover_Reg(AUDIO_CON3);
    Ana_Recover_Reg(AUDIO_CON5);
    Ana_Recover_Reg(AUDIO_CON6);
    Ana_Recover_Reg(AUDIO_CON7);
    Ana_Recover_Reg(AUDIO_CON8);
    if(u4AUD_CON9&0x01) //Follow audio fifo control flow
    {
        Ana_Set_Reg(AUDIO_CON9, u4AUD_CON9, 0xfffffffe);
        Ana_Set_Reg(AUDIO_CON9, u4AUD_CON9, 0x00000001);
    }
    else
    {
        Ana_Recover_Reg(AUDIO_CON9);
    }
    Ana_Recover_Reg(AUDIO_CON10);
    Ana_Recover_Reg(AUDIO_CON14);
    Ana_Recover_Reg(AUDIO_CON17);
}

void Ana_Recover_AudioRecord(void)
{
    PRINTK_ANA_REG("Ana_Recover_AudioRecord\n");
    Ana_Recover_Reg(AUDIO_NCP0);
    Ana_Recover_Reg(AUDIO_NCP1);
    Ana_Recover_Reg(AUDIO_LDO0);
    Ana_Recover_Reg(AUDIO_LDO1);
    Ana_Recover_Reg(AUDIO_LDO2);
    Ana_Recover_Reg(AUDIO_GLB0);
    Ana_Recover_Reg(AUDIO_GLB1);
    Ana_Recover_Reg(AUDIO_REG1);

    Ana_Recover_Reg(AUDIO_CON3);
    Ana_Recover_Reg(AUDIO_CON4);
    Ana_Recover_Reg(AUDIO_CON5);
    Ana_Recover_Reg(AUDIO_CON6);
    Ana_Recover_Reg(AUDIO_CON14);
    Ana_Recover_Reg(AUDIO_CON17);
    Ana_Recover_Reg(AUDIO_CON20);
    Ana_Recover_Reg(AUDIO_CON21);
    Ana_Recover_Reg(AUDIO_CON22);
    Ana_Recover_Reg(AUDIO_CON23);
    Ana_Recover_Reg(AUDIO_CON24);
    Ana_Recover_Reg(AUDIO_CON31);
    Ana_Recover_Reg(AUDIO_CON33);
    Ana_Recover_Reg(AUDIO_CON34);
}
