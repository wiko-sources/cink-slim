#ifndef __CUST_OFN_H__
#define __CUST_OFN_H__

#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_typedefs.h>
#include <mach/mt6577_gpio.h>
#include <mach/mt6577_pm_ldo.h>

/*----------------------------------------------------------------------------*/
typedef enum {
    OFN1026 = 1026,
    OFN1086 = 1086,
    OFN1090 = 1090,
}OFN_ID;
/*----------------------------------------------------------------------------*/
typedef enum {
    OFN_DETECT_POLL     = 0,
    OFN_DETECT_EINT     = 1,            
} OFN_DETECT;
/*----------------------------------------------------------------------------*/
typedef enum {
    OFN_CLASS_KEYBOARD,
    OFN_CLASS_TRACKBALL, 
} OFN_CLASS;
/*----------------------------------------------------------------------------*/
struct ofn_hw {
    int             power_id;   /*!< LDO usage */
    int             power_vol;  /*!< LDO usage */

    int             report_cls; /*!< refer to OFN_CLASS*/
    OFN_ID          chip_id;    /*!< different chip id will different power on sequence*/
    int             slave_addr; /*!< the slave address will be changed according to layout*/   
    int             i2c_num;    /*!< the number of used i2c bus*/
    unsigned int    layout;     /*!< Three bits: (XY_SWAP, Y_INV, X_INV)*/

    /*trackball class*/
    int             quan_x;     /*!< quantized step in x-axis*/
    int             quan_y;     /*!< quantized step in y-axis*/
    int             accu_max;   /*!< the maximum step accumulated*/

    /*keyboard class*/    
    int             detect;     /*!< 0:poll; 1: detect*/        
    int             gpt_num;    /*!< The GPT number used in polling mode*/
    int             gpt_period; /*!< the GPT period used in polling mode*/
    
    int             acc_cnt;    /*!< the maximum period of triggered event in one direction*/
    int             inact_cnt;  /*!< the maximum period of two consecutive motion event in one direction*/
    int             act_cnt;    /*!< the minimum period of two consecutive motion event in one direction*/
    int             sup_cnt;    /*!< supress level: any value report in single motion event larger than this value will be set as this value*/
    int             step;       /*!< the minimum count of triggering key event*/
};
/*----------------------------------------------------------------------------*/
extern struct ofn_hw *get_cust_ofn_hw(void);
/*----------------------------------------------------------------------------*/
#endif
