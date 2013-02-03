#include <linux/string.h>
#ifdef BUILD_UBOOT
#include <asm/arch/mt6577_gpio.h>
#else
#include <mach/mt6577_gpio.h>
#endif
#include <cust_gpio_usage.h>
#include "lcm_drv.h"

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (480)
#define FRAME_HEIGHT (800)
#define LCM_ID       (0x1408)
#define REGFLAG_DELAY             							0XFE
#define REGFLAG_END_OF_TABLE      							0xFF   // END OF REGISTERS MARKER
#define GPIO_DISP_LRSTB_PIN GPIO18
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static unsigned int lcm_compare_id(void);
static LCM_UTIL_FUNCS lcm_util = {0};

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)									lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg											lcm_util.dsi_read_reg()
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)           

#define LCM_DSI_CMD_MODE

static struct LCM_setting_table {
    unsigned cmd;
    unsigned char count;
    unsigned char para_list[64];
};

static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/

	//#Enable Page1
	{0xB0,	1,	{0x04}},
	{REGFLAG_DELAY, 1, {}},
	//# VGMP/VGMN/VOOM Setting, VGMP=4.8V  #VGSP=0.6125V
	{0xB3,	2,	{0x02,0x00}},
	{REGFLAG_DELAY, 1, {}},
	//# VGMN=-4.8V  #VGSN=-0.6125V
	{0xBF,	2,	{0xBD,0x00}},
	{REGFLAG_DELAY, 1, {}},
	//#VCOM=
	{0xC1,	16,	{0x43,0x31,0x39,0x21,0x20,0x32,0x12,0x28,0x0c,
	             0x0c,0xA5,0x0F,0x58,0x21,0x01}},
	{REGFLAG_DELAY, 1, {}},

	//#R+
	{0xC2,	6,	{0x28,0x06,0x06,0x01,0x03,0x00}},
	{REGFLAG_DELAY, 1, {}},
	//#G+
	{0xC8,	24,	{0x00,0x0D,0x1F,0x29,0x36,0x4D,0x37,0x25,0x19,0x11,
				0x08,0x01,0x00,0x0D,0x1F,0x29,0x36,0x4D,0x37,0x25,0x19,
				0x11,0x08,0x01,
			}},
	{REGFLAG_DELAY, 1, {}},
	//#B+
	{0xC9,	24,	{0x00,0x0D,0x1F,0x29,0x36,0x4D,0x37,0x25,0x19,0x11,
				0x08,0x01,0x00,0x0D,0x1F,0x29,0x36,0x4D,0x37,0x25,0x19,
				0x11,0x08,0x01,
			}},
	{REGFLAG_DELAY, 1, {}},
	//#R-
	{0xCA,	24,	{0x00,0x0D,0x1F,0x29,0x36,0x4D,0x37,0x25,0x19,0x11,
				0x08,0x01,0x00,0x0D,0x1F,0x29,0x36,0x4D,0x37,0x25,0x19,
				0x11,0x08,0x01,
			}},
	{REGFLAG_DELAY, 1, {}},
	//#G-
	{0xD0,	16,	{0x29,0x03,0xCE,0xA6,0x00,0x43,0x20,0x10,0x01,0x00,
				0x01,0x01,0x00,0x03,0x01,0x00}},
	{REGFLAG_DELAY, 1, {}},
	//#B-
	{0xD1,	7,	{0x18,0x0C,0x23,0x03,0x75,0x02,0x50}},
	{REGFLAG_DELAY, 1, {}},	
	
	{0xD3,	1,	{0x33}},
	{REGFLAG_DELAY, 1, {}},
	//# AVDD: manual,
	{0xD5,	2,	{0x2A,0x2A}},
	{REGFLAG_DELAY, 1, {}},
	//#Power Control for VCL
	{0xDE,	2,	{0x01,0x51}},
	{REGFLAG_DELAY, 1, {}},
	
	{0xE6,	1,	{0x51}},
	{REGFLAG_DELAY, 1, {}},
	//# AVEE: manual, ©\6V
	{0xFA,	1,	{0x03}},
	{REGFLAG_DELAY, 1, {}},
	
	{0xD6,	1,	{0x28}},//{0x34,0x34,0x34}
	{REGFLAG_DELAY, 100, {}},
	//# VGL(LVGL):
	{0x2A,	4,	{0x00,0x00,0x01,0xDF}},//{0x14,0x14,0x14}
	{REGFLAG_DELAY, 1, {}},
	
	//#Enable Page0
	{0x2B,	4,	{0x00,0x00,0x03,0x1F}},
	{REGFLAG_DELAY, 1, {}},
	
	{0x36,	1,	{0x00}},
	{REGFLAG_DELAY, 1, {}},

	{0x3A,	1,	{0x77}},
	{REGFLAG_DELAY, 1, {}},
	
	{0x11,	0,	{0x00}},
	{REGFLAG_DELAY, 200, {}},
	
	{0x29,	0,	{0x00}},
	{REGFLAG_DELAY, 20, {}},

	{0x2C,	0,	{0x00}},
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};

static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
	unsigned int i;

	for(i = 0; i < count; i++) {
		
	    unsigned cmd;
	    cmd = table[i].cmd;
		
	    switch (cmd) {
			
	        case REGFLAG_DELAY :
	            MDELAY(table[i].count);
	            break;
				
	        case REGFLAG_END_OF_TABLE :
	            break;
				
	        default:
//				dsi_send_cmdq_tinno(cmd, table[i].count, table[i].para_list, force_update);
				dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
	   	}
	}

}

static void init_lcm_registers(void)
{
	unsigned int data_array[16];
#if defined(LCM_DSI_CMD_MODE)
	data_array[0] = 0x04B01500;//B0
	dsi_set_cmdq(&data_array, 1, 1);
	 
	 data_array[0] = 0x00033902;//MIPI DSI Control
	 data_array[1] = 0x000002B3; //B6
	 dsi_set_cmdq(&data_array, 2, 1);
	 
	data_array[0] = 0x00033902;//MIPI DSI Control
	data_array[1] = 0x0000BDBF; //B6
	dsi_set_cmdq(&data_array, 2, 1);
	 
	data_array[0] = 0x00163902;//MIPI DSI Control
	data_array[1] = 0x393143C1; 
	data_array[2] = 0x12322021;
	data_array[3] = 0xA50C0C28;
	data_array[4] = 0x0121580F;
	dsi_set_cmdq(&data_array, 5, 1);
	 
	data_array[0] = 0x00073902;
	data_array[1] = 0x060628C2; 
	data_array[2] = 0x00000301;
	dsi_set_cmdq(&data_array, 3, 1);
	  
	data_array[0] = 0x00253902;//MIPI DSI Control
	data_array[1] = 0x1F0D00C8; 
	data_array[2] = 0x374D3629;
	data_array[3] = 0x08111925;
	data_array[4] = 0x1F0D0001;
	data_array[5] = 0x374D3629;
	data_array[6] = 0x08111925;
	data_array[7] = 0x00000001;
	dsi_set_cmdq(&data_array, 8, 1); 

	data_array[0] = 0x00253902;//MIPI DSI Control
	data_array[1] = 0x1F0D00C9; 
	data_array[2] = 0x374D3629;
	data_array[3] = 0x08111925;
	data_array[4] = 0x1F0D0001;
	data_array[5] = 0x374D3629;
	data_array[6] = 0x08111925;
	data_array[7] = 0x00000001;
	dsi_set_cmdq(&data_array, 8, 1); 
 
	data_array[0] = 0x00253902;//MIPI DSI Control
	data_array[1] = 0x1F0D00CA; 
	data_array[2] = 0x374D3629;
	data_array[3] = 0x08111925;
	data_array[4] = 0x1F0D0001;
	data_array[5] = 0x374D3629;
	data_array[6] = 0x08111925;
	data_array[7] = 0x00000001;
	dsi_set_cmdq(&data_array, 8, 1);  

	data_array[0] = 0x00173902;//MIPI DSI Control
	data_array[1] = 0x0E0329D0; 
	data_array[2] = 0x204300A6;
	data_array[3] = 0x01000110;
	data_array[4] = 0x01030001;
	data_array[5] = 0x00000000;
	dsi_set_cmdq(&data_array, 6, 1);  
	 
	data_array[0] = 0x00083902;//MIPI DSI Control
	data_array[1] = 0x230C18D1; 
	data_array[2] = 0x50027503;
	dsi_set_cmdq(&data_array, 3, 1);  
	 
	data_array[0] = 0x33D31500;//B0
	dsi_set_cmdq(&data_array, 1, 1);
	 

	data_array[0] = 0x00033902;//MIPI DSI Control
	data_array[1] = 0x002A2AD5; 
	dsi_set_cmdq(&data_array, 2, 1);  
	 

	data_array[0] = 0x00033902;//MIPI DSI Control
	data_array[1] = 0x005101DE; 
	dsi_set_cmdq(&data_array, 2, 1);  
	 
	data_array[0] = 0x51E61500;//B0
	dsi_set_cmdq(&data_array, 1, 1);

	data_array[0] = 0x03FA1500;//B0
	dsi_set_cmdq(&data_array, 1, 1);
	 

	data_array[0] = 0x28D61500;//B0
	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(100);

	data_array[0] = 0x00053902;//MIPI DSI Control
	data_array[1] = 0x0100002A; 
	data_array[2] = 0x000000DF; 
	dsi_set_cmdq(&data_array, 3, 1);  

	data_array[0] = 0x00053902;//MIPI DSI Control
	data_array[1] = 0x0300002B; 
	data_array[2] = 0x0000001F; 
	dsi_set_cmdq(&data_array, 3, 1);  

	data_array[0] = 0x00361500;//B0
	dsi_set_cmdq(&data_array, 1, 1); 

	data_array[0] = 0x773A1500;//B0
	dsi_set_cmdq(&data_array, 1, 1);
	 
	data_array[0] = 0x00110500;//B0
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(200);
	 
	data_array[0] = 0x00290500;//B0
	dsi_set_cmdq(&data_array, 1, 1);

	MDELAY(20);
	  
	data_array[0] = 0x002C0500;//B0
	dsi_set_cmdq(&data_array, 1, 1);
/*
	    data_array[0] = 0x04B02300;//B0
        dsi_set_cmdq(&data_array, 1, 1);
    	MDELAY(50);
    	data_array[0] = 0x00032902;//MIPI DSI Control
    	data_array[1] = 0x008352B6; //B6
    	dsi_set_cmdq(&data_array, 2, 1);
  
     	data_array[0] = 0x00152902;//BLC
     	data_array[1] = 0x090900B8; //B8
     	data_array[2] = 0xe6e6ffff;
     	data_array[3] = 0x10101802;
     	data_array[4] = 0xbe875a37;
     	data_array[5] = 0x000000ff;
     	data_array[6] = 0x00000000;
     	dsi_set_cmdq(&data_array, 7, 1);
     	MDELAY(50);
  
     	data_array[0] = 0x00052902;
     	data_array[1] = 0x02ff00B9; //B9
     	data_array[2] = 0x00000008; 
     	dsi_set_cmdq(&data_array, 3, 1);
  
    	data_array[0] = 0x00102902; // panel driving setting 2
    	data_array[1] = 0x043142C1; //C1
    	data_array[2] = 0x12322626;
    	data_array[3] = 0xA50a4a28; 
    	data_array[4] = 0x0121580F; 
    	dsi_set_cmdq(&data_array, 5, 1);
  
     	data_array[0] = 0x28d62300;//D6
     	dsi_set_cmdq(&data_array, 1, 1);
     	MDELAY(100);
  
     	data_array[0] = 0x00053902;
     	data_array[1] = 0x0100002A;//2A
     	data_array[2] = 0x000000DF;
     	dsi_set_cmdq(&data_array, 3, 1);
     	MDELAY(50);
  
     	data_array[0] = 0x00053902;
     	data_array[1] = 0x0300002B;//2B
     	data_array[2] = 0x0000001f;
     	dsi_set_cmdq(&data_array, 3, 1);
     	MDELAY(50);
  
     	data_array[0] = 0x00361500;//36
     	dsi_set_cmdq(&data_array, 1, 1);
     	MDELAY(50);

     	data_array[0] = 0x00351500;//36
     	dsi_set_cmdq(&data_array, 1, 1);
     	MDELAY(50);

     	data_array[0] = 0x773A1500;//3A
     	dsi_set_cmdq(&data_array, 1, 1);
     	MDELAY(50);
  
     	data_array[0] = 0x00110500;//11
     	dsi_set_cmdq(&data_array, 1, 1);
     
     	MDELAY(125);
     
     	data_array[0] = 0x00290500;//29
  	dsi_set_cmdq(&data_array, 1, 1);
 	MDELAY(125); */
#endif
}
// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
   //     lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
   //     MDELAY(10);

		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;
#if defined(LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_EVENT_VDO_MODE;
#endif	
		params->ctrl   = LCM_CTRL_PARALLEL_DBI;
		//params->ctrl   = LCM_CTRL_NONE;
		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;

		// enable tearing-free
//		params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		params->dbi.te_mode 				= LCM_DBI_TE_MODE_DISABLED;
//		params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

	
		// DSI
		// Command mode setting 
		params->dsi.LANE_NUM				= LCM_TWO_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		params->dsi.packet_size=256;

		// Video mode setting		
		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		params->dsi.word_count=480*3;
	
		params->dsi.vertical_sync_active=2;
		params->dsi.vertical_backporch=2;
		params->dsi.vertical_frontporch=2;
		params->dsi.vertical_active_line=800;
	
		params->dsi.line_byte=2180;		// 2256 = 752*3
		params->dsi.horizontal_sync_active_byte=26;
		params->dsi.horizontal_backporch_byte=206;
		params->dsi.horizontal_frontporch_byte=206;	
		params->dsi.rgb_byte=(480*3+6);	
	
		params->dsi.horizontal_sync_active_word_count=20;	
		params->dsi.horizontal_backporch_word_count=200;
		params->dsi.horizontal_frontporch_word_count=200;

		// Bit rate calculation
		params->dsi.pll_div1=30;		// fref=26MHz, fvco=fref*(div1+1)	(div1=0~63, fvco=500MHZ~1GHz)
		params->dsi.pll_div2=1;			// div2=0~15: fout=fvo/(2*div2)

}

static void lcm_init(void)
{
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(25);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    MDELAY(50);

//    init_lcm_registers();
	 push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
	 MDELAY(100);
//	clear_panel();
}


static void lcm_suspend(void)
{
	unsigned int data_array[16];

	data_array[0]=0x00280500; // Display Off
	dsi_set_cmdq(&data_array, 1, 1);
	MDELAY(10); 
	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(100);
//	printk("r61408 lcm_suspend\n");

//lcm_init();
//	lcm_compare_id();
}


static void lcm_resume(void)
{
	unsigned int data_array[16];

	data_array[0] = 0x00110500; // Sleep Out
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(100);
	data_array[0] = 0x00290500; // Display On
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(10);

}


static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	data_array[3]= 0x00053902;
	data_array[4]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[5]= (y1_LSB);
	data_array[6]= 0x002c3909;
//	data_array[6]= 0x002c3901;

	dsi_set_cmdq(data_array, 7, 0);

}

static void lcm_setbacklight(unsigned int level)
{
  unsigned int data_array[16];
  return;
  data_array[0] = 0x00052902;
  data_array[1] = 0x020000B9 | (level << 16); //B9
  data_array[2] = 0x00000008; 
  dsi_set_cmdq(data_array, 3, 1);
}

static unsigned int lcm_compare_id(void)
{
	unsigned int id = 0;
	unsigned char buffer[5];
	unsigned int array[16];
        //NOTE:should reset LCM firstly
         MDELAY(5);
   lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ZERO);
    MDELAY(25);
    lcm_util.set_gpio_out(GPIO_DISP_LRSTB_PIN, GPIO_OUT_ONE);
    	MDELAY(50);


/*	SET_RESET_PIN(1);
	MDELAY(25);
	SET_RESET_PIN(0);
	MDELAY(50);
	SET_RESET_PIN(1);
	MDELAY(150); */

	array[0] = 0x00053700;// read id return 5 byte
	dsi_set_cmdq(array, 1, 1);

	array[0] = 0x04B02300;// unlock for reading ID
	dsi_set_cmdq(array, 1, 1);
	MDELAY(50);

	read_reg_v2(0xBF, buffer, 5);
	id = (buffer[2] << 8) | buffer[3]; //we only need ID
#if defined(BUILD_UBOOT)
    printf("%s, r61408 buffer 2 = 0x%x\n", __func__, buffer[2]);
   printf("%s, r61408 buffer 3 = 0x%x\n", __func__, buffer[3]);
	printf("%s, id1 = 0x%x\n", __func__, id);
/*#else
   printk("%s, r61408 fdsfsdf= 0x%x\n", __func__, id);
   printk("%s, r61408 buffer 2 = 0x%x\n", __func__, buffer[2]);
   printk("%s, r61408 buffer 3 = 0x%x\n", __func__, buffer[3]);
  if( LCM_ID == id)
   	printk("Magnum r61408 compare id OK \n");
*/
#endif
    return (LCM_ID == id)?1:0;
}

LCM_DRIVER r61408_lcm_drv = 
{
    .name			= "r61408",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.update         = lcm_update,
	.compare_id     = lcm_compare_id
};

