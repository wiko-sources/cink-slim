/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2007
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE. 
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#include <mach/mt6577_typedefs.h>
#include <mach/sync_write.h>

#include "jpeg_drv_6577_reg.h"

kal_uint32 _jpeg_dec_int_status = 0;

int jpeg_isr_dec_lisr(void)
{
    
    _jpeg_dec_int_status = REG_JPEG_DEC_INTERRUPT_STATUS;

   if (_jpeg_dec_int_status)
   {
      /// clear the interrupt status register
      REG_JPEG_DEC_INTERRUPT_STATUS = 0;
      
      //REG_JPEG_DEC_INTERRUPT_STATUS = 0x80000000;
      REG_JPEG_DEC_INTERRUPT_STATUS = 0xF;

      return 0;
   }

   return -1;
}

void jpeg_drv_dec_start(void)
{
   //REG_JPEG_DEC_TRIG = 1;
   mt65xx_reg_sync_writel(0x1, JPEG_CODEC_base + 0x40);
}


/**
 * Call this function to reset the JPEG decoder.
 */
void jpeg_drv_dec_reset(void)
{

   REG_JPEG_DEC_RST = 0;
   REG_JPEG_DEC_RST = 0x10001;

   REG_JPEG_DEC_INTERRUPT_STATUS = 0x0;
   REG_JPEG_DEC_INTERRUPT_STATUS = 0x7;

}



void jpeg_drv_dec_set_file_size(kal_uint32 size)
{
   /// Must add 64 bytes pre-fetch count and 3 for 32-bytes alignment
   REG_JPEG_DEC_FILE_TOTAL_SIZE = (size + 64 + 31) & 0xFFFFFFE0;

}



void jpeg_drv_dec_set_file_buffer(kal_uint32 buffer, kal_uint32 size)
{
/*
    if(size % 16 != 0)
    {
        size += (16 - (size % 16));
        size += 16;
    }   
*/
    REG_JEPG_DEC_FILE_BRP = ((buffer + size + 64 + 3) & 0xFFFFFFFC);
    REG_JPEG_DEC_FILE_ADDR = buffer & 0xFFFFFFFC;
    REG_JPEG_DEC_BYTE_OFFSET_MASK = (1 << (buffer & 0x3)) >> 1;
}



void jpeg_drv_dec_set_table_address(kal_uint32 tableAddress)
{
   /// table buffer must be aligned at 2K address
   ASSERT(0 == (tableAddress & 0x7FF));
   REG_JPEG_DEC_TBLS_START_ADDR = tableAddress;
}



void jpeg_drv_dec_set_du(kal_uint32 comp, kal_uint32 du, kal_uint32 dummy_du, kal_uint32 du_per_mcu_row)
{
   REG_JPEG_DEC_DATA_UNIT(comp) = du - 1;
   REG_JPEG_DEC_NONINTLV_DU_NUM_PER_MCU_ROW(comp) = (dummy_du << 12) + du_per_mcu_row;
}



void jpeg_drv_dec_set_total_mcu(kal_uint32 total_mcu)
{
   REG_JPEG_DEC_TOTAL_MCU_NUM = total_mcu - 1;
}



void jpeg_drv_dec_set_mcu_per_row(kal_uint32 mcu_per_row)
{
   REG_JPEG_DEC_INTLV_MCU_NUM_PER_MCU_ROW = mcu_per_row;
}



void jpeg_drv_dec_set_component_id(kal_uint32 id0, kal_uint32 id1, kal_uint32 id2)
{
   REG_JPEG_DEC_COMP_ID = (id0 << 24) | (id1 << 16) | (id2 << 8);
}



void jpeg_drv_dec_set_q_table_id(kal_uint32 id0, kal_uint32 id1, kal_uint32 id2)
{
   REG_JPEG_DEC_QT_ID = (id0 << 8) | (id1 << 4) | (id2 << 0);
}



void jpeg_drv_dec_set_progressive(kal_uint32 progressive)
{
   REG_JPEG_DEC_CTRL &= 0xBFFFFFFF;
   
   if (progressive)
   {
      REG_JPEG_DEC_CTRL |= 0x40000000;
   }
}

kal_uint32 jpeg_drv_dec_get_result(void)
{
    if(_jpeg_dec_int_status & JPEG_DRV_DEC_INT_STATUS_EOF)
    {
        return 0;
    }
    else if(_jpeg_dec_int_status & JPEG_DRV_DEC_INT_STATUS_BREAKPOINT)
    {
        return 1;
    }
    else if(_jpeg_dec_int_status & JPEG_DRV_DEC_INT_STATUS_FILE_OVERFLOW)
    {
        return 2;
    }
    else if(_jpeg_dec_int_status & JPEG_DRV_DEC_INT_STATUS_RANGE_DECODE)
    {
        return 0;
    }

    return 3;
}

kal_uint32 jpeg_drv_dec_set_sampling_factor_related(kal_uint32 format)
{
   /// reserve the progressive bit
   REG_JPEG_DEC_CTRL &= 0x40000000;

   switch (format)
   {
   case 400:
      REG_JPEG_DEC_SAMP_FACTOR = 0x00AA;
      REG_JPEG_DEC_CTRL |= 0x80000000;
      REG_JPEG_DEC_CTRL |= ((0x04 <<  0) | (0x07 <<  3) | (0x07 <<  6) | (0x07 <<  9) | (0x07 << 12) |
                            (0x07 << 15) | (0x07 << 18) | (0x07 << 21) | (0x07 << 24) | (0x07 << 27));
      break;
      
   case 411:
      /// YUV 4:1:1 (Hy=4, Vy=1), (Hu=1, Vu=1), (Hv=1, Vv=1)
      REG_JPEG_DEC_SAMP_FACTOR = 0x0C00;
      REG_JPEG_DEC_CTRL |= ((0x04 <<  0) | (0x04 <<  3) | (0x04 <<  6) | (0x04 <<  9) | (0x05 << 12) |
                            (0x06 << 15) | (0x07 << 18) | (0x07 << 21) | (0x07 << 24) | (0x07 << 27));
      break;

   case 420:
      /// YUV 4:2:0 (Hy=2, Vy=2), (Hu=1, Vu=1), (Hv=1, Vv=1)
      REG_JPEG_DEC_SAMP_FACTOR = 0x0500;
      REG_JPEG_DEC_CTRL |= ((0x04 <<  0) | (0x04 <<  3) | (0x04 <<  6) | (0x04 <<  9) | (0x05 << 12) |
                            (0x06 << 15) | (0x07 << 18) | (0x07 << 21) | (0x07 << 24) | (0x07 << 27));
      break;
      
   case 421 :// 1x2
      /// YUV 4:2:2 (Hy=1, Vy=2), (Hu=1, Vu=1), (Hv=1, Vv=1)
      REG_JPEG_DEC_SAMP_FACTOR = 0x0100;
      REG_JPEG_DEC_CTRL |= ((0x04 <<  0) | (0x04 <<  3) | (0x05 <<  6) | (0x06 <<  9) | (0x07 << 12) |
                            (0x07 << 15) | (0x07 << 18) | (0x07 << 21) | (0x07 << 24) | (0x07 << 27));
      break;
      
   case 422:
      /// YUV 4:2:2 (Hy=2, Vy=1), (Hu=1, Vu=1), (Hv=1, Vv=1)
      REG_JPEG_DEC_SAMP_FACTOR = 0x0400;
      REG_JPEG_DEC_CTRL |= ((0x04 <<  0) | (0x04 <<  3) | (0x05 <<  6) | (0x06 <<  9) | (0x07 << 12) |
                            (0x07 << 15) | (0x07 << 18) | (0x07 << 21) | (0x07 << 24) | (0x07 << 27));
      break;

   case 444:
      /// YUV 4:4:4 (Hy=1, Vy=1), (Hu=1, Vu=1), (Hv=1, Vv=1)
      REG_JPEG_DEC_SAMP_FACTOR = 0x0000;
      REG_JPEG_DEC_CTRL |= ((0x04 <<  0) | (0x05 <<  3) | (0x06 <<  6) | (0x07 <<  9) | (0x07 << 12) |
                            (0x07 << 15) | (0x07 << 18) | (0x07 << 21) | (0x07 << 24) | (0x07 << 27));
      break;

   default:
      //ASSERT(0);
      return 0;
      
   }
   return 1;
}



void jpeg_drv_dec_set_prog_buffer(kal_uint32 *addr[])
{
   REG_JPEG_DEC_COMP0_PROGR_COEFF_START_ADDR = (kal_uint32)addr[0];
   REG_JPEG_DEC_COMP1_PROGR_COEFF_START_ADDR = (kal_uint32)addr[1];
   REG_JPEG_DEC_COMP2_PROGR_COEFF_START_ADDR = (kal_uint32)addr[2];
}



void jpeg_drv_dec_calc_prog_buffer(kal_uint32 format, kal_uint32 *poolAddr, kal_uint32 poolSize,
                                   kal_uint32 *addr[], kal_uint32 size[])
{
   kal_uint32 share;

   switch (format)
   {
   case 400:
      addr[0] = poolAddr;
      addr[1] = poolAddr;
      addr[2] = poolAddr;

      size[0] = poolSize;
      size[1] = poolSize;
      size[2] = poolSize;
      break;

   case 411:
      share = (poolSize / 6) / 4;
      addr[0] = poolAddr;
      addr[1] = poolAddr + (4 * share);
      addr[2] = poolAddr + (5 * share);

      share *= 4;
      size[0] = (4 * share);
      size[1] = share;
      size[2] = share;
      break;
   
   case 420:
      share = (poolSize / 6) / 4;
      addr[0] = poolAddr;
      addr[1] = poolAddr + (4 * share);
      addr[2] = poolAddr + (5 * share);

      share *= 4;
      size[0] = (4 * share);
      size[1] = share;
      size[2] = share;
      break;
   
   case 422:
      share = (poolSize / 4) / 4;
      addr[0] = poolAddr;
      addr[1] = poolAddr + (2 * share);
      addr[2] = poolAddr + (3 * share);

      share *= 4;
      size[0] = (2 * share);
      size[1] = share;
      size[2] = share;
      break;
   
   case 444:
      share = (poolSize / 3) / 4;
      addr[0] = poolAddr;
      addr[1] = poolAddr + share;
      addr[2] = poolAddr + (2 * share);

      share *= 4;
      size[0] = share;
      size[1] = share;
      size[2] = share;
      break;

   default:
      ASSERT(0);
   }
}



void jpeg_drv_dec_query_ext_prog_buffer_size(kal_uint32 *extMemorySize)
{
   *extMemorySize = JPEG_DRV_DEC_PROG_COEF_BUFFER_MAX;
}



void jpeg_drv_dec_query_ext_decode_table_size(kal_uint32 *extMemorySize)
{
   *extMemorySize = (JPEG_DRV_DEC_TABLE_ALIGNED_SIZE + 3) & 0xFFFFFFFC;    /// for decoding table
}



void jpeg_drv_dec_set_prog_intlv_mcu_start_end(kal_uint32 start, kal_uint32 end)
{
   REG_JEPG_DEC_INTLV_FIRST_MCU_INDEX = start;
   REG_JEPG_DEC_INTLV_LAST_MCU_INDEX = end;
}



void jpeg_drv_dec_set_prog_non_intlv_du_start_end(kal_uint32 comp, kal_uint32 start, kal_uint32 end)
{
   REG_JEPG_DEC_FIRST_DU_INDEX(comp) = start;
   REG_JEPG_DEC_LAST_DU_INDEX(comp) = end;
}



kal_uint8 *jpeg_drv_dec_get_current_file_addr(void)
{
   return (kal_uint8 *)REG_JPEG_DEC_FILE_ADDR;
}



kal_uint32 *jpeg_drv_dec_get_aligned_table_address(kal_uint8 *initAddress)
{
   return (kal_uint32 *)(((kal_uint32)initAddress + ~JPEG_DRV_DEC_TABLE_ALIGNED_ADDR) & JPEG_DRV_DEC_TABLE_ALIGNED_ADDR);
}

void jpeg_drv_dec_range_enable(void)
{
    REG_JPEG_DEC_RANG_DEC_EN = 1;
}

void jpeg_drv_dec_range_disable(void)
{
    REG_JPEG_DEC_RANG_DEC_EN = 0;
}

void jpeg_drv_dec_set_idct_index(kal_uint32 start, kal_uint32 end)
{
    REG_JPEG_DEC_START_IDCT_MCU_ROW_INDEX = start;
    REG_JPEG_DEC_END_IDCT_MCU_ROW_INDEX  = end;
}

void jpeg_drv_dec_set_idct_skip_index(kal_uint32 index1, kal_uint32 index2)
{
    REG_JPEG_DEC_IDCT_SKIP_MCU_INDEX1 = index1;
    REG_JPEG_DEC_IDCT_SKIP_MCU_INDEX2 = index2;
}

void jpeg_drv_dec_set_idec_num(kal_uint32 num)
{
    REG_JPEG_DEC_IDCT_MCU_NUM_PER_ROW = num;
}

