/* 
 *
 * (C) Copyright 2010
 * MediaTek <www.mediatek.com>
 * Infinity Chen <infinity.chen@mediatek.com>
 *
 * mt6575 I2C Bus Controller
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <linux/kernel.h>
#include <linux/module.h>

#include <linux/slab.h>
#include <linux/i2c.h>
//	#include <linux/i2c-id.h>
#include <linux/init.h>
#include <linux/time.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/err.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/wait.h>
#include <linux/mm.h>
#include <asm/scatterlist.h>
#include <linux/scatterlist.h>
#include <asm/io.h>
#include <linux/aee.h>
//#include <mach/mt6577_typedefs.h>
#include <mach/mt6577_reg_base.h>
//#include <mach/mt6577_devs.h>
#include <mach/mt_gpio.h> 
#include <mach/system.h> 
#include <linux/hrtimer.h>
#include <linux/mt_sched_mon.h>

#define DRV_NAME    "mt-i2c"
//	#define I2C_DEBUG_LOG
//	#define MT_I2C_HRTIMER_DEBUG

#if (defined(CONFIG_MT6577_FPGA))

#else
#define I2C_CLOCK
#endif

#define I2C_WORKAROUND
#define I2C2_CH2_ERR(dev,addr) 												\
	do{																		\
		dev_err(dev, " %5d mt-i2c:(%x) using ch2.\n",__LINE__, addr);	\
		aee_kernel_exception("i2c dump","%5d Do not use i2c2 Channel 2, addr = %x!!",__LINE__,addr); \
	}while(0)

#ifdef I2C_CLOCK
#include <mach/mt6577_clock_manager.h>
#endif

enum i2c_trans_st_rs {
    I2C_TRANS_STOP = 0,
    I2C_TRANS_REPEATED_START,
};

enum mt_trans_op {
    I2C_MASTER_NONE = 0,
    I2C_MASTER_WR = 1,
    I2C_MASTER_RD,
    I2C_MASTER_WRRD,
};
 
//CONTROL
#define I2C_CONTROL_RS                  (0x1 << 1)	
#define	I2C_CONTROL_DMA_EN              (0x1 << 2)	
#define	I2C_CONTROL_CLK_EXT_EN          (0x1 << 3)
#define	I2C_CONTROL_DIR_CHANGE          (0x1 << 4)
#define	I2C_CONTROL_ACKERR_DET_EN       (0x1 << 5)
#define	I2C_CONTROL_TRANSFER_LEN_CHANGE (0x1 << 6)
#define	I2C_CONTROL_CH_WAIT_EN          (0x1 << 7)

#define I2C_WRITE_REG(base, offset, value) \
    __raw_writel(value, (base) + (offset))
#define I2C_READ_REG(base, offset) \
    __raw_readl((base) + (offset))


/******************************************************
	 	offset for I2C_REGS ADDRESS
******************************************************/

//the offset is based on 32-bit width
enum I2C_REGS_OFFSET {
	OFFSET_DATA_PORT        = 0x00, 
	OFFSET_SLAVE_ADDR       = 0x04,	
	OFFSET_INTR_MASK        = 0x08,	
	OFFSET_INTR_STAT        = 0x0C,	
	OFFSET_CONTROL          = 0x10,	
	OFFSET_TRANSFER_LEN     = 0x14,	
	OFFSET_TRANSAC_LEN      = 0x18,	
	OFFSET_DELAY_LEN        = 0x1C,	
	OFFSET_TIMING           = 0x20,	
	OFFSET_START            = 0x24,	
	OFFSET_FIFO_STAT        = 0x30,	
	OFFSET_FIFO_THRESH      = 0x34,	
	OFFSET_FIFO_ADDR_CLR    = 0x38,	
	OFFSET_IO_CONFIG        = 0x40,
	OFFSET_RSV_DEBUG        = 0x44,
	OFFSET_HS               = 0x48,	
	OFFSET_SOFTRESET        = 0x50,	
	OFFSET_DEBUGSTAT        = 0x64,	
	OFFSET_DEBUGCTRL        = 0x68,	
	OFFSET_CH2_DATA_PORT    = 0x80,	
	OFFSET_CH2_SLAVE_ADDR   = 0x84,	
	OFFSET_CH2_CONTROL      = 0x90,	
	OFFSET_CH2_TRANSFER_LEN = 0x94,	
	OFFSET_CH2_TRANSAC_LEN  = 0x98,	
	OFFSET_CH2_DELAY_LEN    = 0x9C,	
	OFFSET_CH2_TIMING       = 0xa0,
	OFFSET_CH2_START        = 0xa4,	
	OFFSET_CH2_FIFO_STAT    = 0xb0,
	OFFSET_CH2_FIFO_ADDR_CLR = 0xb8,
};

enum DMA_REGS_OFFSET {	
	OFFSET_INT_FLAG     = 0x0,
	OFFSET_INT_EN       = 0x04,
	OFFSET_EN           = 0x08,
	OFFSET_CON          = 0x18,
	OFFSET_MEM_ADDR     = 0x1c,
	OFFSET_LEN          = 0x24,
};

#define I2C_TRANSFER_LEN(len, aux)      (((len) & 0xFF) | (((aux) & 0x1F) << 8))
#define I2C_TRANSAC_LEN(num)            ((num) & 0xFF)
#define I2C_CH2_TRANSFER_LEN(len, aux)  (((len) & 0x07) | (((aux) & 0x07) << 8))
#define I2C_CH2_TRANSAC_LEN(num)        ((num) & 0x07)
#define	I2C_FIFO_STAT_LEN(n)            (((n) >> 4) & 0x0F)

#define I2C_FPGA_CLK_RATE       12000   /* khz used for i2c FPGA clock*/

#define I2C_FIFO_SIZE           8

#define MAX_ST_MODE_SPEED       100	 /* khz */
#define MAX_FS_MODE_SPEED       4062	 /* khz */
#define MAX_HS_MODE_SPEED       4062 /* khz */

#define MAX_DMA_TRANS_SIZE      252	/* Max(255) aligned to 4 bytes = 252 */
#define MAX_DMA_TRANS_NUM       256

#define MAX_SAMPLE_CNT_DIV      8
#define MAX_STEP_CNT_DIV        64
#define MAX_HS_STEP_CNT_DIV     8

#define mt_I2C_DATA_PORT            ((base) + 0x0000)
#define mt_I2C_SLAVE_ADDR           ((base) + 0x0004)
#define mt_I2C_INTR_MASK            ((base) + 0x0008)
#define mt_I2C_INTR_STAT            ((base) + 0x000c)
#define mt_I2C_CONTROL              ((base) + 0x0010)
#define mt_I2C_TRANSFER_LEN         ((base) + 0x0014)
#define mt_I2C_TRANSAC_LEN          ((base) + 0x0018)
#define mt_I2C_DELAY_LEN            ((base) + 0x001c)
#define mt_I2C_TIMING               ((base) + 0x0020)
#define mt_I2C_START                ((base) + 0x0024)
#define mt_I2C_FIFO_STAT            ((base) + 0x0030)
#define mt_I2C_FIFO_THRESH          ((base) + 0x0034)
#define mt_I2C_FIFO_ADDR_CLR        ((base) + 0x0038)
#define mt_I2C_IO_CONFIG            ((base) + 0x0040)
#define mt_I2C_DEBUG                ((base) + 0x0044)
#define mt_I2C_HS                   ((base) + 0x0048)
#define mt_I2C_SOFTRESET            ((base) + 0x0050)
#define mt_I2C_IRQSEL               ((base) + 0x0060)
#define mt_I2C_DEBUGSTAT            ((base) + 0x0064)
#define mt_I2C_DEBUGCTRL            ((base) + 0x0068)
#define mt_I2C_CH2_DATA_PORT        ((base) + 0x0080)
#define mt_I2C_CH2_SLAVE_ADDR       ((base) + 0x0084)
#define mt_I2C_CH2_CONTROL          ((base) + 0x0090)
#define mt_I2C_CH2_TRANSFER_LEN     ((base) + 0x0094)
#define mt_I2C_CH2_TRANSAC_LEN      ((base) + 0x0098)
#define mt_I2C_CH2_DELAY_LEN        ((base) + 0x009c)
#define mt_I2C_CH2_TIMING           ((base) + 0x00a0)
#define mt_I2C_CH2_START            ((base) + 0x00a4)
#define mt_I2C_CH2_FIFO_STAT        ((base) + 0x00b0)
#define mt_I2C_CH2_FIFO_ADDR_CLR    ((base) + 0x00b8)

#define mt_I2C_TRANS_LEN_MASK           (0xff)
#define mt_I2C_TRANS_AUX_LEN_MASK       (0x1f << 8)
#define mt_I2C_CONTROL_MASK             (0x3f << 1)
#define mt_I2C_CH2_TRANS_LEN_MASK       (0x07)
#define mt_I2C_CH2_TRANS_AUX_LEN_MASK   (0x07 << 8)
#define mt_I2C_CH2_CONTROL_MASK         (0x7d << 1)

#define I2C_CH2_ACKERR          (1 << 5)
#define I2C_CH2_TRANSAC_COMP    (1 << 4)
#define I2C_DEBUG               (1 << 3)
#define I2C_HS_NACKERR          (1 << 2)
#define I2C_ACKERR              (1 << 1)
#define I2C_TRANSAC_COMP        (1 << 0)

#define I2C_TX_THR_OFFSET       8
#define I2C_RX_THR_OFFSET       0

#define I2C_START_TRANSAC      \
	do{\
		mb();\
		__raw_writel(0x1,mt_I2C_START);\
	}while(0)
#define I2C_FIFO_CLR_ADDR       __raw_writel(0x1,mt_I2C_FIFO_ADDR_CLR)
#define I2C_FIFO_OFFSET         (__raw_readl(mt_I2C_FIFO_STAT)>>4&0xf)
#define I2C_FIFO_IS_EMPTY       (__raw_readw(mt_I2C_FIFO_STAT)>>0&0x1)
#define I2C_CH2_START_TRANSAC   __raw_writel(0x1,mt_I2C_CH2_START)
#define I2C_CH2_FIFO_CLR_ADDR   __raw_writel(0x1,mt_I2C_CH2_FIFO_ADDR_CLR)
#define I2C_CH2_FIFO_OFFSET     (__raw_readl(mt_I2C_CH2_FIFO_STAT)>>4&0xf)
#define I2C_CH2_FIFO_IS_EMPTY   (__raw_readw(mt_I2C_CH2_FIFO_STAT)>>0&0x1)
#define I2C_IRQSET_CH1             __raw_writel(0x0,mt_I2C_IRQSEL)
#define I2C_IRQSET_CH2             __raw_writel(0x1,mt_I2C_IRQSEL)

#define I2C_SET_BITS(BS,REG)    ((*(volatile u32*)(REG)) |= (u32)(BS))
#define I2C_CLR_BITS(BS,REG)    ((*(volatile u32*)(REG)) &= ~((u32)(BS)))

#define I2C_SET_FIFO_THRESH(tx,rx)	\
    do { u32 tmp = (((tx) & 0x7) << I2C_TX_THR_OFFSET) | \
                (((rx) & 0x7) << I2C_RX_THR_OFFSET); \
        __raw_writel(tmp, mt_I2C_FIFO_THRESH); \
    } while(0)

#define I2C_SET_INTR_MASK(mask) I2C_SET_BITS(mask, mt_I2C_INTR_MASK)

#define I2C_CLR_INTR_MASK(mask)	I2C_CLR_BITS(mask, mt_I2C_INTR_MASK)


#define I2C_SET_SLAVE_ADDR(addr)        __raw_writel((addr)&0xFF, mt_I2C_SLAVE_ADDR)
#define I2C_SET_CH2_SLAVE_ADDR(addr)    __raw_writel((addr)&0xFF, mt_I2C_CH2_SLAVE_ADDR)

#define I2C_SET_TRANS_LEN(len)	 	\
    do { u32 tmp = __raw_readl(mt_I2C_TRANSFER_LEN) & \
                             ~(mt_I2C_TRANS_LEN_MASK); \
        tmp |= ((len) & mt_I2C_TRANS_LEN_MASK); \
        __raw_writel(tmp, mt_I2C_TRANSFER_LEN); \
    } while(0)

#define I2C_SET_CH2_TRANS_LEN(len)	 	\
    do { u32 tmp = __raw_readl(mt_I2C_CH2_TRANSFER_LEN) & \
                             ~(mt_I2C_CH2_TRANS_LEN_MASK); \
        tmp |= ((len) & mt_I2C_CH2_TRANS_LEN_MASK); \
        __raw_writel(tmp, mt_I2C_CH2_TRANSFER_LEN); \
    } while(0)
    
#define I2C_SET_TRANS_AUX_LEN(len)	\
    do { u32 tmp = __raw_readl(mt_I2C_TRANSFER_LEN) & \
                             ~(mt_I2C_TRANS_AUX_LEN_MASK); \
        tmp |= (((len) << 8) & mt_I2C_TRANS_AUX_LEN_MASK); \
        __raw_writel(tmp, mt_I2C_TRANSFER_LEN); \
    } while(0)

#define I2C_SET_CH2_TRANS_AUX_LEN(len)	\
    do { u32 tmp = __raw_readl(mt_I2C_CH2_TRANSFER_LEN) & \
                             ~(mt_I2C_CH2_TRANS_AUX_LEN_MASK); \
        tmp |= (((len) << 8) & mt_I2C_CH2_TRANS_AUX_LEN_MASK); \
        __raw_writel(tmp, mt_I2C_CH2_TRANSFER_LEN); \
    } while(0)
    
#define I2C_SET_TRANSAC_LEN(len)	__raw_writel(len, mt_I2C_TRANSAC_LEN)
#define I2C_SET_TRANS_DELAY(delay)	__raw_writel(delay, mt_I2C_DELAY_LEN)
#define I2C_SET_CH2_TRANSAC_LEN(len)	__raw_writel(len, mt_I2C_CH2_TRANSAC_LEN)
#define I2C_SET_CH2_TRANS_DELAY(delay)	__raw_writel(delay, mt_I2C_CH2_DELAY_LEN)

#define I2C_SET_TRANS_CTRL(ctrl)	\
    do { u32 tmp = __raw_readl(mt_I2C_CONTROL) & ~mt_I2C_CONTROL_MASK; \
        tmp |= ((ctrl) & mt_I2C_CONTROL_MASK); \
        __raw_writel(tmp, mt_I2C_CONTROL); \
    } while(0)
    
#define I2C_SET_CH2_TRANS_CTRL(ctrl)	\
    do { u32 tmp = __raw_readl(mt_I2C_CH2_CONTROL) & ~mt_I2C_CH2_CONTROL_MASK; \
        tmp |= ((ctrl) & mt_I2C_CH2_CONTROL_MASK); \
        __raw_writel(tmp, mt_I2C_CH2_CONTROL); \
    } while(0)

#define I2C_SET_HS_MODE(on_off) \
    do { u32 tmp = __raw_readl(mt_I2C_HS) & ~0x1; \
        tmp |= (on_off & 0x1); \
        __raw_writel(tmp, mt_I2C_HS); \
    } while(0)

#define I2C_READ_BYTE(byte)		\
    do { byte = __raw_readb(mt_I2C_DATA_PORT); } while(0)
#define I2C_CH2_READ_BYTE(byte)		\
    do { byte = __raw_readb(mt_I2C_CH2_DATA_PORT); } while(0)
	
#define I2C_WRITE_BYTE(byte)	\
    do { __raw_writeb(byte, mt_I2C_DATA_PORT); } while(0)
#define I2C_CH2_WRITE_BYTE(byte)	\
    do { __raw_writeb(byte, mt_I2C_CH2_DATA_PORT); } while(0)

#define I2C_CLR_INTR_STATUS(status)	\
    do { __raw_writew(status, mt_I2C_INTR_STAT); } while(0)

#define I2C_INTR_STATUS __raw_readw(mt_I2C_INTR_STAT)

/* mt i2c control bits */
#define TRANS_LEN_CHG       (1 << 6)
#define ACK_ERR_DET_EN      (1 << 5)
#define DIR_CHG             (1 << 4)
#define CLK_EXT             (1 << 3)
#define	DMA_EN              (1 << 2)
#define	REPEATED_START_FLAG (1 << 1)
#define	STOP_FLAG           (0 << 1)

#define CH2_WAIT_EN             (1 << 7)
#define CH2_TRANS_LEN_CHG       (1 << 6)
#define CH2_ACK_ERR_DET_EN      (1 << 5)
#define CH2_DIR_CHG             (1 << 4)
#define CH2_CLK_EXT             (1 << 3)
#define	CH2_DMA_EN              (1 << 2)
#define	CH2_REPEATED_START_FLAG (1 << 1)
#define	CH2_STOP_FLAG           (0 << 1)

enum {
    ST_MODE,
    FS_MODE,
    HS_MODE,
};

struct mt_i2c_hrtimer_t{
	ktime_t expire;
	int data_size;
	u8 * data_ptr;				//data point
	struct hrtimer hrtimer;
};
struct mt_i2c {
    struct i2c_adapter  *adap;		/* i2c host adapter */
    struct device       *dev;		/* the device object of i2c host adapter */
    u32                 base;		/* i2c base addr */
    u16                 id;
    u16                 irqnr;		/* i2c interrupt number */
    u16                 irq_stat;	/* i2c interrupt status */
    spinlock_t          lock;		/* for mt_i2c struct protection */
    wait_queue_head_t   wait;		/* i2c transfer wait queue */
    
    atomic_t            trans_err;	/* i2c transfer error */
    atomic_t            trans_comp;	/* i2c transfer completion */
    atomic_t            trans_stop;	/* i2c transfer stop */
    
    unsigned long       clk;		/* host clock speed in khz */
    unsigned long       sclk_ch1;		/* khz */
    unsigned long       sclk_ch2;
    
    unsigned char       master_code;/* master code in HS mode */
    unsigned char       mode_ch1;		/* ST/FS/HS mode */
    unsigned char       mode_ch2;
    
    enum                i2c_trans_st_rs st_rs;
    bool                use_ch2;
    u16                 delay_len;
    enum                mt_trans_op op;
#ifdef I2C_CLOCK
    MT65XX_CLOCK        pdn;
#endif
	struct mt_i2c_hrtimer_t i2c_hrtimer;
};

typedef struct {
    u32 data:8;
    u32 reserved:24;
} i2c_data_reg;

typedef struct {
    u32 slave_addr:8;
    u32 reserved:24;
} i2c_slave_addr_reg;

typedef struct {
    u32 transac_comp:1;
    u32 ackerr:1;
    u32 hs_nackerr:1;
    u32 debug:1;
    u32 ch2_transac_comp:1;
    u32 ch2_ackerr:1;
    u32 reserved:26;
} i2c_intr_mask_reg;

typedef struct {
    u32 transac_comp:1;
    u32 ackerr:1;
    u32 hs_nackerr:1;
    u32 reserved:1;
    u32 ch2_transac_comp:1;
    u32 ch2_ackerr:1;    
    u32 reserved1:26;
} i2c_intr_stat_reg;

typedef struct {
    u32 reserved:1;
    u32 rs_stop:1;
    u32 dma_en:1;
    u32 clk_ext_en:1;
    u32 dir_chg:1;
    u32 ackerr_det_en:1;
    u32 trans_len_chg:1;
    u32 reserved1:25;
} i2c_control_reg;

typedef struct {
    u32 trans_len:8;
    u32 trans_aux_len:5;
    u32 reserved:19;
} i2c_trans_len_reg;

typedef struct {
    u32 transac_len:8;
    u32 reserved:24;
} i2c_transac_len_reg;

typedef struct {
    u32 delay_len:8;
    u32 reserved:24;
} i2c_delay_len_reg;

typedef struct {
    u32 step_cnt_div:6;
    u32 reserved:2;
    u32 sample_cnt_div:3;
    u32 reserved1:1;
    u32 data_rd_time:3;
    u32 data_rd_adj:1;
    u32 reserved2:16;
} i2c_timing_reg;

typedef struct {
    u32 start:1;
    u32 reserved:31;
} i2c_start_reg;

typedef struct {
    u32 rd_empty:1;
    u32 wr_full:1;
    u32 reserved:2;
    u32 fifo_offset:4;
    u32 wr_addr:4;
    u32 rd_addr:4;
    u32 reserved1:16;
} i2c_fifo_stat_reg;

typedef struct {
    u32 rx_trig:3;
    u32 reserved0:5;
    u32 tx_trig:3;
    u32 reserved1:21;
} i2c_fifo_thresh_reg;

typedef struct {
    u32 scl_io_cfg:1;
    u32 sda_io_cfg:1;
    u32 io_sync_en:1;
    u32 reserved:29;
} i2c_io_config_reg;

typedef struct {
    u32 debug:3;
    u32 reserved:29;
} i2c_debug_reg;

typedef struct {
    u32 hs_en:1;
    u32 hs_nackerr_det_en:1;
    u32 reserved:2;
    u32 master_code:3;
    u32 reserved1:1;
    u32 hs_step_cnt_div:3;
    u32 reserved2:1;
    u32 hs_sample_cnt_div:3;
    u32 reserved3:17;
} i2c_hs_reg;

typedef struct {
    u32 master_stat:4;
    u32 master_rd:1;
    u32 master_wr:1;
    u32 bus_busy:1;	
    u32 reserved:25;
} i2c_dbg_stat_reg;

typedef struct {
    u32 fifo_apb_dbg:1;
    u32 apb_dbg_rd:1;
    u32 reserved:30;
} i2c_dbg_ctrl_reg;

typedef struct {
    u32 data:8;
    u32 reserved:24;
} i2c_ch2_data_reg;

typedef struct {
    u32 slave_addr:8;
    u32 reserved:24;
} i2c_ch2_slave_addr_reg;

typedef struct {
    u32 reserved:1;
    u32 rs_stop:1;
    u32 reserved1:1;
    u32 clk_ext_en:1;
    u32 dir_chg:1;
    u32 ackerr_det_en:1;
    u32 trans_len_chg:1;
    u32 ch_wait_en:1;
    u32 reserved2:24;
} i2c_ch2_control_reg;

typedef struct {
    u32 trans_len:3;
    u32 reserved:5;
    u32 trans_aux_len:3;
    u32 reserved1:21;
} i2c_ch2_trans_len_reg;

typedef struct {
    u32 transac_len:3;
    u32 reserved:29;
} i2c_ch2_transac_len_reg;

typedef struct {
    u32 delay_len:2;
    u32 reserved:30;
} i2c_ch2_delay_len_reg;

typedef struct {
    u32 step_cnt_div:6;
    u32 reserved:2;
    u32 sample_cnt_div:3;
    u32 reserved1:1;
    u32 data_rd_time:3;
    u32 data_rd_adj:1;
    u32 reserved2:16;
} i2c_ch2_timing_reg;

typedef struct {
    u32 start:1;
    u32 reserved:31;
} i2c_ch2_start_reg;

typedef struct {
    u32 rd_empty:1;
    u32 wr_full:1;
    u32 reserved:2;
    u32 fifo_offset:4;
    u32 wr_addr:4;
    u32 rd_addr:4;
    u32 reserved1:16;
} i2c_ch2_fifo_stat_reg;
/*
typedef struct {
    i2c_data_reg            *data;
    i2c_slave_addr_reg      *slave_addr;
    i2c_intr_mask_reg       *intr_mask;
    i2c_intr_stat_reg       *intr_stat;
    i2c_control_reg         *control;
    i2c_trans_len_reg       *trans_len;
    i2c_transac_len_reg     *transac_len;
    i2c_delay_len_reg       *delay_len;
    i2c_timing_reg          *timing;
    i2c_start_reg           *start;
    i2c_fifo_stat_reg       *fifo_stat;
    i2c_fifo_thresh_reg     *fifo_thresh;
    i2c_io_config_reg       *io_config;
    i2c_debug_reg           *debug;
    i2c_hs_reg              *hs;
    i2c_dbg_stat_reg        *dbg_stat;
    i2c_dbg_ctrl_reg        *dbg_ctrl; 
    i2c_ch2_data_reg        *ch2_data;
    i2c_ch2_slave_addr_reg  *ch2_slave_addr;
    i2c_ch2_control_reg     *ch2_control;
    i2c_ch2_trans_len_reg   *ch2_trans_len;
    i2c_ch2_transac_len_reg *ch2_transac_len;
    i2c_ch2_delay_len_reg   *ch2_delay_len;
    i2c_ch2_timing_reg      *ch2_timing;
    i2c_ch2_start_reg       *ch2_start;
    i2c_ch2_fifo_stat_reg   *ch2_fifo_stat; 
} i2c_regs;
*/
extern unsigned int mt6577_get_bus_freq(void);
static int mt_i2c_set_speed(struct mt_i2c *i2c, int mode, unsigned long khz);

#ifdef MT_I2C_HRTIMER_DEBUG

static struct i2c_hrtimer_debug_t{
	u32 t_start;
	u32 t_end;
	u32 t[256];

	u16 count;
	u8 fifo[256];
	u32 data_size;
	u32 t_ori;
	struct mt_i2c *i2c;	
};

struct i2c_hrtimer_debug_t *hrtimer_debug;

void i2c_hrtimer_debug_log(void)
{
	struct mt_i2c *i2c = hrtimer_debug->i2c;
	u8 i;
	U8 full_count=0;
	
	if(!i2c)
		return;
	printk("i2c%d transfer %dBytes,real time is %d::%d\n",i2c->id, hrtimer_debug->data_size, hrtimer_debug->t_end - hrtimer_debug->t_start,hrtimer_debug->t_ori);

	if(hrtimer_debug->count == 0)
		return;
	for(i=1;i<hrtimer_debug->count;i++){
		if(hrtimer_debug->fifo[i] == 8)
			full_count ++;
	}
	printk("i2c%d:hrtimer %lld, FIFO empty count %d, total count = %d\n",i2c->id,i2c->i2c_hrtimer.expire.tv64,full_count,hrtimer_debug->count );
	
	if(hrtimer_debug->count > 256){					
		hrtimer_debug->count = 255;
	}
	printk("HRtimer:");
	for(i=1;i<hrtimer_debug->count;i++){
		printk("%d-",hrtimer_debug->t[i]-hrtimer_debug->t[i-1]);
	}
	printk("\nFIFO offset:");
	for(i=0;i<hrtimer_debug->count;i++){
		printk("%d-",hrtimer_debug->fifo[i]);
	}
	printk("\n");
	hrtimer_debug->count = 0;

}
EXPORT_SYMBOL(i2c_hrtimer_debug_log);
#else
void i2c_hrtimer_debug_log(void)
{
	return ;
}
EXPORT_SYMBOL(i2c_hrtimer_debug_log);

#endif

/*
static i2c_regs mt_i2c_regs[3];
static void mt_i2c_init_regs(struct mt_i2c *i2c)
{
    u32 base            = i2c->base;
    i2c_regs *p         = &mt_i2c_regs[i2c->id];
    p->data             = (i2c_data_reg*)mt_I2C_DATA_PORT;
    p->slave_addr       = (i2c_slave_addr_reg*)mt_I2C_SLAVE_ADDR;
    p->intr_mask        = (i2c_intr_mask_reg*)mt_I2C_INTR_MASK;
    p->intr_stat        = (i2c_intr_stat_reg*)mt_I2C_INTR_STAT;
    p->control          = (i2c_control_reg*)mt_I2C_CONTROL;
    p->trans_len        = (i2c_trans_len_reg*)mt_I2C_TRANSFER_LEN;
    p->transac_len      = (i2c_transac_len_reg*)mt_I2C_TRANSAC_LEN;
    p->delay_len        = (i2c_delay_len_reg*)mt_I2C_DELAY_LEN;
    p->timing           = (i2c_timing_reg*)mt_I2C_TIMING;
    p->start            = (i2c_start_reg*)mt_I2C_START;
    p->fifo_stat        = (i2c_fifo_stat_reg*)mt_I2C_FIFO_STAT;
    p->fifo_thresh      = (i2c_fifo_thresh_reg*)mt_I2C_FIFO_THRESH;
    p->io_config        = (i2c_io_config_reg*)mt_I2C_IO_CONFIG;
    p->debug            = (i2c_debug_reg*)mt_I2C_DEBUG;
    p->hs               = (i2c_hs_reg*)mt_I2C_HS;
    p->dbg_stat         = (i2c_dbg_stat_reg*)mt_I2C_DEBUGSTAT;
    p->dbg_ctrl         = (i2c_dbg_ctrl_reg*)mt_I2C_DEBUGCTRL;		
    p->ch2_data         = (i2c_ch2_data_reg*)mt_I2C_CH2_DATA_PORT;
    p->ch2_slave_addr   = (i2c_ch2_slave_addr_reg*)mt_I2C_CH2_SLAVE_ADDR;
    p->ch2_control      = (i2c_ch2_control_reg*)mt_I2C_CH2_CONTROL;
    p->ch2_trans_len    = (i2c_ch2_trans_len_reg*)mt_I2C_CH2_TRANSFER_LEN;
    p->ch2_transac_len  = (i2c_ch2_transac_len_reg*)mt_I2C_CH2_TRANSAC_LEN;
    p->ch2_delay_len    = (i2c_ch2_delay_len_reg*)mt_I2C_CH2_DELAY_LEN;
    p->ch2_timing       = (i2c_ch2_timing_reg*)mt_I2C_CH2_TIMING;
    p->ch2_start        = (i2c_ch2_start_reg*)mt_I2C_CH2_START;
    p->ch2_fifo_stat    = (i2c_ch2_fifo_stat_reg*)mt_I2C_CH2_FIFO_STAT; 
}
*/
static u32 mt_i2c_functionality(struct i2c_adapter *adap)
{
    return I2C_FUNC_I2C | I2C_FUNC_10BIT_ADDR | I2C_FUNC_SMBUS_EMUL;
}

static void mt_i2c_post_isr(struct mt_i2c *i2c, u16 addr)
{
		if (unlikely(true == i2c->use_ch2))
		{
			if (i2c->irq_stat & I2C_CH2_TRANSAC_COMP) {
        	atomic_set(&i2c->trans_err, 0);
        	atomic_set(&i2c->trans_comp, 1);
      }
      if (i2c->irq_stat & I2C_CH2_ACKERR) {
        if (likely(!(addr & I2C_A_FILTER_MSG)))
            dev_err(i2c->dev, "I2C_ACKERR\n");
    	}
		}
		else
		{	
    	if (i2c->irq_stat & I2C_TRANSAC_COMP) {
        	atomic_set(&i2c->trans_err, 0);
        	atomic_set(&i2c->trans_comp, 1);
    	}
    
    	if (i2c->irq_stat & I2C_HS_NACKERR) {
        if (likely(!(addr & I2C_A_FILTER_MSG)))
            dev_err(i2c->dev, "I2C_HS_NACKERR\n");
    	}
    	if (i2c->irq_stat & I2C_ACKERR) {
        if (likely(!(addr & I2C_A_FILTER_MSG)))
            dev_err(i2c->dev, "I2C_ACKERR\n");
    	}
    }
    atomic_set(&i2c->trans_err, i2c->irq_stat & (I2C_HS_NACKERR | I2C_ACKERR ));
}

static void i2c_hrtimer_set_expire(struct mt_i2c *i2c, u16 time, u16 len)
{
	u32 tmp;
	
	if(0 == (time & 0xffff))
		time = 100;
	else 
		time &=0xffff;
#ifdef MT_I2C_HRTIMER_DEBUG	
	u16 trans_len;
    u16 trans_auxlen;
    u16 trans_num;
    u16 data_bit;
	        
	trans_len = (len) & 0xFF;	
	trans_num = (len >> 8) & 0xFF;	

	if(0 == trans_num)
		trans_num = 1;

	data_bit =((trans_len + 1)*9 + 2)*trans_num + trans_num/2;
	hrtimer_debug->t_ori = 1000000/(time)*data_bit;
	hrtimer_debug->data_size = trans_num*trans_len;
#endif	

	if((len>>8) > 1){
		if((len&0xff) != 0){				
			tmp = 1000000/(time)*9*7*((len&0xff) + 1)/(len&0xff);
		}else{
			tmp = 1000000/(time)*9*7*2;
		}
	}else{
		tmp= 1000000/(time)*9*6;	
	}
	
	i2c->i2c_hrtimer.expire = ktime_set(0,tmp);

}
static int i2c_hrtimer_start(struct mt_i2c_hrtimer_t *i2c_timer, u8 *ptr, u16 data_size)
{
	int ret = 0;
	
	i2c_timer->data_ptr = ptr;
	i2c_timer->data_size = data_size;
	
#ifdef MT_I2C_HRTIMER_DEBUG	
	
	hrtimer_debug->count = 0;
	hrtimer_debug->i2c = container_of(i2c_timer, struct mt_i2c, i2c_hrtimer);;
#endif	

	ret = hrtimer_start(&i2c_timer->hrtimer, i2c_timer->expire, HRTIMER_MODE_REL);
	return ret;
}

static enum hrtimer_restart mt_i2c_hrtimer_func(struct hrtimer *timer)
{

	struct mt_i2c_hrtimer_t *i2c_timer = container_of(timer, struct mt_i2c_hrtimer_t, hrtimer);
	struct mt_i2c *i2c = container_of(i2c_timer, struct mt_i2c, i2c_hrtimer);
    u32 base;
	u16 fifo_stat;	
	u8 fifo_left;
	u8 fifo_offset;

	u8 i;
	if(i2c == NULL){
		printk("mt_i2c_hrtimer:i2c NULL point\n");
		return HRTIMER_NORESTART;
	}
	
	base = i2c->base;
	fifo_stat = I2C_READ_REG(i2c->base, OFFSET_FIFO_STAT);	
	fifo_offset = (fifo_stat >> 4) & 0xf;
	fifo_left = I2C_FIFO_SIZE - fifo_offset;
	
#ifdef MT_I2C_HRTIMER_DEBUG	
	if(hrtimer_debug->count < 256){
		hrtimer_debug->t[hrtimer_debug->count] = sched_clock();
		hrtimer_debug->fifo[hrtimer_debug->count] = fifo_offset;
	}
	hrtimer_debug->count++;
#endif	
	if(i2c->op == I2C_MASTER_WR){
	if((fifo_left > 0) && (i2c_timer->data_size > 0)){	//fifo is not full
		for(i=0;i < fifo_left; i++){
			I2C_WRITE_BYTE(*(i2c_timer->data_ptr));		
			if((--(i2c_timer->data_size)) == 0)
				break;						
			(i2c_timer->data_ptr)++;
		}
	}
	if(i2c_timer->data_size == 0){
		
		i2c_timer->data_ptr = NULL;
			return HRTIMER_NORESTART;
		} 
	}else if(i2c->op == I2C_MASTER_RD){
		if((fifo_offset > 0) && (i2c_timer->data_size > 0)){//fifo is not empty
			for(i=0;i < fifo_offset; i++){
				I2C_READ_BYTE(*(i2c_timer->data_ptr));
#ifdef I2C_DEBUG_LOG 
				dev_info(i2c->dev, "read byte = 0x%.2X\n", *(i2c_timer->data_ptr));
#endif
				(i2c_timer->data_ptr)++;
				(i2c_timer->data_size)--;

			}
			if((i2c_timer->data_size) <= 8){
						return HRTIMER_NORESTART;
			}
		}

	}else{		
		return HRTIMER_NORESTART;
	}
	//eror then out
	if(atomic_read(&i2c->trans_err)){
//			printk("hrtimer:trans_err, stop timer\n");
		return HRTIMER_NORESTART;
	}
	hrtimer_forward(&i2c_timer->hrtimer, i2c_timer->hrtimer.base->get_time(), i2c_timer->expire);

	return HRTIMER_RESTART;
}

static int mt_i2c_start_xfer(struct mt_i2c *i2c, struct i2c_msg *msg)
{
    u32 base = i2c->base;
    u16 addr = msg->addr;
    u16 flags = msg->flags;
    u16 read = (flags & I2C_M_RD);
    u16 len = msg->len;
    u8 *ptr = msg->buf;
    long tmo = i2c->adap->timeout;
    int speedret = 0;
    int ret = len;
    
    u16 trans_len;
    u16 trans_auxlen;
    u16 trans_num;
    u16 data_size;
    u16 control;  
	u8 i;
	u8 burst_en = 0;
	struct mt_i2c_hrtimer_t *i2c_timer = &i2c->i2c_hrtimer;

    if( (base != I2C0_BASE) && (base != I2C1_BASE) && (base != I2C2_BASE) )
    {
        dev_err(i2c->dev, "[mt_i2c_start_xfer] mt-i2c:base(%x) is error.\n", base);
        aee_kernel_exception("i2c dump","i2c->base is wrong in mt_i2c_start_xfer!!");
        ret = -EINVAL;
        goto error;
    }
	if(!msg->buf){
        dev_err(i2c->dev, "mt-i2c:data buffer is NULL.\n");
		ret = -EINVAL;
		goto error;
    }
	if(!msg->addr){
       	dev_err(i2c->dev, "mt-i2c:addr is invalid.\n");
		ret = -EINVAL;
		goto error;
	}
    if (unlikely(msg->addr & I2C_CH2_FLAG))
    {
        i2c->use_ch2 = true;
		
		I2C2_CH2_ERR(i2c->dev,msg->addr);
		ret = -EINVAL;
		goto error;
		//for dvt,please remove 
        I2C_IRQSET_CH2;
        if(i2c->id != 2)
					dev_err(i2c->dev, "mt-i2c:(%x) this port does not support channel2 transfer.\n", msg->addr);
				if (unlikely(msg->addr & I2C_HS_FLAG)) 
          dev_err(i2c->dev, "mt-i2c:(%x) channel 2 does not support high speed transfer.\n", msg->addr);
        if(msg->addr & 0x0100)
          dev_err(i2c->dev, "mt-i2c:(%x) channel 2 does not support bus hold.\n", msg->addr);
			}
    else
    	{
        i2c->use_ch2 = false;
        I2C_IRQSET_CH1;
      }
/*********************************************************************
************************set clock(speed)*******************************
**********************************************************************/       
    if(0 == (msg->timing & 0xffff)){
        speedret = mt_i2c_set_speed(i2c, ST_MODE, MAX_ST_MODE_SPEED);
		    	
    }else{
        if (unlikely(msg->addr & I2C_HS_FLAG)) 
            speedret = mt_i2c_set_speed(i2c, HS_MODE, msg->timing & 0xffff);
        else
            speedret = mt_i2c_set_speed(i2c, FS_MODE, msg->timing & 0xffff);
	
    }
//		i2c_hrtimer_set_expire(i2c, msg->timing, msg->len);

    if(-ENOTSUPP == speedret){ 
    	dev_err(i2c->dev, "mt-i2c:(%x) speed set %d is invalid.\n", msg->addr,msg->timing);
		ret = -EINVAL;
		goto error;
    }

/*********************************************************************
************************set stop or repeat start*******************************
**********************************************************************/    
    if(msg->addr & I2C_RS_FLAG) 
        i2c->st_rs = I2C_TRANS_REPEATED_START;
    else
        i2c->st_rs = I2C_TRANS_STOP;

/*********************************************************************
*******************set delay time between two transfer****************
**********************************************************************/       
    i2c->delay_len = (msg->timing & 0x00ff0000) >> 16;
    if(I2C_TRANS_STOP == i2c->st_rs) {
    	if (unlikely(true == i2c->use_ch2)) {
			I2C2_CH2_ERR(i2c->dev,msg->addr);
        	if((i2c->delay_len) >> 2)
            dev_err(i2c->dev, "mt-i2c:(%x) the delay_len is too long.\n", msg->addr);
        }else{
        	if((i2c->delay_len) >> 8)
            dev_err(i2c->dev, "mt-i2c:(%x) the delay_len is too long.\n", msg->addr);
      	}        	
    }
    if(0 == i2c->delay_len)
        i2c->delay_len = 2;
/*********************************************************************
************************set transfer mode ****************************
**********************************************************************/    
    if (msg->addr & I2C_WR_FLAG)
        i2c->op = I2C_MASTER_WRRD;
    else {
        if(msg->flags & I2C_M_RD)
            i2c->op = I2C_MASTER_RD;
        else
            i2c->op = I2C_MASTER_WR;
    }
    
    if(I2C_MASTER_WRRD != i2c->op && I2C_MASTER_WR != i2c->op && I2C_MASTER_RD != i2c->op){
        dev_err(i2c->dev, "mt-i2c:(%x) op is invalid.\n", msg->addr);
		ret = -EINVAL;
		goto error;
	}
    
/*********************************************************************
*******************set transfer length(parsing msg length)************
**********************************************************************/

        if(I2C_MASTER_WRRD != i2c->op) {
            if (unlikely(true == i2c->use_ch2)) {
				I2C2_CH2_ERR(i2c->dev,msg->addr);
                trans_len = (msg->len) & 0x07;	
                trans_num = (msg->len >> 8) & 0x07;	
            } else {
                trans_len = (msg->len) & 0xFF;	
                trans_num = (msg->len >> 8) & 0xFF;	
            }
            if(0 == trans_num)
                trans_num = 1;
            trans_auxlen = 0;
            data_size = trans_len*trans_num;
            if(!trans_len || !trans_num) {
                dev_err(i2c->dev, "mt-i2c:(%x) non-WRRD transfer length is not right. trans_len=%x, tans_num=%x, msg len=%x\n", msg->addr, trans_len, trans_num, msg->len);
				ret = -EINVAL;
				goto error;
            }
			if(data_size > I2C_FIFO_SIZE){
				burst_en = 1;
				i2c_hrtimer_set_expire(i2c, msg->timing, msg->len);
			}		
        } else {
            if (unlikely(true == i2c->use_ch2)) {
				I2C2_CH2_ERR(i2c->dev,msg->addr);
                trans_len = (msg->len) & 0x07;	
                trans_auxlen = (msg->len >> 8) & 0x07;
            } else {
                trans_len = (msg->len) & 0xFF;	
                trans_auxlen = (msg->len >> 8) & 0x3F;
            }
            trans_num = 2;
            data_size = trans_len;
            if(!trans_len || !trans_auxlen) {
               	dev_err(i2c->dev, "mt-i2c:(%x) WRRD transfer length is not right. trans_len=%x, tans_num=%x, trans_auxlen=%x\n", msg->addr, trans_len, trans_num, trans_auxlen);
				ret = -EINVAL;
				goto error;
			}
        }
#ifdef I2C_DEBUG_LOG    
    dev_info(i2c->dev, "addr %.2x transfer length = 0x%.2X\n", addr, msg->len);
#endif
    atomic_set(&i2c->trans_stop, 0);
    atomic_set(&i2c->trans_comp, 0);
    atomic_set(&i2c->trans_err, 0);

    addr = read ? ((addr<<1) | 0x1) : ((addr<<1) & ~0x1);
    control = I2C_CONTROL_ACKERR_DET_EN | I2C_CONTROL_CLK_EXT_EN;    

    
    if(I2C_MASTER_WRRD == i2c->op)
        control |= I2C_CONTROL_DIR_CHANGE;
    
    
    if (unlikely(true == i2c->use_ch2)) {
		
		I2C2_CH2_ERR(i2c->dev,msg->addr);
    	 if(HS_MODE == i2c->mode_ch2 || (trans_num > 1 && I2C_TRANS_REPEATED_START == i2c->st_rs)) 
        	control |= I2C_CONTROL_RS;
    }
    else
    {
    	if(HS_MODE == i2c->mode_ch1 || (trans_num > 1 && I2C_TRANS_REPEATED_START == i2c->st_rs)) 
        	control |= I2C_CONTROL_RS;
  	}	
    
    if (unlikely(true == i2c->use_ch2)) {
		I2C2_CH2_ERR(i2c->dev,msg->addr);
        I2C_SET_CH2_SLAVE_ADDR(addr);
        I2C_CLR_INTR_STATUS(I2C_CH2_ACKERR|I2C_CH2_TRANSAC_COMP);
        I2C_SET_INTR_MASK(I2C_CH2_ACKERR|I2C_CH2_TRANSAC_COMP);
        I2C_CH2_FIFO_CLR_ADDR;
        I2C_WRITE_REG(base, OFFSET_CH2_CONTROL, control);
        if(~control & I2C_CONTROL_RS){	// bit is set to 1, i.e.,use repeated stop
            I2C_WRITE_REG(base, OFFSET_CH2_DELAY_LEN, i2c->delay_len);
        }
        I2C_WRITE_REG(base, OFFSET_CH2_TRANSFER_LEN, I2C_CH2_TRANSFER_LEN(trans_len, trans_auxlen));
        I2C_WRITE_REG(base, OFFSET_CH2_TRANSAC_LEN, I2C_CH2_TRANSAC_LEN(trans_num));
    } else {
        I2C_SET_SLAVE_ADDR(addr);
        I2C_CLR_INTR_STATUS(I2C_HS_NACKERR|I2C_ACKERR|I2C_TRANSAC_COMP);
        I2C_SET_INTR_MASK(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
        I2C_FIFO_CLR_ADDR;
        I2C_WRITE_REG(base, OFFSET_CONTROL, control);
        if(~control & I2C_CONTROL_RS){	// bit is set to 1, i.e.,use repeated stop
            I2C_WRITE_REG(base, OFFSET_DELAY_LEN, i2c->delay_len);
        }
				if(msg->addr & I2C_PUSHPULL_FLAG){
						__raw_writew(0x0, mt_I2C_IO_CONFIG);
				}else{
						__raw_writew(0x3, mt_I2C_IO_CONFIG);
				}
        I2C_WRITE_REG(base, OFFSET_TRANSFER_LEN, I2C_TRANSFER_LEN(trans_len, trans_auxlen));
        I2C_WRITE_REG(base, OFFSET_TRANSAC_LEN, I2C_TRANSAC_LEN(trans_num));
    }  
	
/****************************************************************************	
*********I2C oparation mode switch. write-read, write, read 

***************************************************************************/	
  
	//master read
    if (I2C_MASTER_RD == i2c->op) {
        
        if(!burst_en){
        if (unlikely(true == i2c->use_ch2)) {
			I2C2_CH2_ERR(i2c->dev,msg->addr);
            I2C_CH2_START_TRANSAC;
        } else {
	            I2C_START_TRANSAC;
	        }
        } else {
			ret = i2c_hrtimer_start(i2c_timer, ptr, data_size);
			if(ret){
				dev_err(i2c->dev, "addr: %.2x, transfer start rttimer err, restart\n", msg->addr);
				return -EREMOTEIO;
			}
#ifdef MT_I2C_HRTIMER_DEBUG
			
			if( i2c->id != 2 )
				hrtimer_debug->t_start = sched_clock();
#endif
        if (unlikely(true == i2c->use_ch2)) {
			I2C2_CH2_ERR(i2c->dev,msg->addr);
            I2C_CH2_START_TRANSAC;
        } else {
            I2C_START_TRANSAC;
			}			
        }
        tmo = wait_event_timeout(i2c->wait, atomic_read(&i2c->trans_stop), tmo);
        mt_i2c_post_isr(i2c, addr);     

        if (atomic_read(&i2c->trans_comp)) {
			if(burst_en){
				ptr = i2c_timer->data_ptr;
			}
            if (unlikely(true == i2c->use_ch2)) {
				I2C2_CH2_ERR(i2c->dev,msg->addr);
                data_size = I2C_FIFO_STAT_LEN(I2C_READ_REG(base, OFFSET_CH2_FIFO_STAT));
                while (data_size--) {
                    I2C_CH2_READ_BYTE(*ptr);
#ifdef I2C_DEBUG_LOG            
                    dev_info(i2c->dev, "addr %.2x read byte = 0x%.2X\n", addr, *ptr);
#endif
                    ptr++;
                }
            } else {
            	 data_size = I2C_FIFO_STAT_LEN(I2C_READ_REG(base, OFFSET_FIFO_STAT));
               while (data_size--) {
               I2C_READ_BYTE(*ptr);
#ifdef I2C_DEBUG_LOG 
               dev_info(i2c->dev, "addr %.2x read byte = 0x%.2X\n", addr, *ptr);
#endif
               ptr++;
               } 
           }
       }

	//master write
    } else if(I2C_MASTER_WR == i2c->op) {
        
		if(!burst_en){
            if (unlikely(true == i2c->use_ch2)) {
				I2C2_CH2_ERR(i2c->dev,msg->addr);
                while (data_size--) {
                    I2C_CH2_WRITE_BYTE(*ptr);
#ifdef I2C_DEBUG_LOG 
                    dev_info(i2c->dev, "addr %.2x write byte = 0x%.2X\n", addr, *ptr);
#endif
                    ptr++;
                }
            } else {
                while (data_size--) {
                    I2C_WRITE_BYTE(*ptr);
#ifdef I2C_DEBUG_LOG 
                    dev_info(i2c->dev, "addr %.2x write byte = 0x%.2X\n", addr, *ptr);
#endif
                    ptr++;
                }  
            }
#ifdef MT_I2C_HRTIMER_DEBUG
		if ( i2c->id != 2 )
			hrtimer_debug->t_start = sched_clock(); 
#endif
        if (unlikely(true == i2c->use_ch2)) {
			I2C2_CH2_ERR(i2c->dev,msg->addr);
            I2C_CH2_START_TRANSAC;
        } else {
            I2C_START_TRANSAC;
        } 
	    }else{

			for(i=0; i < I2C_FIFO_SIZE; i++){
				I2C_WRITE_BYTE(*ptr);
#ifdef I2C_DEBUG_LOG 
                dev_info(i2c->dev, "addr %.2x write byte = 0x%.2X\n", addr, *ptr);
#endif
				ptr++;				
			}
			data_size -= I2C_FIFO_SIZE;
			ret = i2c_hrtimer_start(i2c_timer, ptr, data_size);
			if(ret){
				dev_err(i2c->dev, "addr: %.2x, transfer start rttimer err, restart\n", msg->addr);
				return -EREMOTEIO;
			}
#ifdef MT_I2C_HRTIMER_DEBUG
			
		if ( i2c->id != 2 )
			hrtimer_debug->t_start = sched_clock();
#endif
			I2C_START_TRANSAC;	
		}		
        tmo = wait_event_timeout(i2c->wait, 
                               atomic_read(&i2c->trans_stop), i2c->adap->timeout);
        mt_i2c_post_isr(i2c, addr);
	//master write then read
    } else {
        if (unlikely(true == i2c->use_ch2)) {
			
			I2C2_CH2_ERR(i2c->dev,msg->addr);
            while (data_size--) {
                I2C_CH2_WRITE_BYTE(*ptr);
#ifdef I2C_DEBUG_LOG 
                dev_info(i2c->dev, "addr %.2x write byte = 0x%.2X\n", addr, *ptr);
#endif
                ptr++;
            }
            I2C_CH2_START_TRANSAC;
        } else {
            while (data_size--) {
                I2C_WRITE_BYTE(*ptr);
#ifdef I2C_DEBUG_LOG 
                dev_info(i2c->dev, "addr %.2x write byte = 0x%.2X\n", addr, *ptr);
#endif
                ptr++;
            }

            I2C_START_TRANSAC;
               
        }
        
        tmo = wait_event_timeout(i2c->wait, 
                               atomic_read(&i2c->trans_stop), tmo);
        mt_i2c_post_isr(i2c, addr);
        
        ptr = msg->buf;
        
        if (atomic_read(&i2c->trans_comp)) {
            if (unlikely(true == i2c->use_ch2)) {
				I2C2_CH2_ERR(i2c->dev,msg->addr);
                data_size = I2C_FIFO_STAT_LEN(I2C_READ_REG(base, OFFSET_CH2_FIFO_STAT));
                while (data_size--) {
                    I2C_CH2_READ_BYTE(*ptr);
#ifdef I2C_DEBUG_LOG 
                    dev_info(i2c->dev, "addr %.2x read byte = 0x%.2X\n", addr, *ptr);
#endif
                    ptr++;
                }
            } else {
                data_size = I2C_FIFO_STAT_LEN(I2C_READ_REG(base, OFFSET_FIFO_STAT));
                while (data_size--) {
                    I2C_READ_BYTE(*ptr);
#ifdef I2C_DEBUG_LOG 
										dev_info(i2c->dev, "addr %.2x read byte = 0x%.2X\n", addr, *ptr);
#endif
                    ptr++;
                }  
            }
        }
    }    

   	if(burst_en){
		hrtimer_cancel(&i2c_timer->hrtimer);		
//			dev_err(i2c->dev, "addr: %.2x, Hrtimer cancel %d\n", msg->addr,ret);
   	}
	//time out process   
    if (tmo == 0) {
        if (likely(!(addr & I2C_A_FILTER_MSG))) {
            dev_err(i2c->dev, "addr: %.2x, transfer timeout\n", msg->addr);

            if (unlikely(true == i2c->use_ch2)) {
            		dev_err(i2c->dev, "I2C structure:\nMode_ch2 %x\nSt_rs %x\nOp %x\nTrans_len %x\nTrans_num %x\nTrans_auxlen %x\nData_size %x\nIrq_stat %x\nTrans_stop %d\nTrans_comp %d\nTrans_error %d\n", i2c->mode_ch2, i2c->st_rs, i2c->op, trans_len, trans_num, trans_auxlen, data_size, i2c->irq_stat, atomic_read(&i2c->trans_stop), atomic_read(&i2c->trans_comp), atomic_read(&i2c->trans_err));
                dev_err(i2c->dev, "I2C register:\nCH2_SLAVE_ADDR %x\nINTR_MASK %x\nINTR_STAT %x\nCH2_CONTROL %x\nCH2_TRANSFER_LEN %x\nCH2_TRANSAC_LEN %x\nCH2_DELAY_LEN %x\nCH2_TIMING %x\nCH2_START %x\nCH2_FIFO_STAT %x\nDEBUGSTAT %x\n",(__raw_readl(mt_I2C_CH2_SLAVE_ADDR)),(__raw_readl(mt_I2C_INTR_MASK)),(__raw_readl(mt_I2C_INTR_STAT)),(__raw_readl(mt_I2C_CH2_CONTROL)),(__raw_readl(mt_I2C_CH2_TRANSFER_LEN)),(__raw_readl(mt_I2C_CH2_TRANSAC_LEN)),(__raw_readl(mt_I2C_CH2_DELAY_LEN)),(__raw_readl(mt_I2C_CH2_TIMING)),(__raw_readl(mt_I2C_CH2_START)),(__raw_readl(mt_I2C_CH2_FIFO_STAT)),(__raw_readl(mt_I2C_DEBUGSTAT)));
            } else {
            		dev_err(i2c->dev, "I2C structure:\nMode_ch1 %x\nSt_rs %x\nOp %x\nTrans_len %x\nTrans_num %x\nTrans_auxlen %x\nData_size %x\nIrq_stat %x\nTrans_stop %d\nTrans_comp %d\nTrans_error %d\n", i2c->mode_ch1, i2c->st_rs, i2c->op, trans_len, trans_num, trans_auxlen, data_size, i2c->irq_stat, atomic_read(&i2c->trans_stop), atomic_read(&i2c->trans_comp), atomic_read(&i2c->trans_err));
                dev_err(i2c->dev, "I2C register:\nSLAVE_ADDR %x\nINTR_MASK %x\nINTR_STAT %x\nCONTROL %x\nTRANSFER_LEN %x\nTRANSAC_LEN %x\nDELAY_LEN %x\nTIMING %x\nSTART %x\nFIFO_STAT %x\nIO_CONFIG %x\nHS %x\nDEBUGSTAT %x\n",(__raw_readl(mt_I2C_SLAVE_ADDR)),(__raw_readl(mt_I2C_INTR_MASK)),(__raw_readl(mt_I2C_INTR_STAT)),(__raw_readl(mt_I2C_CONTROL)),(__raw_readl(mt_I2C_TRANSFER_LEN)),(__raw_readl(mt_I2C_TRANSAC_LEN)),(__raw_readl(mt_I2C_DELAY_LEN)),(__raw_readl(mt_I2C_TIMING)),(__raw_readl(mt_I2C_START)),(__raw_readl(mt_I2C_FIFO_STAT)),(__raw_readl(mt_I2C_IO_CONFIG)),(__raw_readl(mt_I2C_HS)),(__raw_readl(mt_I2C_DEBUGSTAT)));
            }
            
            dev_err(i2c->dev, "Power register:\nCG %x\n",(__raw_readl(0xf1000010)));
        }
        
        if (i2c->id != 2){
            if ( (base == I2C2_BASE)){
                dev_err(i2c->dev, "[mt_i2c_start_xfer] mt-i2c:base(%x)=I2C2_BASE but ID !=2.\n", base);
                aee_kernel_exception("i2c dump","(i2c->id != 2) && (base == I2C2_BASE) !!");
                ret = -EINVAL;
                goto error;
            }
            dev_err(i2c->dev, "[mt_i2c_start_xfer] SOFTREST mt-i2c:base(%x).\n", base);
            I2C_WRITE_REG(base, OFFSET_SOFTRESET, 0x1);
        }
        if (unlikely(true == i2c->use_ch2)) {			
			I2C2_CH2_ERR(i2c->dev,msg->addr);
            I2C_SET_CH2_SLAVE_ADDR(0x00);
            I2C_CLR_INTR_STATUS(I2C_CH2_ACKERR|I2C_CH2_TRANSAC_COMP);
            I2C_CH2_FIFO_CLR_ADDR;
        } else {
            I2C_SET_SLAVE_ADDR(0x00);
            I2C_CLR_INTR_STATUS(I2C_HS_NACKERR|I2C_ACKERR|I2C_TRANSAC_COMP);
            I2C_FIFO_CLR_ADDR;    
        }
        ret = -ETIMEDOUT;
	//result analyse
    } else if (atomic_read(&i2c->trans_err)) {
        if (likely(!(addr & I2C_A_FILTER_MSG))) {
            dev_err(i2c->dev, "addr: %.2x, transfer error\n", msg->addr);

            
            if (unlikely(true == i2c->use_ch2)) {
				I2C2_CH2_ERR(i2c->dev,msg->addr);
            		dev_err(i2c->dev, "I2C structure:\nMode_ch2 %x\nSt_rs %x\nOp %x\nTrans_len %x\nTrans_num %x\nTrans_auxlen %x\nData_size %x\nIrq_stat %x\nTrans_stop %d\nTrans_comp %d\nTrans_error %d\n", i2c->mode_ch2, i2c->st_rs, i2c->op, trans_len, trans_num, trans_auxlen, data_size, i2c->irq_stat, atomic_read(&i2c->trans_stop), atomic_read(&i2c->trans_comp), atomic_read(&i2c->trans_err));
                dev_err(i2c->dev, "I2C register:\nCH2_SLAVE_ADDR %x\nINTR_MASK %x\nINTR_STAT %x\nCH2_CONTROL %x\nCH2_TRANSFER_LEN %x\nCH2_TRANSAC_LEN %x\nCH2_DELAY_LEN %x\nCH2_TIMING %x\nCH2_START %x\nCH2_FIFO_STAT %x\nDEBUGSTAT %x\n",(__raw_readl(mt_I2C_CH2_SLAVE_ADDR)),(__raw_readl(mt_I2C_INTR_MASK)),(__raw_readl(mt_I2C_INTR_STAT)),(__raw_readl(mt_I2C_CH2_CONTROL)),(__raw_readl(mt_I2C_CH2_TRANSFER_LEN)),(__raw_readl(mt_I2C_CH2_TRANSAC_LEN)),(__raw_readl(mt_I2C_CH2_DELAY_LEN)),(__raw_readl(mt_I2C_CH2_TIMING)),(__raw_readl(mt_I2C_CH2_START)),(__raw_readl(mt_I2C_CH2_FIFO_STAT)),(__raw_readl(mt_I2C_DEBUGSTAT)));
            } else {
            		dev_err(i2c->dev, "I2C structure:\nMode_ch1 %x\nSt_rs %x\nOp %x\nTrans_len %x\nTrans_num %x\nTrans_auxlen %x\nData_size %x\nIrq_stat %x\nTrans_stop %d\nTrans_comp %d\nTrans_error %d\n", i2c->mode_ch1, i2c->st_rs, i2c->op, trans_len, trans_num, trans_auxlen, data_size, i2c->irq_stat, atomic_read(&i2c->trans_stop), atomic_read(&i2c->trans_comp), atomic_read(&i2c->trans_err));
                dev_err(i2c->dev, "I2C register:\nSLAVE_ADDR %x\nINTR_MASK %x\nINTR_STAT %x\nCONTROL %x\nTRANSFER_LEN %x\nTRANSAC_LEN %x\nDELAY_LEN %x\nTIMING %x\nSTART %x\nFIFO_STAT %x\nIO_CONFIG %x\nHS %x\nDEBUGSTAT %x\n",(__raw_readl(mt_I2C_SLAVE_ADDR)),(__raw_readl(mt_I2C_INTR_MASK)),(__raw_readl(mt_I2C_INTR_STAT)),(__raw_readl(mt_I2C_CONTROL)),(__raw_readl(mt_I2C_TRANSFER_LEN)),(__raw_readl(mt_I2C_TRANSAC_LEN)),(__raw_readl(mt_I2C_DELAY_LEN)),(__raw_readl(mt_I2C_TIMING)),(__raw_readl(mt_I2C_START)),(__raw_readl(mt_I2C_FIFO_STAT)),(__raw_readl(mt_I2C_IO_CONFIG)),(__raw_readl(mt_I2C_HS)),(__raw_readl(mt_I2C_DEBUGSTAT)));
            }
            
            dev_err(i2c->dev, "Power register:\nCG %x\n",(__raw_readl(0xf1000010)));
        }
        
        if (i2c->id != 2){
            if ( (base == I2C2_BASE)){
                dev_err(i2c->dev, "[mt_i2c_start_xfer] mt-i2c:base(%x)=I2C2_BASE but ID !=2.\n", base);
                aee_kernel_exception("i2c dump","(i2c->id != 2) && (base == I2C2_BASE) !!");
                ret = -EINVAL;
                goto error;
            }
            dev_err(i2c->dev, "[mt_i2c_start_xfer] SOFTRESET mt-i2c:base(%x)\n", base);
            I2C_WRITE_REG(base, OFFSET_SOFTRESET, 0x1);
        }

        if (unlikely(true == i2c->use_ch2)) {
			I2C2_CH2_ERR(i2c->dev,msg->addr);
            I2C_SET_CH2_SLAVE_ADDR(0x00);
            I2C_CLR_INTR_STATUS(I2C_CH2_ACKERR|I2C_CH2_TRANSAC_COMP);
            I2C_CH2_FIFO_CLR_ADDR;
        } else {
            I2C_SET_SLAVE_ADDR(0x00);
            I2C_CLR_INTR_STATUS(I2C_HS_NACKERR|I2C_ACKERR|I2C_TRANSAC_COMP);
            I2C_FIFO_CLR_ADDR;   
        }
        ret = -EREMOTEIO;
    }
    
    if(ret < 0)
        ret = -EAGAIN;
    return ret;
error:
	//paremeter error process
    return ret;
}

static irqreturn_t mt_i2c_irq(int irqno, void *dev_id)
{
    struct mt_i2c *i2c;
    u32 base;
    
    i2c = (struct mt_i2c*)dev_id;
    base = i2c->base;

    if( (base != I2C0_BASE) && (base != I2C1_BASE) && (base != I2C2_BASE) )
    {
        dev_err(i2c->dev, "[mt_i2c_irq] mt-i2c:base(%x) is error.\n", base);	
        aee_kernel_exception("i2c dump","i2c->base is wrong in mt_i2c_irq!!");
    }
#ifdef MT_I2C_HRTIMER_DEBUG    
	 if ( i2c->id != 2 )
		 hrtimer_debug->t_end = sched_clock();
#endif
	
    if (unlikely(true == i2c->use_ch2)){
		I2C2_CH2_ERR(i2c->dev,base);
    	I2C_CLR_INTR_MASK(I2C_CH2_ACKERR|I2C_CH2_TRANSAC_COMP);
    }else{
   		I2C_CLR_INTR_MASK(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
    }
    i2c->irq_stat = I2C_INTR_STATUS;
#ifdef I2C_DEBUG_LOG
    dev_info(i2c->dev, "I2C interrupt status 0x%04X\n", i2c->irq_stat);
#endif    

    if (unlikely(true == i2c->use_ch2)){
		I2C2_CH2_ERR(i2c->dev,base);
    	I2C_CLR_INTR_STATUS(I2C_CH2_ACKERR|I2C_CH2_TRANSAC_COMP);
    }else{
    	I2C_CLR_INTR_STATUS(I2C_HS_NACKERR|I2C_ACKERR|I2C_TRANSAC_COMP);
    }
    atomic_set(&i2c->trans_stop, 1);
	wake_up(&i2c->wait);
    
    return IRQ_HANDLED;
}

static int mt_i2c_do_transfer(struct mt_i2c *i2c, struct i2c_msg *msgs, int num)
{
    int ret = 0;
    int left_num = num;
    
    while (left_num--) {
        ret = mt_i2c_start_xfer(i2c, msgs++);
        if (ret < 0)
            return ret;
    }
    //  the return value is number of executed messages
    return num;
}


static int mt_i2c_transfer(struct i2c_adapter *adap, struct i2c_msg msgs[], int num)
{
    struct mt_i2c *i2c = i2c_get_adapdata(adap);
    
    int	retry;
    int	ret;
#ifdef I2C_CLOCK    
		enable_clock(i2c->pdn, "i2c");
#endif    

	if(i2c->id == 2){	//PMIC use only.
	    for (retry = 0; retry < 3; retry++) {
	        ret = mt_i2c_do_transfer(i2c, msgs, num);
	        if (ret != -EAGAIN) {
	            break;
	        }
	    
	        dev_info(i2c->dev, "Retrying transmission (%d)\n", retry);
	        udelay(100);
	    }
		/*PMIC 3 times retry fail, reboot system*/
		if((retry >= 3) && (ret == -EAGAIN)){
			mt_show_current_irq_counts();
			dev_err(i2c->dev, "PMIC error 3 times, reboot system,%d\n",irqs_disabled());
			dump_stack();
			//BUG();	
		}
	}else{
		ret = mt_i2c_do_transfer(i2c, msgs, num);
	}
#ifdef I2C_CLOCK    
		disable_clock(i2c->pdn, "i2c");
#endif		
    if (ret != -EAGAIN)
        return ret;
    else
        return -EREMOTEIO;
}


static void mt_i2c_free(struct mt_i2c *i2c)
{
    if (!i2c)
        return;
    
    free_irq(i2c->irqnr, i2c);
    if (i2c->adap)
        i2c_del_adapter(i2c->adap);
    kfree(i2c);
}

static int mt_i2c_set_speed(struct mt_i2c *i2c, int mode, unsigned long khz)
{
    u32 base = i2c->base;
    int ret = 0;
    unsigned short sample_cnt_div = 0;
    unsigned short step_cnt_div = 0;
    unsigned short max_step_cnt_div = (mode == HS_MODE) ? 
            MAX_HS_STEP_CNT_DIV : MAX_STEP_CNT_DIV;
    unsigned long tmp, sclk, hclk = i2c->clk;

    if( (base != I2C0_BASE) && (base != I2C1_BASE) && (base != I2C2_BASE) )
	{
        dev_err(i2c->dev, "[mt_i2c_set_speed] mt-i2c:base(%x) is error.\n", base);
        aee_kernel_exception("i2c dump","i2c->base is wrong in mt_i2c_set_speed!!");
    }
    
    if((mode == FS_MODE && khz > MAX_FS_MODE_SPEED) || 
        (mode == HS_MODE && khz > MAX_HS_MODE_SPEED))
        dev_err(i2c->dev, "mt-i2c: the speed is too fast for this mode.\n");
    
    if(unlikely(true == i2c->use_ch2))
    {
    		if((mode == i2c->mode_ch2) && (khz == i2c->sclk_ch2))
        		return 0;
    }
    else
    {    			
    		if((mode == i2c->mode_ch1) && (khz == i2c->sclk_ch1))    			
        		return 0;
    }	
        
    spin_lock(&i2c->lock);
    
    {
        unsigned long diff, min_diff = i2c->clk;
        unsigned short sample_div = MAX_SAMPLE_CNT_DIV;
        unsigned short step_div = max_step_cnt_div;
        for (sample_cnt_div = 1; sample_cnt_div <= MAX_SAMPLE_CNT_DIV; sample_cnt_div++) {
            for (step_cnt_div = 2; step_cnt_div <= max_step_cnt_div; step_cnt_div++) {
                sclk = (hclk >> 1) / (sample_cnt_div * step_cnt_div);
                if (sclk > khz) 
                continue;
                diff = khz - sclk;
                if (diff < min_diff) {
                    min_diff = diff;
                    sample_div = sample_cnt_div;
                    step_div   = step_cnt_div;
                }											
            }
        }
        sample_cnt_div = sample_div;
        step_cnt_div   = step_div;
    }
    
    sclk = hclk / (2 * sample_cnt_div * step_cnt_div);
    if (sclk > khz) {
        dev_err(i2c->dev, "%s mode: unsupported speed (%ldkhz)\n", 
        (mode == HS_MODE) ? "HS" : "ST/FT", khz);
        ret = -ENOTSUPP;
        goto end;
    }
    step_cnt_div--;
    sample_cnt_div--;
    
    if (mode == HS_MODE) {
            tmp  = __raw_readw(mt_I2C_TIMING) & ~((0x7 << 8) | (0x3f << 0));
            tmp  = (0 & 0x0) << 8 | (20 & 0x3f) << 0 | tmp;
            __raw_writew(tmp, mt_I2C_TIMING);
        
        tmp  = __raw_readw(mt_I2C_HS) & ~((0x7 << 12) | (0x7 << 8));
        tmp  = (sample_cnt_div & 0x7) << 12 | (step_cnt_div & 0x7) << 8 | tmp;
        __raw_writew(tmp, mt_I2C_HS);
        I2C_SET_HS_MODE(1);
    }
    else {
        if (unlikely(true == i2c->use_ch2)) {
            tmp  = __raw_readw(mt_I2C_CH2_TIMING) & ~((0x7 << 8) | (0x3f << 0));
            tmp  = (sample_cnt_div & 0x7) << 8 | (step_cnt_div & 0x3f) << 0 | tmp;
            __raw_writew(tmp, mt_I2C_CH2_TIMING);
        } else {
            tmp  = __raw_readw(mt_I2C_TIMING) & ~((0x7 << 8) | (0x3f << 0));
            tmp  = (sample_cnt_div & 0x7) << 8 | (step_cnt_div & 0x3f) << 0 | tmp;
            __raw_writew(tmp, mt_I2C_TIMING);    
        }
        I2C_SET_HS_MODE(0);
    }
    
    if(unlikely(true == i2c->use_ch2))
    {
    		i2c->mode_ch2 = mode;
    		i2c->sclk_ch2 = khz;
    }
    else
    {
    		i2c->mode_ch1 = mode;
    		i2c->sclk_ch1 = khz;
  	}
#ifdef I2C_DEBUG_LOG 
    dev_info(i2c->dev, "mt-i2c: set sclk to %ldkhz (orig: %ldkhz), sample=%d, step=%d\n", sclk, khz, sample_cnt_div, step_cnt_div);
#endif
end:
    spin_unlock(&i2c->lock);
   
    return ret;
}

static void mt_i2c_init_hw(struct mt_i2c *i2c)
{
    u32 base = i2c->base;

    if( (base != I2C0_BASE) && (base != I2C1_BASE) && (base != I2C2_BASE) )
    {
        dev_err(i2c->dev, "[mt_i2c_init_hw] mt-i2c:base(%x) is error.\n", base);
        aee_kernel_exception("i2c dump","i2c->base is wrong in mt_i2c_init_hw!!");
    }

    I2C_WRITE_REG(base, OFFSET_SOFTRESET, 0x1);
}

static struct i2c_algorithm mt_i2c_algorithm = {    
    .master_xfer   = mt_i2c_transfer,
    .smbus_xfer    = NULL,
    .functionality = mt_i2c_functionality,
};

static struct i2c_adapter mt_i2c_adaptor[] = {
    {
//        .id                     = 0,
        .owner                  = THIS_MODULE,
        .name                   = "mt-i2c",
        .algo                   = &mt_i2c_algorithm,
        .algo_data              = NULL,
        //.client_register      = NULL,
        //.client_unregister    = NULL,
        .timeout                = 2 * HZ,
        .retries                = 1, 
    },
    {
//        .id                     = 1,
        .owner                  = THIS_MODULE,
        .name                   = "mt-i2c",
        .algo                   = &mt_i2c_algorithm,
        .algo_data              = NULL,
        //.client_register      = NULL,
        //.client_unregister    = NULL,
        .timeout                = 2 * HZ,
        .retries                = 1, 
    },    
    {
//        .id                     = 2,
        .owner                  = THIS_MODULE,
        .name                   = "mt-i2c",
        .algo                   = &mt_i2c_algorithm,
        .algo_data              = NULL,
        //.client_register      = NULL,
        //.client_unregister    = NULL,
        .timeout                = 2 * HZ,
        .retries                = 1, 
    },   
};

static struct i2c_adapter * i2c2_adapter = NULL;

static int mt_i2c_probe(struct platform_device *pdev)
{
    int ret, irq;
    
    struct mt_i2c *i2c = NULL;

    /* Request IO memory */
    if (!request_mem_region(pdev->resource[0].start,
            pdev->resource[0].end - pdev->resource[0].start + 1, 
            pdev->name)) {
        return -EBUSY;
    }
	
    if (NULL == (i2c = kzalloc(sizeof(struct mt_i2c), GFP_KERNEL))) 
        return -ENOMEM;
    
    
    /* initialize mt_i2c structure */
    irq = pdev->resource[1].start;
    i2c->id = pdev->id;
    i2c->base = pdev->resource[0].start;
    i2c->irqnr = irq;
	
	/*if bus frequnce is not correct, there will be speed set up error or timing set error. 
		Will happen on FPGA varification*/
#if (defined(CONFIG_MT6575T_FPGA)  || defined(CONFIG_MT6577_FPGA))
	i2c->clk    = I2C_FPGA_CLK_RATE;
#else
	i2c->clk    = mt6577_get_bus_freq() / 16;
#endif
    	

	
    i2c->adap   = &mt_i2c_adaptor[pdev->id];
    i2c->dev    = &mt_i2c_adaptor[pdev->id].dev;
    i2c->adap->dev.parent = &pdev->dev;
#ifdef I2C_CLOCK    
		if(0 == i2c->id)
		{
			i2c->pdn = MT65XX_PDN_PERI_I2C0;
		}
		else if(1 == i2c->id)
		{
			i2c->pdn = MT65XX_PDN_PERI_I2C1;
		}
		else
		{
			i2c->pdn = MT65XX_PDN_PERI_I2C_DUAL;

#ifdef  I2C_WORKAROUND
			i2c->mode_ch1 =	ST_MODE;
			i2c->sclk_ch1 = MAX_ST_MODE_SPEED;
#endif
		}
#endif		
    spin_lock_init(&i2c->lock);	
    init_waitqueue_head(&i2c->wait);
    
//	    mt65xx_irq_set_sens(irq, MT65xx_EDGE_SENSITIVE);
//	    mt65xx_irq_set_polarity(irq, MT65xx_POLARITY_LOW);
    
    ret = request_irq(irq, mt_i2c_irq, IRQF_TRIGGER_LOW, DRV_NAME, i2c);
    
    if (ret){
        dev_err(&pdev->dev, "Can Not request I2C IRQ %d\n", irq);
        goto free;
    }

		
    mt_i2c_init_hw(i2c);
    
	hrtimer_init(&i2c->i2c_hrtimer.hrtimer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
	i2c->i2c_hrtimer.hrtimer.function = mt_i2c_hrtimer_func;	
#ifdef MT_I2C_HRTIMER_DEBUG

	if(i2c->id != 2){
		if (NULL == (hrtimer_debug = kzalloc(sizeof(struct i2c_hrtimer_debug_t), GFP_KERNEL))) 
        return -ENOMEM;
	}
#endif
    i2c_set_adapdata(i2c->adap, i2c);
    	/*
	 * If "dev->id" is negative we consider it as zero.
	 * The reason to do so is to avoid sysfs names that only make
	 * sense when there are multiple adapters.
	 */
    i2c->adap->nr = pdev->id != -1 ? pdev->id : 0;
    snprintf(i2c->adap->name, sizeof(i2c->adap->name), "mt_i2c-i2c.%u",
        i2c->adap->nr);

//    ret = i2c_add_adapter(i2c->adap);
    ret = i2c_add_numbered_adapter(i2c->adap);

    if (ret){
        dev_err(&pdev->dev, "failed to add i2c bus to i2c core\n");
        goto free;
    }

    if ( i2c->id == 2 )
	i2c2_adapter = i2c->adap;

    platform_set_drvdata(pdev, i2c);	

    return ret;
	
free:
    mt_i2c_free(i2c);
    return ret;
}

static int mt_i2c_remove(struct platform_device *pdev)
{
    struct mt_i2c *i2c = platform_get_drvdata(pdev);

    if (i2c) {
        platform_set_drvdata(pdev, NULL);
        mt_i2c_free(i2c);
    }
	hrtimer_cancel(&i2c->i2c_hrtimer.hrtimer);

    return 0;
}

#ifdef CONFIG_PM
static int mt_i2c_suspend(struct platform_device *pdev, pm_message_t state)
{
    struct mt_i2c *i2c = platform_get_drvdata(pdev); 
    dev_dbg(i2c->dev,"[I2C %d] Suspend!\n", i2c->id);
    
    #if 0
    struct mt_i2c *i2c = dev_get_drvdata(pdev);    
    
    if (i2c) {
        dev_dbg(i2c->dev,"[I2C] Suspend!\n");
        /* Check if i2c bus is already in used by other modules or not.
         * Parent module should be stopped or wait for i2c bus completed before
         * entering i2c suspend mode.
         */
        WARN_ON(PDN_Get_Peri_Status(PDN_PERI_I2C) == KAL_FALSE);        
        while (PDN_Get_Peri_Status(PDN_PERI_I2C) == KAL_FALSE)
            msleep(10);
    }
    #endif
    return 0;
}

static int mt_i2c_resume(struct platform_device *pdev)
{
    struct mt_i2c *i2c = platform_get_drvdata(pdev); 
    dev_dbg(i2c->dev,"[I2C %d] Resume!\n", i2c->id);

    #if 0
    struct mt_i2c *i2c = dev_get_drvdata(pdev);

    if (i2c) {
        dev_dbg(i2c->dev,"[I2C] Resume!\n");
        mt_i2c_init_hw(i2c);
    }
    #endif
    return 0;
}
#endif

/* device driver for platform bus bits */
static struct platform_driver mt_i2c_driver = {
    .probe	 = mt_i2c_probe,
    .remove	 = mt_i2c_remove,
#ifdef CONFIG_PM
    .suspend = mt_i2c_suspend,
    .resume	 = mt_i2c_resume,
#endif
    .driver  = {
        .name  = DRV_NAME,
        .owner = THIS_MODULE,
    },
};

static int __init mt_i2c_init(void)
{
    int ret;

    ret = platform_driver_register(&mt_i2c_driver);

    return ret;
}

static void __exit mt_i2c_exit(void)
{
    platform_driver_unregister(&mt_i2c_driver);
}

module_init(mt_i2c_init);
module_exit(mt_i2c_exit);



/*-----------------------------------------------------------------------
 * Read/Write interface: Read bytes
 *   addr:    I2C chip address, range 0..127 (slave address?)
 *   buffer:  Where to read/write the data
 *   len:     How many bytes to read/write
 *
 *   Returns : 0 on success, not 0 on failure
 *   Example : i2c_read(i2c_addr, reg, 1, &buf, 1);
 */

int mt_i2c_polling_read(int port, unsigned char addr, unsigned char *buffer, int len)
{
  u32   base;
	u8   *ptr  = buffer;
	int   ret  = len;
	long  tmo = 0xffff;    
  unsigned short status; 
  
	base = I2C2_BASE;
	
	if(len > I2C_FIFO_SIZE)
		printk(KERN_ERR"mt-i2c polling: len is too long.\n");

    /* CHECKME. mt doesn't support len = 0. */
	if (!len)
	{
		printk(KERN_ERR"mt-i2c polling: data buffer is NULL.\n");
		return 0;
	}
	
	if ( i2c2_adapter == NULL )
			return -EAGAIN;
	
	if (in_atomic() || irqs_disabled()) {
	        ret = rt_mutex_trylock(&i2c2_adapter->bus_lock);
	        if (!ret)
		          /* I2C activity is ongoing. */
		        return -EAGAIN;
		} else {
		    rt_mutex_lock(&i2c2_adapter->bus_lock);
		}

#ifdef I2C_CLOCK    
		enable_clock(MT65XX_PDN_PERI_I2C_DUAL, "i2c");
#endif 
    /* bit 0 is to indicate read REQ or write REQ */
	addr = (addr | 0x1);
	
//	I2C_SET_BITS(1<<18, PDN_CLR0);

    /* control registers */
	I2C_SET_SLAVE_ADDR(addr);
	I2C_SET_TRANS_LEN(len);
	I2C_SET_TRANSAC_LEN(1);
	I2C_CLR_INTR_MASK(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
	I2C_FIFO_CLR_ADDR;

	I2C_SET_TRANS_CTRL(ACK_ERR_DET_EN | CLK_EXT | STOP_FLAG);

    /* start trnasfer transaction */
	I2C_START_TRANSAC;

    /* see if transaction complete */
    while (1) {
        status = I2C_INTR_STATUS;

        if ( status & I2C_TRANSAC_COMP && (!I2C_FIFO_IS_EMPTY) ) {
            ret = 0;
            break;
        }
        else if ( status & I2C_HS_NACKERR) {
            ret = 1;
            printk(KERN_ERR"mt-i2c polling: transaction nack error %d\n", ret);  
            break;
        }
        else if ( status & I2C_ACKERR) {
            ret = 2;
            printk(KERN_ERR"mt-i2c polling: transaction ack error %d\n", ret);
		
			printk(KERN_ERR"I2C register:\nSLAVE_ADDR %x\nINTR_MASK %x\nINTR_STAT %x\nCONTROL %x\nTRANSFER_LEN %x\nTRANSAC_LEN %x\nDELAY_LEN %x\nTIMING %x\nSTART %x\nFIFO_STAT %x\nIO_CONFIG %x\nHS %x\nDEBUGSTAT %x\n",(__raw_readl(mt_I2C_SLAVE_ADDR)),(__raw_readl(mt_I2C_INTR_MASK)),(__raw_readl(mt_I2C_INTR_STAT)),(__raw_readl(mt_I2C_CONTROL)),(__raw_readl(mt_I2C_TRANSFER_LEN)),(__raw_readl(mt_I2C_TRANSAC_LEN)),(__raw_readl(mt_I2C_DELAY_LEN)),(__raw_readl(mt_I2C_TIMING)),(__raw_readl(mt_I2C_START)),(__raw_readl(mt_I2C_FIFO_STAT)),(__raw_readl(mt_I2C_IO_CONFIG)),(__raw_readl(mt_I2C_HS)),(__raw_readl(mt_I2C_DEBUGSTAT)));
			
			I2C_SET_SLAVE_ADDR(0x00);
			I2C_CLR_INTR_STATUS(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
			I2C_FIFO_CLR_ADDR;
            break;
        }
        else if (0 == tmo) {
            ret = 3;
            printk(KERN_ERR"mt-i2c polling: transaction timeout\n");
            printk(KERN_ERR"I2C register:\nSLAVE_ADDR %x\nINTR_MASK %x\nINTR_STAT %x\nCONTROL %x\nTRANSFER_LEN %x\nTRANSAC_LEN %x\nDELAY_LEN %x\nTIMING %x\nSTART %x\nFIFO_STAT %x\nIO_CONFIG %x\nHS %x\nDEBUGSTAT %x\n",(__raw_readl(mt_I2C_SLAVE_ADDR)),(__raw_readl(mt_I2C_INTR_MASK)),(__raw_readl(mt_I2C_INTR_STAT)),(__raw_readl(mt_I2C_CONTROL)),(__raw_readl(mt_I2C_TRANSFER_LEN)),(__raw_readl(mt_I2C_TRANSAC_LEN)),(__raw_readl(mt_I2C_DELAY_LEN)),(__raw_readl(mt_I2C_TIMING)),(__raw_readl(mt_I2C_START)),(__raw_readl(mt_I2C_FIFO_STAT)),(__raw_readl(mt_I2C_IO_CONFIG)),(__raw_readl(mt_I2C_HS)),(__raw_readl(mt_I2C_DEBUGSTAT)));
						
			I2C_SET_SLAVE_ADDR(0x00);
			I2C_CLR_INTR_STATUS(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
			I2C_FIFO_CLR_ADDR;
            break;
        }
        tmo --;
    };

	I2C_CLR_INTR_STATUS(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
	
    if (!ret) {
        while (len--) {
		    I2C_READ_BYTE(*ptr);
//			printk(KERN_ERR"mt-i2c polling: read byte = 0x%.2X\n", *ptr);
			ptr++;
		}
    }

	
//	I2C_SET_BITS(1<<18, PDN_SET0);
#ifdef I2C_CLOCK    
		disable_clock(MT65XX_PDN_PERI_I2C_DUAL, "i2c");
#endif  
	rt_mutex_unlock(&i2c2_adapter->bus_lock);
	return ret;
}

/*-----------------------------------------------------------------------
 * Read/Write interface: Write bytes
 *   addr:    I2C chip address, range 0..127
 *   buffer:  Where to read/write the data
 *   len:     How many bytes to read/write
 *
 *   Returns: 0 on success, not 0 on failure
 */

int mt_i2c_polling_write(int port, unsigned char addr, unsigned char *buffer, int len)
{
  u32    base;
	u8    *ptr = buffer;
	int    ret = len;
	long   tmo = 0xffff;    
  unsigned short status; 

  
  base = I2C2_BASE;
  
	if(len > I2C_FIFO_SIZE)
		printk(KERN_ERR"mt-i2c polling: len is too long.\n");

    /* CHECKME. mt doesn't support len = 0. */
	if (!len)
	{
		printk(KERN_ERR"mt-i2c polling: data buffer is NULL.\n");
		return 0;
	}
	
	if ( i2c2_adapter == NULL )
			return -EAGAIN;
	
	if (in_atomic() || irqs_disabled()) {
	        ret = rt_mutex_trylock(&i2c2_adapter->bus_lock);
	        if (!ret)
		          /* I2C activity is ongoing. */
		        return -EAGAIN;
		} else {
		    rt_mutex_lock(&i2c2_adapter->bus_lock);
		}

#ifdef I2C_CLOCK    
		enable_clock(MT65XX_PDN_PERI_I2C_DUAL, "i2c");
#endif 
    /* bit 0 is to indicate read REQ or write REQ */
	addr = (addr & ~0x1);
	
//	I2C_SET_BITS(1<<18, PDN_CLR0);

    /* control registers */
	I2C_SET_SLAVE_ADDR(addr);
	I2C_SET_TRANS_LEN(len);
	I2C_SET_TRANSAC_LEN(1);
	I2C_CLR_INTR_MASK(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
	I2C_FIFO_CLR_ADDR;

	I2C_SET_TRANS_CTRL(ACK_ERR_DET_EN | CLK_EXT | STOP_FLAG);

    /* start to write data */
    while (len--) {
	    I2C_WRITE_BYTE(*ptr);
//		printk(KERN_ERR"mt-i2c polling: write byte = 0x%.2X\n", *ptr);
		ptr++;
    }

    /* start trnasfer transaction */
    I2C_START_TRANSAC;

    /* see if transaction complete */
    while (1) {
        status = I2C_INTR_STATUS;

        if ( status & I2C_TRANSAC_COMP ) {
            ret = 0;
            break;
        }
        else if ( status & I2C_HS_NACKERR) {
            ret = 1;
            printk(KERN_ERR"mt-i2c polling: transaction nack error %d\n", ret); 
			
            break;
        }
        else if ( status & I2C_ACKERR) {
            ret = 2;
            printk(KERN_ERR"mt-i2c polling: transaction ack error %d\n", ret);
			
			printk(KERN_ERR"I2C register:\nSLAVE_ADDR %x\nINTR_MASK %x\nINTR_STAT %x\nCONTROL %x\nTRANSFER_LEN %x\nTRANSAC_LEN %x\nDELAY_LEN %x\nTIMING %x\nSTART %x\nFIFO_STAT %x\nIO_CONFIG %x\nHS %x\nDEBUGSTAT %x\n",(__raw_readl(mt_I2C_SLAVE_ADDR)),(__raw_readl(mt_I2C_INTR_MASK)),(__raw_readl(mt_I2C_INTR_STAT)),(__raw_readl(mt_I2C_CONTROL)),(__raw_readl(mt_I2C_TRANSFER_LEN)),(__raw_readl(mt_I2C_TRANSAC_LEN)),(__raw_readl(mt_I2C_DELAY_LEN)),(__raw_readl(mt_I2C_TIMING)),(__raw_readl(mt_I2C_START)),(__raw_readl(mt_I2C_FIFO_STAT)),(__raw_readl(mt_I2C_IO_CONFIG)),(__raw_readl(mt_I2C_HS)),(__raw_readl(mt_I2C_DEBUGSTAT)));
			
			I2C_SET_SLAVE_ADDR(0x00);
			I2C_CLR_INTR_STATUS(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
			I2C_FIFO_CLR_ADDR;
            break;
        }
        else if (0 == tmo) {
            ret = 3;
            printk(KERN_ERR"mt-i2c polling: transaction timeout\n");
			
            printk(KERN_ERR"I2C register:\nSLAVE_ADDR %x\nINTR_MASK %x\nINTR_STAT %x\nCONTROL %x\nTRANSFER_LEN %x\nTRANSAC_LEN %x\nDELAY_LEN %x\nTIMING %x\nSTART %x\nFIFO_STAT %x\nIO_CONFIG %x\nHS %x\nDEBUGSTAT %x\n",(__raw_readl(mt_I2C_SLAVE_ADDR)),(__raw_readl(mt_I2C_INTR_MASK)),(__raw_readl(mt_I2C_INTR_STAT)),(__raw_readl(mt_I2C_CONTROL)),(__raw_readl(mt_I2C_TRANSFER_LEN)),(__raw_readl(mt_I2C_TRANSAC_LEN)),(__raw_readl(mt_I2C_DELAY_LEN)),(__raw_readl(mt_I2C_TIMING)),(__raw_readl(mt_I2C_START)),(__raw_readl(mt_I2C_FIFO_STAT)),(__raw_readl(mt_I2C_IO_CONFIG)),(__raw_readl(mt_I2C_HS)),(__raw_readl(mt_I2C_DEBUGSTAT)));
			
			I2C_SET_SLAVE_ADDR(0x00);
			I2C_CLR_INTR_STATUS(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
			I2C_FIFO_CLR_ADDR;
            break;
        }
        tmo --;
  	};

	I2C_CLR_INTR_STATUS(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);

	
//	I2C_SET_BITS(1<<18, PDN_SET0);
#ifdef I2C_CLOCK    
		disable_clock(MT65XX_PDN_PERI_I2C_DUAL, "i2c");
#endif 

	rt_mutex_unlock(&i2c2_adapter->bus_lock);
	return ret;
}


int mt_i2c_polling_writeread(int port, unsigned char addr, unsigned char *buffer, int write_len, int read_len)
{
  u32    base;
	u8    *ptr = buffer;
	int    ret = read_len;
	long   tmo = 0xffff;    
  unsigned short status; 
  
  base = I2C2_BASE;
  
	if(write_len > I2C_FIFO_SIZE || read_len > I2C_FIFO_SIZE)
		printk(KERN_ERR"mt-i2c polling: len is too long.\n");

    /* CHECKME. mt doesn't support len = 0. */
	if (!write_len || !read_len)
	{
		printk(KERN_ERR"mt-i2c polling: data buffer is NULL.\n");
		return 0;
	}

	if ( i2c2_adapter == NULL )
			return -EAGAIN;
	
	if (in_atomic() || irqs_disabled()) {
	        ret = rt_mutex_trylock(&i2c2_adapter->bus_lock);
	        if (!ret)
		          /* I2C activity is ongoing. */
		        return -EAGAIN;
		} else {
		    rt_mutex_lock(&i2c2_adapter->bus_lock);
		}

#ifdef I2C_CLOCK    
		enable_clock(MT65XX_PDN_PERI_I2C_DUAL, "i2c");
#endif 		
    /* bit 0 is to indicate read REQ or write REQ */
	addr = (addr & ~0x1);
	
//	I2C_SET_BITS(1<<18, PDN_CLR0);

    /* control registers */
	I2C_SET_SLAVE_ADDR(addr);
	I2C_SET_TRANS_AUX_LEN(read_len);
	I2C_SET_TRANS_LEN(write_len);
	I2C_SET_TRANSAC_LEN(2);
	I2C_CLR_INTR_MASK(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
	I2C_FIFO_CLR_ADDR;

	I2C_SET_TRANS_CTRL(ACK_ERR_DET_EN | CLK_EXT | REPEATED_START_FLAG | DIR_CHG);

    /* start to write data */

	 while (write_len--) {
	    I2C_WRITE_BYTE(*ptr);
//		printk(KERN_ERR"mt-i2c polling: write byte = 0x%.2X\n", *ptr);
		ptr++;
    }
		ptr = buffer;
    /* start trnasfer transaction */
    I2C_START_TRANSAC;

    /* see if transaction complete */
    while (1) {
        status = I2C_INTR_STATUS;

        if ( status & I2C_TRANSAC_COMP ) {
            ret = 0;
            break;
        }
        else if ( status & I2C_HS_NACKERR) {
            ret = 1;
            printk(KERN_ERR"mt-i2c polling: transaction nack error %d\n", ret);    
            break;
        }
        else if ( status & I2C_ACKERR) {
            ret = 2;
            printk(KERN_ERR"mt-i2c polling: transaction ack error %d\n", ret);
			
			printk(KERN_ERR"I2C register:\nSLAVE_ADDR %x\nINTR_MASK %x\nINTR_STAT %x\nCONTROL %x\nTRANSFER_LEN %x\nTRANSAC_LEN %x\nDELAY_LEN %x\nTIMING %x\nSTART %x\nFIFO_STAT %x\nIO_CONFIG %x\nHS %x\nDEBUGSTAT %x\n",(__raw_readl(mt_I2C_SLAVE_ADDR)),(__raw_readl(mt_I2C_INTR_MASK)),(__raw_readl(mt_I2C_INTR_STAT)),(__raw_readl(mt_I2C_CONTROL)),(__raw_readl(mt_I2C_TRANSFER_LEN)),(__raw_readl(mt_I2C_TRANSAC_LEN)),(__raw_readl(mt_I2C_DELAY_LEN)),(__raw_readl(mt_I2C_TIMING)),(__raw_readl(mt_I2C_START)),(__raw_readl(mt_I2C_FIFO_STAT)),(__raw_readl(mt_I2C_IO_CONFIG)),(__raw_readl(mt_I2C_HS)),(__raw_readl(mt_I2C_DEBUGSTAT)));
			
			I2C_SET_SLAVE_ADDR(0x00);
			I2C_CLR_INTR_STATUS(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
			I2C_FIFO_CLR_ADDR;
            break;
        }
        else if (0 == tmo) {
            ret = 3;
            printk(KERN_ERR"mt-i2c polling: transaction timeout\n");
            printk(KERN_ERR"I2C register:\nSLAVE_ADDR %x\nINTR_MASK %x\nINTR_STAT %x\nCONTROL %x\nTRANSFER_LEN %x\nTRANSAC_LEN %x\nDELAY_LEN %x\nTIMING %x\nSTART %x\nFIFO_STAT %x\nIO_CONFIG %x\nHS %x\nDEBUGSTAT %x\n",(__raw_readl(mt_I2C_SLAVE_ADDR)),(__raw_readl(mt_I2C_INTR_MASK)),(__raw_readl(mt_I2C_INTR_STAT)),(__raw_readl(mt_I2C_CONTROL)),(__raw_readl(mt_I2C_TRANSFER_LEN)),(__raw_readl(mt_I2C_TRANSAC_LEN)),(__raw_readl(mt_I2C_DELAY_LEN)),(__raw_readl(mt_I2C_TIMING)),(__raw_readl(mt_I2C_START)),(__raw_readl(mt_I2C_FIFO_STAT)),(__raw_readl(mt_I2C_IO_CONFIG)),(__raw_readl(mt_I2C_HS)),(__raw_readl(mt_I2C_DEBUGSTAT)));
			
			I2C_SET_SLAVE_ADDR(0x00);
			I2C_CLR_INTR_STATUS(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
			I2C_FIFO_CLR_ADDR;
            break;
        }
        tmo --;
  	};

	I2C_CLR_INTR_STATUS(I2C_HS_NACKERR | I2C_ACKERR | I2C_TRANSAC_COMP);
	
	if (!ret) {
        while (read_len--) {
		    I2C_READ_BYTE(*ptr);
//			printk(KERN_ERR"mt-i2c polling: read byte = 0x%.2X\n", *ptr);
			ptr++;
		}
  }
	
//	I2C_SET_BITS(1<<18, PDN_SET0);
#ifdef I2C_CLOCK    
		disable_clock(MT65XX_PDN_PERI_I2C_DUAL, "i2c");
#endif 

	rt_mutex_unlock(&i2c2_adapter->bus_lock);
	return ret;
}

EXPORT_SYMBOL(mt_i2c_polling_read);
EXPORT_SYMBOL(mt_i2c_polling_write);
EXPORT_SYMBOL(mt_i2c_polling_writeread);
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("MediaTek mt I2C Bus Driver");
MODULE_AUTHOR("Changlei Gao <changlei.gao@mediatek.com>");

