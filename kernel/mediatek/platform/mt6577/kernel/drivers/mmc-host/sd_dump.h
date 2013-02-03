#ifndef __MT6577_SD_DUMP__
#define __MT6577_SD_DUMP__

/* ===================================== */
/*        register define                */
/* ===================================== */
/* common register */
#define MSDC_CFG                ((volatile u32*)(base + 0x00))
#define MSDC_IOCON              ((volatile u32*)(base + 0x04))
#define MSDC_PS                 ((volatile u32*)(base + 0x08))
#define MSDC_INT                ((volatile u32*)(base + 0x0c))
#define MSDC_INTEN              ((volatile u32*)(base + 0x10))
#define MSDC_FIFOCS             ((volatile u32*)(base + 0x14))
#define MSDC_TXDATA             ((volatile u32*)(base + 0x18))
#define MSDC_RXDATA             ((volatile u32*)(base + 0x1c))

/* sdmmc register */
#define SDC_CFG                 ((volatile u32*)(base + 0x30))
#define SDC_CMD                 ((volatile u32*)(base + 0x34))
#define SDC_ARG                 ((volatile u32*)(base + 0x38))
#define SDC_STS                 ((volatile u32*)(base + 0x3c))
#define SDC_RESP0               ((volatile u32*)(base + 0x40))
#define SDC_RESP1               ((volatile u32*)(base + 0x44))
#define SDC_RESP2               ((volatile u32*)(base + 0x48))
#define SDC_RESP3               ((volatile u32*)(base + 0x4c))
#define SDC_BLK_NUM             ((volatile u32*)(base + 0x50))
#define SDC_CSTS                ((volatile u32*)(base + 0x58))
#define SDC_CSTS_EN             ((volatile u32*)(base + 0x5c))
#define SDC_DCRC_STS            ((volatile u32*)(base + 0x60))

/* emmc register*/
#define EMMC_CFG0               ((volatile u32*)(base + 0x70))
#define EMMC_CFG1               ((volatile u32*)(base + 0x74))
#define EMMC_STS                ((volatile u32*)(base + 0x78))
#define EMMC_IOCON              ((volatile u32*)(base + 0x7c))

/* auto command register */
#define SDC_ACMD_RESP           ((volatile u32*)(base + 0x80))
#define SDC_ACMD19_TRG          ((volatile u32*)(base + 0x84))
#define SDC_ACMD19_STS          ((volatile u32*)(base + 0x88))

/* dma register */
#define MSDC_DMA_SA             ((volatile u32*)(base + 0x90))
#define MSDC_DMA_CA             ((volatile u32*)(base + 0x94))
#define MSDC_DMA_CTRL           ((volatile u32*)(base + 0x98))
#define MSDC_DMA_CFG            ((volatile u32*)(base + 0x9c))

/* pad ctrl register */
#define MSDC_PAD_CTL0           ((volatile u32*)(base + 0xe0))
#define MSDC_PAD_CTL1           ((volatile u32*)(base + 0xe4))
#define MSDC_PAD_CTL2           ((volatile u32*)(base + 0xe8))

/* data read delay */
#define MSDC_DAT_RDDLY0         ((volatile u32*)(base + 0xf0))
#define MSDC_DAT_RDDLY1         ((volatile u32*)(base + 0xf4))

/* debug register */
#define MSDC_DBG_SEL            ((volatile u32*)(base + 0xa0))
#define MSDC_DBG_OUT            ((volatile u32*)(base + 0xa4))

/* misc register */
#define MSDC_PATCH_BIT          ((volatile u32*)(base + 0xb0))
#define MSDC_PAD_TUNE           ((volatile u32*)(base + 0xec))
#define MSDC_HW_DBG             ((volatile u32*)(base + 0xf8))
//#define MSDC_VERSION            
//#define MSDC_ECO_VER          

/* ===================================== */
/*        register field                 */
/* ===================================== */
/* MSDC_CFG mask */
#define MSDC_CFG_MODE           (0x1  << 0)     /* RW */
#define MSDC_CFG_CKPDN          (0x1  << 1)     /* RW */
#define MSDC_CFG_RST            (0x1  << 2)     /* RW */
#define MSDC_CFG_PIO            (0x1  << 3)     /* RW */
#define MSDC_CFG_CKDRVEN        (0x1  << 4)     /* RW */
#define MSDC_CFG_BV18SDT        (0x1  << 5)     /* RW */
#define MSDC_CFG_BV18PSS        (0x1  << 6)     /* R  */
#define MSDC_CFG_CKSTB          (0x1  << 7)     /* R  */
#define MSDC_CFG_CKDIV          (0xff << 8)     /* RW */
#define MSDC_CFG_CKMOD          (0x3  << 16)    /* RW */

/* MSDC_IOCON mask */
#define MSDC_IOCON_SDR104CKS    (0x1  << 0)     /* RW */
#define MSDC_IOCON_RSPL         (0x1  << 1)     /* RW */
#define MSDC_IOCON_DSPL         (0x1  << 2)     /* RW */
#define MSDC_IOCON_DDLSEL       (0x1  << 3)     /* RW */
//#define MSDC_IOCON_DDR50CKD     (0x1  << 4)     /* RW */   /* not in document! [Fix me] */
//#define MSDC_IOCON_DSPLSEL      (0x1  << 5)     /* RW */
//#define MSDC_IOCON_D0SPL        (0x1  << 16)    /* RW */
//#define MSDC_IOCON_D1SPL        (0x1  << 17)    /* RW */
//#define MSDC_IOCON_D2SPL        (0x1  << 18)    /* RW */
//#define MSDC_IOCON_D3SPL        (0x1  << 19)    /* RW */
//#define MSDC_IOCON_D4SPL        (0x1  << 20)    /* RW */
//#define MSDC_IOCON_D5SPL        (0x1  << 21)    /* RW */
//#define MSDC_IOCON_D6SPL        (0x1  << 22)    /* RW */
//#define MSDC_IOCON_D7SPL        (0x1  << 23)    /* RW */
#define MSDC_IOCON_RISCSZ       (0x3  << 24)    /* RW */

/* MSDC_PS mask */
#define MSDC_PS_CDEN            (0x1  << 0)     /* RW */
#define MSDC_PS_CDSTS           (0x1  << 1)     /* R  */
#define MSDC_PS_CDDEBOUNCE      (0xf  << 12)    /* RW */
#define MSDC_PS_DAT             (0xff << 16)    /* R  */
#define MSDC_PS_DAT0            (0x1  << 16)    /* R  */
#define MSDC_PS_CMD             (0x1  << 24)    /* R  */
#define MSDC_PS_WP              (0x1UL<< 31)    /* R  */

/* MSDC_INT mask */
#define MSDC_INT_MMCIRQ         (0x1  << 0)     /* W1C */
#define MSDC_INT_CDSC           (0x1  << 1)     /* W1C */
#define MSDC_INT_ACMDRDY        (0x1  << 3)     /* W1C */
#define MSDC_INT_ACMDTMO        (0x1  << 4)     /* W1C */
#define MSDC_INT_ACMDCRCERR     (0x1  << 5)     /* W1C */
#define MSDC_INT_DMAQ_EMPTY     (0x1  << 6)     /* W1C */
#define MSDC_INT_SDIOIRQ        (0x1  << 7)     /* W1C */
#define MSDC_INT_CMDRDY         (0x1  << 8)     /* W1C */
#define MSDC_INT_CMDTMO         (0x1  << 9)     /* W1C */
#define MSDC_INT_RSPCRCERR      (0x1  << 10)    /* W1C */
#define MSDC_INT_CSTA           (0x1  << 11)    /* R */
#define MSDC_INT_XFER_COMPL     (0x1  << 12)    /* W1C */
#define MSDC_INT_DXFER_DONE     (0x1  << 13)    /* W1C */
#define MSDC_INT_DATTMO         (0x1  << 14)    /* W1C */
#define MSDC_INT_DATCRCERR      (0x1  << 15)    /* W1C */
#define MSDC_INT_ACMD19_DONE    (0x1  << 16)    /* W1C */

/* MSDC_INTEN mask */
#define MSDC_INTEN_MMCIRQ       (0x1  << 0)     /* RW */
#define MSDC_INTEN_CDSC         (0x1  << 1)     /* RW */
#define MSDC_INTEN_ACMDRDY      (0x1  << 3)     /* RW */
#define MSDC_INTEN_ACMDTMO      (0x1  << 4)     /* RW */
#define MSDC_INTEN_ACMDCRCERR   (0x1  << 5)     /* RW */
#define MSDC_INTEN_DMAQ_EMPTY   (0x1  << 6)     /* RW */
#define MSDC_INTEN_SDIOIRQ      (0x1  << 7)     /* RW */
#define MSDC_INTEN_CMDRDY       (0x1  << 8)     /* RW */
#define MSDC_INTEN_CMDTMO       (0x1  << 9)     /* RW */
#define MSDC_INTEN_RSPCRCERR    (0x1  << 10)    /* RW */
#define MSDC_INTEN_CSTA         (0x1  << 11)    /* RW */
#define MSDC_INTEN_XFER_COMPL   (0x1  << 12)    /* RW */
#define MSDC_INTEN_DXFER_DONE   (0x1  << 13)    /* RW */
#define MSDC_INTEN_DATTMO       (0x1  << 14)    /* RW */
#define MSDC_INTEN_DATCRCERR    (0x1  << 15)    /* RW */
#define MSDC_INTEN_ACMD19_DONE  (0x1  << 16)    /* RW */

/* MSDC_FIFOCS mask */
#define MSDC_FIFOCS_RXCNT       (0xff << 0)     /* R */
#define MSDC_FIFOCS_TXCNT       (0xff << 16)    /* R */
#define MSDC_FIFOCS_CLR         (0x1UL<< 31)    /* RW */

/* SDC_CFG mask */
//#define SDC_CFG_SDIOINTWKUP     (0x1  << 0)     /* RW */   /* not in document! [Fix me] */
#define SDC_CFG_INSWKUP         (0x1  << 1)     /* RW */
#define SDC_CFG_BUSWIDTH        (0x3  << 16)    /* RW */
#define SDC_CFG_SDIO            (0x1  << 19)    /* RW */
//#define SDC_CFG_SDIOIDE         (0x1  << 20)    /* RW */      
#define SDC_CFG_INTATGAP        (0x1  << 21)    /* RW */
#define SDC_CFG_DTOC            (0xffUL << 24)  /* RW */

/* SDC_CMD mask */
#define SDC_CMD_OPC             (0x3f << 0)     /* RW */
#define SDC_CMD_BRK             (0x1  << 6)     /* RW */
#define SDC_CMD_RSPTYP          (0x7  << 7)     /* RW */
#define SDC_CMD_DTYP            (0x3  << 11)    /* RW */
#define SDC_CMD_RW              (0x1  << 13)    /* RW */
#define SDC_CMD_STOP            (0x1  << 14)    /* RW */
#define SDC_CMD_GOIRQ           (0x1  << 15)    /* RW */
#define SDC_CMD_BLKLEN          (0xfff<< 16)    /* RW */
#define SDC_CMD_AUTOCMD         (0x3  << 28)    /* RW */
#define SDC_CMD_VOLSWTH         (0x1  << 30)    /* RW */

/* SDC_STS mask */
#define SDC_STS_SDCBUSY         (0x1  << 0)     /* RW */
#define SDC_STS_CMDBUSY         (0x1  << 1)     /* RW */
#define SDC_STS_SWR_COMPL       (0x1  << 31)    /* RW */

/* SDC_DCRC_STS mask */
#define SDC_DCRC_STS_NEG        (0xf  << 8)     /* RO */
#define SDC_DCRC_STS_POS        (0xff << 0)     /* RO */

/* EMMC_CFG0 mask */
#define EMMC_CFG0_BOOTSTART     (0x1  << 0)     /* W */
#define EMMC_CFG0_BOOTSTOP      (0x1  << 1)     /* W */
#define EMMC_CFG0_BOOTMODE      (0x1  << 2)     /* RW */
#define EMMC_CFG0_BOOTACKDIS    (0x1  << 3)     /* RW */
#define EMMC_CFG0_BOOTWDLY      (0x7  << 12)    /* RW */
#define EMMC_CFG0_BOOTSUPP      (0x1  << 15)    /* RW */

/* EMMC_CFG1 mask */
#define EMMC_CFG1_BOOTDATTMC    (0xfffff << 0)  /* RW */
#define EMMC_CFG1_BOOTACKTMC    (0xfffUL << 20) /* RW */

/* EMMC_STS mask */
#define EMMC_STS_BOOTCRCERR     (0x1  << 0)     /* W1C */
#define EMMC_STS_BOOTACKERR     (0x1  << 1)     /* W1C */
#define EMMC_STS_BOOTDATTMO     (0x1  << 2)     /* W1C */
#define EMMC_STS_BOOTACKTMO     (0x1  << 3)     /* W1C */
#define EMMC_STS_BOOTUPSTATE    (0x1  << 4)     /* R */
#define EMMC_STS_BOOTACKRCV     (0x1  << 5)     /* W1C */
#define EMMC_STS_BOOTDATRCV     (0x1  << 6)     /* R */

/* EMMC_IOCON mask */
#define EMMC_IOCON_BOOTRST      (0x1  << 0)     /* RW */

/* SDC_ACMD19_TRG mask */
#define SDC_ACMD19_TRG_TUNESEL  (0xf  << 0)     /* RW */

/* MSDC_DMA_CTRL mask */
#define MSDC_DMA_CTRL_START     (0x1  << 0)     /* W */
#define MSDC_DMA_CTRL_STOP      (0x1  << 1)     /* W */
#define MSDC_DMA_CTRL_RESUME    (0x1  << 2)     /* W */
#define MSDC_DMA_CTRL_MODE      (0x1  << 8)     /* RW */
#define MSDC_DMA_CTRL_LASTBUF   (0x1  << 10)    /* RW */
#define MSDC_DMA_CTRL_BRUSTSZ   (0x7  << 12)    /* RW */
#define MSDC_DMA_CTRL_XFERSZ    (0xffffUL << 16)/* RW */

/* MSDC_DMA_CFG mask */
#define MSDC_DMA_CFG_STS        (0x1  << 0)     /* R */
#define MSDC_DMA_CFG_DECSEN     (0x1  << 1)     /* RW */
#define MSDC_DMA_CFG_BDCSERR    (0x1  << 4)     /* R */
#define MSDC_DMA_CFG_GPDCSERR   (0x1  << 5)     /* R */

/* MSDC_PATCH_BIT mask */
#define MSDC_PATCH_BIT_WFLSMODE (0x1  << 0)     /* RW */
#define MSDC_PATCH_BIT_ODDSUPP  (0x1  << 1)     /* RW */
#define MSDC_PATCH_BIT_IODSSEL  (0x1  << 16)    /* RW */
#define MSDC_PATCH_BIT_IOINTSEL (0x1  << 17)    /* RW */
#define MSDC_PATCH_BIT_BUSYDLY  (0xf  << 18)    /* RW */
#define MSDC_PATCH_BIT_WDOD     (0xf  << 22)    /* RW */
#define MSDC_PATCH_BIT_IDRTSEL  (0x1  << 26)    /* RW */
#define MSDC_PATCH_BIT_CMDFSEL  (0x1  << 27)    /* RW */
#define MSDC_PATCH_BIT_INTDLSEL (0x1  << 28)    /* RW */
#define MSDC_PATCH_BIT_SPCPUSH  (0x1  << 29)    /* RW */
#define MSDC_PATCH_BIT_DECRCTMO (0x1  << 30)    /* RW */

/* MSDC_PAD_CTL0 mask */
#define MSDC_PAD_CTL0_CLKDRVN   (0x7  << 0)     /* RW */
#define MSDC_PAD_CTL0_CLKDRVP   (0x7  << 4)     /* RW */
#define MSDC_PAD_CTL0_CLKSR     (0x1  << 8)     /* RW */
#define MSDC_PAD_CTL0_CLKPD     (0x1  << 16)    /* RW */
#define MSDC_PAD_CTL0_CLKPU     (0x1  << 17)    /* RW */
#define MSDC_PAD_CTL0_CLKSMT    (0x1  << 18)    /* RW */
#define MSDC_PAD_CTL0_CLKIES    (0x1  << 19)    /* RW */
#define MSDC_PAD_CTL0_CLKTDSEL  (0xf  << 20)    /* RW */
#define MSDC_PAD_CTL0_CLKRDSEL  (0xffUL<< 24)   /* RW */

/* MSDC_PAD_CTL1 mask */
#define MSDC_PAD_CTL1_CMDDRVN   (0x7  << 0)     /* RW */
#define MSDC_PAD_CTL1_CMDDRVP   (0x7  << 4)     /* RW */
#define MSDC_PAD_CTL1_CMDSR     (0x1  << 8)     /* RW */
#define MSDC_PAD_CTL1_CMDPD     (0x1  << 16)    /* RW */
#define MSDC_PAD_CTL1_CMDPU     (0x1  << 17)    /* RW */
#define MSDC_PAD_CTL1_CMDSMT    (0x1  << 18)    /* RW */
#define MSDC_PAD_CTL1_CMDIES    (0x1  << 19)    /* RW */
#define MSDC_PAD_CTL1_CMDTDSEL  (0xf  << 20)    /* RW */
#define MSDC_PAD_CTL1_CMDRDSEL  (0xffUL<< 24)   /* RW */

/* MSDC_PAD_CTL2 mask */
#define MSDC_PAD_CTL2_DATDRVN   (0x7  << 0)     /* RW */
#define MSDC_PAD_CTL2_DATDRVP   (0x7  << 4)     /* RW */
#define MSDC_PAD_CTL2_DATSR     (0x1  << 8)     /* RW */
#define MSDC_PAD_CTL2_DATPD     (0x1  << 16)    /* RW */
#define MSDC_PAD_CTL2_DATPU     (0x1  << 17)    /* RW */
#define MSDC_PAD_CTL2_DATIES    (0x1  << 19)    /* RW */
#define MSDC_PAD_CTL2_DATSMT    (0x1  << 18)    /* RW */
#define MSDC_PAD_CTL2_DATTDSEL  (0xf  << 20)    /* RW */
#define MSDC_PAD_CTL2_DATRDSEL  (0xffUL<< 24)   /* RW */

/* MSDC_PAD_TUNE mask */
#define MSDC_PAD_TUNE_DATWRDLY  (0x1F << 0)     /* RW */
#define MSDC_PAD_TUNE_DATRRDLY  (0x1F << 8)     /* RW */
#define MSDC_PAD_TUNE_CMDRDLY   (0x1F << 16)    /* RW */
#define MSDC_PAD_TUNE_CMDRRDLY  (0x1FUL << 22)  /* RW */
#define MSDC_PAD_TUNE_CLKTXDLY  (0x1FUL << 27)  /* RW */

/* MSDC_DAT_RDDLY0/1 mask */
#define MSDC_DAT_RDDLY0_D0      (0x1F << 0)     /* RW */
#define MSDC_DAT_RDDLY0_D1      (0x1F << 8)     /* RW */
#define MSDC_DAT_RDDLY0_D2      (0x1F << 16)    /* RW */
#define MSDC_DAT_RDDLY0_D3      (0x1F << 24)    /* RW */

#define MSDC_DAT_RDDLY1_D4      (0x1F << 0)     /* RW */
#define MSDC_DAT_RDDLY1_D5      (0x1F << 8)     /* RW */
#define MSDC_DAT_RDDLY1_D6      (0x1F << 16)    /* RW */
#define MSDC_DAT_RDDLY1_D7      (0x1F << 24)    /* RW */


/* ===================================== */
/*        register read/write            */
/* ===================================== */
static inline unsigned int uffs(unsigned int x)
{
    unsigned int r = 1;

    if (!x)
        return 0;
    if (!(x & 0xffff)) {
        x >>= 16;
        r += 16;
    }
    if (!(x & 0xff)) {
        x >>= 8;
        r += 8;
    }
    if (!(x & 0xf)) {
        x >>= 4;
        r += 4;
    }
    if (!(x & 3)) {
        x >>= 2;
        r += 2;
    }
    if (!(x & 1)) {
        x >>= 1;
        r += 1;
    }
    return r;
}

#define sdr_read16(reg)          __raw_readw(reg)
#define sdr_read32(reg)          __raw_readl(reg)
#define sdr_write16(reg,val)     __raw_writew(val,reg)
#define sdr_write32(reg,val)     __raw_writel(val,reg)

#define sdr_set_bits(reg,bs)     ((*(volatile u32*)(reg)) |= (u32)(bs))
#define sdr_clr_bits(reg,bs)     ((*(volatile u32*)(reg)) &= ~((u32)(bs)))

#define sdr_set_field(reg,field,val) \
     do {    \
         volatile unsigned int tv = sdr_read32(reg); \
         tv &= ~(field); \
         tv |= ((val) << (uffs((unsigned int)field) - 1)); \
         sdr_write32(reg,tv); \
     } while(0)
#define sdr_get_field(reg,field,val) \
     do {    \
         volatile unsigned int tv = sdr_read32(reg); \
         val = ((tv & (field)) >> (uffs((unsigned int)field) - 1)); \
     } while(0)

/* ===================================== */
/*        Internal Structures            */
/* ===================================== */     
#define MMC_TYPE_UNKNOWN    (0)          /* Unknown card */
#define MMC_TYPE_MMC        (0x00000001) /* MMC card */
#define MMC_TYPE_SD         (0x00000002) /* SD card */
#define MMC_TYPE_SDIO       (0x00000004) /* SDIO card */

enum {
    standard_capacity = 0,
    high_capacity = 1,
    extended_capacity = 2,
};
enum {
    FAT16 = 0,
    FAT32 = 1,
    exFAT = 2,
};

struct msdc_card {
    u32                     rca;           /* relative card address of device */
    unsigned int            type;          /* card type */
    unsigned short          state;         /* (our) card state */
    unsigned short          file_system;   /* FAT16/FAT32 */ 
    unsigned short          card_cap;      /* High Capcity/standard*/          
};

typedef enum {
    MSDC_CLKSRC_26M = 0,
    MSDC_CLKSRC_197M = 1,
    MSDC_CLKSRC_208M = 2,
    MSDC_CLKSRC_NONE = 3
} clk_source_t;

struct msdc_host
{
    struct msdc_card *card;
    u32 base;         /* host base address */
    u8  id;           /* host id number */
    u32 clk;          /* host clock speed */     /* clock value from clock source */
    u32 sclk;         /* SD/MS clock speed */    /* working clock */
    u8  clksrc;       /* clock source */
    void *priv;       /* private data */
};

/* Lvqiang: using PIO first, No DMA! [Fix me]*/

/* ===================================== */
/*        Export Functions               */
/* ===================================== */
extern int  msdc_init(void);
extern void msdc_soft_reset(void);
extern int  msdc_write_sectors(unsigned char* dst, u32 blk_num, u32 nblks);
extern int  msdc_read_sectors (unsigned char* dst, u32 blk_num, u32 nblks);

#endif
