#ifndef __MTK_CCCI_HELPER_H
#define __MTK_CCCI_HELPER_H

#include <mach/eint.h>
#include <mach/mt6577_gpio.h>
#include <mach/mt6577_reg_base.h>


#define MAX_KERN_API 20
#define MAX_SLEEP_API 20


//error code
#define E_NO_EXIST			-1
#define E_PARAM				-2



/*modem image version definitions*/
typedef enum{
	AP_IMG_INVALID = 0,
	AP_IMG_2G,
	AP_IMG_3G
}AP_IMG_TYPE;


typedef enum {
	ID_GET_MD_WAKEUP_SRC = 0,
	ID_CCCI_DORMANCY = 1,
}KERN_FUNC_ID;
typedef int (*ccci_kern_cb_func_t)(char *buf, unsigned int len);

typedef struct{
	KERN_FUNC_ID id;
	ccci_kern_cb_func_t func;
}ccci_kern_func_info;

typedef enum {
	 RSM_ID_RESUME_WDT_IRQ = 0,
	 RSM_ID_MD_LOCK_DORMANT = 1,
	 RSM_ID_MAX
}RESUME_ID;
 
typedef enum {
	 SLP_ID_MD_FAST_DROMANT = 0,
	 SLP_ID_MD_UNLOCK_DORMANT = 1,
	 SLP_ID_MAX
}SLEEP_ID;

extern AP_IMG_TYPE get_ap_img_ver(void);

extern int IMM_get_adc_channel_num(char *channel_name, int len);
extern int get_td_eint_info(char * eint_name, unsigned int len);
extern int get_md_gpio_info(char *gpio_name, unsigned int len);
extern int get_md_adc_info(char *adc_name, unsigned int len);

extern void AudSys_Power_On(bool on);

extern int register_ccci_kern_func(unsigned int id, ccci_kern_cb_func_t func);
extern int exec_ccci_kern_func(unsigned int id, char *buf, unsigned int len);

extern int get_dram_info(int *clk, int *type);
extern int get_dram_type_clk(int *clk, int *type);

extern void register_suspend_notify(unsigned int id, void (*func)(void));
extern void register_resume_notify(unsigned int id, void (*func)(void));



#endif
