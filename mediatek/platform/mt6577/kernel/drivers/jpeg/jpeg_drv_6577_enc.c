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

#include "jpeg_drv_6577_reg.h"


kal_uint32 _jpeg_enc_int_status = 0;


int jpeg_isr_enc_lisr(void)
{
    _jpeg_enc_int_status = REG_JPEG_ENC_INTERRUPT_STATUS;

    /// clear the interrupt status register
    if(_jpeg_enc_int_status)
    {
        REG_JPEG_ENC_INTERRUPT_STATUS = 0;
        return 0;
    }

    return -1;
}

void jpeg_drv_enc_start(void)
{
//   REG_JPEG_ENC_CTRL &= ~(JPEG_ENC_CTRL_CONT_SHOOT_BIT | JPEG_ENC_CTRL_CONT_SHOOT_ADDR_SW_BIT);
   REG_JPEG_ENC_CTRL |= (JPEG_ENC_CTRL_IN_EN_BIT | JPEG_ENC_CTRL_ENABLE_BIT);
}

/**
 * Call this function to reset the JPEG encoder.
 */
void jpeg_drv_enc_reset(void)
{
   REG_JPEG_ENC_RST = 0;
   REG_JPEG_ENC_RST = 1;

   _jpeg_enc_int_status = 0;
}


/**
 * Call this function to set the destination address.
 */
void jpeg_drv_enc_set_dst_buffer_info(kal_uint32 addr, kal_uint32 stallOffset, kal_uint32 dstIndex)
{
    if(dstIndex == 0)
    {
        REG_JPEG_ENC_DEST_ADDR1  = (kal_uint32)addr & 0xFFFFFFFC;
        REG_JPEG_ENC_BYTE_OFFSET_MASK |= ((1 << (addr & 0x3)) >> 1);
        REG_JPEG_ENC_STALL_ADDR1 = ((kal_uint32)addr + stallOffset) & 0xFFFFFFFC;
        
    }
    else
    {
        REG_JPEG_ENC_DEST_ADDR2  = (kal_uint32)addr & 0xFFFFFFFC;
        REG_JPEG_ENC_BYTE_OFFSET_MASK |= ((1 << (addr & 0x3)) >> 1) << 16;
        REG_JPEG_ENC_STALL_ADDR2 = ((kal_uint32)addr + stallOffset) & 0xFFFFFFFC;
    }
}

/**
 * Call this function to set the file format of JPEG encoder.
 *
 * @remarks The file format must be either zero (JPEG) or non-zero (JFIF/EXIF)
 */
void jpeg_drv_enc_set_file_format(kal_uint32 fileFormat)
{
   REG_JPEG_ENC_CTRL &= ~(JPEG_ENC_CTRL_FILE_FORMAT_BIT);
   
   if (fileFormat)
   {
      REG_JPEG_ENC_CTRL |= JPEG_DEV_ENC_FILE_FORMAT_JFIF_EXIF;
   }
}

void jpeg_drv_enc_set_sync_reset(unsigned char enable_bit)
{
   REG_JPEG_ENC_CTRL &= ~(JPEG_ENC_CTRL_INIT_EN_BIT);
   
   if (enable_bit == 1)
   {
      REG_JPEG_ENC_CTRL |= JPEG_ENC_CTRL_INIT_EN_BIT;
   }
}

void jpeg_drv_enc_set_mode(kal_uint32 mode, kal_uint32 frames)
{
    switch (mode)
    {
        case 0:
            REG_JPEG_ENC_FRAME_NUMBER = 0;
            REG_JPEG_ENC_CTRL &= ~JPEG_ENC_CTRL_CONT_SHOOT_BIT;            
            break;
        case 1:
            REG_JPEG_ENC_FRAME_NUMBER = frames;
            REG_JPEG_ENC_CTRL |= JPEG_ENC_CTRL_CONT_SHOOT_BIT;
            REG_JPEG_ENC_CTRL &= ~JPEG_ENC_CTRL_CONT_SHOOT_ADDR_SW_BIT;
            break;
        case 2:
            REG_JPEG_ENC_FRAME_NUMBER = frames;
            REG_JPEG_ENC_CTRL |= JPEG_ENC_CTRL_CONT_SHOOT_BIT;
            REG_JPEG_ENC_CTRL |= JPEG_ENC_CTRL_CONT_SHOOT_ADDR_SW_BIT;
            break;
    }
}

/**
 * Call this function to set the quality of JPEG encoder.
 *
 * @remarks The quality factor ranges from 0 (best) to 5 (worst)
 */
void jpeg_drv_enc_set_quality(kal_uint32 quality)
{
    REG_JPEG_ENC_QUALITY = quality;
}


/**
 * Call this function to set the properties related to sampling format.
 *
 * @remarks the format must be one of the integer values 422, 420, 411 and 400 (for graylevel).
 * The properties set in this function are:
 *    REG_JPEG_ENC_BLK_NUM
 *    REG_JPEG_CTRL[1:1]
 *    REG_JPEG_CTRL[3:4]
 */
kal_uint32 jpeg_drv_enc_set_sample_format_related(kal_uint32 width, kal_uint32 height, kal_uint32 samplingFormat)
{   
   REG_JPEG_ENC_CTRL &= ~(JPEG_ENC_CTRL_YUV_BIT | JPEG_ENC_CTRL_GRAY_BIT);

   switch (samplingFormat)
   {
   case 422:
      REG_JPEG_ENC_CTRL |= JPEG_DRV_ENC_YUV422;
      REG_JPEG_ENC_BLK_NUM = ((width + 15) >> 4) * ((height + 7) >> 3) * 4 - 1;
      break;

   case 420:
      REG_JPEG_ENC_CTRL |= JPEG_DRV_ENC_YUV420;
      REG_JPEG_ENC_BLK_NUM = ((width + 15) >> 4) * ((height + 15) >> 4) * 6 - 1;
      break;
      
   case 411:
      REG_JPEG_ENC_CTRL |= JPEG_DRV_ENC_YUV411;
      REG_JPEG_ENC_BLK_NUM = ((width + 31) >> 5) * ((height + 7) >> 3) * 6 - 1;
      break;

   case 400:
      REG_JPEG_ENC_CTRL |= JPEG_DRV_ENC_YUV400;
      REG_JPEG_ENC_BLK_NUM = ((width + 7) >> 3) * ((height + 7) >> 3) - 1;
      break;

   default:
      return 1;
   }

   REG_JPEG_ENC_IMG_SIZE = (width << 16) + (height);
   
   return 0;
}


kal_uint32 jpeg_drv_enc_query_required_memory(kal_uint32 *intMemory, kal_uint32 *extMemory)
{
   /// JPEG HW encoder need no memory.

   *intMemory = 0;
   *extMemory = 0;
   
   return 1;
}


kal_uint32 jpeg_drv_enc_get_dma_addr(kal_uint8 index)
{
    if(index == 0)
        return REG_JPEG_ENC_DMA_ADDR1;

    return REG_JPEG_ENC_DMA_ADDR2;
}

kal_uint32 jpeg_drv_enc_get_dst_addr(kal_uint8 index)
{
    if(index == 0)
        return REG_JPEG_ENC_DEST_ADDR1;

    return REG_JPEG_ENC_DEST_ADDR2;
}

kal_uint32 jpeg_drv_enc_get_file_size(kal_uint8 index)
{
    return jpeg_drv_enc_get_dma_addr(index) - jpeg_drv_enc_get_dst_addr(index);
}

kal_uint32 jpeg_drv_enc_get_result(kal_uint32 *fileSize)
{
    *fileSize = jpeg_drv_enc_get_file_size(0);

    if(_jpeg_enc_int_status & JPEG_DRV_ENC_INT_STATUS_DONE)
    {
        return 0;
    }
    else if(_jpeg_enc_int_status & JPEG_DRV_ENC_INT_STATUS_STALL)
    {
        return 1;
    }

    return 2;
}

kal_uint8 jpeg_drv_enc_get_current_frame(void)
{
    return REG_JPEG_ENC_CURR_FRAME_CNT & 1;
}

//#endif /// #if defined(__DRV_GRAPHICS_JPEG_3351_SERIES__) && defined(__MTK_TARGET__)
