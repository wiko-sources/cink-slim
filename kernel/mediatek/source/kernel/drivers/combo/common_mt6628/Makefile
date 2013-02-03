# Copyright Statement:
#
# This software/firmware and related documentation ("MediaTek Software") are
# protected under relevant copyright laws. The information contained herein
# is confidential and proprietary to MediaTek Inc. and/or its licensors.
# Without the prior written permission of MediaTek inc. and/or its licensors,
# any reproduction, modification, use or disclosure of MediaTek Software,
# and information contained herein, in whole or in part, shall be strictly prohibited.
#
# MediaTek Inc. (C) 2010. All rights reserved.
#
# BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
# THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
# RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
# AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
# NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
# SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
# SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
# THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
# THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
# CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
# SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
# STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
# CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
# AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
# OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
# MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
#
# The following software/firmware and/or related documentation ("MediaTek Software")
# have been modified by MediaTek Inc. All revisions are subject to any receiver's
# applicable license agreements with MediaTek Inc.


#ccflags-y += -I$(src)/core/include
ccflags-y += -I$(src)/linux/include -I$(src)/linux/pri/include -I$(src)/core/include -I$(src)/include -I$(src)/

ifeq ($(MTK_COMBO_CHIP), MT6628)
EXT_FLAG=
else
EXT_FLAG=_mt6628
endif

COMMON_SRC_PATH := $(src)
COMMON_OBJ_PATH := $(src)

# WMT DRIVER
obj-$(CONFIG_MTK_COMBO) += mtk_stp_wmt$(EXT_FLAG).o
# WMT DRIVER-core part
mtk_stp_wmt$(EXT_FLAG)-objs	:= core/wmt_core.module core/wmt_ctrl.module  core/wmt_func.module core/wmt_ic_6620.module core/wmt_lib.module core/wmt_conf.module

#ifeq ($(MTK_COMBO_CHIP), MT6628)
mtk_stp_wmt$(EXT_FLAG)-objs	+= core/wmt_ic_6628.module
#endif
# WMT DRIVER-linux private part
mtk_stp_wmt$(EXT_FLAG)-objs	+= linux/pri/wmt_dev.module linux/pri/wmt_exp.module

# WMT DRIVER-OSAL
mtk_stp_wmt$(EXT_FLAG)-objs	+= linux/pub/osal.o
# WMT DRIVER-platform implementation
ccflags-y += -D WMT_PLAT_ALPS
mtk_stp_wmt$(EXT_FLAG)-objs	+= platform/alps/wmt_plat_alps.o

# FIXME: select desired hw according to project configuration
ifeq ($(MTK_COMBO_CHIP), MT6628)
mtk_stp_wmt$(EXT_FLAG)-objs	+= platform/alps/mtk_wcn_cmb_hw_6628.o 
else
mtk_stp_wmt$(EXT_FLAG)-objs	+= platform/alps/mtk_wcn_cmb_hw_6620.o
endif

mtk_stp_wmt$(EXT_FLAG)-objs	+= linux/pri/stp_exp.module core/stp_core.module core/psm_core.module core/btm_core.module linux/pri/stp_dbg.module
ifeq ($(MTK_COMBO_CHIP), MT6628)
# WMT stub part (built-in kernel image)
obj-y			+= platform/alps/mtk_wcn_cmb_stub_alps.o
endif

obj-$(CONFIG_MTK_COMBO) += mtk_stp_uart$(EXT_FLAG).o
mtk_stp_uart$(EXT_FLAG)-objs	:= linux/pri/stp_uart.module


obj-$(CONFIG_MTK_COMBO) += mtk_hif_sdio$(EXT_FLAG).o
mtk_hif_sdio$(EXT_FLAG)-objs	:= linux/pub/hif_sdio.o

obj-$(CONFIG_MTK_COMBO) += mtk_stp_sdio$(EXT_FLAG).o
mtk_stp_sdio$(EXT_FLAG)-objs	:= linux/pri/stp_sdio.module
mtk_stp_sdio$(EXT_FLAG)-objs	+= linux/pub/osal.o


obj-$(CONFIG_MTK_COMBO_BT) += mtk_stp_bt$(EXT_FLAG).o
mtk_stp_bt$(EXT_FLAG)-objs		:= linux/pub/stp_chrdev_bt.o

obj-$(CONFIG_MTK_COMBO_GPS) += mtk_stp_gps$(EXT_FLAG).o
mtk_stp_gps$(EXT_FLAG)-objs	:= linux/pub/stp_chrdev_gps.o


obj-$(CONFIG_MTK_COMBO_WIFI) += mtk_wmt_wifi$(EXT_FLAG).o
mtk_wmt_wifi$(EXT_FLAG)-objs       := linux/pub/wmt_chrdev_wifi.o 
