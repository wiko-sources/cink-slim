obj-$(CONFIG_MTK_PMIC)          += pmic_mt6329.o pmic_mt6329_bank1.o upmu_common.o

ifeq ($(MTK_NCP1851_SUPPORT),yes) 
    obj-$(CONFIG_MTK_SMART_BATTERY) += mt6577_battery_tbl.o mt6577_fuel_gauge_tbl.o ncp1851.o
else
    ifeq ($(MTK_FAN5405_SUPPORT),yes)
        obj-$(CONFIG_MTK_SMART_BATTERY) += mt6577_battery_fan5405.o mt6577_fuel_gauge.o fan5405.o
    else
        obj-$(CONFIG_MTK_SMART_BATTERY) += mt6577_battery.o mt6577_fuel_gauge.o
    endif
endif