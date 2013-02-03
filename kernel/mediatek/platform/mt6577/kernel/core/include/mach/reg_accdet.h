#include <mach/mt6577_reg_base.h>
//Register address define
#define PMIC_RESERVE_CON2 0xF0007500

#define ACCDET_RSTB              ACCDET_BASE + 0x00
#define ACCDET_CTRL              ACCDET_BASE + 0x04
#define ACCDET_STATE_SWCTRL      ACCDET_BASE + 0x08
#define ACCDET_PWM_WIDTH         	 ACCDET_BASE + 0x0C
#define ACCDET_PWM_THRESH        	 ACCDET_BASE + 0x10
#define ACCDET_EN_DELAY_NUM      ACCDET_BASE + 0x24
#define ACCDET_PWM_IDLE          ACCDET_BASE + 0x28
#define ACCDET_DEBOUNCE0         ACCDET_BASE + 0x2C
#define ACCDET_DEBOUNCE1         ACCDET_BASE + 0x30
#define ACCDET_DEBOUNCE2         ACCDET_BASE + 0x34
#define ACCDET_DEBOUNCE3         ACCDET_BASE + 0x38
#define ACCDET_IRQ_STS           ACCDET_BASE + 0x3C
#define ACCDET_CURR_IN           ACCDET_BASE + 0x40
#define ACCDET_SAMPLE_IN         ACCDET_BASE + 0x44
#define ACCDET_MEMORIZED_IN      ACCDET_BASE + 0x48
#define ACCDET_LAST_MEM_IN       ACCDET_BASE + 0x4C
#define ACCDET_FSM_STATE         ACCDET_BASE + 0x50
#define ACCDET_CURR_DEBDS        ACCDET_BASE + 0x54
#define ACCDET_TV_START_LINE0    ACCDET_BASE + 0x58
#define ACCDET_TV_END_LINE0      ACCDET_BASE + 0x5C
#define ACCDET_TV_START_LINE1    ACCDET_BASE + 0x60
#define ACCDET_TV_END_LINE1      ACCDET_BASE + 0x64
#define ACCDET_TV_PRE_LINE       ACCDET_BASE + 0x68
#define ACCDET_TV_START_PXL      ACCDET_BASE + 0x6C
#define ACCDET_TV_END_PXL        ACCDET_BASE + 0x70
#define ACCDET_TV_EN_DELAY_NUM   ACCDET_BASE + 0x74
#define ACCDET_TV_DIV_RATE       ACCDET_BASE + 0x78
#define ACCDET_RSV_CON3			 ACCDET_BASE + 0x0F0C


//Register value define
#define RSTB_BIT        (0<<0)
#define RSTB_FINISH_BIT (1<<0)
#define MIC_INIT_BIT (1<<1)
#define TV_INIT_BIT (1<<2)

#define IRQ_STATUS_BIT  (1<<0)
#define IRQ_CLR_BIT     (1<<8)
#define IRQ_CLR_FINISH  (0<<8)
#define IRQ_CLR_SC_BIT  (1<<9)

#define ACCDET_DEFVAL_SEL (1<<15)

#define ACCDET_ENABLE   (1<<0)
#define ACCDET_DISABLE  (0<<0)

#define ANASW_BIT  (1<<0)
#define TV_DET_BIT    (1<<1)
#define CMP_PWM_EN_BIT (1<<2)
#define VTH_PWM_EN_BIT (1<<3)
#define MBIAS_PWM_EN_BIT (1<<4)
#define ACCDET_SWCTRL_EN (MBIAS_PWM_EN_BIT | VTH_PWM_EN_BIT | CMP_PWM_EN_BIT | ANASW_BIT)
#define ACCDET_MIC_CHA	(0<<0)
#define ACCDET_TV_CHA	(1<<0)  

//PMIC reserve CON2 value define
#define MIC_FORCE_LOW   (1<<12)

#define VMIC_EN_BIT (1<<4)
#define VMIC_LOW_POWER_BIT (1<<7)

