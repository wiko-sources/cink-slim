include $(TOPDIR)/config.mk

LIB	= $(obj)lib$(SOC).a

COBJS := mt6577.o \
	mt6577_board.o \
	mt65xx_leds.o \
	boot_mode.o \
	load_image.o \
	addr_trans.o \
	mmc_core.o \
	mmc_test.o \
	msdc_utils.o \
	msdc.o \
	mtk_serial.o \
	mtk_timer.o \
	mt6577_nand.o \
	mt6577_meta.o \
	mtk_key.o \
	mt6577_rtc.o \
	mt6577_recovery.o \
	mt6577_factory.o \
	mtk_auxadc.o\
	mtk_wdt.o\
	mt6577_gpio.o \
	mt6577_gpio_init.o \
	mt6577_pwm.o \
	mt6577_i2c.o \
    mt65xx_disp_drv.o \
	disp_drv.o \
 	disp_assert_layer.o \
	disp_drv_dbi.o \
   	lcd_drv.o \
	disp_drv_dsi.o \
 	dsi_drv.o \
       disp_drv_dpi.o \
 	dpi_drv.o \
    mt65xx_logo.o \
    mt6577_pll.o \
  bmt.o \
  partition_mt.o \
  mt6577_sleep.o \
  irq.o\
  mt65xx_partition.o
  
ifeq ($(MTK_NCP1851_SUPPORT),yes) 
    COBJS += mt6577_bat_tbl.o mt6577_i2c0.o ncp1851.o
else
    ifeq ($(MTK_FAN5405_SUPPORT),yes)
        COBJS += mt6577_bat_fan5405.o mt6577_i2c0.o fan5405.o
    else
        COBJS += mt6577_bat.o
    endif
endif

ifeq ($(CUSTOM_KERNEL_TOUCHPANEL),GT827)
COBJS += mt6577_pmic6329_tbl.o
else
COBJS += mt6577_pmic6329.o
endif

###	custom/$(MTK_PROJECT)/mt65xx_partition.o \
			
SOBJS	:= lowlevel_init.o

SRCS	:= $(SOBJS:.o=.S) $(COBJS:.o=.c)
OBJS	:= $(addprefix $(obj),$(COBJS))
SOBJS	:= $(addprefix $(obj),$(SOBJS))

$(LIB):	$(obj).depend $(OBJS) $(SOBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS) $(SOBJS)

clean:
	rm -f $(SOBJS) $(OBJS)

distclean:	clean
	rm -f $(LIB) core *.bak $(obj).depend

#########################################################################

# defines $(obj).depend target
include $(SRCTREE)/rules.mk

sinclude $(obj).depend

#########################################################################
