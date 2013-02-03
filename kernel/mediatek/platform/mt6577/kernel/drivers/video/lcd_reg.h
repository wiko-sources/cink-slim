#ifndef __LCD_REG_H__
#define __LCD_REG_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct
{
    unsigned RUN        : 1;
    unsigned WAIT_CMDQ  : 1;
    unsigned rsv_2      : 1;
    unsigned WAIT_HTT   : 1;
    unsigned WAIT_SYNC  : 1;
    unsigned BUSY       : 1;
    unsigned GMC        : 1;
    unsigned rsv_7      : 9;
} LCD_REG_STATUS, *PLCD_REG_STATUS;


typedef struct
{
    unsigned COMPLETED      : 1;
    unsigned REG_COMPLETED  : 1;
    unsigned CMDQ_COMPLETED : 1;
    unsigned rsv_3          : 1;
    unsigned HTT            : 1;
    unsigned SYNC           : 1;
    unsigned rsv_6          : 10;
} LCD_REG_INTERRUPT, *PLCD_REG_INTERRUPT;

 
typedef struct
{
	unsigned RESET     : 1;
    unsigned rsv_1     : 14;
    unsigned START     : 1;
} LCD_REG_START, *PLCD_REG_START;

typedef struct
{
    unsigned SIZE_0       : 3;
    unsigned THREE_WIRE_0 : 1;
    unsigned SDI_0        : 1;
    unsigned FIRST_POL_0    : 1;
    unsigned SCK_DEF_0    : 1;
    unsigned DIV2_0       : 1;
	unsigned SIZE_1       : 3;
    unsigned THREE_WIRE_1 : 1;
    unsigned SDI_1        : 1;
    unsigned FIRST_POL_1    : 1;
    unsigned SCK_DEF_1    : 1;
    unsigned DIV2_1       : 1;
    unsigned rsv_16       : 8;
	unsigned HW_CS        : 1;
	unsigned rsv_25       : 7;
} LCD_REG_SCNF, *PLCD_REG_SCNF;

typedef struct
{
    unsigned WST        : 6;
    unsigned rsv_6      : 2;
    unsigned C2WS       : 4;
    unsigned C2WH       : 4;
    unsigned RLT        : 6;
    unsigned rsv_22     : 2;
    unsigned C2RS       : 4;
    unsigned C2RH       : 4;
} LCD_REG_PCNF, *PLCD_REG_PCNF;

typedef struct
{
    unsigned ENABLE     : 1;
    unsigned EDGE_SEL   : 1;
    unsigned MODE       : 1;
    unsigned TE_REPEAT  : 1;
	unsigned rsv_4      : 11;
    unsigned SW_TE      : 1;
    unsigned rsv_16     : 16;
} LCD_REG_TECON, *PLCD_REG_TECON;

typedef struct
{
    unsigned PCNF0_DW   : 3;
    unsigned rsv_3      : 1;
    unsigned PCNF1_DW   : 3;
    unsigned rsv_7      : 1;
    unsigned PCNF2_DW   : 3;
    unsigned rsv_11     : 5;
	unsigned PCNF0_CHW  : 4;
	unsigned PCNF1_CHW  : 4;
	unsigned PCNF2_CHW  : 4;
	unsigned rsv_28     : 4;
} LCD_REG_PCNFDW, *PLCD_REG_PCNFDW;

typedef struct
{
    UINT16 WIDTH;
    UINT16 HEIGHT;
} LCD_REG_SIZE, *PLCD_REG_SIZE;

typedef struct {
    unsigned x      : 11;
    unsigned rsv_11 : 5;
    unsigned y      : 11;
    unsigned rsv_27 : 5;
} LCD_REG_W2M_OFFSET;

typedef struct
{
    UINT16 X;
    UINT16 Y;
} LCD_REG_COORD, *PLCD_REG_COORD;

typedef struct
{
	unsigned rsv_0   :4;
	unsigned addr    :4;
	unsigned rsv_8   :8;
} LCD_REG_CMD_ADDR, *PLCD_REG_CMD_ADDR;

typedef struct
{
	unsigned rsv_0   :4;
	unsigned addr    :4;
	unsigned rsv_8   :8;
} LCD_REG_DAT_ADDR, *PLCD_REG_DAT_ADDR;

typedef struct
{
    unsigned rsv_9      : 4;
    unsigned DBS_EN     : 1;
    unsigned rsv_14     : 18;
} LCD_REG_WROI_FMT, *PLCD_REG_WROI_FMT;

typedef struct
{
    unsigned RGB_ORDER      : 1;
    unsigned BYTE_ORDER     : 1;
    unsigned PADDING        : 1;
    unsigned DATA_FMT       : 3;
    unsigned IF_FMT         : 2;
    unsigned COMMAND        : 6;
    unsigned W2M            : 1;
    unsigned ENC            : 1;
    unsigned rsv_16         : 8;
    unsigned SEND_RES_MODE  : 1;
    unsigned IF_24          : 1;
    unsigned EN5            : 1;
    unsigned EN4            : 1;
    unsigned EN3            : 1;
    unsigned EN2            : 1;
    unsigned EN1            : 1;
    unsigned EN0            : 1;
}LCD_REG_WROI_CON, *PLCD_REG_WROI_CON;


typedef struct
{
    unsigned SWREG      : 1;
    unsigned HWREF_MOD  : 1;
    unsigned SWREF_MOD  : 1;
    unsigned rsv_3      : 4;
    unsigned HWEN       : 1;
    unsigned rsv_8      : 8;
    unsigned HWREF_SEL  : 2;
    unsigned rsv_18     : 8;    
    unsigned EN5        : 1;
    unsigned EN4        : 1;
    unsigned EN3        : 1;
    unsigned EN2        : 1;
    unsigned EN1        : 1;
    unsigned EN0        : 1;
} LCD_WROI_HWREF, *PLCD_WROI_HWREF;


typedef struct
{
    unsigned rsv_0      : 16;
    unsigned AUTO_LOOP  : 1;
    unsigned rsv_17     : 9;
    unsigned EN5        : 1;
    unsigned EN4        : 1;
    unsigned EN3        : 1;
    unsigned EN2        : 1;
    unsigned EN1        : 1;
    unsigned EN0        : 1;
} LCD_REG_WROI_DC, *PLCD_REG_WROI_DC;


typedef struct
{
    unsigned OPA        : 8;
    unsigned OPAEN      : 1;
    unsigned rsv_9      : 2;
    unsigned ROTATE     : 3;
    unsigned SRC_KEY_EN : 1;
    unsigned SRC        : 1;
    unsigned SWP        : 1;
    unsigned SRCL_EN    : 1;
    unsigned DITHER_EN  : 1;
    unsigned GMA_EN     : 1;
    unsigned CLRDPT     : 3;
    unsigned rsv_23     : 1;
    unsigned DST_KEY_EN : 1;
    unsigned MATRIX_EN  : 1;
    unsigned RGB_SWAP   : 1;
	unsigned THREE_D         : 1;
	unsigned LANDSCAPE  : 1;
	unsigned R_FIRST    : 1;
    unsigned rsv_30     : 2;
} LCD_REG_LAYER_CON, *PLCD_REG_LAYER_CON;


typedef struct
{
    LCD_REG_LAYER_CON CONTROL;
    UINT32            COLORKEY;
    LCD_REG_COORD     OFFSET;
    UINT32            ADDRESS;
    LCD_REG_SIZE      SIZE;
    LCD_REG_COORD     SCRL_OFFSET;
    UINT32            WINDOW_OFFSET; 
    UINT16            WINDOW_PITCH;               // 001c
    UINT16            rsvr_001e;               //001e
    UINT32            DB_ADD;                  //0020
    UINT32            reserved_0024[3];        // 0024..0030
} LCD_REG_LAYER, *PLCD_REG_LAYER;

typedef struct
{
    unsigned COL2   : 10;
    unsigned COL1   : 10;
    unsigned COL0   : 10;
    unsigned rsv_30 : 2;
} LCD_REG_COEF_ROW, *PLCD_REG_COEF_ROW;

typedef struct
{
    UINT8  XOFF;
    UINT8  YOFF;
    UINT16 SLOPE;
} LCD_REG_GAMMA, *PLCD_REG_GAMMA;

typedef struct {
    UINT32 COMMAND[32];    // 0000..007F
} LCD_REG_COMMAND_QUEUE;

typedef struct {
    UINT32 rsv;
} LCD_REG_GAMCON;

typedef struct {
    unsigned MAX_BURST          : 3;
    unsigned rsv_3              : 1;
    unsigned THROTTLE_EN        : 1;
    unsigned rsv_5              : 11;
    unsigned THROTTLE_PERIOD    : 16;
} LCD_REG_GMCCON;

/*
typedef struct {
    unsigned x      : 11;
    unsigned rsv_11 : 5;
    unsigned y      : 11;
    unsigned rsv_27 : 5;
} LCD_REG_W2M_OFFSET;
*/
typedef struct
{
    unsigned W2LCM      : 1;
    unsigned W2M_FMT    : 2;
    unsigned rsv_3      : 1;
    unsigned ADDINC_DIS : 1;
    unsigned DC_TV_ROT  : 1;
    unsigned rsv_6      : 2;
    unsigned OUT_ALPHA  : 8;
    unsigned FB1_EN     : 1;
    unsigned FB2_EN     : 1;
    unsigned MFB_DIS    : 1;
    unsigned rsv_19     : 1;
    unsigned DLY_EN     : 1;
    unsigned RGB_SWP    : 1;
    unsigned BYTE_SWP   : 1;
    unsigned rsv_23     : 8;
    unsigned FBSEQ_RST  : 1;
} LCD_REG_WROI_W2MCON, *PLCD_REG_WROI_W2MCON;

typedef struct
{
    unsigned DC_DSI      : 1;
    unsigned BYTE_SWAP   : 1;
    unsigned RGB_SWAP    : 1;
    unsigned PAD_MSB     : 1;
    unsigned CLR_FMT     : 3;
    unsigned rsv_7       : 9;
	unsigned PACKET_SIZE : 9;
	unsigned rsv_25      : 7;
} LCD_REG_DSI_DC, *PLCD_REG_DSI_DC;

typedef struct
{
    unsigned DB_B        : 2;
    unsigned rsv_2       : 2;
    unsigned DB_G        : 2;
    unsigned rsv_6       : 2;
    unsigned DB_R        : 2;
    unsigned rsv_10      : 2;
    unsigned LFSR_B_SEED : 4;
    unsigned LFSR_G_SEED : 4;
    unsigned LFSR_R_SEED : 4;
	unsigned ERR_DIFF_EN : 1;
	unsigned ERR_DIFF_LAYER : 3;
    unsigned rsv_28      : 4;
} LCD_REG_DITHER_CON, *PLCD_REG_DITHER_CON;

typedef struct 
{
	unsigned WR_2ND		: 4;
	unsigned WR_1ST		: 4;
	unsigned RD_2ND		: 4;
	unsigned RD_1ST		: 4;
	unsigned CSH		: 4;
	unsigned CSS		: 4;
	unsigned rsv_24		: 8;
} LCD_REG_SIF_TIMING, *PLCD_REG_SIF_TIMING;

typedef struct
{
	unsigned CS0		: 1;
	unsigned CS1		: 1;
	unsigned rsv30		:30;
} LCD_REG_SIF_CS, *PLCD_REG_SIF_CS;

typedef struct 
{
	unsigned TIME_OUT	: 12;
	unsigned rsv_12		: 4;
	unsigned COUNT		: 12;
	unsigned rsv_28		: 4;
} LCD_REG_CALC_HTT, *PLCD_REG_CALC_HTT;

typedef struct 
{
	unsigned HTT		: 10;
	unsigned rsv_10		: 6;
	unsigned VTT		: 12;
	unsigned rsv_28		: 4;
} LCD_REG_SYNC_LCM_SIZE, *PLCD_REG_SYNC_LCM_SIZE;


typedef struct 
{
	unsigned WAITLINE	: 12;
	unsigned rsv_12		: 4;
	unsigned SCANLINE	: 12;
	unsigned rsv_28		: 4;
} LCD_REG_SYNC_CNT, *PLCD_REG_SYNC_CNT;


typedef struct
{
    LCD_REG_STATUS				STATUS;				// 1000
//    UINT16                    	rsv_0002;           // 1002
    LCD_REG_INTERRUPT         	INT_ENABLE;         // 1004
//    UINT16                    	rsv_0006;           // 1006
    LCD_REG_INTERRUPT         	INT_STATUS;         // 1008
//    UINT16                    	rsv_000A;           // 100A
    LCD_REG_START             	START;              // 100C
//    UINT16                    	rsv_000E;           // 100E
    UINT32                    	RESET;              // 1010
    UINT32                    	rsv_0014[2];        // 1014..1018
	LCD_REG_SIF_TIMING		  	SIF_TIMING[2];	  	// 101C..1020
    UINT32                    	rsv_0024;        	// 1024
	LCD_REG_SCNF			  	SERIAL_CFG;		  	// 1028
	LCD_REG_SIF_CS			  	SIF_CS;			  	// 102C
    LCD_REG_PCNF              	PARALLEL_CFG[3];    // 1030..1038
    LCD_REG_PCNFDW            	PARALLEL_DW;        // 103C
    LCD_REG_GAMCON            	GAMCON;             // 1040
	LCD_REG_CALC_HTT			CALC_HTT;			// 1044
	LCD_REG_SYNC_LCM_SIZE		SYNC_LCM_SIZE;		// 1048
	LCD_REG_SYNC_CNT			SYNC_CNT;			// 104C
    LCD_REG_TECON             	TEARING_CFG;        // 1050
    LCD_REG_GMCCON            	GMC_CON;            // 1054
    UINT32                    	rsv_0054[2];        // 1058..105C
    UINT32                    	WROI_W2M_ADDR[3];   // 1060..1068
    UINT32                    	rsv_006c;           // 106C
    UINT16                    	W2M_PITCH;          // 1070
    UINT16                   	rsv_0072;           // 1072    
    LCD_REG_W2M_OFFSET        	WROI_W2M_OFFSET;    // 1074
    LCD_REG_WROI_W2MCON       	WROI_W2M_CONTROL;   // 1078
    UINT32                    	rsv_007C;	        // 107C
    LCD_REG_WROI_CON          	WROI_CONTROL;       // 1080
    LCD_REG_COORD             	WROI_OFFSET;        // 1084
    LCD_REG_CMD_ADDR           	WROI_CMD_ADDR;      // 1088
//    UINT16                      rsv_008A;
    LCD_REG_DAT_ADDR           	WROI_DATA_ADDR;     // 108c
//    UINT16                      rsv_008E;
    LCD_REG_SIZE              	WROI_SIZE;          // 1090
    LCD_WROI_HWREF            	WROI_HW_REFRESH;    // 1094
    LCD_REG_WROI_DC           	WROI_DC;            // 1098
    UINT32                    	WROI_BG_COLOR;      // 109C
    LCD_REG_DSI_DC            	DS_DSI_CON;         // 10A0
    UINT32                    	rsv_00A4[3];        // 10A4..10AC
    LCD_REG_LAYER             	LAYER[6];           // 10B0..11CC
    UINT32                    	rsv_00D0[4];        // 11D0..11DC
    UINT32                    	WROI_HWREF_BLK;     // 11E0
    UINT32                    	WROI_HWREF_DLY;     // 11E4
    UINT32                    	rsv_01E8[2];        // 11E8..11EC
    LCD_REG_DITHER_CON        	DITHER_CON;         // 11F0
    UINT32                    	rsv_01F4[3];        // 11F4..11FC
    UINT32                    	LGMA_CON[20];       // 1200..124C
    LCD_REG_COEF_ROW          	COEF_ROW[6];        // 1250..1264
//	#warning "need to implement the debug/control register"
    UINT32                    	rsv_0268[358];      // 1268..17FC
	UINT32                    	GAMMA[256];			// 1800..1BFF
	UINT32						CMDQ[64];		    // 1C00..1CFC
	UINT32						rsv_1D00[128];		// 1D00..1EFC
    UINT32                    	PCMD0;             // 1F00
    UINT32                    	rsv_1F04[7];	    // 1F04..1F1C
    UINT32                    	PCMD1;             // 1F20 
    UINT32                    	rsv_1F24[7];  		// 1F24..1F3C
    UINT32                    	PCMD2;             // 1F40 
    UINT32                    	rsv_1F44[15];	   	// 1F44..1F7C
    UINT32                    	SCMD0;             // 1F80
    UINT32                    	rsv_1F84[7];   		// 1F84..1F9C
    UINT32                    	SCMD1;             // 1FA0
    UINT32                    	rsv_1FA4[7];    	// 1FA4..1FBC
} volatile LCD_REGS, *PLCD_REGS;

#ifndef BUILD_UBOOT
STATIC_ASSERT(0x30 == sizeof(LCD_REG_LAYER));

STATIC_ASSERT(0x0000 == offsetof(LCD_REGS, STATUS));
STATIC_ASSERT(0x0004 == offsetof(LCD_REGS, INT_ENABLE));
STATIC_ASSERT(0x0028 == offsetof(LCD_REGS, SERIAL_CFG));
STATIC_ASSERT(0x0030 == offsetof(LCD_REGS, PARALLEL_CFG));
STATIC_ASSERT(0x0040 == offsetof(LCD_REGS, GAMCON));
STATIC_ASSERT(0x0050 == offsetof(LCD_REGS, TEARING_CFG));

STATIC_ASSERT(0x0060 == offsetof(LCD_REGS, WROI_W2M_ADDR));
STATIC_ASSERT(0x00B0 == offsetof(LCD_REGS, LAYER));
STATIC_ASSERT(0x01E0 == offsetof(LCD_REGS, WROI_HWREF_BLK));

STATIC_ASSERT(0x01F0 == offsetof(LCD_REGS, DITHER_CON));
STATIC_ASSERT(0x0250 == offsetof(LCD_REGS, COEF_ROW));
STATIC_ASSERT(0x0800 == offsetof(LCD_REGS, GAMMA));
STATIC_ASSERT(0x0C00 == offsetof(LCD_REGS, CMDQ));

STATIC_ASSERT((0xF00) == offsetof(LCD_REGS, PCMD0));
STATIC_ASSERT((0xF80) == offsetof(LCD_REGS, SCMD0));

STATIC_ASSERT(0xFC0 == sizeof(LCD_REGS));
#endif

#define LCD_A0_LOW_OFFSET  (0x0)
#define LCD_A0_HIGH_OFFSET (0x10)

#ifdef __cplusplus
}
#endif

#endif // __LCD_REG_H__
