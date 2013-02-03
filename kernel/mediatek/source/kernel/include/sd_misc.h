#ifndef SD_MISC_H
#define SD_MISC_H

#ifdef __KERNEL__
#include <linux/bitops.h>
#include <linux/mmc/host.h>
#endif



struct msdc_ioctl{
	int  opcode;
	int  host_num;
	int  iswrite;
	int  trans_type;
	unsigned int  total_size;
	unsigned int  address;
	unsigned int* buffer;
	int  cmd_pu_driving;
	int  cmd_pd_driving;
	int  dat_pu_driving;
	int  dat_pd_driving;
	int  clk_pu_driving;
	int  clk_pd_driving;
	int  clock_freq;
  int  partition;
  int  hopping_bit;
  int  hopping_time;
	int  result;
};

/**************for msdc_ssc***********************/
#define AUDPLL_CTL_REG12				(0xF0007070)
#define AUDPLL_CTL_REG01				(0xF00071E0)
#define AUDPLL_CTL_REG02				(0xF100000C)

#define AUDPLL_TSEL_MASK				(1792)  //MASK = 00000111 00000000
#define AUDPLL_TSEL_RESULT1				(0)	//REG = 00000000 00000000   30.5us
#define AUDPLL_TSEL_RESULT2				(256)	//REG = 00000001 00000000   61.0us
#define AUDPLL_TSEL_RESULT3				(512)   //REG = 00000010 00000000   122.1us
#define AUDPLL_TSEL_RESULT4				(768)   //REG = 00000011 00000000   244.1us
#define AUDPLL_TSEL_RESULT5				(1024)  //REG = 00000100 00000000   448.3us

#define AUDPLL_BSEL_MASK				(7)     //MASK = 00000000 00000111
#define AUDPLL_BSEL_RESULT0				(0)     //REG = 00000000 00000000   REG init val
#define AUDPLL_BSEL_RESULT1				(1)     //REG = 00000000 00000001   2.26MHz
#define AUDPLL_BSEL_RESULT2				(2)     //REG = 00000000 00000010   4.52MHz
#define AUDPLL_BSEL_RESULT3				(4)     //REG = 00000000 00000100   9.04MHz

#define SET_HOP_BIT_NONE      (0)
#define SET_HOP_BIT1					(1)
#define SET_HOP_BIT2					(2)
#define SET_HOP_BIT3					(3)

#define SET_HOP_TIME0					(0)
#define SET_HOP_TIME1					(1)
#define SET_HOP_TIME2					(2)
#define SET_HOP_TIME3					(3)
#define SET_HOP_TIME4					(4)


/**************for msdc_ssc***********************/

#define MSDC_DRIVING_SETTING              (0)
#define MSDC_CLOCK_FREQUENCY              (1)
#define MSDC_SINGLE_READ_WRITE            (2)
#define MSDC_MULTIPLE_READ_WRITE          (3)
#define MSDC_GET_CID                      (4)
#define MSDC_GET_CSD                      (5)
#define MSDC_GET_EXCSD                    (6)
#define MSDC_ERASE_PARTITION              (7)
#define MSDC_HOPPING_SETTING              (8)
#ifdef MTK_SD_REINIT_SUPPORT
#define MSDC_REINIT_SDCARD            _IOW('r', 9, int)
#endif
#define MSDC_CARD_DUNM_FUNC               (0xff)

typedef enum {
    USER_PARTITION = 0,
    BOOT_PARTITION_1,
    BOOT_PARTITION_2,
    RPMB_PARTITION,
    GP_PARTITION_1,
    GP_PARTITION_2,
    GP_PARTITION_3,
    GP_PARTITION_4,
}PARTITON_ACCESS_T;


#endif /* end of SD_MISC_H */
