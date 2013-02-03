ifeq ($(MTK_MODEM_SUPPORT), modem_2g)
   zreladdr-y   := 0x00A08000
params_phys-y   := 0x00A00100
else
ifeq ($(MTK_MODEM_SUPPORT), modem_3g)
   zreladdr-y   := 0x01608000
params_phys-y   := 0x01600100
else
   zreladdr-y   := 0x00008000
params_phys-y   := 0x00000100
endif
endif
initrd_phys-y   := 0x02F00000
