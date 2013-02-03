/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */


#ifndef _STP_DEBUG_H_
#define _STP_DEBUG_H_

#include <linux/time.h>

#define CONFIG_LOG_STP_INTERNAL

#if 1//#ifndef CONFIG_LOG_STP_INTERNAL
#define STP_PKT_SZ  16
#define STP_DMP_SZ 2048
#define STP_PKT_NO 2048

#define STP_DBG_LOG_ENTRY_NUM 2048
#define STP_DBG_LOG_ENTRY_SZ  2048

#else

#define STP_PKT_SZ  16
#define STP_DMP_SZ 16
#define STP_PKT_NO 16

#define STP_DBG_LOG_ENTRY_NUM 28
#define STP_DBG_LOG_ENTRY_SZ 64


#endif


typedef enum {
    STP_DBG_EN         = 0,
    STP_DBG_PKT        = 1,
    STP_DBG_DR         = 2,
    STP_DBG_FW_ASSERT  = 3,
    STP_DBG_FW_LOG = 4,
    STP_DBG_FW_DMP = 5, 
    STP_DBG_MAX
}STP_DBG_OP_T;

typedef enum {
    STP_DBG_PKT_FIL_ALL = 0,
    STP_DBG_PKT_FIL_BT  = 1,
    STP_DBG_PKT_FIL_GPS = 2,
    STP_DBG_PKT_FIL_FM  = 3,
    STP_DBG_PKT_FIL_WMT = 4,
    STP_DBG_PKT_FIL_MAX
} STP_DBG_PKT_FIL_T;

static  char * const gStpDbgType[]={
    "< BT>",
    "< FM>",
    "<GPS>",
    "<WiFi>",
    "<WMT>",
    "<STP>",
    "<DBG>",
    "<UNKOWN>"
};


typedef enum {
    STP_DBG_DR_MAX = 0,
} STP_DBG_DR_FIL_T;

typedef enum {
    STP_DBG_FW_MAX = 0,
} STP_DBG_FW_FIL_T;

typedef enum {
    PKT_DIR_RX = 0,
    PKT_DIR_TX
} STP_DBG_PKT_DIR_T;

/*simple log system ++*/

typedef struct {
    int  id; /*type: 0. pkt trace 1. fw info 2. assert info 3. trace32 dump . -1. linked to the the previous*/
    int  len;
    char buffer[STP_DBG_LOG_ENTRY_SZ];
} MTKSTP_LOG_ENTRY_T;

typedef struct log_sys {
    MTKSTP_LOG_ENTRY_T  queue[STP_DBG_LOG_ENTRY_NUM];
    unsigned int size;
    unsigned int in;
    unsigned int out;
    spinlock_t   lock;
} MTKSTP_LOG_SYS_T;
/*--*/

typedef struct stp_dbg_pkt_hdr{
    //packet information
    unsigned int   sec;
    unsigned int   usec;
    unsigned int   dbg_type;
    unsigned int   dmy;
    unsigned int   no;
    unsigned int   dir;

    //packet content
    unsigned int  type;
    unsigned int  len;
    unsigned int  ack;
    unsigned int  seq;
    unsigned int  chs;
    unsigned int  crc;
}STP_DBG_HDR_T;

typedef struct stp_dbg_pkt{
    struct stp_dbg_pkt_hdr hdr;
    unsigned char raw[STP_DMP_SZ];
}STP_PACKET_T;

typedef struct mtkstp_dbg_t{
    /*log_sys*/
    int pkt_trace_no;
    void *btm;
    int is_enable;
    MTKSTP_LOG_SYS_T *logsys;
}MTKSTP_DBG_T;

extern int stp_dbg_enable(MTKSTP_DBG_T *stp_dbg);
extern int stp_dbg_disable(MTKSTP_DBG_T *stp_dbg);
extern MTKSTP_DBG_T *stp_dbg_init(void *);
extern int stp_dbg_deinit(MTKSTP_DBG_T *stp_dbg);
extern int stp_dbg_dmp_out_ex (char *buf, int *len);
extern int stp_dbg_dmp_out(MTKSTP_DBG_T *stp_dbg, char *buf, int *len);
extern int stp_dbg_dmp_printk(MTKSTP_DBG_T *stp_dbg);
extern char
stp_dbg_nl_send(
    char *  aucMsg,
    unsigned char      cmd
    );

extern int
stp_dbg_log_pkt (
    MTKSTP_DBG_T *stp_dbg,
    int dbg_type,
    int type,
    int ack_no,
    int seq_no,
    int crc,
    int dir,
    int len,
    const unsigned char *body);
extern int stp_dbg_log_ctrl (unsigned int on);
#endif /* end of _STP_DEBUG_H_ */

