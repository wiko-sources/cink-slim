/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2009
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
#include <linux/ioctl.h>

#ifndef __JPEG_DRV_H__
#define __JPEG_DRV_H__

// JPEG Decoder Structure
typedef struct
{
    unsigned int srcStreamAddr;
    unsigned int srcStreamSize;
        
    unsigned int mcuRow;                  ///< number of MCU row in the JPEG file
    unsigned int mcuColumn;               ///< number of MCU column in the JPEG file

    unsigned int totalDU[3];              ///< (required by HW decoder) number of DU for each component
    unsigned int duPerMCURow[3];          ///< (required by HW decoder) DU per MCU row for each component
    unsigned int dummyDU[3];              ///< (required by HW decoder) number of dummy DU for each component

    unsigned int samplingFormat;
    
    // JPEG component information
    unsigned int  componentNum;
    unsigned int  componentID[3];          ///< Ci
    unsigned int  hSamplingFactor[3];      ///< Hi
    unsigned int  vSamplingFactor[3];      ///< Vi
    unsigned int  qTableSelector[3];       ///< Tqi

    unsigned int  table_addr;
}JPEG_DEC_DRV_IN;

typedef struct
{
    long timeout;
    unsigned int *result;
    
}JPEG_DEC_DRV_OUT;

// JPEG Encoder Structure
typedef struct
{
    
    unsigned char* dstBufferAddr;
    unsigned int dstBufferSize;
    
    unsigned int dstWidth;
    unsigned int dstHeight;
    
    unsigned char enableEXIF;
    unsigned char allocBuffer;
    unsigned char enableSyncReset;
    
    unsigned int dstQuality;
    unsigned int dstFormat; 

}JPEG_ENC_DRV_IN;

typedef struct
{
    long timeout;
    unsigned int *fileSize;  
    unsigned int *result;
    
}JPEG_ENC_DRV_OUT;

typedef struct {
    unsigned long startAddr;//In : 
    unsigned long size;
    unsigned long result;// 0 : out of pmem range, 1 : inside pmem range, 2 : partially overlap with pmem range
} JPEG_PMEM_RANGE;

#define JPEG_IOCTL_MAGIC        'x'

#define JPEG_DEC_IOCTL_INIT     _IO  (JPEG_IOCTL_MAGIC, 1)
#define JPEG_DEC_IOCTL_CONFIG   _IOW (JPEG_IOCTL_MAGIC, 2, JPEG_DEC_DRV_IN)
#define JPEG_DEC_IOCTL_START    _IO  (JPEG_IOCTL_MAGIC, 3)
#define JPEG_DEC_IOCTL_WAIT     _IOWR(JPEG_IOCTL_MAGIC, 6, JPEG_DEC_DRV_OUT) 
#define JPEG_DEC_IOCTL_DEINIT   _IO  (JPEG_IOCTL_MAGIC, 8)
//#define JPEG_DEC_IOCTL_RESUME   _IOW(JPEG_IOCTL_MAGIC, 4, JPEG_DEC_RESUME_IN)
//#define JPEG_DEC_IOCTL_RANGE    _IOWR(JPEG_IOCTL_MAGIC, 5, JPEG_DEC_RANGE_IN) 
//#define JPEG_DEC_IOCTL_COPY     _IOWR(JPEG_IOCTL_MAGIC, 7, int) 

#define JPEG_ENC_IOCTL_INIT     _IO  (JPEG_IOCTL_MAGIC, 11)
#define JPEG_ENC_IOCTL_CONFIG   _IOW (JPEG_IOCTL_MAGIC, 12, JPEG_ENC_DRV_IN)
#define JPEG_ENC_IOCTL_WAIT     _IOWR(JPEG_IOCTL_MAGIC, 13, JPEG_ENC_DRV_OUT)
#define JPEG_ENC_IOCTL_DEINIT   _IO  (JPEG_IOCTL_MAGIC, 14)
#define JPEG_ENC_IOCTL_START    _IO  (JPEG_IOCTL_MAGIC, 15)

#endif

