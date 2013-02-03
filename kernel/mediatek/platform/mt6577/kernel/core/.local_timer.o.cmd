cmd_mediatek/platform/mt6577/kernel/core/local_timer.o := arm-linux-androideabi-gcc -Wp,-MD,mediatek/platform/mt6577/kernel/core/.local_timer.o.d  -nostdinc -isystem /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/arm-linux-androideabi-4.4.x/bin/../lib/gcc/arm-linux-androideabi/4.4.3/include -I/home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include -Iarch/arm/include/generated -Iinclude  -include include/generated/autoconf.h -I./mediatek/custom//s8073/common -I../mediatek/platform/mt6577/kernel/core/include/ -I../mediatek/source/kernel/include/ -I../mediatek/custom/out/s8073/kernel/magnetometer/ -I../mediatek/custom/out/s8073/kernel/headset/ -I../mediatek/custom/out/s8073/kernel/kpd/ -I../mediatek/custom/out/s8073/kernel/accelerometer/ -I../mediatek/custom/out/s8073/kernel/touchpanel/ -I../mediatek/custom/out/s8073/kernel/rtc/ -I../mediatek/custom/out/s8073/kernel/core/ -I../mediatek/custom/out/s8073/kernel/leds/ -I../mediatek/custom/out/s8073/kernel/battery/ -I../mediatek/custom/out/s8073/kernel/camera/ -I../mediatek/custom/out/s8073/kernel/alsps/ -I../mediatek/custom/out/s8073/kernel/vibrator/ -I../mediatek/custom/out/s8073/kernel/dct/ -I../mediatek/custom/out/s8073/kernel/usb/ -I../mediatek/custom/out/s8073/kernel/magnetometer/inc/ -I../mediatek/custom/out/s8073/kernel/gyroscope/inc/ -I../mediatek/custom/out/s8073/kernel/sound/inc/ -I../mediatek/custom/out/s8073/kernel/sound/ -I../mediatek/custom/out/s8073/kernel/lens/inc/ -I../mediatek/custom/out/s8073/kernel/lens/ -I../mediatek/custom/out/s8073/kernel/accelerometer/inc/ -I../mediatek/custom/out/s8073/kernel/eeprom/inc/ -I../mediatek/custom/out/s8073/kernel/eeprom/ -I../mediatek/custom/out/s8073/kernel/hdmi/inc/ -I../mediatek/custom/out/s8073/kernel/jogball/inc/ -I../mediatek/custom/out/s8073/kernel/leds/inc/ -I../mediatek/custom/out/s8073/kernel/barometer/inc/ -I../mediatek/custom/out/s8073/kernel/./ -I../mediatek/custom/out/s8073/kernel/lcm/inc/ -I../mediatek/custom/out/s8073/kernel/lcm/ -I../mediatek/custom/out/s8073/kernel/alsps/inc/ -I../mediatek/custom/out/s8073/kernel/imgsensor/inc/ -I../mediatek/custom/out/s8073/kernel/imgsensor/ -I../mediatek/custom/out/s8073/kernel/flashlight/inc/ -I../mediatek/custom/out/s8073/kernel/flashlight/ -I../mediatek/custom/out/s8073/kernel/ccci/ -I../mediatek/custom/out/s8073/hal/sensors/ -I../mediatek/custom/out/s8073/hal/camera/ -I../mediatek/custom/out/s8073/hal/audioflinger/ -I../mediatek/custom/out/s8073/hal/combo/ -I../mediatek/custom/out/s8073/hal/inc/ -I../mediatek/custom/out/s8073/hal/ant/ -I../mediatek/custom/out/s8073/hal/fmradio/ -I../mediatek/custom/out/s8073/hal/lens/ -I../mediatek/custom/out/s8073/hal/eeprom/ -I../mediatek/custom/out/s8073/hal/vt/ -I../mediatek/custom/out/s8073/hal/bluetooth/ -I../mediatek/custom/out/s8073/hal/imgsensor/ -I../mediatek/custom/out/s8073/hal/flashlight/ -I../mediatek/custom/out/s8073/hal/matv/ -I../mediatek/custom/out/s8073/common -D__KERNEL__ -mlittle-endian -DMODEM_3G -Imediatek/platform/mt6577/kernel/core/include -Wall -Wundef -Wstrict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -Werror-implicit-function-declaration -Wno-format-security -fno-delete-null-pointer-checks -O2 -fno-pic -marm -fno-dwarf2-cfi-asm -fno-omit-frame-pointer -mapcs -mno-sched-prolog -mabi=aapcs-linux -mno-thumb-interwork -D__LINUX_ARM_ARCH__=7 -march=armv7-a -msoft-float -Uarm -DHAVE_XLOG_FEATURE -DMTK_WB_SPEECH_SUPPORT -DMTK_AUTORAMA_SUPPORT -DMTK_MULTI_STORAGE_SUPPORT -DCUSTOM_KERNEL_ALSPS -DMTK_EAP_SIM_AKA -DMTK_TVOUT_SUPPORT -DCUSTOM_KERNEL_ACCELEROMETER -DMTK_FD_SUPPORT -DHAVE_AACENCODE_FEATURE -DMTK_IPV6_SUPPORT -DMTK_MASS_STORAGE -DMTK_FACEBEAUTY_SUPPORT -DMTK_MAV_SUPPORT -DMTK_CAMERA_BSP_SUPPORT -DCUSTOM_KERNEL_MAGNETOMETER -DMTK_VT3G324M_SUPPORT -DHAVE_AWBENCODE_FEATURE -DMTK_CAMERA_APP_3DHW_SUPPORT -DMTK_SENSOR_SUPPORT -DMTK_M4U_SUPPORT -DMTK_HDR_SUPPORT -DMTK_EMMC_SUPPORT -DMTK_AUTO_DETECT_ACCELEROMETER -DMTK_USES_HD_VIDEO -DMTK_SD_REINIT_SUPPORT -DBCM_GPS_SUPPORT -DBCM_BT_SUPPORT -DBCM_WLAN_SUPPORT -DBCM_FM_SUPPORT -DMTK_2SDCARD_SWAP -DMTK_SWAP_STATIC_MODE -DMT6577 -DFM50AF -DDUMMY_LENS -DLT4015W_DSI -DR61408 -DNT35510_DSI -DMODEM_3G -DOV5647_MIPI_RAW -DBCM4330 -DCONSTANT_FLASHLIGHT -DHI704_YUV -DDUMMY_LENS -DOV5647_MIPI_RAW -DHI704_YUV -DFM_ANALOG_INPUT -DFM50AF -DFM_ANALOG_OUTPUT -DBCM4330_FM -DMTK_LCM_PHYSICAL_ROTATION=\"0\" -DLCM_WIDTH=\"480\" -DMTK_SHARE_MODEM_SUPPORT=\"2\" -DMTK_NEON_SUPPORT=\"yes\" -DMTK_SHARE_MODEM_CURRENT=\"2\" -DLCM_HEIGHT=\"800\" -DCUSTOM_BUILD_VERNO=\"S8073B_PP_F4_B1B8_2.09_V12\" -Wframe-larger-than=1024 -fno-stack-protector -fno-omit-frame-pointer -fno-optimize-sibling-calls -g -Wdeclaration-after-statement -Wno-pointer-sign -fno-strict-overflow -fconserve-stack -I../mediatek/platform/mt6577/kernel/drivers/power -I../mediatek/source/kernel/drivers/wdk    -D"KBUILD_STR(s)=\#s" -D"KBUILD_BASENAME=KBUILD_STR(local_timer)"  -D"KBUILD_MODNAME=KBUILD_STR(local_timer)" -c -o mediatek/platform/mt6577/kernel/core/local_timer.o mediatek/platform/mt6577/kernel/core/local_timer.c

source_mediatek/platform/mt6577/kernel/core/local_timer.o := mediatek/platform/mt6577/kernel/core/local_timer.c

deps_mediatek/platform/mt6577/kernel/core/local_timer.o := \
  include/linux/init.h \
    $(wildcard include/config/modules.h) \
    $(wildcard include/config/hotplug.h) \
  include/linux/compiler.h \
    $(wildcard include/config/sparse/rcu/pointer.h) \
    $(wildcard include/config/trace/branch/profiling.h) \
    $(wildcard include/config/profile/all/branches.h) \
    $(wildcard include/config/enable/must/check.h) \
    $(wildcard include/config/enable/warn/deprecated.h) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/arch/supports/optimized/inlining.h) \
    $(wildcard include/config/optimize/inlining.h) \
  include/linux/compiler-gcc4.h \
  include/linux/smp.h \
    $(wildcard include/config/smp.h) \
    $(wildcard include/config/use/generic/smp/helpers.h) \
    $(wildcard include/config/debug/preempt.h) \
  include/linux/errno.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/errno.h \
  include/asm-generic/errno.h \
  include/asm-generic/errno-base.h \
  include/linux/types.h \
    $(wildcard include/config/uid16.h) \
    $(wildcard include/config/lbdaf.h) \
    $(wildcard include/config/arch/dma/addr/t/64bit.h) \
    $(wildcard include/config/phys/addr/t/64bit.h) \
    $(wildcard include/config/64bit.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/posix_types.h \
  include/linux/list.h \
    $(wildcard include/config/debug/list.h) \
  include/linux/poison.h \
    $(wildcard include/config/illegal/pointer/value.h) \
  include/linux/const.h \
  include/linux/cpumask.h \
    $(wildcard include/config/cpumask/offstack.h) \
    $(wildcard include/config/hotplug/cpu.h) \
    $(wildcard include/config/debug/per/cpu/maps.h) \
    $(wildcard include/config/disable/obsolete/cpumask/functions.h) \
  include/linux/kernel.h \
    $(wildcard include/config/preempt/voluntary.h) \
    $(wildcard include/config/debug/spinlock/sleep.h) \
    $(wildcard include/config/prove/locking.h) \
    $(wildcard include/config/ring/buffer.h) \
    $(wildcard include/config/tracing.h) \
    $(wildcard include/config/numa.h) \
    $(wildcard include/config/compaction.h) \
    $(wildcard include/config/ftrace/mcount/record.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/arm-linux-androideabi-4.4.x/bin/../lib/gcc/arm-linux-androideabi/4.4.3/include/stdarg.h \
  include/linux/linkage.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/linkage.h \
  include/linux/bitops.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/bitops.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/system.h \
    $(wildcard include/config/function/graph/tracer.h) \
    $(wildcard include/config/cpu/32v6k.h) \
    $(wildcard include/config/cpu/xsc3.h) \
    $(wildcard include/config/cpu/fa526.h) \
    $(wildcard include/config/arch/has/barriers.h) \
    $(wildcard include/config/arm/dma/mem/bufferable.h) \
    $(wildcard include/config/cpu/sa1100.h) \
    $(wildcard include/config/cpu/sa110.h) \
    $(wildcard include/config/cpu/v6.h) \
  include/linux/irqflags.h \
    $(wildcard include/config/trace/irqflags.h) \
    $(wildcard include/config/preempt/monitor.h) \
    $(wildcard include/config/irqsoff/tracer.h) \
    $(wildcard include/config/preempt/tracer.h) \
    $(wildcard include/config/trace/irqflags/support.h) \
  include/linux/typecheck.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/irqflags.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/cpu/endian/be8.h) \
    $(wildcard include/config/arm/thumb.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/hwcap.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/outercache.h \
    $(wildcard include/config/outer/cache/sync.h) \
    $(wildcard include/config/outer/cache.h) \
  include/asm-generic/cmpxchg-local.h \
  include/asm-generic/bitops/non-atomic.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/sched.h \
  include/asm-generic/bitops/hweight.h \
  include/asm-generic/bitops/arch_hweight.h \
  include/asm-generic/bitops/const_hweight.h \
  include/asm-generic/bitops/lock.h \
  include/asm-generic/bitops/le.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/linux/log2.h \
    $(wildcard include/config/arch/has/ilog2/u32.h) \
    $(wildcard include/config/arch/has/ilog2/u64.h) \
  include/linux/printk.h \
    $(wildcard include/config/printk.h) \
    $(wildcard include/config/dynamic/debug.h) \
  include/linux/dynamic_debug.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/bug.h \
    $(wildcard include/config/bug.h) \
    $(wildcard include/config/debug/bugverbose.h) \
  include/asm-generic/bug.h \
    $(wildcard include/config/generic/bug.h) \
    $(wildcard include/config/generic/bug/relative/pointers.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/div64.h \
  include/linux/threads.h \
    $(wildcard include/config/nr/cpus.h) \
    $(wildcard include/config/base/small.h) \
  include/linux/bitmap.h \
  include/linux/string.h \
    $(wildcard include/config/binary/printf.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/string.h \
  include/linux/preempt.h \
    $(wildcard include/config/preempt.h) \
    $(wildcard include/config/preempt/notifiers.h) \
  include/linux/thread_info.h \
    $(wildcard include/config/compat.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/thread_info.h \
    $(wildcard include/config/arm/thumbee.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/fpstate.h \
    $(wildcard include/config/vfpv3.h) \
    $(wildcard include/config/iwmmxt.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/domain.h \
    $(wildcard include/config/io/36.h) \
    $(wildcard include/config/cpu/use/domains.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/smp.h \
  include/linux/clockchips.h \
    $(wildcard include/config/generic/clockevents/build.h) \
    $(wildcard include/config/generic/clockevents.h) \
  include/linux/clocksource.h \
    $(wildcard include/config/ia64.h) \
    $(wildcard include/config/clocksource/watchdog.h) \
    $(wildcard include/config/generic/time/vsyscall.h) \
  include/linux/timex.h \
  include/linux/time.h \
    $(wildcard include/config/arch/uses/gettimeoffset.h) \
  include/linux/cache.h \
    $(wildcard include/config/arch/has/cache/line/size.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/cache.h \
    $(wildcard include/config/arm/l1/cache/shift.h) \
    $(wildcard include/config/aeabi.h) \
  include/linux/seqlock.h \
  include/linux/spinlock.h \
    $(wildcard include/config/debug/spinlock.h) \
    $(wildcard include/config/generic/lockbreak.h) \
    $(wildcard include/config/debug/lock/alloc.h) \
  include/linux/stringify.h \
  include/linux/bottom_half.h \
  include/linux/spinlock_types.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/spinlock_types.h \
  include/linux/lockdep.h \
    $(wildcard include/config/lockdep.h) \
    $(wildcard include/config/lock/stat.h) \
    $(wildcard include/config/prove/rcu.h) \
  include/linux/rwlock_types.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/spinlock.h \
    $(wildcard include/config/thumb2/kernel.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/processor.h \
    $(wildcard include/config/have/hw/breakpoint.h) \
    $(wildcard include/config/mmu.h) \
    $(wildcard include/config/arm/errata/754327.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/hw_breakpoint.h \
  include/linux/rwlock.h \
  include/linux/spinlock_api_smp.h \
    $(wildcard include/config/inline/spin/lock.h) \
    $(wildcard include/config/inline/spin/lock/bh.h) \
    $(wildcard include/config/inline/spin/lock/irq.h) \
    $(wildcard include/config/inline/spin/lock/irqsave.h) \
    $(wildcard include/config/inline/spin/trylock.h) \
    $(wildcard include/config/inline/spin/trylock/bh.h) \
    $(wildcard include/config/inline/spin/unlock.h) \
    $(wildcard include/config/inline/spin/unlock/bh.h) \
    $(wildcard include/config/inline/spin/unlock/irq.h) \
    $(wildcard include/config/inline/spin/unlock/irqrestore.h) \
  include/linux/rwlock_api_smp.h \
    $(wildcard include/config/inline/read/lock.h) \
    $(wildcard include/config/inline/write/lock.h) \
    $(wildcard include/config/inline/read/lock/bh.h) \
    $(wildcard include/config/inline/write/lock/bh.h) \
    $(wildcard include/config/inline/read/lock/irq.h) \
    $(wildcard include/config/inline/write/lock/irq.h) \
    $(wildcard include/config/inline/read/lock/irqsave.h) \
    $(wildcard include/config/inline/write/lock/irqsave.h) \
    $(wildcard include/config/inline/read/trylock.h) \
    $(wildcard include/config/inline/write/trylock.h) \
    $(wildcard include/config/inline/read/unlock.h) \
    $(wildcard include/config/inline/write/unlock.h) \
    $(wildcard include/config/inline/read/unlock/bh.h) \
    $(wildcard include/config/inline/write/unlock/bh.h) \
    $(wildcard include/config/inline/read/unlock/irq.h) \
    $(wildcard include/config/inline/write/unlock/irq.h) \
    $(wildcard include/config/inline/read/unlock/irqrestore.h) \
    $(wildcard include/config/inline/write/unlock/irqrestore.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/atomic.h \
    $(wildcard include/config/generic/atomic64.h) \
  include/asm-generic/atomic-long.h \
  include/linux/math64.h \
  include/linux/param.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/param.h \
    $(wildcard include/config/hz.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/timex.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/timex.h \
  include/linux/timer.h \
    $(wildcard include/config/timer/stats.h) \
    $(wildcard include/config/debug/objects/timers.h) \
  include/linux/ktime.h \
    $(wildcard include/config/ktime/scalar.h) \
  include/linux/jiffies.h \
  include/linux/debugobjects.h \
    $(wildcard include/config/debug/objects.h) \
    $(wildcard include/config/debug/objects/free.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/io.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/memory.h \
    $(wildcard include/config/page/offset.h) \
    $(wildcard include/config/highmem.h) \
    $(wildcard include/config/dram/size.h) \
    $(wildcard include/config/dram/base.h) \
    $(wildcard include/config/have/tcm.h) \
    $(wildcard include/config/arm/patch/phys/virt.h) \
    $(wildcard include/config/arm/patch/phys/virt/16bit.h) \
  ../mediatek/platform/mt6577/kernel/core/include/mach/memory.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/sizes.h \
  include/asm-generic/sizes.h \
  include/asm-generic/memory_model.h \
    $(wildcard include/config/flatmem.h) \
    $(wildcard include/config/discontigmem.h) \
    $(wildcard include/config/sparsemem/vmemmap.h) \
    $(wildcard include/config/sparsemem.h) \
  ../mediatek/platform/mt6577/kernel/core/include/mach/io.h \
  include/linux/notifier.h \
  include/linux/mutex.h \
    $(wildcard include/config/debug/mutexes.h) \
    $(wildcard include/config/mt/lock/debug.h) \
    $(wildcard include/config/have/arch/mutex/cpu/relax.h) \
  include/linux/rwsem.h \
    $(wildcard include/config/rwsem/generic/spinlock.h) \
  include/linux/rwsem-spinlock.h \
  include/linux/srcu.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/irq.h \
  ../mediatek/platform/mt6577/kernel/core/include/mach/irqs.h \
    $(wildcard include/config/fiq/glue.h) \
  ../mediatek/platform/mt6577/kernel/core/include/mach/mt6577_irq.h \
    $(wildcard include/config/mt6577/fpga.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/smp_twd.h \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/localtimer.h \
    $(wildcard include/config/local/timers.h) \
    $(wildcard include/config/have/arm/twd.h) \
  /home/xiuyongtang/OrderTinno/A_GPL/A_GPL/kernel/arch/arm/include/asm/smp_twd.h \

mediatek/platform/mt6577/kernel/core/local_timer.o: $(deps_mediatek/platform/mt6577/kernel/core/local_timer.o)

$(deps_mediatek/platform/mt6577/kernel/core/local_timer.o):
