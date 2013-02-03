#!/bin/bash
SRC_BASE=../..
DST_BASE=bcmdhd
# create epiver.h
make -C $SRC_BASE/include
mkdir -p $DST_BASE
cp -u Kconfig.android $DST_BASE/Kconfig
cp -u Makefile.android $DST_BASE/Makefile
cp -u mk.android $DST_BASE/mk.sh
mkdir -p $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/802.11_bta.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/802.1d.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/bcmeth.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/bcmevent.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/802.11e.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/ethernet.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/sdspi.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/bcmip.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/vlan.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/p2p.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/802.11.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/bt_amp_hci.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/wpa.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/proto/eapol.h $DST_BASE/include/proto
cp -u $SRC_BASE/include/sbsdpcmdev.h $DST_BASE/include
cp -u $SRC_BASE/include/aidmp.h $DST_BASE/include
cp -u $SRC_BASE/include/wlioctl.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmendian.h $DST_BASE/include
cp -u $SRC_BASE/include/hndpmu.h $DST_BASE/include
cp -u $SRC_BASE/include/hndsoc.h $DST_BASE/include
cp -u $SRC_BASE/include/sbpcmcia.h $DST_BASE/include
cp -u $SRC_BASE/include/linuxver.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmdefs.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmdevs.h $DST_BASE/include
cp -u $SRC_BASE/include/sbchipc.h $DST_BASE/include
cp -u $SRC_BASE/include/sbconfig.h $DST_BASE/include
cp -u $SRC_BASE/include/miniopt.h $DST_BASE/include
cp -u $SRC_BASE/include/hndrte_cons.h $DST_BASE/include
cp -u $SRC_BASE/include/osl.h $DST_BASE/include
cp -u $SRC_BASE/include/packed_section_end.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmperf.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmutils.h $DST_BASE/include
cp -u $SRC_BASE/include/epivers.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmcdc.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmsdh_sdmmc.h $DST_BASE/include
cp -u $SRC_BASE/include/sbhnddma.h $DST_BASE/include
cp -u $SRC_BASE/include/pcicfg.h $DST_BASE/include
cp -u $SRC_BASE/include/hndrte_armtrap.h $DST_BASE/include
cp -u $SRC_BASE/include/sbsocram.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmsdstd.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmwifi.h $DST_BASE/include
cp -u $SRC_BASE/include/siutils.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmpcispi.h $DST_BASE/include
cp -u $SRC_BASE/include/packed_section_start.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmsdpcm.h $DST_BASE/include
cp -u $SRC_BASE/include/wlfc_proto.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmsdbus.h $DST_BASE/include
cp -u $SRC_BASE/include/htsf.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmspi.h $DST_BASE/include
cp -u $SRC_BASE/include/typedefs.h $DST_BASE/include
cp -u $SRC_BASE/include/sdiovar.h $DST_BASE/include
cp -u $SRC_BASE/include/sdio.h $DST_BASE/include
cp -u $SRC_BASE/include/sbsdio.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmsdh.h $DST_BASE/include
cp -u $SRC_BASE/include/sdioh.h $DST_BASE/include
cp -u $SRC_BASE/include/trxhdr.h $DST_BASE/include
cp -u $SRC_BASE/include/Makefile $DST_BASE/include
cp -u $SRC_BASE/include/dhdioctl.h $DST_BASE/include
cp -u $SRC_BASE/include/msgtrace.h $DST_BASE/include
cp -u $SRC_BASE/include/linux_osl.h $DST_BASE/include
cp -u $SRC_BASE/include/bcmsdspi.h $DST_BASE/include
cp -u $SRC_BASE/dongle/*.h $DST_BASE 
cp -u $SRC_BASE/shared/*.h $DST_BASE 
cp -u $SRC_BASE/shared/aiutils.c $DST_BASE 
cp -u $SRC_BASE/shared/siutils.c $DST_BASE
cp -u $SRC_BASE/shared/bcmutils.c $DST_BASE
cp -u $SRC_BASE/shared/bcmwifi.c $DST_BASE
cp -u $SRC_BASE/shared/bcmevent.c $DST_BASE
cp -u $SRC_BASE/shared/hndpmu.c $DST_BASE
cp -u $SRC_BASE/shared/linux_osl.c $DST_BASE
cp -u $SRC_BASE/shared/sbutils.c $DST_BASE
cp -u $SRC_BASE/bcmsdio/sys/bcmsdh_sdmmc_linux.c $DST_BASE 
cp -u $SRC_BASE/bcmsdio/sys/bcmsdh.c $DST_BASE
cp -u $SRC_BASE/bcmsdio/sys/bcmsdh_linux.c $DST_BASE
cp -u $SRC_BASE/bcmsdio/sys/bcmsdh_sdmmc.c $DST_BASE
cp -u $SRC_BASE/dhd/sys/*.h $DST_BASE 
cp -u $SRC_BASE/dhd/sys/dhd_linux.c $DST_BASE 
cp -u $SRC_BASE/dhd/sys/dhd_linux_sched.c $DST_BASE
cp -u $SRC_BASE/dhd/sys/dhd_sdio.c $DST_BASE
cp -u $SRC_BASE/dhd/sys/dhd_bta.c $DST_BASE
cp -u $SRC_BASE/dhd/sys/dhd_cdc.c $DST_BASE
cp -u $SRC_BASE/dhd/sys/dhd_common.c $DST_BASE
cp -u $SRC_BASE/dhd/sys/dhd_custom_gpio.c $DST_BASE
cp -u $SRC_BASE/dhd/sys/dhd_cfg80211.c $DST_BASE
cp -u $SRC_BASE/wl/sys/*.h $DST_BASE
cp -u $SRC_BASE/wl/sys/wldev_common.c $DST_BASE
cp -u $SRC_BASE/wl/sys/wl_android.c $DST_BASE
cp -u $SRC_BASE/wl/sys/wl_iw.c $DST_BASE
cp -u $SRC_BASE/wl/sys/wl_cfg80211.c $DST_BASE
cp -u $SRC_BASE/wl/sys/wl_cfgp2p.c $DST_BASE
cp -u $SRC_BASE/wl/sys/wl_linux_mon.c $DST_BASE
chmod 644 $DST_BASE/*.c
chmod 644 $DST_BASE/*.h
chmod 644 $DST_BASE/include/*.h
chmod 644 $DST_BASE/include/proto/*.h
chmod 755 $DST_BASE/mk.sh

