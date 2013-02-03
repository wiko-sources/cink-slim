#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/uaccess.h>
#include <linux/mmc/host.h>
#include <mach/mt6577_gpt.h>
#include <mach/mt6577_clock_manager.h>
#include <asm/io.h>
//for fpga early porting
#include <linux/mmc/mmc.h>
#include <linux/mmc/card.h>
#include <linux/scatterlist.h>
#include <mach/mt6577_typedefs.h>
//end for fpga early porting
#include "dbg.h"

typedef enum
{
	MODE_NULL = 0,
	SDHC_HIGHSPEED,		/* 0x1 Host supports HS mode */
	UHS_SDR12,		    /* 0x2 Host supports UHS SDR12 mode */
	UHS_SDR25,			/* 0x3 Host supports UHS SDR25 mode */
	UHS_SDR50,			/* 0x4 Host supports UHS SDR50 mode */
	UHS_SDR104,			/* 0x5 Host supports UHS SDR104 mode */
	UHS_DDR50,			/* 0x6 Host supports UHS DDR50 mode */

	DRIVER_TYPE_A,	    /* 0x7 Host supports Driver Type A */
	DRIVER_TYPE_B,		/* 0x8 Host supports Driver Type B */
	DRIVER_TYPE_C,		/* 0x9 Host supports Driver Type C */
	DRIVER_TYPE_D,		/* 0xA Host supports Driver Type D */

	MAX_CURRENT_200,    /* 0xB Host max current limit is 200mA */
	MAX_CURRENT_400,	/* 0xC Host max current limit is 400mA */
	MAX_CURRENT_600,	/* 0xD Host max current limit is 600mA */
	MAX_CURRENT_800,	/* 0xE Host max current limit is 800mA */

	SDXC_NO_POWER_CONTROL,/*0xF   Host not supports >150mA current at 3.3V /3.0V/1.8V*/
	SDXC_POWER_CONTROL,   /*0x10 Host supports >150mA current at 3.3V /3.0V/1.8V*/
}HOST_CAPS;

static char cmd_buf[256];

/* for debug zone */
unsigned int sd_debug_zone[HOST_MAX_NUM]={
	0,
	0,
	0,
	0
};

/* mode select */
u32 dma_size[HOST_MAX_NUM]={
	512,
	512,
	512,
	512
};
msdc_mode drv_mode[HOST_MAX_NUM]={
	MODE_SIZE_DEP, /* using DMA or not depend on the size */
	MODE_SIZE_DEP,
	MODE_SIZE_DEP,
	MODE_SIZE_DEP
};
unsigned char msdc_clock_src[HOST_MAX_NUM]={
	0,
	0,
	0,
	0
};
drv_mod msdc_drv_mode[HOST_MAX_NUM];

u32 msdc_host_mode[HOST_MAX_NUM]={
	0,
	0,
	0,
	0
};


/* for driver profile */
#define TICKS_ONE_MS  (13000)
u32 gpt_enable = 0;
u32 sdio_pro_enable = 0;   /* make sure gpt is enabled */
u32 sdio_pro_time = 0;     /* no more than 30s */
struct sdio_profile sdio_perfomance = {0};  
void msdc_dump_reg(unsigned int base)
{
	printk("[SD_Debug]Reg[00] MSDC_CFG       = 0x%.8x\n", sdr_read32(base + 0x00));
    printk("[SD_Debug]Reg[04] MSDC_IOCON     = 0x%.8x\n", sdr_read32(base + 0x04));
    printk("[SD_Debug]Reg[08] MSDC_PS        = 0x%.8x\n", sdr_read32(base + 0x08));
    printk("[SD_Debug]Reg[0C] MSDC_INT       = 0x%.8x\n", sdr_read32(base + 0x0C));
    printk("[SD_Debug]Reg[10] MSDC_INTEN     = 0x%.8x\n", sdr_read32(base + 0x10));    
    printk("[SD_Debug]Reg[14] MSDC_FIFOCS    = 0x%.8x\n", sdr_read32(base + 0x14));
    printk("[SD_Debug]Reg[18] MSDC_TXDATA    = not read\n");                    	
    printk("[SD_Debug]Reg[1C] MSDC_RXDATA    = not read\n");
    printk("[SD_Debug]Reg[30] SDC_CFG        = 0x%.8x\n", sdr_read32(base + 0x30));
    printk("[SD_Debug]Reg[34] SDC_CMD        = 0x%.8x\n", sdr_read32(base + 0x34));
    printk("[SD_Debug]Reg[38] SDC_ARG        = 0x%.8x\n", sdr_read32(base + 0x38));
    printk("[SD_Debug]Reg[3C] SDC_STS        = 0x%.8x\n", sdr_read32(base + 0x3C));
    printk("[SD_Debug]Reg[40] SDC_RESP0      = 0x%.8x\n", sdr_read32(base + 0x40));
    printk("[SD_Debug]Reg[44] SDC_RESP1      = 0x%.8x\n", sdr_read32(base + 0x44));
    printk("[SD_Debug]Reg[48] SDC_RESP2      = 0x%.8x\n", sdr_read32(base + 0x48));                                
    printk("[SD_Debug]Reg[4C] SDC_RESP3      = 0x%.8x\n", sdr_read32(base + 0x4C));
    printk("[SD_Debug]Reg[50] SDC_BLK_NUM    = 0x%.8x\n", sdr_read32(base + 0x50));
    printk("[SD_Debug]Reg[58] SDC_CSTS       = 0x%.8x\n", sdr_read32(base + 0x58));
    printk("[SD_Debug]Reg[5C] SDC_CSTS_EN    = 0x%.8x\n", sdr_read32(base + 0x5C));
    printk("[SD_Debug]Reg[60] SDC_DATCRC_STS = 0x%.8x\n", sdr_read32(base + 0x60));
    printk("[SD_Debug]Reg[70] EMMC_CFG0      = 0x%.8x\n", sdr_read32(base + 0x70));                        
    printk("[SD_Debug]Reg[74] EMMC_CFG1      = 0x%.8x\n", sdr_read32(base + 0x74));
    printk("[SD_Debug]Reg[78] EMMC_STS       = 0x%.8x\n", sdr_read32(base + 0x78));
    printk("[SD_Debug]Reg[7C] EMMC_IOCON     = 0x%.8x\n", sdr_read32(base + 0x7C));            
    printk("[SD_Debug]Reg[80] SD_ACMD_RESP   = 0x%.8x\n", sdr_read32(base + 0x80));
    printk("[SD_Debug]Reg[84] SD_ACMD19_TRG  = 0x%.8x\n", sdr_read32(base + 0x84));      
    printk("[SD_Debug]Reg[88] SD_ACMD19_STS  = 0x%.8x\n", sdr_read32(base + 0x88));
    printk("[SD_Debug]Reg[90] DMA_SA         = 0x%.8x\n", sdr_read32(base + 0x90));
    printk("[SD_Debug]Reg[94] DMA_CA         = 0x%.8x\n", sdr_read32(base + 0x94));
    printk("[SD_Debug]Reg[98] DMA_CTRL       = 0x%.8x\n", sdr_read32(base + 0x98));
    printk("[SD_Debug]Reg[9C] DMA_CFG        = 0x%.8x\n", sdr_read32(base + 0x9C));                        
    printk("[SD_Debug]Reg[A0] SW_DBG_SEL     = 0x%.8x\n", sdr_read32(base + 0xA0));
    printk("[SD_Debug]Reg[A4] SW_DBG_OUT     = 0x%.8x\n", sdr_read32(base + 0xA4));
    printk("[SD_Debug]Reg[B0] PATCH_BIT0     = 0x%.8x\n", sdr_read32(base + 0xB0));            
    printk("[SD_Debug]Reg[B4] PATCH_BIT1     = 0x%.8x\n", sdr_read32(base + 0xB4));
    printk("[SD_Debug]Reg[E0] SD_PAD_CTL0  = 0x%.8x\n", sdr_read32(base + 0xE0));        
    printk("[SD_Debug]Reg[E4] SD_PAD_CTL1  = 0x%.8x\n", sdr_read32(base + 0xE4));
    printk("[SD_Debug]Reg[E8] SD_PAD_CTL2  = 0x%.8x\n", sdr_read32(base + 0xE8));
    printk("[SD_Debug]Reg[EC] PAD_TUNE       = 0x%.8x\n", sdr_read32(base + 0xEC));
    printk("[SD_Debug]Reg[F0] DAT_RD_DLY0    = 0x%.8x\n", sdr_read32(base + 0xF0));                        
    printk("[SD_Debug]Reg[F4] DAT_RD_DLY1    = 0x%.8x\n", sdr_read32(base + 0xF4));
    printk("[SD_Debug]Reg[F8] HW_DBG_SEL     = 0x%.8x\n", sdr_read32(base + 0xF8));
    printk("[SD_Debug]Rg[100] MAIN_VER       = 0x%.8x\n", sdr_read32(base + 0x100));     
    printk("[SD_Debug]Rg[104] ECO_VER        = 0x%.8x\n", sdr_read32(base + 0x104));
}
void msdc_set_field(unsigned int address,unsigned int start_bit,unsigned int len,unsigned int value)
{
	unsigned long field;
	if(start_bit > 31 || start_bit < 0|| len > 32 || len <= 0)
		printk("[****SD_Debug****]reg filed beyoned (0~31) or length beyoned (1~32)\n");
	else{
		field = ((1 << len) -1) << start_bit;
		value &= (1 << len) -1; 
		printk("[****SD_Debug****]Original:0x%x (0x%x)\n",address,sdr_read32(address));
		sdr_set_field(address,field, value);
		printk("[****SD_Debug****]Modified:0x%x (0x%x)\n",address,sdr_read32(address));
	}
}
void msdc_get_field(unsigned int address,unsigned int start_bit,unsigned int len,unsigned int value)
{
	unsigned long field;
	if(start_bit > 31 || start_bit < 0|| len > 32 || len <= 0)
		printk("[****SD_Debug****]reg filed beyoned (0~31) or length beyoned (1~32)\n");
	else{
		field = ((1 << len) -1) << start_bit;
		sdr_get_field(address,field,value);
		printk("[****SD_Debug****]Reg:0x%x start_bit(%d)len(%d)(0x%x)\n",address,start_bit,len,value);
		}
}
void msdc_init_gpt(void)
{
    GPT_CONFIG config;	
    
    config.num  = GPT6;
    config.mode = GPT_FREE_RUN;
    config.clkSrc = GPT_CLK_SRC_SYS;
    config.clkDiv = GPT_CLK_DIV_1;   /* 13MHz GPT6 */
            
    if (GPT_Config(config) == FALSE )
        return;                       
        
    GPT_Start(GPT6);	
}

u32 msdc_time_calc(u32 old_L32, u32 old_H32, u32 new_L32, u32 new_H32)
{
    u32 ret = 0; 
    
    if (new_H32 == old_H32) {
        ret = new_L32 - old_L32;
    } else if(new_H32 == (old_H32 + 1)) {
        if (new_L32 > old_L32) {	
            printk("msdc old_L<0x%x> new_L<0x%x>\n", old_L32, new_L32);	
        }
        ret = (0xffffffff - old_L32);  	      
        ret += new_L32; 
    } else {
        printk("msdc old_H<0x%x> new_H<0x%x>\n", old_H32, new_H32);		
    }

    return ret;        	
}

void msdc_sdio_profile(struct sdio_profile* result)
{
    struct cmd_profile*  cmd;
    u32 i; 	
    
    printk("sdio === performance dump ===\n");
    printk("sdio === total execute tick<%d> time<%dms> Tx<%dB> Rx<%dB>\n", 
                    result->total_tc, result->total_tc / TICKS_ONE_MS, 
                    result->total_tx_bytes, result->total_rx_bytes);    

    /* CMD52 Dump */
    cmd = &result->cmd52_rx; 
    printk("sdio === CMD52 Rx <%d>times tick<%d> Max<%d> Min<%d> Aver<%d>\n", cmd->count, cmd->tot_tc, 
                    cmd->max_tc, cmd->min_tc, cmd->tot_tc/cmd->count);     
    cmd = &result->cmd52_tx; 
    printk("sdio === CMD52 Tx <%d>times tick<%d> Max<%d> Min<%d> Aver<%d>\n", cmd->count, cmd->tot_tc, 
                    cmd->max_tc, cmd->min_tc, cmd->tot_tc/cmd->count);   
                    
    /* CMD53 Rx bytes + block mode */
    for (i=0; i<512; i++) {
        cmd = &result->cmd53_rx_byte[i];
        if (cmd->count) {
            printk("sdio<%6d><%3dB>_Rx_<%9d><%9d><%6d><%6d>_<%9dB><%2dM>\n", cmd->count, i, cmd->tot_tc, 
                             cmd->max_tc, cmd->min_tc, cmd->tot_tc/cmd->count,
                             cmd->tot_bytes, (cmd->tot_bytes/10)*13 / (cmd->tot_tc/10));                        	
        }	
    }  
    for (i=0; i<100; i++) {
        cmd = &result->cmd53_rx_blk[i];
        if (cmd->count) {
            printk("sdio<%6d><%3d>B_Rx_<%9d><%9d><%6d><%6d>_<%9dB><%2dM>\n", cmd->count, i, cmd->tot_tc, 
                             cmd->max_tc, cmd->min_tc, cmd->tot_tc/cmd->count,
                             cmd->tot_bytes, (cmd->tot_bytes/10)*13 / (cmd->tot_tc/10));                        	
        }	
    }

    /* CMD53 Tx bytes + block mode */
    for (i=0; i<512; i++) {
        cmd = &result->cmd53_tx_byte[i];
        if (cmd->count) {
            printk("sdio<%6d><%3dB>_Tx_<%9d><%9d><%6d><%6d>_<%9dB><%2dM>\n", cmd->count, i, cmd->tot_tc, 
                             cmd->max_tc, cmd->min_tc, cmd->tot_tc/cmd->count,
                             cmd->tot_bytes, (cmd->tot_bytes/10)*13 / (cmd->tot_tc/10));                          	
        }	
    }          
    for (i=0; i<100; i++) {
        cmd = &result->cmd53_tx_blk[i];
        if (cmd->count) {
            printk("sdio<%6d><%3d>B_Tx_<%9d><%9d><%6d><%6d>_<%9dB><%2dM>\n", cmd->count, i, cmd->tot_tc, 
                             cmd->max_tc, cmd->min_tc, cmd->tot_tc/cmd->count,
                             cmd->tot_bytes, (cmd->tot_bytes/10)*13 / (cmd->tot_tc/10));                            	
        }	
    }     
    
    printk("sdio === performance dump done ===\n");      
}

//========= sdio command table ===========
void msdc_performance(u32 opcode, u32 sizes, u32 bRx, u32 ticks)
{
    struct sdio_profile* result = &sdio_perfomance; 
    struct cmd_profile*  cmd; 
    u32 block;     	

    if (sdio_pro_enable == 0) {
        return;
    }

    if (opcode == 52) {
        cmd = bRx ?  &result->cmd52_rx : &result->cmd52_tx;   	
    } else if (opcode == 53) {
        if (sizes < 512) {
            cmd = bRx ?  &result->cmd53_rx_byte[sizes] : &result->cmd53_tx_byte[sizes];    	
        } else {
            block = sizes / 512; 
            if (block >= 99) {
               printk("cmd53 error blocks\n"); 
               while(1);	
            }
            cmd = bRx ?  &result->cmd53_rx_blk[block] : &result->cmd53_tx_blk[block];       	
        }   	
    } else {
        return; 	
    }
        
    /* update the members */
    if (ticks > cmd->max_tc){
        cmd->max_tc = ticks;	
    }
    if (cmd->min_tc == 0 || ticks < cmd->min_tc) {
        cmd->min_tc = ticks; 	  
    }
    cmd->tot_tc += ticks;
    cmd->tot_bytes += sizes; 
    cmd->count ++; 
    
    if (bRx) {
        result->total_rx_bytes += sizes;    	
    } else {
        result->total_tx_bytes += sizes; 	
    }
    result->total_tc += ticks; 
    
    /* dump when total_tc > 30s */
    if (result->total_tc >= sdio_pro_time * TICKS_ONE_MS * 1000) {
        msdc_sdio_profile(result);       
        memset(result, 0 , sizeof(struct sdio_profile));                                             
    }
}



#define COMPARE_ADDRESS_MMC   0x0
#define COMPARE_ADDRESS_SD    0x2000
#define COMPARE_ADDRESS_SDIO  0x0
#define COMPARE_ADDRESS_SD_COMBO  0x2000

#define MSDC_MULTI_BUF_LEN  (4*1024)
u32 msdc_multi_wbuf[MSDC_MULTI_BUF_LEN];
u32 msdc_multi_rbuf[MSDC_MULTI_BUF_LEN];

#define is_card_present(h)     (((struct msdc_host*)(h))->card_inserted)
static int sd_multi_rw_compare_slave(int host_num, int read, uint address)
{
	struct scatterlist msdc_sg;
	struct mmc_data  msdc_data;
	struct mmc_command msdc_cmd;
	struct mmc_command msdc_stop;
	struct mmc_request  msdc_mrq;
	struct msdc_host *host_ctl;
	//struct msdc_host *host = msdc_6577_host[host_num];
	int result = 0, forIndex = 0;
	u8 *wPtr;
	u8 wData[16]= {
			0x67, 0x45, 0x23, 0x01,
			0xef, 0xcd, 0xab, 0x89,
			0xce, 0x8a, 0x46, 0x02,
			0xde, 0x9b, 0x57, 0x13 }; 
	
	host_ctl = msdc_6577_host[host_num];
	if(!host_ctl || !host_ctl->mmc || !host_ctl->mmc->card)
	{
		printk(" there is no card initialized in host[%d]\n",host_num);
		return -1; 
	}

	if(!is_card_present(host_ctl))
	{
		printk("  [%s]: card is removed!\n", __func__);
		return -1;
	}

	mmc_claim_host(host_ctl->mmc);

	memset(&msdc_data, 0, sizeof(struct mmc_data));
	memset(&msdc_mrq, 0, sizeof(struct mmc_request));
	memset(&msdc_cmd, 0, sizeof(struct mmc_command));
	memset(&msdc_stop, 0, sizeof(struct mmc_command));

	msdc_mrq.cmd = &msdc_cmd;
	msdc_mrq.data = &msdc_data;

	if (read){
		//init read command
		msdc_data.flags = MMC_DATA_READ;
		msdc_cmd.opcode = MMC_READ_MULTIPLE_BLOCK;
		msdc_data.blocks = sizeof(msdc_multi_rbuf) / 512;
		wPtr =(u8*)msdc_multi_rbuf;
		//init read buffer
		for(forIndex=0;forIndex<MSDC_MULTI_BUF_LEN*4;forIndex++)
				*(wPtr + forIndex) = 0x0;						
		//for(forIndex=0;forIndex<MSDC_MULTI_BUF_LEN;forIndex++)
				//printk("R_buffer[0x%x] \n",msdc_multi_rbuf[forIndex]);
	} else {
		//init write command
		msdc_data.flags = MMC_DATA_WRITE;
		msdc_cmd.opcode = MMC_WRITE_MULTIPLE_BLOCK;
		msdc_data.blocks = sizeof(msdc_multi_wbuf) / 512;
		//init write buffer
		wPtr =(u8*)msdc_multi_wbuf;
		for(forIndex=0;forIndex<MSDC_MULTI_BUF_LEN*4;forIndex++)
				*(wPtr + forIndex) = wData[forIndex%16];				
		//for(forIndex=0;forIndex<MSDC_MULTI_BUF_LEN;forIndex++)
				//printk("W_buffer[0x%x]\n",msdc_multi_wbuf[forIndex]);
	}
	
	msdc_cmd.arg = address;

	BUG_ON(!host_ctl->mmc->card);
	if (!mmc_card_blockaddr(host_ctl->mmc->card)){
		//printk("this device use byte address!!\n");
		msdc_cmd.arg <<= 9;
	}
	msdc_cmd.flags = MMC_RSP_SPI_R1 | MMC_RSP_R1 | MMC_CMD_ADTC;

	msdc_stop.opcode = MMC_STOP_TRANSMISSION;
	msdc_stop.arg = 0;
	msdc_stop.flags = MMC_RSP_SPI_R1B | MMC_RSP_R1B | MMC_CMD_AC;

	msdc_data.stop = &msdc_stop;
	msdc_data.blksz = 512;
	msdc_data.sg = &msdc_sg;
	msdc_data.sg_len = 1;

	if(read)
		sg_init_one(&msdc_sg, msdc_multi_rbuf, sizeof(msdc_multi_rbuf));
	else
		sg_init_one(&msdc_sg, msdc_multi_wbuf, sizeof(msdc_multi_wbuf));
		
	mmc_set_data_timeout(&msdc_data, host_ctl->mmc->card);
	mmc_wait_for_req(host_ctl->mmc, &msdc_mrq);

	if (read){
		for(forIndex=0;forIndex<MSDC_MULTI_BUF_LEN;forIndex++){					
			//printk("index[%d]\tW_buffer[0x%x]\tR_buffer[0x%x]\t\n", forIndex, msdc_multi_wbuf[forIndex], msdc_multi_rbuf[forIndex]);
			if(msdc_multi_wbuf[forIndex]!=msdc_multi_rbuf[forIndex]){
				printk("index[%d]\tW_buffer[0x%x]\tR_buffer[0x%x]\tfailed\n", forIndex, msdc_multi_wbuf[forIndex], msdc_multi_rbuf[forIndex]);
				result =-1;
			}
		}
		/*if(result == 0)
			printk("pid[%d][%s]: data compare successed!!\n", current->pid, __func__);
		else					
			printk("pid[%d][%s]: data compare failed!! \n", current->pid, __func__);*/
	}

	mmc_release_host(host_ctl->mmc);
  
	if (msdc_cmd.error)
		result = msdc_cmd.error;

	if (msdc_data.error){
		result = msdc_data.error;
	} else {
		result = 0;
	}

	return result;
}

static int sd_multi_rw_compare(int host_num, uint address, int count)
{
	int i=0, j=0;
	int error = 0;
	
	for(i=0; i<count; i++) 
	{
		//printk("============ cpu[%d] pid[%d]: start the %d time compare ============\n", task_cpu(current), current->pid, i);

		error = sd_multi_rw_compare_slave(host_num, 0, address); //write
		if(error)
		{
			printk("[%s]: failed to write data, error=%d\n", __func__, error);
			break;
		}
	
		for(j=0; j<1; j++){
			error = sd_multi_rw_compare_slave(host_num, 1, address); //read
			if(error)
			{
				printk("[%s]: failed to read data, error=%d\n", __func__, error);
				break;
			}
		}
		if(error)
		   printk("============ cpu[%d] pid[%d]: FAILED the %d time compare ============\n", task_cpu(current), current->pid, i);
		else
		   printk("============ cpu[%d] pid[%d]: FINISH the %d time compare ============\n", task_cpu(current), current->pid, i);
	}
	
	if(i == count)
		printk("pid[%d]: successed to compare data within %d times\n", current->pid, count);
		
	return error;
}



#define MAX_THREAD_NUM_FOR_SMP 20

static uint smp_address_on_sd[MAX_THREAD_NUM_FOR_SMP] = 
{
  0x2000,
  0x20000, 
  0x200000,
  0x2000000,
  0x2200000,
  0x2400000,
  0x2800000,
  0x2c00000,
  0x4000000,
  0x4200000,
  0x4400000,
  0x4800000,
  0x4c00000,
  0x8000000,
  0x8200000,
  0x8400000,
  0x8800000,
  0x8c00000,
  0xc000000,
  0xc200000
};

static uint smp_address_on_mmc[MAX_THREAD_NUM_FOR_SMP] = 
{
  0x200,
  0x2000,
  0x20000, 
  0x200000,
  0x2000000,
  0x2200000,
  0x2400000,
  0x2800000,
  0x2c00000,
  0x4000000,
  0x4200000,
  0x4400000,
  0x4800000,
  0x4c00000,
  0x8000000,
  0x8200000,
  0x8400000,
  0x8800000,
  0x8c00000,
  0xc000000,
};

static uint smp_address_on_sd_combo[MAX_THREAD_NUM_FOR_SMP] = 
{
  0x2000,
  0x20000, 
  0x200000,
  0x2000000,
  0x2200000,
  0x2400000,
  0x2800000,
  0x2c00000,
  0x4000000,
  0x4200000,
  0x4400000,
  0x4800000,
  0x4c00000,
  0x8000000,
  0x8200000,
  0x8400000,
  0x8800000,
  0x8c00000,
  0xc000000,
  0xc200000
};
struct write_read_data{
    int host_id;        //the target host you want to do SMP test on.  
    uint start_address; //where you want to do write/read of the memory card 
    int count;          //how many times you want to do read after write bit by bit comparison 
};

static struct write_read_data wr_data[HOST_MAX_NUM][MAX_THREAD_NUM_FOR_SMP]; 
/*
 * 2012-03-25
 * the SMP thread function
 * do read after write the memory card, and bit by bit comparison
 */
static int write_read_thread(void* ptr)
{
    struct write_read_data* data = (struct write_read_data*)ptr;
    sd_multi_rw_compare(data->host_id, data->start_address, data->count); 
    return 0; 
}

/* 
 * 2012-03-25
 * function:         do SMP test on the same one MSDC host
 * thread_num:       the number of thread you want to trigger on this host.
 * host_id:          the target host you want to do SMP test on.
 * count:            how many times you want to do read after write bit by bit comparison in each thread.
 * multi_address:    whether do read/write the same/different address of the memory card in each thread.
 */
static int smp_test_on_one_host(int thread_num, int host_id, int count, int multi_address)
{
   int i=0, ret=0;
   char thread_name[128];
   struct msdc_host *host_ctl; 

   printk("============================[%s] start ================================\n\n", __func__);
   printk(" host %d run %d thread, each thread run %d RW comparison\n", host_id, thread_num, count);
   if(host_id >= HOST_MAX_NUM || host_id < 0)
   {
      printk(" bad host id: %d\n", host_id);
      ret = -1;
      goto out; 
   }
   if(thread_num > MAX_THREAD_NUM_FOR_SMP)// && (multi_address != 0))
   {
      printk(" too much thread for SMP test, thread_num=%d\n", thread_num);
      ret = -1;
      goto out; 
   }

   host_ctl = msdc_6577_host[host_id];
   if(!host_ctl || !host_ctl->mmc || !host_ctl->mmc->card)
   {
      printk(" there is no card initialized in host[%d]\n",host_id);
      ret = -1;
      goto out; 
   }


   for(i=0; i<thread_num; i++)
   {
       switch(host_ctl->mmc->card->type)
       {
         case MMC_TYPE_MMC:
            if(!multi_address)
               wr_data[host_id][i].start_address = COMPARE_ADDRESS_MMC;
            else
               wr_data[host_id][i].start_address = smp_address_on_mmc[i];
            if(i == 0)
               printk(" MSDC[%d], MMC:\n", host_id);
            break;
         case MMC_TYPE_SD:
            if(!multi_address)
               wr_data[host_id][i].start_address = COMPARE_ADDRESS_SD;
            else
               wr_data[host_id][i].start_address = smp_address_on_sd[i];
            if(i == 0)
               printk(" MSDC[%d], SD:\n", host_id);
            break;
         case MMC_TYPE_SDIO:
            if(i == 0)
            {
               printk(" MSDC[%d], SDIO:\n", host_id);
               printk("   please manually trigger wifi application instead of write/read something on SDIO card\n");
            }
            ret = -1;
            goto out;
         case MMC_TYPE_SD_COMBO:
            if(!multi_address)
               wr_data[host_id][i].start_address = COMPARE_ADDRESS_SD_COMBO;
            else
               wr_data[host_id][i].start_address = smp_address_on_sd_combo[i];
            if(i == 0)
               printk(" MSDC[%d], SD_COMBO:\n", host_id);
            break;
         default:
            if(i == 0)
               printk(" MSDC[%d], cannot recognize this card\n", host_id);
            ret = -1;
            goto out;
       }
       wr_data[host_id][i].host_id = host_id;
       wr_data[host_id][i].count = count;
       sprintf(thread_name, "msdc_H%d_T%d", host_id, i); 
       kthread_run(write_read_thread, &wr_data[host_id][i], thread_name);
       printk("   start thread: %s, at address 0x%x\n", thread_name, wr_data[host_id][i].start_address);
   }
out: 
   printk("============================[%s] end ================================\n\n", __func__);
   return ret;
} 
  
/* 
 * 2012-03-25
 * function:         do SMP test on all MSDC hosts
 * thread_num:       the number of thread you want to trigger on this host.
 * count:            how many times you want to do read after write bit by bit comparison in each thread.
 * multi_address:    whether do read/write the same/different address of the memory card in each thread.
 */
static int smp_test_on_all_host(int thread_num, int count, int multi_address)
{
   int i=0;
   int j=0;
   int ret=0;
   char thread_name[128];
   struct msdc_host *host_ctl;
   
   printk("============================[%s] start ================================\n\n", __func__);
   printk(" each host run %d thread, each thread run %d RW comparison\n", thread_num, count);
   if(thread_num > MAX_THREAD_NUM_FOR_SMP) //&& (multi_address != 0))
   {
      printk(" too much thread for SMP test, thread_num=%d\n", thread_num);
      ret = -1;
      goto out;
   }

   for(i=0; i<HOST_MAX_NUM; i++)
   {
      host_ctl = msdc_6577_host[i];
      if(!host_ctl || !host_ctl->mmc || !host_ctl->mmc->card)
      {
         printk(" MSDC[%d], no card is initialized\n", i);
         continue;
      }
      if(host_ctl->mmc->card->type == MMC_TYPE_SDIO)
      {
         printk(" MSDC[%d], SDIO, please manually trigger wifi application instead of write/read something on SDIO card\n", i);
         continue;
      }
      for(j=0; j<thread_num; j++)
      {
         wr_data[i][j].host_id = i;
         wr_data[i][j].count = count;
         switch(host_ctl->mmc->card->type)
         {
            case MMC_TYPE_MMC:
               if(!multi_address)
                  wr_data[i][j].start_address = COMPARE_ADDRESS_MMC;
               else
                  wr_data[i][j].start_address = smp_address_on_mmc[i];
               if(j == 0)
                  printk(" MSDC[%d], MMC:\n ", i);
               break;
            case MMC_TYPE_SD:
               if(!multi_address)
                  wr_data[i][j].start_address = COMPARE_ADDRESS_SD;
               else
                  wr_data[i][j].start_address = smp_address_on_sd[i];
               if(j == 0)
                  printk(" MSDC[%d], SD:\n", i);
               break;
            case MMC_TYPE_SDIO:
               if(j == 0)
               {
                  printk(" MSDC[%d], SDIO:\n", i);
                  printk("   please manually trigger wifi application instead of write/read something on SDIO card\n");
               }
               ret = -1;
               goto out;
            case MMC_TYPE_SD_COMBO:
               if(!multi_address)
                  wr_data[i][j].start_address = COMPARE_ADDRESS_SD_COMBO;
               else
                  wr_data[i][j].start_address = smp_address_on_sd_combo[i];
               if(j == 0)
                  printk(" MSDC[%d], SD_COMBO:\n", i);
               break;
            default:
               if(j == 0)
                  printk(" MSDC[%d], cannot recognize this card\n", i);
               ret = -1;
               goto out;
         }
         sprintf(thread_name, "msdc_H%d_T%d", i, j); 
         kthread_run(write_read_thread, &wr_data[i][j], thread_name);
         printk("   start thread: %s, at address: 0x%x\n", thread_name, wr_data[i][j].start_address);
      }
   }
out:
   printk("============================[%s] end ================================\n\n", __func__);
   return ret;
}


static int msdc_help_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;

    p += sprintf(p, "\n====================[msdc_help]=====================\n");

    p += sprintf(p, "\n   LOG control:           echo %x [host_id] [debug_zone] > msdc_debug\n", SD_TOOL_ZONE);
    p += sprintf(p, "          [debug_zone]       DMA:0x1,  CMD:0x2,  RSP:0x4,   INT:0x8,   CFG:0x10,  FUC:0x20,\n");
    p += sprintf(p, "                             OPS:0x40, FIO:0x80, WRN:0x100, PWR:0x200, CLK:0x400, RW:0x1000, NRW:0x2000\n");
    p += sprintf(p, "\n   DMA mode:\n");
    p += sprintf(p, "          set DMA mode:      echo %x 0 [host_id] [dma_mode] [dma_size] > msdc_debug\n", SD_TOOL_DMA_SIZE);
    p += sprintf(p, "          get DMA mode:      echo %x 1 [host_id] > msdc_debug\n", SD_TOOL_DMA_SIZE);
    p += sprintf(p, "            [dma_mode]       0:PIO, 1:DMA, 2:SIZE_DEP\n");
    p += sprintf(p, "            [dma_size]       valid for SIZE_DEP mode, the min size can trigger the DMA mode\n");
    p += sprintf(p, "\n   SDIO profile:          echo %x [enable] [time] > msdc_debug\n", SD_TOOL_SDIO_PROFILE);
    p += sprintf(p, "\n   CLOCK control:  \n");
    p += sprintf(p, "          set clk src:       echo %x 0 [host_id] [clk_src] > msdc_debug\n", SD_TOOL_CLK_SRC_SELECT);
    p += sprintf(p, "          get clk src:       echo %x 1 [host_id] > msdc_debug\n", SD_TOOL_CLK_SRC_SELECT);
    p += sprintf(p, "            [clk_src]        0:SYS PLL(26Mhz), 1:3G PLL(197Mhz), 2:AUD PLL(208Mhz)\n");
    p += sprintf(p, "\n   REGISTER control:\n");
    p += sprintf(p, "          write register:    echo %x 0 [host_id] [register_offset] [value] > msdc_debug\n", SD_TOOL_REG_ACCESS);
    p += sprintf(p, "          read register:     echo %x 1 [host_id] [register_offset] > msdc_debug\n", SD_TOOL_REG_ACCESS);
    p += sprintf(p, "          write mask:        echo %x 2 [host_id] [register_offset] [start_bit] [len] [value] > msdc_debug\n", SD_TOOL_REG_ACCESS);
    p += sprintf(p, "          read mask:         echo %x 3 [host_id] [register_offset] [start_bit] [len] > msdc_debug\n", SD_TOOL_REG_ACCESS);
    p += sprintf(p, "          dump all:          echo %x 4 [host_id]> msdc_debug\n", SD_TOOL_REG_ACCESS);
    p += sprintf(p, "\n   DRVING control: \n"); 
    p += sprintf(p, "          set driving:       echo %x 0 [host_id] [clk_drv] [cmd_drv] [dat_drv] > msdc_debug\n", SD_TOOL_SET_DRIVING);
    p += sprintf(p, "          get driving:       echo %x 1 [host_id] > msdc_debug\n", SD_TOOL_SET_DRIVING);
    p += sprintf(p, "\n   DESENSE control: \n");
    p += sprintf(p, "          write register:    echo %x 0 [value] > msdc_debug\n", SD_TOOL_DESENSE);
    p += sprintf(p, "          read register:     echo %x 1 > msdc_debug\n", SD_TOOL_DESENSE);
    p += sprintf(p, "          write mask:        echo %x 2 [start_bit] [len] [value] > msdc_debug\n", SD_TOOL_DESENSE);
    p += sprintf(p, "          read mask:         echo %x 3 [start_bit] [len] > msdc_debug\n", SD_TOOL_DESENSE);
    p += sprintf(p, "\n   RW_COMPARE test:       echo %x [host_id] [compare_count] > msdc_debug\n", RW_BIT_BY_BIT_COMPARE);
    p += sprintf(p, "          [compare_count]    how many time you want to \"write=>read=>compare\"\n");
    p += sprintf(p, "\n   SMP_ON_ONE_HOST test:  echo %x [host_id] [thread_num] [compare_count] [multi_address] > msdc_debug\n", SMP_TEST_ON_ONE_HOST);
    p += sprintf(p, "          [thread_num]       how many R/W comparision thread you want to run at host_id\n");
    p += sprintf(p, "          [compare_count]    how many time you want to \"write=>read=>compare\" in each thread\n");
    p += sprintf(p, "          [multi_address]    whether read/write different address in each thread, 0:No, 1:Yes\n");
    p += sprintf(p, "\n   SMP_ON_ALL_HOST test:  echo %x [thread_num] [compare_count] [multi_address] > msdc_debug\n", SMP_TEST_ON_ALL_HOST);
    p += sprintf(p, "          [thread_num]       how many R/W comparision thread you want to run at each host\n");
    p += sprintf(p, "          [compare_count]    how many time you want to \"write=>read=>compare\" in each thread\n");
    p += sprintf(p, "          [multi_address]    whether read/write different address in each thread, 0:No, 1:Yes\n");
    p += sprintf(p, "\n   SPEED_MODE control:\n"); 
    p += sprintf(p, "          set speed mode:    echo %x 0 [host_id] [speed_mode] [driver_type] [max_current] [power_control] > msdc_debug\n", SD_TOOL_MSDC_HOST_MODE);
    p += sprintf(p, "          get speed mode:    echo %x 1 [host_id]\n", SD_TOOL_MSDC_HOST_MODE);
    p += sprintf(p, "            [speed_mode]       0:N/A,  1:HS,      2:SDR12,   3:SDR25,   4:SDR:50,  5:SDR104,  6:DDR\n");
    p += sprintf(p, "            [driver_type]      0:N/A,  7:type A,  8:type B,  9:type C,  a:type D\n");
    p += sprintf(p, "            [max_current]      0:N/A,  b:200mA,   c:400mA,   d:600mA,   e:800mA\n");
    p += sprintf(p, "            [power_control]    0:N/A,  f:disable, 10:enable\n");
    p += sprintf(p, "\n   DMA viloation:         echo %x [host_id] [ops]> msdc_debug\n", SD_TOOL_DMA_STATUS);
    p += sprintf(p, "          [ops]              0:get latest dma address,  1:start violation test\n");
    p += sprintf(p, "\n   NOTE: All input data is Hex number! \n");

    p += sprintf(p, "\n======================================================\n\n");

    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len : count;
}

//========== driver proc interface ===========
static int msdc_debug_proc_read(char *page, char **start, off_t off, int count, int *eof, void *data)
{
    char *p = page;
    int len = 0;

    p += sprintf(p, "\n=========================================\n");

    p += sprintf(p, "Index<0> + Id + Zone\n");   
    p += sprintf(p, "-> PWR<9> WRN<8> | FIO<7> OPS<6> FUN<5> CFG<4> | INT<3> RSP<2> CMD<1> DMA<0>\n");        
    p += sprintf(p, "-> echo 0 3 0x3ff >msdc_bebug -> host[3] debug zone set to 0x3ff\n");              
    p += sprintf(p, "-> MSDC[0] Zone: 0x%.8x\n", sd_debug_zone[0]);
    p += sprintf(p, "-> MSDC[1] Zone: 0x%.8x\n", sd_debug_zone[1]);
    p += sprintf(p, "-> MSDC[2] Zone: 0x%.8x\n", sd_debug_zone[2]);
    p += sprintf(p, "-> MSDC[3] Zone: 0x%.8x\n", sd_debug_zone[3]);

    p += sprintf(p, "Index<1> + ID:4|Mode:4 + DMA_SIZE\n");  
    p += sprintf(p, "-> 0)PIO 1)DMA 2)SIZE\n");      
    p += sprintf(p, "-> echo 1 22 0x200 >msdc_bebug -> host[2] size mode, dma when >= 512\n");        
    p += sprintf(p, "-> MSDC[0] mode<%d> size<%d>\n", drv_mode[0], dma_size[0]);
    p += sprintf(p, "-> MSDC[1] mode<%d> size<%d>\n", drv_mode[1], dma_size[1]);    
    p += sprintf(p, "-> MSDC[2] mode<%d> size<%d>\n", drv_mode[2], dma_size[2]);
    p += sprintf(p, "-> MSDC[3] mode<%d> size<%d>\n", drv_mode[3], dma_size[3]);

    p += sprintf(p, "Index<3> + SDIO_PROFILE + TIME\n"); 
    p += sprintf(p, "-> echo 3 1 0x1E >msdc_bebug -> enable sdio_profile, 30s\n"); 
    p += sprintf(p, "-> SDIO_PROFILE<%d> TIME<%ds>\n", sdio_pro_enable, sdio_pro_time); 
    p += sprintf(p, "-> Clokc SRC selection Host[0]<%d>\n", msdc_clock_src[0]); 
	p += sprintf(p, "-> Clokc SRC selection Host[1]<%d>\n", msdc_clock_src[1]); 
	p += sprintf(p, "-> Clokc SRC selection Host[2]<%d>\n", msdc_clock_src[2]); 
	p += sprintf(p, "-> Clokc SRC selection Host[3]<%d>\n", msdc_clock_src[3]); 
    p += sprintf(p, "-> Driving mode Host[0] clk_drv<%d> cmd_drv<%d> dat_drv<%d>\n", msdc_drv_mode[0].clk_drv,msdc_drv_mode[0].cmd_drv,msdc_drv_mode[0].dat_drv); 
	p += sprintf(p, "-> Driving mode Host[1] clk_drv<%d> cmd_drv<%d> dat_drv<%d>\n", msdc_drv_mode[1].clk_drv,msdc_drv_mode[1].cmd_drv,msdc_drv_mode[1].dat_drv); 
	p += sprintf(p, "-> Driving mode Host[2] clk_drv<%d> cmd_drv<%d> dat_drv<%d>\n", msdc_drv_mode[2].clk_drv,msdc_drv_mode[2].cmd_drv,msdc_drv_mode[2].dat_drv); 
	p += sprintf(p, "-> Driving mode Host[3] clk_drv<%d> cmd_drv<%d> dat_drv<%d>\n", msdc_drv_mode[3].clk_drv,msdc_drv_mode[3].cmd_drv,msdc_drv_mode[3].dat_drv); 

    p += sprintf(p, "=========================================\n\n");
    
    *start = page + off;

    len = p - page;
    if (len > off)
        len -= off;
    else
        len = 0;

    return len < count ? len : count;
} 

static int msdc_debug_proc_write(struct file *file, const char *buf, unsigned long count, void *data)
{
	int ret;
	
	int cmd, p1, p2, p3, p4, p5, p6;   
	int id, zone;
	int mode, size;  
	int thread_num, compare_count, multi_address;
	unsigned int base;
	unsigned int offset = 0;
	unsigned int reg_value;	
	HOST_CAPS spd_mode = MODE_NULL;
	HOST_CAPS drv_type = MODE_NULL;
	HOST_CAPS current_limit = MODE_NULL;
	HOST_CAPS pw_cr = MODE_NULL;
	struct dma_addr *dma_address, *p_dma_address;
	int dma_status;
	
	if (count == 0)return -1;
	if(count > 255)count = 255;

	ret = copy_from_user(cmd_buf, buf, count);
	if (ret < 0)return -1;
	
	cmd_buf[count] = '\0';
	printk("[****SD_Debug****]msdc Write %s\n", cmd_buf);

	sscanf(cmd_buf, "%x %x %x %x %x %x %x", &cmd, &p1, &p2, &p3, &p4, &p5, &p6);
	
	if(cmd == SD_TOOL_ZONE) {
		id = p1; zone = p2; //zone &= 0x3ff;		
		printk("[****SD_Debug****]msdc host_id<%d> zone<0x%.8x>\n", id, zone);
		if(id >=0 && id<=HOST_MAX_NUM-1){
			sd_debug_zone[id] = zone;
		}
		else if(id == HOST_MAX_NUM){
			sd_debug_zone[0] = sd_debug_zone[1] = zone;
			sd_debug_zone[2] = sd_debug_zone[3] = zone;
		}
		else{
			printk("[****SD_Debug****]msdc host_id error when set debug zone\n");
		}
	} else if (cmd == SD_TOOL_DMA_SIZE) {
		id = p2;  mode = p3; size = p4; 
		if(id >=0 && id<=HOST_MAX_NUM-1){
			if(p1==0){
		      drv_mode[id] = mode;
		      dma_size[id] = size; 
			}else{
		      printk("-> MSDC[%d] mode<%d> size<%d>\n", id, drv_mode[id], dma_size[id]);
			}
		}
		else{
			printk("[****SD_Debug****]msdc host_id error when select mode\n");
		}	
	} else if (cmd == SD_TOOL_SDIO_PROFILE) {
		if (p1 == 1) { /* enable profile */
			if (gpt_enable == 0) {
				msdc_init_gpt();
				gpt_enable = 1;
			} 
			sdio_pro_enable = 1;
			if (p2 == 0) p2 = 1; if (p2 >= 30) p2 = 30; 				
			sdio_pro_time = p2 ; 
		}	else if (p1 == 0) {
			/* todo */
			sdio_pro_enable = 0;
		}			
	}else if (cmd == SD_TOOL_CLK_SRC_SELECT){
		id = p2;
		if(id>=0 && id < HOST_MAX_NUM){ 
			if(p1 == 0){
			   if(p3 >= 0 && p3< CLK_SRC_MAX_NUM){
			      msdc_clock_src[id] = p3;
			      printk("[****SD_Debug****]msdc%d's pll source changed to %d\n", id, msdc_clock_src[id]);
			      printk("[****SD_Debug****]to enable the above settings, please suspend and resume the phone again\n");
			   }else {
			      printk("[****SD_Debug****] invalide clock src id:%d, check /proc/msdc_help\n", p3);
			   }
			}
			else if(p1 == 1){
				switch(id)
				{
					case 0:
						printk("[****SD_Debug****]msdc%d's pll source is %d\n", id, ((sdr_read32(MSDC_CLKSRC_REG) & 0x3) >> 0));
						break;
					case 1:
						printk("[****SD_Debug****]msdc%d's pll source is %d\n", id, ((sdr_read32(MSDC_CLKSRC_REG) & 0x18) >> 3));
						break;
					case 2:
						printk("[****SD_Debug****]msdc%d's pll source is %d\n", id, ((sdr_read32(MSDC_CLKSRC_REG) & 0x60) >> 5));
						break;
					case 3:
						printk("[****SD_Debug****]msdc%d's pll source is %d\n", id, ((sdr_read32(MSDC_CLKSRC_REG) & 0x180) >> 7));
						break;
		 		}				
			}
		}
		else
			printk("[****SD_Debug****]msdc host_id error when select clock source\n");
	}else if (cmd == SD_TOOL_REG_ACCESS){
		id = p2;
		offset = (unsigned int)p3;
		
		if(id >= HOST_MAX_NUM || id < 0)
			printk("[****SD_Debug****]msdc host_id error when modify msdc reg\n");
		else{
			if(id == 0)
				base = MSDC_0_BASE;
			if(id == 1)
				base = MSDC_1_BASE;
			if(id == 2)
				base = MSDC_2_BASE;
			if(id == 3)
				base = MSDC_3_BASE;				
			
			if((offset == 0x18 || offset == 0x1C) && p1 != 4){
				printk("[****SD_Debug****]Err: Accessing TXDATA and RXDATA is forbidden\n");
				return count;
			}
#ifndef FPGA_PLATFORM
			enable_clock(PERI_MSDC0_PDN + id, "SD"); 
#endif
			if(p1 == 0){
				reg_value = p4;
				printk("[****SD_Debug****][MSDC Reg]Original:0x%x+0x%x (0x%x)\n",base,offset,sdr_read32(base+offset));
				sdr_write32(base+offset,reg_value );
				printk("[****SD_Debug****][MSDC Reg]Modified:0x%x+0x%x (0x%x)\n",base,offset,sdr_read32(base+offset));
			}
			else if(p1 == 1)
				printk("[****SD_Debug****][MSDC Reg]Reg:0x%x+0x%x (0x%x)\n",base,offset,sdr_read32(base+offset));
			else if(p1 == 2)
				msdc_set_field(base+offset,p4,p5,p6);
			else if(p1 == 3)
				msdc_get_field(base+offset,p4,p5,p6);
			else if(p1 == 4)
				msdc_dump_reg(base);
#ifndef FPGA_PLATFORM
			disable_clock(PERI_MSDC0_PDN + id, "SD"); 
#endif
		}
			
	} 
	else if(cmd == SD_TOOL_SET_DRIVING){
		id = p2;
		if(id >= HOST_MAX_NUM || id < 0)
		    printk("[****SD_Debug****]msdc host_id error when modify msdc driving\n");
		else{
		    if(p1 == 0){
          if((unsigned char)p3 > 7 || (unsigned char)p4 > 7 ||(unsigned char)p5 > 7)
             printk("[****SD_Debug****]Some drving value was not right(correct:0~7)\n");
          else{
             msdc_drv_mode[id].clk_drv = (unsigned char)p3;
             msdc_drv_mode[id].cmd_drv = (unsigned char)p4;
             msdc_drv_mode[id].dat_drv = (unsigned char)p5;
             printk("[****SD_Debug****]msdc%d, you'd better suspend and resume the device, to make following setting valid\n", id);
             printk("[****SD_Debug****]clk_drv=%d, cmd_drv=%d, dat_drv=%d\n", p3, p4, p5);
          }
		    }else if(p1 == 1){
          printk("[****SD_Debug****]msdc%d: clk_drv=%d, cmd_drv=%d, dat_drv=%d\n", id, msdc_drv_mode[id].clk_drv, msdc_drv_mode[id].cmd_drv, msdc_drv_mode[id].dat_drv);     
		    }
		}
	}
	else if(cmd == SD_TOOL_DESENSE){
		if(p1 == 0){
			reg_value = p2;
			printk("[****SD_Debug****][De-Sense Reg]Original:0x%x(0x%x)\n",MSDC_DESENSE_REG,sdr_read32(MSDC_DESENSE_REG));
			sdr_write32(MSDC_DESENSE_REG,reg_value);
			printk("[****SD_Debug****][De-Sense Reg]Modified:0x%x(0x%x)\n",MSDC_DESENSE_REG,sdr_read32(MSDC_DESENSE_REG));
		}
		else if(p1 == 1){
			printk("[****SD_Debug****][De-Sense Reg]Reg:0x%x(0x%x)\n",MSDC_DESENSE_REG,sdr_read32(MSDC_DESENSE_REG));
		}else if(p1 == 2){
			msdc_set_field(MSDC_DESENSE_REG,p2,p3,p4);
		}else if(p1 == 3){
			msdc_get_field(MSDC_DESENSE_REG,p2,p3,p4);
		}
	}else if(cmd == RW_BIT_BY_BIT_COMPARE){
		id = p1;
		compare_count = p2;
		if(id >= HOST_MAX_NUM || id < 0)
		{
	  	   printk("[****SD_Debug****]: bad host id: %d\n", id);
	  	   return 0;
		}
		if(compare_count < 0)
		{
	  	   printk("[****SD_Debug****]: bad compare count: %d\n", compare_count);
	  	   return 0;
		}
               
		if(id == 0) //for msdc0
		{
#ifdef MTK_EMMC_SUPPORT
		   sd_multi_rw_compare(0, COMPARE_ADDRESS_MMC, compare_count);//test the address 0 of eMMC card, since there a little memory.
#else
		   sd_multi_rw_compare(0, COMPARE_ADDRESS_SD, compare_count); //test a larger address of SD card
#endif
		}else {
	  	   sd_multi_rw_compare(id, COMPARE_ADDRESS_SD, compare_count);
		}
	}
	else if (cmd == SMP_TEST_ON_ONE_HOST){
		id = p1;
		thread_num = p2;
		compare_count = p3;
		multi_address = p4;
		smp_test_on_one_host(thread_num, id, compare_count, multi_address); 
	}
	else if (cmd == SMP_TEST_ON_ALL_HOST){
		thread_num = p1;
		compare_count = p2;
		multi_address = p3;
		smp_test_on_all_host(thread_num, compare_count, multi_address); 
	}
	else if(cmd == SD_TOOL_MSDC_HOST_MODE){
		id = p2;
		if(id >= HOST_MAX_NUM || id < 0)
			printk("[****SD_Debug****]msdc host_id error when modify msdc host mode\n");
		else {
			if(p1 == 0){
				if(p3 <= UHS_DDR50 && p3 >= SDHC_HIGHSPEED)
					spd_mode = p3;
				if(p4 <= DRIVER_TYPE_D && p4 >= DRIVER_TYPE_A)
					drv_type = p4;
				if(p5 <= MAX_CURRENT_800 && p5 >= MAX_CURRENT_200 )
					current_limit = p5;
				if(p6 <= SDXC_POWER_CONTROL && p6 >= SDXC_NO_POWER_CONTROL)
					pw_cr = p6;
				if(spd_mode != 0){
					switch(spd_mode){
						case SDHC_HIGHSPEED  : msdc_host_mode[id] |= MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED;
					  	  msdc_host_mode[id] &= (~MMC_CAP_UHS_SDR12)&(~MMC_CAP_UHS_SDR25)&(~MMC_CAP_UHS_SDR50)&(~MMC_CAP_UHS_DDR50)&(~MMC_CAP_UHS_SDR104);
					  	  printk("[****SD_Debug****]host will support Highspeed\n");
					  	  break;
						case UHS_SDR12  : msdc_host_mode[id] |= MMC_CAP_UHS_SDR12;
					  	  msdc_host_mode[id] &= (~MMC_CAP_UHS_SDR25)&(~MMC_CAP_UHS_SDR50)&(~MMC_CAP_UHS_DDR50)&(~MMC_CAP_UHS_SDR104);
					  	  printk("[****SD_Debug****]host will support UHS-SDR12\n");
					  	  break;
						case UHS_SDR25  : msdc_host_mode[id] |= MMC_CAP_UHS_SDR12|MMC_CAP_UHS_SDR25;
					  	  msdc_host_mode[id] &= (~MMC_CAP_UHS_SDR50)&(~MMC_CAP_UHS_DDR50)&(~MMC_CAP_UHS_SDR104);
					  	  printk("[****SD_Debug****]host will support UHS-SDR25\n");
					  	  break;
						case UHS_SDR50  : msdc_host_mode[id] |= MMC_CAP_UHS_SDR12|MMC_CAP_UHS_SDR25|MMC_CAP_UHS_SDR50;
					  	  msdc_host_mode[id] &= (~MMC_CAP_UHS_DDR50)&(~MMC_CAP_UHS_SDR104);
					  	  printk("[****SD_Debug****]host will support UHS-SDR50\n");
					  	  break;
						case UHS_SDR104 : msdc_host_mode[id] |= MMC_CAP_UHS_SDR12|MMC_CAP_UHS_SDR25|MMC_CAP_UHS_SDR50|MMC_CAP_UHS_SDR104;
					  	  printk("[****SD_Debug****]host will support UHS-SDR104\n");
					  	  break;
						case UHS_DDR50  : msdc_host_mode[id] |= MMC_CAP_UHS_SDR12|MMC_CAP_UHS_SDR25|MMC_CAP_UHS_DDR50;
					  	  msdc_host_mode[id] &= (~MMC_CAP_UHS_SDR50)&(~MMC_CAP_UHS_SDR104);
					  	  printk("[****SD_Debug****]host will support UHS-DDR50\n");
					  	  break;
						default: break;
						}
					}
				if(drv_type != 0){
					switch(drv_type){
						case DRIVER_TYPE_A : msdc_host_mode[id] |= MMC_CAP_DRIVER_TYPE_A;
					  	  msdc_host_mode[id] &= (~MMC_CAP_DRIVER_TYPE_C)&(~MMC_CAP_DRIVER_TYPE_D);
					  	  printk("[****SD_Debug****]host will support DRIVING TYPE A\n");
					  	  break;
						case DRIVER_TYPE_B : msdc_host_mode[id] &= (~MMC_CAP_DRIVER_TYPE_A)&(~MMC_CAP_DRIVER_TYPE_C)&(~MMC_CAP_DRIVER_TYPE_D);
					  	  printk("[****SD_Debug****]host will support DRIVING TYPE B\n");
					  	  break;
						case DRIVER_TYPE_C : msdc_host_mode[id] |= MMC_CAP_DRIVER_TYPE_C;
					  	  msdc_host_mode[id] &= (~MMC_CAP_DRIVER_TYPE_A)&(~MMC_CAP_DRIVER_TYPE_D);
					  	  printk("[****SD_Debug****]host will support DRIVING TYPE C\n");
					  	  break;
						case DRIVER_TYPE_D : msdc_host_mode[id] |= MMC_CAP_DRIVER_TYPE_D;
					  	  msdc_host_mode[id] &= (~MMC_CAP_DRIVER_TYPE_A)&(~MMC_CAP_DRIVER_TYPE_C);
					  	  printk("[****SD_Debug****]host will support DRIVING TYPE D\n");
					  	  break;
						default: break;
						}
					}
				if(current_limit != 0){
					switch(current_limit){
						case MAX_CURRENT_200 : msdc_host_mode[id] |= MMC_CAP_MAX_CURRENT_200;
						   msdc_host_mode[id] &= (~MMC_CAP_MAX_CURRENT_400)&(~MMC_CAP_MAX_CURRENT_600)&(~MMC_CAP_MAX_CURRENT_800);
						   printk("[****SD_Debug****]host will support MAX_CURRENT_200\n");
						   break;
						case MAX_CURRENT_400 : msdc_host_mode[id] |= MMC_CAP_MAX_CURRENT_200 | MMC_CAP_MAX_CURRENT_400;
						   msdc_host_mode[id] &= (~MMC_CAP_MAX_CURRENT_600)&(~MMC_CAP_MAX_CURRENT_800);
						   printk("[****SD_Debug****]host will support MAX_CURRENT_400\n");
						   break;
						case MAX_CURRENT_600 : msdc_host_mode[id] |= MMC_CAP_MAX_CURRENT_200 | MMC_CAP_MAX_CURRENT_400|MMC_CAP_MAX_CURRENT_600;
						   msdc_host_mode[id] &= (~MMC_CAP_MAX_CURRENT_800);
						   printk("[****SD_Debug****]host will support MAX_CURRENT_600\n");
						   break;
						case MAX_CURRENT_800 : msdc_host_mode[id] |= MMC_CAP_MAX_CURRENT_200 | MMC_CAP_MAX_CURRENT_400|MMC_CAP_MAX_CURRENT_600|MMC_CAP_MAX_CURRENT_800;
						   printk("[****SD_Debug****]host will support MAX_CURRENT_800\n");
						   break;
						default: break;
						}
					}
				if(pw_cr != 0)
					switch(pw_cr){
						case SDXC_NO_POWER_CONTROL : msdc_host_mode[id] &= (~MMC_CAP_SET_XPC_330) & (~MMC_CAP_SET_XPC_300) & (~MMC_CAP_SET_XPC_180);
						    printk("[****SD_Debug****]host will not support SDXC power control\n");
						    break;
						case SDXC_POWER_CONTROL    : msdc_host_mode[id] |= MMC_CAP_SET_XPC_330 | MMC_CAP_SET_XPC_300 | MMC_CAP_SET_XPC_180;
						     printk("[****SD_Debug****]host will support SDXC power control\n");
						     break;
						default: break;
					}
					printk("[****SD_Debug****]to enable the above settings, please suspend and resume the phone again\n");
			}else {
				printk("[****SD_Debug****]msdc[%d] supports: \n", id); 
				{
				   printk("[****SD_Debug****]      speed mode: ");
				   if((msdc_host_mode[id] & MMC_CAP_MMC_HIGHSPEED) || (msdc_host_mode[id] & MMC_CAP_SD_HIGHSPEED)) printk("HS, ");
				   if(msdc_host_mode[id] & MMC_CAP_UHS_SDR12) printk("SDR12, ");
				   if(msdc_host_mode[id] & MMC_CAP_UHS_SDR25) printk("SDR25, ");
				   if(msdc_host_mode[id] & MMC_CAP_UHS_SDR50) printk("SDR50, ");
				   if(msdc_host_mode[id] & MMC_CAP_UHS_SDR104) printk("SDR104, ");
				   if(msdc_host_mode[id] & MMC_CAP_UHS_DDR50) printk("DDR50 ");
				   if(!(msdc_host_mode[id] & (MMC_CAP_MMC_HIGHSPEED | MMC_CAP_SD_HIGHSPEED | MMC_CAP_UHS_SDR12 | MMC_CAP_UHS_SDR25 | MMC_CAP_UHS_SDR50 | MMC_CAP_UHS_SDR104 | MMC_CAP_UHS_DDR50))) printk("N/A");
				   printk("\n");
				}
				{			
				   printk("[****SD_Debug****]      driver_type: ");
				   if(msdc_host_mode[id] & MMC_CAP_DRIVER_TYPE_A) printk("A, ");
				   printk("B, ");
				   if(msdc_host_mode[id] & MMC_CAP_DRIVER_TYPE_C) printk("C, ");
				   if(msdc_host_mode[id] & MMC_CAP_DRIVER_TYPE_D) printk("D, ");
				   printk("\n");
				}
				{				
				   printk("[****SD_Debug****]      current limit: ");
				   if(msdc_host_mode[id] & MMC_CAP_MAX_CURRENT_200) printk("200mA, ");
				   if(msdc_host_mode[id] & MMC_CAP_MAX_CURRENT_400) printk("400mA, ");
				   if(msdc_host_mode[id] & MMC_CAP_MAX_CURRENT_600) printk("600mA, ");
				   if(msdc_host_mode[id] & MMC_CAP_MAX_CURRENT_800) printk("800mA, ");
				   if(!(msdc_host_mode[id] & (MMC_CAP_MAX_CURRENT_200 | MMC_CAP_MAX_CURRENT_400 | MMC_CAP_MAX_CURRENT_600 | MMC_CAP_MAX_CURRENT_800))) printk("N/A");
				   printk("\n");
				}
				{				
				   printk("[****SD_Debug****]      power control: ");
				   if(msdc_host_mode[id] & MMC_CAP_SET_XPC_330) printk("3.3v ");
				   if(msdc_host_mode[id] & MMC_CAP_SET_XPC_300) printk("3v ");
				   if(msdc_host_mode[id] & MMC_CAP_SET_XPC_180) printk("1.8v ");
				   if(!(msdc_host_mode[id] & (MMC_CAP_SET_XPC_330 | MMC_CAP_SET_XPC_300 | MMC_CAP_SET_XPC_180))) printk("N/A");
				   printk("\n");
				}
			}
		}
	}
	else if(cmd == SD_TOOL_DMA_STATUS)
	{
		id = p1;
		if(p2 == 0){
       dma_status = msdc_get_dma_status(id);
       printk(">>>> msdc%d: dma_status=%d, ", id, dma_status);
       if(dma_status == 0){
          printk("DMA mode is disabled Now\n");
       }else if (dma_status == 1){
          printk("Write data from SD to DRAM within DMA mode\n");
       }else if (dma_status == 2){
          printk("Write data from DRAM to SD within DMA mode\n");
       }else if (dma_status == -1){
          printk("No data transaction or the device is not present until now\n");
       }

       if(dma_status > 0)
       {
          dma_address = msdc_get_dma_address(id); 
          if(dma_address){
              printk(">>>> msdc%d: \n", id);
              p_dma_address = dma_address;
              while (p_dma_address){
                 printk(">>>>     addr=0x%x, size=%d\n", p_dma_address->start_address, p_dma_address->size);
                 if(p_dma_address->end)
                     break;
                 p_dma_address = p_dma_address->next;
              }
          }else {
              printk(">>>> msdc%d: BD count=0\n", id);
          }
       }
		}else if(p2 == 1){
       printk(">>>> msdc%d: start dma violation test\n", id);
       g_dma_debug[id] = 1;
       sd_multi_rw_compare(id, COMPARE_ADDRESS_SD, 3);
		}
	}

	return count;
}

int msdc_debug_proc_init(void) 
{   	
    struct proc_dir_entry *prEntry;

#ifdef USER_BUILD_KERNEL
    prEntry = create_proc_entry("msdc_debug", 0664, 0);
#else
    prEntry = create_proc_entry("msdc_debug", 0667, 0);
#endif
    if(prEntry)
    {
       prEntry->read_proc  = msdc_debug_proc_read;
       prEntry->write_proc = msdc_debug_proc_write;
       printk("[%s]: successfully create /proc/msdc_debug\n", __func__);
    }else{
       printk("[%s]: failed to create /proc/msdc_debug\n", __func__);
    }

#ifdef USER_BUILD_KERNEL
    prEntry = create_proc_entry("msdc_help", 0664, 0);
#else
    prEntry = create_proc_entry("msdc_help", 0667, 0);
#endif
    if(prEntry)
    {
       prEntry->read_proc = msdc_help_proc_read;
       printk("[%s]: successfully create /proc/msdc_help\n", __func__);
    }else{
       printk("[%s]: failed to create /proc/msdc_help\n", __func__);
    }

    memset(msdc_drv_mode,0,sizeof(msdc_drv_mode));
    return 0 ;
}
EXPORT_SYMBOL_GPL(msdc_debug_proc_init);
