#ifndef _MT6577_CLOCK_MANAGER_H
#define _MT6577_CLOCK_MANAGER_H

#include <linux/list.h>
#include "mach/mt6577_reg_base.h"
#include "mach/mt6577_sc.h"

/* Top Clock Generator Register Definition */
#define TOP_CKMUXSEL        			(INFRA_SYS_CFG_BASE + 0x0000)
#define TOP_CKDIV0          			(INFRA_SYS_CFG_BASE + 0x0004)
#define TOP_CKDIV1          			(INFRA_SYS_CFG_BASE + 0x0008)
#define TOP_CKDIV23         			(INFRA_SYS_CFG_BASE + 0x000C)
#define TOP_CKCTL           			(INFRA_SYS_CFG_BASE + 0x0020)

#define TOPAXI_SI4_CTL					(INFRA_SYS_CFG_BASE + 0x0210)

#define MIPI_CON               			(INFRA_SYS_CFG_BASE + 0x0890)

/* MCU Sys Register Definition */
#define CLK_CTL0            			(MCUSYS_CFGREG_BASE + 0x00)

#define PERI_MSDC_SRC_CLK_SEL           (PERICFG_BASE + 0x000C)

/* Clock Gating Register Definition */
#define PERI_GLOBALCON_PDN0 			(PERICFG_BASE + 0x0010)
#define PERI_GLOBALCON_PDN1 			(PERICFG_BASE + 0x0014)
#define PERI_GLOBALCON_PDN2 			(PERICFG_BASE + 0x0018)
#define PERI_GLOBALCON_PDN3 			(PERICFG_BASE + 0x001C)

#define MMSYS1_CG_CON0      			(MMSYS1_CONFIG_BASE + 0x0000)
#define MMSYS1_CG_CON1      			(MMSYS1_CONFIG_BASE + 0x0004)
#define MMSYS1_CG_CON2      			(MMSYS1_CONFIG_BASE + 0x0008)
#define MMSYS1_CG_SET0      			(MMSYS1_CONFIG_BASE + 0x0010)
#define MMSYS1_CG_SET1      			(MMSYS1_CONFIG_BASE + 0x0014)
#define MMSYS1_CG_SET2      			(MMSYS1_CONFIG_BASE + 0x0018)
#define MMSYS1_CG_CLR0      			(MMSYS1_CONFIG_BASE + 0x0020)
#define MMSYS1_CG_CLR1      			(MMSYS1_CONFIG_BASE + 0x0024)
#define MMSYS1_CG_CLR2      			(MMSYS1_CONFIG_BASE + 0x0028)

#define MMSYS2_CG_CON       			(MMSYS2_CONFG_BASE + 0x0300)
#define MMSYS2_CG_SET       			(MMSYS2_CONFG_BASE + 0x0304)
#define MMSYS2_CG_CLR       			(MMSYS2_CONFG_BASE + 0x0308)
#define MMSYS2_MM2AXI_CON0     			(MMSYS2_CONFG_BASE + 0x0400)

#define AUDIO_TOP_CON0      			(AUDIO_BASE + 0x0000)

/* MixedSys Register Definition */ 
#define XOSC_CON0           			(APMIXED_BASE + 0x0000)
#define XOSC_CON1           			(APMIXED_BASE + 0x0004)
#define CLKSQ_CON0          			(APMIXED_BASE + 0x0020)
#define PLL_CON0            			(APMIXED_BASE + 0x0040)
#define PLL_CON1            			(APMIXED_BASE + 0x0044)
#define PLL_CON2            			(APMIXED_BASE + 0x0048)
#define PLL_CON3            			(APMIXED_BASE + 0x004C)	
#define PLL_CON4            			(APMIXED_BASE + 0x0050)
#define PLL_CON11           			(APMIXED_BASE + 0x006C)
#define ARMPLL_CON0         			(APMIXED_BASE + 0x0100)
#define ARMPLL_CON1         			(APMIXED_BASE + 0x0104)
#define ARMPLL_CON2         			(APMIXED_BASE + 0x0108)
#define ARMPLL_CON3         			(APMIXED_BASE + 0x010C)
#define MAINPLL_CON0        			(APMIXED_BASE + 0x0120)
#define MAINPLL_CON1        			(APMIXED_BASE + 0x0124)
#define MAINPLL_CON2        			(APMIXED_BASE + 0x0128)
#define MAINPLL_CON3        			(APMIXED_BASE + 0x012C)
#define IPLL_CON0           			(APMIXED_BASE + 0x0140)
#define IPLL_CON1           			(APMIXED_BASE + 0x0144)
#define IPLL_CON2           			(APMIXED_BASE + 0x0148)
#define IPLL_CON3           			(APMIXED_BASE + 0x014C)
#define UPLL_CON0           			(APMIXED_BASE + 0x0160)
#define UPLL_CON1           			(APMIXED_BASE + 0x0164)
#define UPLL_CON2           			(APMIXED_BASE + 0x0168)
#define UPLL_CON3           			(APMIXED_BASE + 0x016C)
#define MDPLL_CON0          			(APMIXED_BASE + 0x0180)
#define MDPLL_CON1          			(APMIXED_BASE + 0x0184)
#define MDPLL_CON2          			(APMIXED_BASE + 0x0188)
#define MDPLL_CON3          			(APMIXED_BASE + 0x018C)
#define TVDDS_CON0          			(APMIXED_BASE + 0x01A0)
#define TVDDS_CON1          			(APMIXED_BASE + 0x01A4)
#define TVDDS_CON2          			(APMIXED_BASE + 0x01A8)
#define TVDDS_CON3          			(APMIXED_BASE + 0x01AC)
#define WPLL_CON0           			(APMIXED_BASE + 0x01C0)
#define WPLL_CON1           			(APMIXED_BASE + 0x01C4)
#define WPLL_CON2           			(APMIXED_BASE + 0x01C8)
#define AUDPLL_CON0         			(APMIXED_BASE + 0x01E0)
#define AUDPLL_CON1         			(APMIXED_BASE + 0x01E4)
#define AUDPLL_CON2         			(APMIXED_BASE + 0x01E8)
#define MEMPLL_CON0         			(APMIXED_BASE + 0x0200)
#define MEMPLL_CON1         			(APMIXED_BASE + 0x0204)
#define MEMPLL_CON2         			(APMIXED_BASE + 0x0208)
#define MEMPLL_CON3         			(APMIXED_BASE + 0x020C)
#define MEMPLL_CON4         			(APMIXED_BASE + 0x0210)
#define TOP_CKMUXSEL2                   (APMIXED_BASE + 0x0B0C)

#define MIPI_TX_CON0                    (MIPI_CONFG_BASE + 0x0800)
#define MIPI_TX_CON6                    (MIPI_CONFG_BASE + 0x0818)
#define MIPI_TX_CON9                    (MIPI_CONFG_BASE + 0x0824)
#define MIPI_RX_CON1                    (MIPI_CONFG_BASE + 0x0844)
#define MIPI_RX_CON4                    (MIPI_CONFG_BASE + 0x0850)
#define MIPI_RX_CON5                    (MIPI_CONFG_BASE + 0x0854)

#define CLK_REG_WIDTH                   32

enum mt65xx_clock_category {
    MT65XX_CLOCK_PERI_PDN0              = 0,
    MT65XX_CLOCK_PERI_PDN1              = 1,
    MT65XX_CLOCK_MMSYS1_PDN0            = 2,
    MT65XX_CLOCK_MMSYS1_PDN1            = 3,
    MT65XX_CLOCK_MMSYS1_PDN2            = 4,
    MT65XX_CLOCK_MMSYS2_PDN             = 5, 
    MT65XX_CLOCK_AUDIO_PDN              = 6, 
    MT65XX_CLOCK_CATEGORY_COUNT         = 7,
};

enum mt65xx_clock_id {
    /* PERI_GLOBALCON_PDN0 */
    MT65XX_PDN_PERI_NFI                 = 0,
    MT65XX_PDN_PERI_THERM               = 1,
    MT65XX_PDN_PERI_PWM1                = 2,
    MT65XX_PDN_PERI_PWM2                = 3,
    MT65XX_PDN_PERI_PWM3                = 4,
    MT65XX_PDN_PERI_PWM456              = 5,
    MT65XX_PDN_PERI_PWM7                = 6,
    MT65XX_PDN_PERI_SIMIF0              = 7,
    MT65XX_PDN_PERI_SIMIF1              = 8,
    MT65XX_PDN_PERI_USB1                = 9,
    MT65XX_PDN_PERI_USB2                = 10,
    MT65XX_PDN_PERI_CCIF                = 11,
    MT65XX_PDN_PERI_APDMA               = 12,
    MT65XX_PDN_PERI_APXGPT              = 13,
    MT65XX_PDN_PERI_MSDC0               = 15,
    MT65XX_PDN_PERI_MSDC1               = 16,
    MT65XX_PDN_PERI_MSDC2               = 17,
    MT65XX_PDN_PERI_MSDC3               = 18,
    MT65XX_PDN_PERI_APHIF               = 19,
    MT65XX_PDN_PERI_MDHIF               = 20,
    MT65XX_PDN_PERI_NLI_ARB             = 21,
    MT65XX_PDN_PERI_ACCDET              = 22,
    MT65XX_PDN_PERI_IRDA                = 23,
    MT65XX_PDN_PERI_UART0               = 24,
    MT65XX_PDN_PERI_UART1               = 25,
    MT65XX_PDN_PERI_UART2               = 26,
    MT65XX_PDN_PERI_UART3               = 27,
    MT65XX_PDN_PERI_I2C0                = 28,
    MT65XX_PDN_PERI_I2C1                = 29,
    MT65XX_PDN_PERI_I2C_DUAL            = 30,
    MT65XX_PDN_PERI_AUXADC              = 31,
    
    /* PERI_GLOBALCON_PDN1 */
    MT65XX_PDN_PERI_SEJ                 = 32,
    MT65XX_PDN_PERI_TRNG                = 33,
    
    /* MMSYS1 Clock Gating #0 */
    MT65XX_PDN_MM_VBUF                  = 64,
    MT65XX_PDN_MM_VDEC                  = 65,
    MT65XX_PDN_MM_VENC                  = 66,
    MT65XX_PDN_MM_SMI_LARB2_ACP_BUS     = 67,
    MT65XX_PDN_MM_SMI_LARB2_260MHZ      = 68,
    MT65XX_PDN_MM_SMI_LARB2_EMI         = 69,
    MT65XX_PDN_MM_SMI_LARB2_ACP_BUS_EMI = 70,
    MT65XX_PDN_MM_SMI_LARB1_EMI         = 71,
    MT65XX_PDN_MM_SMI_LARB0_EMI         = 72,
      
    /* MMSYS1 Clock Gating #1 */
    MT65XX_PDN_MM_VRZ1                  = 96,
    MT65XX_PDN_MM_CSI2                  = 97,
    MT65XX_PDN_MM_FD            		= 98,
    MT65XX_PDN_MM_RESZ_LB       		= 99,
    MT65XX_PDN_MM_TV_ROT        		= 100,
    MT65XX_PDN_MM_LCD           		= 101,
    MT65XX_PDN_MM_RGB_ROT2      		= 102,
    MT65XX_PDN_MM_PRZ1          		= 103,
    MT65XX_PDN_MM_R_DMA1        		= 104,
    MT65XX_PDN_MM_VDO_ROT1      		= 105,
    MT65XX_PDN_MM_RGB_ROT1      		= 106,
    MT65XX_PDN_MM_VRZ           		= 107,
    MT65XX_PDN_MM_RGB_ROT0      		= 108,
    MT65XX_PDN_MM_PRZ0_MOUT     		= 109,
    MT65XX_PDN_MM_PRZ0          		= 110,
    MT65XX_PDN_MM_VDO_ROT0      		= 111,
    MT65XX_PDN_MM_MOUT          		= 112,
    MT65XX_PDN_MM_OVL_DMA_MIMO  		= 113,
    MT65XX_PDN_MM_OVL_DMA_BPS   		= 114,
    MT65XX_PDN_MM_OVL_DMA       		= 115,
    MT65XX_PDN_MM_IPP_MOUT      		= 116,
    MT65XX_PDN_MM_IPP           		= 117,
    MT65XX_PDN_MM_EIS           		= 118,
    MT65XX_PDN_MM_CRZ           		= 119,
    MT65XX_PDN_MM_JPEG_DMA      		= 120,
    MT65XX_PDN_MM_BRZ_MOUT      		= 121,
    MT65XX_PDN_MM_BRZ           		= 122,
    MT65XX_PDN_MM_JPEG_DEC      		= 123,
    MT65XX_PDN_MM_JPEG_ENC      		= 124,
    MT65XX_PDN_MM_R_DMA0_MOUT   		= 125,
    MT65XX_PDN_MM_R_DMA0        		= 126,
    MT65XX_PDN_MM_CAM                   = 127,

    /*MMSYS1 Clock Gating #2 */
    MT65XX_PDN_MM_SCAM                  = 128,
    MT65XX_PDN_MM_SPI       			= 129,
    MT65XX_PDN_MM_TVC       			= 130,
    MT65XX_PDN_MM_TVE       			= 131,
    MT65XX_PDN_MM_DPI       			= 132,
    MT65XX_PDN_MM_DSI       			= 133,
    MT65XX_PDN_MM_SMI_LARB2 			= 134,
    MT65XX_PDN_MM_SMI_LARB1 			= 135,
    MT65XX_PDN_MM_SMI_LARB0 			= 136,
    MT65XX_PDN_MM_LCD_EMI   			= 136,
    
    /* MMSYS2 Clock Gating */
    MT65XX_PDN_MM_SMI_LARB3_FULL    	= 160,
    MT65XX_PDN_MM_GDC_SHARE_MACRO   	= 162,
    MT65XX_PDN_MM_G2D               	= 163,
    MT65XX_PDN_MM_MFG               	= 164,
    MT65XX_PDN_MM_G3D               	= 174,
    MT65XX_PDN_MM_F26M              	= 175,
    MT65XX_PDN_MM_GDC_SHARE_MACRO_HALF  = 176,
    MT65XX_PDN_MM_SMI_LARB3_HALF    	= 177,
    MT65XX_PDN_MM_AUDIO_CORE            = 178,
    MT65XX_PDN_MM_MFG_HALF          	= 179,

    /* MMSYS2 Clock Gating */
    MT65XX_PDN_AUDIO_AFE                = 194,
    MT65XX_PDN_AUDIO_ADC                = 197,
    MT65XX_PDN_AUDIO_I2S                = 198,

    /* Alias */
    MT65XX_CLOCK_COUNT_BEGIN            = MT65XX_PDN_PERI_NFI,
    
    MT65XX_PERI_PDN0_BEGIN              = MT65XX_PDN_PERI_NFI,
    MT65XX_PERI_PDN0_END                = MT65XX_PDN_PERI_AUXADC,
    
    MT65XX_PERI_PDN1_BEGIN              = MT65XX_PDN_PERI_SEJ,
    MT65XX_PERI_PDN1_END                = MT65XX_PDN_PERI_TRNG,
    
    MT65XX_MM1_PDN0_BEGIN               = MT65XX_PDN_MM_VBUF,
    MT65XX_MM1_PDN0_END                 = MT65XX_PDN_MM_SMI_LARB0_EMI,
    
    MT65XX_MM1_PDN1_BEGIN               = MT65XX_PDN_MM_VRZ1,
    MT65XX_MM1_PDN1_END                 = MT65XX_PDN_MM_CAM,
    
    MT65XX_MM1_PDN2_BEGIN               = MT65XX_PDN_MM_SCAM,
    MT65XX_MM1_PDN2_END                 = MT65XX_PDN_MM_LCD_EMI,
    
    MT65XX_MM2_PDN_BEGIN                = MT65XX_PDN_MM_SMI_LARB3_FULL,
    MT65XX_MM2_PDN_END                  = MT65XX_PDN_MM_MFG_HALF,
    
    MT65XX_AUDIO_PDN_BEGIN              = CLK_REG_WIDTH * MT65XX_CLOCK_AUDIO_PDN, /* 32*6 = 192*/
    MT65XX_AUDIO_PDN_END                = MT65XX_PDN_AUDIO_I2S,

    MT65XX_CLOCK_COUNT,
};

enum mt65xx_pll_id {
    MT65XX_ARMPLL                       = 0,
    MT65XX_MAINPLL                      = 1,
    MT65XX_IPLL                         = 2,
    MT65XX_UPLL                         = 3,
    MT65XX_MDPLL                        = 4,
    MT65XX_TVDDS                        = 5,
    MT65XX_WPLL                         = 6,
    MT65XX_AUDPLL                       = 7,
    MT65XX_MEMPLL                       = 8,
    MT65XX_PLL_COUNT                    = 9,
};

enum mt65xx_subsys_id {
    MT65XX_SUBSYS_CPU                   = 0,
    MT65XX_SUBSYS_CA9MP                 = 1,
    MT65XX_SUBSYS_CPUSYS                = 2,
    MT65XX_SUBSYS_NEON                  = 3,
    MT65XX_SUBSYS_MM1                   = 4,
    MT65XX_SUBSYS_MM2                   = 5,
    MT65XX_SUBSYS_COUNT                 = 6,
};

enum mt65xx_mipi_id {
    MT65XX_MIPI_TX                      = 0,
    MT65XX_MIPI_RX                      = 1,
    MT65XX_MIPI_COUNT                   = 2,
};


/* clksrc usr id */
#define CLKSRC_USR_MSDC0        0
#define CLKSRC_USR_MSDC1        1
#define CLKSRC_USR_MSDC2        2
#define CLKSRC_USR_MSDC3        3
#define CLKSRC_USR_USB1         4
#define CLKSRC_USR_USB2         5

/* clksrc selection */
#define CLKSRC_MSDC_26MHz       0   /* default */
#define CLKSRC_MSDC_197MHz      1
#define CLKSRC_MSDC_208MHz      2
#define CLKSRC_USB_61MHz        0   /* default */
#define CLKSRC_USB_65MHz        1


/* interface for turning on/off clock / pll / subsys */
extern int enable_clock(enum mt65xx_clock_id id, char *mod_name);
extern int disable_clock(enum mt65xx_clock_id id, char *mod_name);
extern int enable_pll(enum mt65xx_pll_id id, char *mod_name);
extern int disable_pll(enum mt65xx_pll_id id, char *mod_name);
extern int enable_subsys(enum mt65xx_subsys_id id, char *mod_name);
extern int disable_subsys(enum mt65xx_subsys_id id, char *mod_name);

extern int enable_mipi(enum mt65xx_mipi_id id, char *mod_name);
extern int disable_mipi(enum mt65xx_mipi_id id, char *mod_name);

extern int enable_larb(int larb_idx, char *mod_name);
extern int disable_larb(int larb_idx, char *mod_name);

extern bool larb_is_on(int larb_idx);
extern bool subsys_is_on(enum mt65xx_subsys_id id);

extern void clksrc_select(int id, unsigned int selection);
extern unsigned int get_clksrc_selection(int id);


/* larb monitor mechanism definition*/
enum {
    LARB_MONITOR_LEVEL_HIGH     = 10,
    LARB_MONITOR_LEVEL_MEDIUM   = 20,
    LARB_MONITOR_LEVEL_LOW      = 30,
};

struct larb_monitor {
    struct list_head link;
    int level;
    void (*backup)(struct larb_monitor *h, int larb_idx);       /* called before disable larb clock */
    void (*restore)(struct larb_monitor *h, int larb_idx);      /* called after enable larb clock */
};

extern void register_larb_monitor(struct larb_monitor *handler);
extern void unregister_larb_monitor(struct larb_monitor *handler);


/* mt6577 clock manager init */
extern void mt6577_clk_mgr_init(void);
extern void mt6577_pm_log_init(void);


#include "mach/mt6577_typedefs.h"
#if 1
typedef enum mt65xx_clock_id MT65XX_CLOCK;
extern kal_bool hwEnableClock(MT65XX_CLOCK clockId, char *mod_name);
extern kal_bool hwDisableClock(MT65XX_CLOCK clockId, char *mod_name);
#endif

extern kal_uint32 mt6577_get_bus_freq(void);

#endif
