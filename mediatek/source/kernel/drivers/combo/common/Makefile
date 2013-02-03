#ccflags-y += -I$(src)/core/include
ccflags-y += -I$(src)/linux/include -I$(src)/linux/pri/include -I$(src)/core/include -I$(src)/include -I$(src)/ -I$(MTK_PATH_PLATFORM)/core/include/

ifeq ($(MTK_COMBO_CHIP), MT6628)
EXT_FLAG =_mt6620
else
EXT_FLAG=
endif

COMMON_SRC_PATH := $(src)
COMMON_OBJ_PATH := $(src)

# WMT DRIVER
obj-$(CONFIG_MTK_COMBO) += mtk_stp_wmt$(EXT_FLAG).o
# WMT DRIVER-core part
mtk_stp_wmt$(EXT_FLAG)-objs	:= core/wmt_core.module core/wmt_ctrl.module  core/wmt_func.module core/wmt_ic_6620.module core/wmt_lib.module core/wmt_conf.module

#ifeq ($(MTK_COMBO_CHIP), MT6628)
mtk_stp_wmt-objs	+= core/wmt_ic_6628.module

#endif
# WMT DRIVER-linux private part
mtk_stp_wmt$(EXT_FLAG)-objs	+= linux/pri/wmt_dev.module linux/pri/wmt_exp.module
mtk_stp_wmt$(EXT_FLAG)-objs	+= linux/pri/wmt_tm.module

# WMT DRIVER-OSAL
mtk_stp_wmt$(EXT_FLAG)-objs	+= linux/osal.o
# WMT DRIVER-platform implementation
ccflags-y += -D WMT_PLAT_ALPS
mtk_stp_wmt$(EXT_FLAG)-objs	+= platform/alps/wmt_plat_alps.o
# FIXME: select desired hw according to project configuration
mtk_stp_wmt$(EXT_FLAG)-objs	+= platform/alps/mtk_wcn_cmb_hw_6620.o 
#mtk_stp_wmt$(EXT_FLAG)-objs	+= platform/alps/mtk_wcn_cmb_hw_6628.o 

mtk_stp_wmt$(EXT_FLAG)-objs	+= linux/pri/stp_exp.module core/stp_core.module core/psm_core.module core/btm_core.module linux/pri/stp_dbg.module
ifneq ($(MTK_COMBO_CHIP), MT6628)


# WMT stub part (built-in kernel image)
obj-y			+= platform/alps/mtk_wcn_cmb_stub_alps.o
endif

obj-$(CONFIG_MTK_COMBO) += mtk_stp_uart$(EXT_FLAG).o
mtk_stp_uart$(EXT_FLAG)-objs	:= linux/pri/stp_uart.module


obj-$(CONFIG_MTK_COMBO) += mtk_hif_sdio$(EXT_FLAG).o
mtk_hif_sdio$(EXT_FLAG)-objs	:= linux/hif_sdio.o

obj-$(CONFIG_MTK_COMBO) += mtk_stp_sdio$(EXT_FLAG).o
mtk_stp_sdio$(EXT_FLAG)-objs	:= linux/pri/stp_sdio.module
mtk_stp_sdio$(EXT_FLAG)-objs	+= linux/osal.o

obj-$(CONFIG_MTK_COMBO_BT) += mtk_stp_bt$(EXT_FLAG).o
mtk_stp_bt$(EXT_FLAG)-objs		:= linux/stp_chrdev_bt.o

obj-$(CONFIG_MTK_COMBO_GPS) += mtk_stp_gps$(EXT_FLAG).o
mtk_stp_gps$(EXT_FLAG)-objs	:= linux/stp_chrdev_gps.o


obj-$(CONFIG_MTK_COMBO_WIFI) += mtk_wmt_wifi$(EXT_FLAG).o
mtk_wmt_wifi$(EXT_FLAG)-objs       := linux/wmt_chrdev_wifi.o 
