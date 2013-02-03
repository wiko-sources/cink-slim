#ifndef __MTK_MAU_REG_H__
#define __MTK_MAU_REG_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


#define REG_SMI_MAU_ENTR0_START      0x0300
#define REG_SMI_MAU_ENTR0_END        0x0304
#define REG_SMI_MAU_ENTR0_GID_0      0x0308
#define REG_SMI_MAU_ENTR1_START      0x0310
#define REG_SMI_MAU_ENTR1_END        0x0314
#define REG_SMI_MAU_ENTR1_GID_0      0x0318
#define REG_SMI_MAU_ENTR2_START      0x0320
#define REG_SMI_MAU_ENTR2_END        0x0324
#define REG_SMI_MAU_ENTR2_GID_0      0x0328


#define REG_SMI_MAU_ENTR0_STAT       0x0500
#define REG_SMI_MAU_ENTR1_STAT       0x0504
#define REG_SMI_MAU_ENTR2_STAT       0x0508





typedef struct {
    unsigned STR    : 30;
    unsigned RD     : 1;
    unsigned WR     : 1;
} MAU_REG_RANGE_START, *PMAU_REG_RANGE_START;

typedef struct {
    unsigned END        : 30;
    unsigned VP_CTRL    : 1;  // 0:SYSRAM PA 1: MVA
    unsigned rsv        : 1;  // no use
} MAU_REG_RANGE_END, *PMAU_REG_RANGE_END;





// MAU0 ASSERT ID bit: Entyr 0: 4 Entyr 1: 7 Entyr 2: 7
// MAU1 ASSERT ID bit: Entyr 0: 5 Entyr 1: 7 Entyr 2: 7
// MAU2 ASSERT ID bit: Entyr 0: 3 Entyr 1: 7 Entyr 2: 7
// MAU2 ASSERT ID bit: Entyr 0: 2 Entyr 1: 7 Entyr 2: 7



typedef struct {
    unsigned ASSERT_ID : 7;
    unsigned ASSERT    : 1;
    unsigned rsv_8     : 24;
} MAU_REG_ENT_STATUS, *PMAU_REG_ENT_STATUS;





#define MAU0_SMI_LARB_CON        0xF2081010 //MAU_IRQ_EN bit:10 RO
#define MAU0_SMI_LARB_CON_SET    0xF2081014 //MAU_IRQ_EN bit:10 WO
#define MAU0_SMI_LARB_CON_RESET  0xF2081018 //MAU_IRQ_EN bit:10 WO

#define MAU1_SMI_LARB_CON        0xF2082010 //MAU_IRQ_EN bit:10 RO
#define MAU1_SMI_LARB_CON_SET    0xF2082014 //MAU_IRQ_EN bit:10 WO
#define MAU1_SMI_LARB_CON_RESET  0xF2082018 //MAU_IRQ_EN bit:10 WO

#define MAU2_SMI_LARB_CON        0xF2083010 //MAU_IRQ_EN bit:10 RO
#define MAU2_SMI_LARB_CON_SET    0xF2083014 //MAU_IRQ_EN bit:10 WO
#define MAU2_SMI_LARB_CON_RESET  0xF2083018 //MAU_IRQ_EN bit:10 WO

#define MAU3_SMI_LARB_CON        0xF20C1010 //MAU_IRQ_EN bit:10 RO
#define MAU3_SMI_LARB_CON_SET    0xF20C1014 //MAU_IRQ_EN bit:10 WO
#define MAU3_SMI_LARB_CON_RESET  0xF20C1018 //MAU_IRQ_EN bit:10 WO

#define MAU0_REG_BASE            0xF2081300
#define MAU1_REG_BASE            0xF2082300
#define MAU2_REG_BASE            0xF2083300
#define MAU3_REG_BASE            0xF20C1300



typedef struct {
    MAU_REG_RANGE_START  ENT0_STR;   //+00
    MAU_REG_RANGE_END    ENT0_END;   //+04
    unsigned int         ENT0_GID;   //+08
    unsigned int         rsv_0C;

    MAU_REG_RANGE_START  ENT1_STR;   //10
    unsigned int         ENT1_END;   //14
    unsigned int         ENT1_GID;   //18
    unsigned int         rsv_1C;

    MAU_REG_RANGE_START  ENT2_STR;   //20
    unsigned int         ENT2_END;   //24
    unsigned int         ENT2_GID;   //28
    unsigned int         rsv_2C[117];     //2c

    MAU_REG_ENT_STATUS   ENT0_STATUS;      //+200
    MAU_REG_ENT_STATUS   ENT1_STATUS;      //+204
    MAU_REG_ENT_STATUS   ENT2_STATUS;      //+208

    unsigned int         rsv_20C[5];

    unsigned int         INTERRUPT;       //+220
} volatile MAU_REGS, *PMAU_REGS;



extern PMAU_REGS const MAU0_REG;
extern PMAU_REGS const MAU1_REG;
extern PMAU_REGS const MAU2_REG;
extern PMAU_REGS const MAU3_REG;


#ifdef __cplusplus
}
#endif

#endif // __MTK_MAU_REG_H__






