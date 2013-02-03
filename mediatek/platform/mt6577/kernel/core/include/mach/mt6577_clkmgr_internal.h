#ifndef _MT6577_CLKMGR_INTERNAL_H
#define _MT6577_CLKMGR_INTERNAL_H

#include <linux/list.h>
#include "mach/mt6577_clock_manager.h"


/* Macro definition for clock debug mechanism */
//#define OP_MODE_DEBUG
#define CLK_MGR_DEBUG
#define FORCE_UNGATE_DEBUG 



#define MAX_NAME_LEN                    32

enum state {
    STATE_ON    = 0,   /* clock, pll, subsys is enabled or ungated or power on */
    STATE_OFF   = 1,   /* clock, pll, subsys is disabled or gated or power down */
};

struct clock {
    int count;
    enum state init_state;
    char name[MAX_NAME_LEN];
#ifdef CLK_MGR_DEBUG
    struct list_head list;
#endif
};


struct pll { 
    int count; 
    enum state init_state;
    char name[MAX_NAME_LEN];        
#ifdef CLK_MGR_DEBUG
    struct list_head list;
#endif
};

struct subsys {
    enum state init_state;
    char name[MAX_NAME_LEN];
#ifdef CLK_MGR_DEBUG
    struct list_head list;
#endif
};

#ifdef CLK_MGR_DEBUG
struct module_info {
    char module_name[MAX_NAME_LEN];
    int enable_cnt;
    int disable_cnt;
    struct list_head link;
};
#endif

struct clock_manager {    
    struct clock clock[MT65XX_CLOCK_CATEGORY_COUNT][CLK_REG_WIDTH];
    struct pll pll[MT65XX_PLL_COUNT];
    struct subsys subsys[MT65XX_SUBSYS_COUNT];
    unsigned int clock_state[MT65XX_CLOCK_CATEGORY_COUNT];
    unsigned int pll_state;  
    unsigned int subsys_state;  
    unsigned int mipi_state;
    unsigned int mmsys_state;
		unsigned int mmsys_state_mfg;
};


extern struct clock_manager clk_mgr;

struct mipi {
    int count;
    char name[MAX_NAME_LEN];
#ifdef CLK_MGR_DEBUG
    struct list_head list;
#endif
};

extern struct mipi mipi[MT65XX_MIPI_COUNT];

extern const unsigned int valid_clock_mask[MT65XX_CLOCK_CATEGORY_COUNT];
extern const int mt65xx_clock_number_per_category[MT65XX_CLOCK_CATEGORY_COUNT];

/* array for name */
extern const char mt65xx_category_name[MT65XX_CLOCK_CATEGORY_COUNT][MAX_NAME_LEN];
extern const char mt65xx_pll_name[MT65XX_PLL_COUNT][MAX_NAME_LEN];
extern const char mt65xx_subsys_name[MT65XX_SUBSYS_COUNT][MAX_NAME_LEN];

/* array for register address */
extern const unsigned int mt65xx_cg_con_regs[MT65XX_CLOCK_CATEGORY_COUNT];
extern const unsigned int mt65xx_pll_regs[MT65XX_PLL_COUNT];
extern const unsigned int mt65xx_subsys_regs[MT65XX_SUBSYS_COUNT]; 


extern unsigned int get_pll_value(enum mt65xx_pll_id id);
extern int pll_fsel(enum mt65xx_pll_id id, unsigned int pll_value);


extern int request_mmsys_clksrc(int opcode, int is_mfg);


extern void dump_clk_regs(void);
extern void dump_pll_regs(void);


/* invalid clock id includes ids that not exist or not allowed to operate by AP side */
#define INVALID_CATEGORY_ID(id) \
    ((id) < 0 || (id) >= MT65XX_CLOCK_CATEGORY_COUNT)

#define INVALID_CLOCK_ID(category, mask) \
    ((valid_clock_mask[category] & (mask)) != (mask)) /* support multi bits check */

#define INVALID_PLL_ID(id) \
    ((id) < 0 || (id) >= MT65XX_PLL_COUNT)

#define INVALID_SUBSYS_ID(id) \
    ((id) <= MT65XX_SUBSYS_NEON || (id) >= MT65XX_SUBSYS_COUNT)



enum {
    TYPE_CATEGORY   = 0x0,
    TYPE_PLL        = 0x1,
    TYPE_SUBSYS     = 0x2,
};

enum {
    LOCAL_STATE = 1U << 0,     /* state recorded by clock manager */
    REG_STATE   = 1U << 1,     /* state read from registers */
};

extern int get_state(int type, int id, int mode);

extern int get_clkmgr_init_state(void);

/* API for debug mechanism */

extern unsigned int freq_meter(int clk_sel, unsigned int delay);

/* API for debugging larb monitor mechanism */
extern int get_larb_monitor_handler_number(void);
extern struct larb_monitor* get_larb_monitor_handler(int handler_idx);
extern void get_larb_monitor_handlers(struct larb_monitor **array);

/* API for partial log printing mechanism */
extern unsigned int partial_log_mask[MT65XX_CLOCK_CATEGORY_COUNT];
extern void set_partial_log_bit(enum mt65xx_clock_id id);
extern void clr_partial_log_bit(enum mt65xx_clock_id id);
extern void set_partial_log_mask(int category, unsigned int mask);
extern void clr_partial_log_mask(int category, unsigned int mask);

/* API for force ungate clock bits mechanism */
#ifdef FORCE_UNGATE_DEBUG
extern unsigned int force_ungate_mask[MT65XX_CLOCK_CATEGORY_COUNT];
extern void set_force_ungate_bit(enum mt65xx_clock_id id);
extern void clr_force_ungate_bit(enum mt65xx_clock_id id);
extern void set_force_ungate_mask(int category, unsigned int mask);
extern void clr_force_ungate_mask(int category, unsigned int mask);
#endif /* !ifdef FORCE_UNGATE_DEBUG */


#endif /* !ifdef _MT6577_CLKMGR_INTERNAL_H */
