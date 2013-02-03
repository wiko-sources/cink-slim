ifeq ($(CONFIG_MT6577_FPGA),y)

obj-y := core.o irq.o system.o timer.o mt6577_board.o mt6577_gpt.o clock.o mt6577_outercache.o mt6577_devs.o mt6577_eint.o mt6577_boot.o mt6577_pwm.o 

obj-$(CONFIG_SMP) += mt-smp.o hotplug.o mt-headsmp.o local_timer.o mtk_hotplug_mechanism.o
obj-y += sec_osal.o

# Fix-me: marked others for early porting
#obj-y := core.o irq.o system.o timer.o mt6577_devs.o mt6577_board.o mt6577_dma.o mt6577_boot.o mt6577_gpt.o clock.o mt6577_eint.o mt6577_pm_init.o mt6577_outercache.o mt6577_ca9_power.o mt6577_pwm.o
#obj-y := core.o irq.o system.o timer.o mt6577_devs.o mt6577_board.o mt6577_gpt.o clock.o mt6577_dma.o mt6577_eint.o mt6577_gpio.o mt6577_outercache.o mt6577_ca9_power.o mt6577_pwm.o

obj-y += sbchk_base.o
obj-y += mt_rfkill.o
#obj-y += kdump_sdhc_fs.o
obj-y += camera_isp.o
obj-y += camera_sysram.o 
obj-y += camera_res_mgr.o
obj-y += camera_eis.o
#obj-y += camera_fdvt.o
obj-y += gpio.o
obj-y += mt6577_mdp.o
obj-y += mt6577_mdp_kernel_trap.o
obj-y += mt6577_emi_bwl.o
ifneq ($(CONFIG_MT6577_FPGA),y)
ifneq ($(CONFIG_MT6577_ESL),y)
#obj-y += mt6577_emi_mpu.o
endif
endif
#obj-y += mt6577_boot.o
#obj-y += mt6577_pm_ldo.o mt6577_pmic_feature_api.o
#obj-y += mt6577_auxadc.o
#obj-y += mt6577_dcm.o mt6577_clock_manager.o mt6577_pm_log.o
#obj-y += cpufreq-mt6577.o
#obj-y += mt6577_thermal.o mt6577_cpe.o
#obj-y += mt6577_sleep.o mt6577_sc.o
#obj-y += ca9_power.o
#obj-y += mt6577_wdt.o
ifneq ($(MTK_EMMC_SUPPORT),yes)
obj-y += sbchk_base.o
endif
obj-$(CONFIG_MT65XX_TRACER) += pmu_v7.o mon_interface.o mt6577_emi_bm.o mt6577_mon.o
#obj-y += innercache.o
obj-y += ple.o
obj-y += dram_overclock.o
obj-y += mtk_ccci_helper.o
#obj-y += mt6577_dramc.o
obj-y += mt6577_smi.o

ifeq ($(MTK_BT_SUPPORT), yes)
# MTK COMBO chip or not
ifeq ($(CONFIG_MTK_COMBO),)
#obj-y += mt_bt.o
endif
endif

ccflags-y += -I$(MTK_PATH_PLATFORM)/drivers/power \
             -I$(MTK_PATH_SOURCE)/drivers/wdk
#obj-y += mt6577_dormant.o cpu_dormant.o
KBUILD_AFLAGS	:=$(KBUILD_AFLAGS:-msoft-float=-Wa,-mfpu=neon)

else

obj-y := core.o irq.o system.o timer.o clock.o mt6577_devs.o mt6577_board.o 
obj-y += mt6577_dma.o mt6577_boot.o mt6577_gpt.o mt6577_eint.o mt6577_pm_init.o mt6577_outercache.o mt6577_ca9_power.o mt6577_pwm.o sbchk_base.o
obj-y += sec_osal.o

obj-$(CONFIG_SMP) += mt-smp.o hotplug.o mt-headsmp.o local_timer.o mtk_hotplug_mechanism.o

obj-y += mt_rfkill.o
obj-y += camera_isp.o
obj-y += camera_sysram.o 
obj-y += camera_eis.o
obj-y += camera_fdvt.o
obj-y += camera_res_mgr.o
obj-y += gpio.o
obj-y += mt6577_mdp.o
obj-y += mt6577_mdp_kernel_trap.o
obj-y += mt6577_emi_bwl.o
obj-y += mt6577_emi_mpu.o
obj-y += mt6577_pm_ldo.o mt6577_pmic_feature_api.o
obj-y += mt6577_auxadc.o
obj-y += mt6577_dcm.o mt6577_clock_manager.o mt6577_pm_log.o
obj-y += cpufreq-mt6577.o mt6577_cpe.o
obj-y += mt6577_sleep.o mt6577_sc.o
obj-y += ca9_power.o
# obj-y += mt6577_wdt.o
#ifneq ($(MTK_EMMC_SUPPORT),yes)
#obj-y += sbchk_base.o
#endif
obj-$(CONFIG_MT65XX_TRACER) += pmu_v7.o mon_interface.o mt6577_emi_bm.o mt6577_mon.o
obj-y += innercache.o
obj-y += ple.o
obj-y += dram_overclock.o
obj-y += mtk_ccci_helper.o
obj-y += mt6577_dramc.o
obj-y += mt6577_smi.o
obj-$(CONFIG_SMP) += fiq_smp_call.o

ifeq ($(MTK_BT_SUPPORT), yes)
# MTK COMBO chip or not
ifeq ($(CONFIG_MTK_COMBO),)
obj-y += mt_bt.o
endif
endif

ifeq ($(BCM_WLAN_SUPPORT), yes)
obj-y += board-broadcom-wifi.o
endif

ifeq ($(CONFIG_THERMAL),y)
ifeq ($(CONFIG_SMP),y)
obj-y += mtk_cpu_management.o
obj-y += mtk_ts_cpu.o
else
obj-y += mtk_cpu_management_up.o
obj-y += mtk_ts_cpu_up.o
endif

obj-y += mtk_ts_battery.o
obj-y += mtk_ts_pa.o
obj-y += mtk_ts_abb.o
obj-y += mtk_ts_pmic.o
endif

obj-y += ca9_speed.o ca9_speed_indicative.o ca9_speed_test.o ca9_speed_neon.o ca9_neon.o ca9_neon_indicative.o ca9_neon_test.o slt_ca9_helpers.o slt_ca9_helpers_c.o mt6577_efuse.o

ccflags-y += -I$(MTK_PATH_PLATFORM)/drivers/power \
             -I$(MTK_PATH_SOURCE)/drivers/wdk
obj-y += mt6577_dormant.o cpu_dormant.o
KBUILD_AFLAGS	:=$(KBUILD_AFLAGS:-msoft-float=-Wa,-mfpu=neon)

endif
