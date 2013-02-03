#include <linux/module.h>
#include "partition_define.h"
struct excel_info PartInfo[PART_NUM]={
			{"preloader",262144,0x0, EMMC, 0,BOOT_1},
			{"dsp_bl",6029312,0x40000, EMMC, 0,BOOT_1},
			{"mbr",16384,0x600000, EMMC, 0,USER},
			{"ebr1",16384,0x604000, EMMC, 1,USER},
			{"pmt",4194304,0x608000, EMMC, 0,USER},
			{"nvram",5242880,0xa08000, EMMC, 0,USER},
			{"seccfg",131072,0xf08000, EMMC, 0,USER},
			{"uboot",393216,0xf28000, EMMC, 0,USER},
			{"bootimg",6291456,0xf88000, EMMC, 0,USER},
			{"recovery",6291456,0x1588000, EMMC, 0,USER},
			{"sec_ro",6291456,0x1b88000, EMMC, 2,USER},
			{"misc",393216,0x2188000, EMMC, 0,USER},
			{"logo",3145728,0x21e8000, EMMC, 0,USER},
			{"expdb",2097152,0x24e8000, EMMC, 0,USER},
			{"android",537919488,0x26e8000, EMMC, 3,USER},
			{"cache",537919488,0x227e8000, EMMC, 4,USER},
			{"usrdata",1611661312,0x428e8000, EMMC, 5,USER},
			{"fat",0,0xa29e8000, EMMC, 6,USER},
			{"bmtpool",22020096,0xFFFF00a8, EMMC, 0,USER},
 };
EXPORT_SYMBOL(PartInfo);

#ifdef  MTK_EMMC_SUPPORT
struct MBR_EBR_struct MBR_EBR_px[MBR_COUNT]={
	{"mbr", {1, 2, 3, 4, }},
	{"ebr1", {5, 6, }},
};

EXPORT_SYMBOL(MBR_EBR_px);
#endif

