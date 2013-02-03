obj-$(CONFIG_MTK_SERIAL) := uart.o
obj-$(CONFIG_FIQ_DEBUGGER) += mt_fiq_debugger.o
ifeq ($(MTK_DT_SUPPORT), yes)
ccflags-y  += -DUART_EN_MTK_DT_SUPPORT
endif
