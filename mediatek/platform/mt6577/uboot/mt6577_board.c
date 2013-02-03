/*
 * (C) Copyright 2008
 * MediaTek <www.mediatek.com>
 * Infinity Chen <infinity.chen@mediatek.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/arch/mt65xx_disp_drv.h>
//#include <asm/arch/mt65xx_logo.h>     //Remove tmp for 6577 fpga porting
#include <asm/arch/mtk_uart.h>
#include <asm/arch/mtk_serial.h>
#include <asm/arch/mtk_wdt.h>

#include <asm/arch/boot_mode.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include "mt65xx_partition.h"
#include <asm/arch/mt65xx_leds.h> //Remove tmp for 6577 fpga porting

#ifdef CFG_META_MODE
#include <asm/arch/mt65xx_meta.h>
#endif



//Ivan added
#define MENU_BOOT_ENABLE 1

DECLARE_GLOBAL_DATA_PTR;

// ===========================================
// EXTERNAL DEFINITIONS
// ===========================================
extern void dbg_print(char *sz,...);
extern u32 memory_size(void);
extern void mt65xx_pmu_init(void);
extern U32 pmic6329_init (void);


#ifdef CFG_HAS_USB
extern int	drv_usbtty_init (void);
#endif 

#if 1
void Uboot_power_saving(void)
{
#if 0
    u32 u4Val = 0;
    
#if 0   
	printf("GPIO Set\n");
    /* GPIO48~55, turn-off PWM 0~1 */
    DRV_WriteReg32(0x80002660, 0x0555);
    /* GPIO56~63, turn-off EINT 0~4 */
    DRV_WriteReg32(0x80002670, 0x0015);
    /* GPIO64~71, turn-off EINT 5~7, UART4 CTS/RTS */
    DRV_WriteReg32(0x80002680, 0x1400);
    /* GPIO112~119, turn-off KP Row2~4, CLK_OUT0~4 */
    DRV_WriteReg32(0x800026E0, 0x0000);
    /* GPIO128~135, Keep I2C SCL2 ,turn-off others */
    DRV_WriteReg32(0x80002700, 0x4000);    
    /* GPIO136~143, Keep I2C SDA2 ,turn-off TRACE signal */
    DRV_WriteReg32(0x80002710, 0x0001);    
#endif

    /* TV power down*/
    DRV_ClrReg32(TVENC, 0x13E0);

    /* AFE power down*/
    DRV_WriteReg32(PDN_AFE_AAPDN, 0); 
    DRV_WriteReg32(PDN_AFE_AAC_NEW, 0);
    DRV_WriteReg32(PDN_AFE_AAC_CON1, 0x0003);

    /* MIPI power down 
       Jett: Don't power down MIPI during uboot, or the DPI signal would
             be turned off and the uboot logo is disappeared
    */
#if 0
    PW_DEBUG("MIPI power down\n");
    DRV_WriteReg32(MIPI_PD_B00, 0);
    DRV_WriteReg32(MIPI_PD_B04, 0);
    DRV_WriteReg32(MIPI_PD_B08, 0);
    DRV_WriteReg32(MIPI_PD_B0C, 0);
    DRV_WriteReg32(MIPI_PD_B10, 0);
    DRV_WriteReg32(MIPI_PD_B14, 0);
    DRV_WriteReg32(MIPI_PD_B18, 0);
    DRV_WriteReg32(MIPI_PD_B1C, 0);
    DRV_WriteReg32(MIPI_PD_B40, 0);
    DRV_WriteReg32(MIPI_PD_B44, 0);
    DRV_WriteReg32(MIPI_PD_B48, 0);
    DRV_WriteReg32(MIPI_PD_B4C, 0);
    DRV_WriteReg32(MIPI_PD_04C, 1);
#endif

	/* MCU CG*/
    DRV_SetReg32 (APMCUSYS_PDN_SET0, (1<<8)|(1<<9)|(1<<21));

    /* MCU memory PDN*/
    u4Val = 0;
    u4Val = ((1<<5)|(1<<6)|(1<<7)|(1<<8)|(1<<9)|(1<<10)|    \
        (1<<11)|(1<<12)|(1<<13)|(1<<16) );
    DRV_SetReg32 (MCU_MEM_PDN, u4Val);


    /* GRAPH1SYS CG*/
    DRV_SetReg32 (GRAPH1SYS_CG_SET, (1<<0));

    /* GRAPH1SYS memory PDN*/
    u4Val = 0;
    u4Val = ((1<<2)|(1<<3)|(1<<4)|(1<<6)|(1<<9));
    DRV_SetReg32 (G1_MEM_PDN, u4Val);

	/* GRAPH2SYS CG */
    DRV_SetReg32 (GRAPH2SYS_CG_SET, 0x1FF);	

    /* GRAPH2SYS  memory PDN*/
    u4Val = 0;
    u4Val = ((1<<0));
    DRV_SetReg32 (G2_MEM_PDN, u4Val);

	/* GRAPH2SYS MTCMOS */
	DRV_SetReg16(ISO_EN, 1<<4);
	DRV_SetReg16(IN_ISO_EN, 1<<4 );    
	DRV_SetReg16(PWR_OFF, 1<<4 );
	DRV_SetReg16(ACK_CLR, 0x2); 
	
    /* CEVA memory PDN*/
    u4Val = 0;
    u4Val = ((1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5));
    DRV_SetReg32 (CEVA_MEM_PDN, u4Val);

	/* CEVA MTCMOS */	
	DRV_SetReg16(ISO_EN, 1<<5);
	DRV_SetReg16(IN_ISO_EN, 1<<5 );    
	DRV_SetReg16(PWR_OFF, 1<<5 );
	DRV_SetReg16(ACK_CLR, 0x1); 

    /* Stop CEVA PLL*/
    DRV_ClrReg32(CEVAPLL2,1<<0);

    /* Stop UPLL*/
    DRV_ClrReg32(PDN_CON,1<<4);


    /* Stop DPLL*/
#if 0
	printf("DPLL Power Down\n");
    DRV_ClrReg32(PDN_CON,1<<3);
#endif

	/* Stop VUSB */
	//pmic_config_interface(0x3D, KAL_FALSE, VUSB_EN_MASK, VUSB_EN_SHIFT);	 //pmic_vsdio_enable(KAL_TRUE);

#endif
}    

void Uboot_power_saving2(void)
{
#if 0
    u32 u4Val = 0;

#if 0   
	printf("GPIO Set\n");
    /* GPIO48~55, turn-off PWM 0~1 */
    DRV_WriteReg32(0x80002660, 0x0555);
    /* GPIO56~63, turn-off EINT 0~4 */
    DRV_WriteReg32(0x80002670, 0x0015);
    /* GPIO64~71, turn-off EINT 5~7, UART4 CTS/RTS */
    DRV_WriteReg32(0x80002680, 0x1400);
    /* GPIO112~119, turn-off KP Row2~4, CLK_OUT0~4 */
    DRV_WriteReg32(0x800026E0, 0x0000);
    /* GPIO128~135, Keep I2C SCL2 ,turn-off others */
    DRV_WriteReg32(0x80002700, 0x4000);    
    /* GPIO136~143, Keep I2C SDA2 ,turn-off TRACE signal */
    DRV_WriteReg32(0x80002710, 0x0001);    
#endif

    /* TV power down*/
    DRV_ClrReg32(TVENC, 0x13E0);

    /* AFE power down*/
    DRV_WriteReg32(PDN_AFE_AAPDN, 0); 
    DRV_WriteReg32(PDN_AFE_AAC_NEW, 0);
    DRV_WriteReg32(PDN_AFE_AAC_CON1, 0x0003);

    /* MIPI power down*/
    DRV_WriteReg32(MIPI_PD_B00, 0);
    DRV_WriteReg32(MIPI_PD_B04, 0);
    DRV_WriteReg32(MIPI_PD_B08, 0);
    DRV_WriteReg32(MIPI_PD_B0C, 0);
    DRV_WriteReg32(MIPI_PD_B10, 0);
    DRV_WriteReg32(MIPI_PD_B14, 0);
    DRV_WriteReg32(MIPI_PD_B18, 0);
    DRV_WriteReg32(MIPI_PD_B1C, 0);
    DRV_WriteReg32(MIPI_PD_B40, 0);
    DRV_WriteReg32(MIPI_PD_B44, 0);
    DRV_WriteReg32(MIPI_PD_B48, 0);
    DRV_WriteReg32(MIPI_PD_B4C, 0);
    DRV_WriteReg32(MIPI_PD_04C, 1);
#if 1
	/* MCU CG*/
    DRV_SetReg32 (APMCUSYS_PDN_SET0, (1<<8)|(1<<9)|(1<<21));

    /* MCU memory PDN*/
    u4Val = 0;
    u4Val = ((1<<5)|(1<<6)|(1<<7)|(1<<8)|(1<<9)|(1<<10)|    \
        (1<<11)|(1<<12)|(1<<13)|(1<<16) );
    DRV_SetReg32 (MCU_MEM_PDN, u4Val);
#endif
#if 1
    /* GRAPH1SYS CG*/
    DRV_SetReg32 (GRAPH1SYS_CG_SET, (1<<0));

    /* GRAPH1SYS memory PDN*/
    u4Val = 0;
    u4Val = ((1<<2)|(1<<3)|(1<<4)|(1<<6)|(1<<9));
    DRV_SetReg32 (G1_MEM_PDN, u4Val);

	/* GRAPH2SYS CG */
    DRV_SetReg32 (GRAPH2SYS_CG_SET, 0x1FF);	

    /* GRAPH2SYS  memory PDN*/
    u4Val = 0;
    u4Val = ((1<<0));
    DRV_SetReg32 (G2_MEM_PDN, u4Val);

	/* GRAPH2SYS MTCMOS */
	DRV_SetReg16(ISO_EN, 1<<4);
	DRV_SetReg16(IN_ISO_EN, 1<<4 );    
	DRV_SetReg16(PWR_OFF, 1<<4 );
	DRV_SetReg16(ACK_CLR, 0x2); 
#endif	
    /* CEVA memory PDN*/
    u4Val = 0;
    u4Val = ((1<<0)|(1<<1)|(1<<2)|(1<<3)|(1<<4)|(1<<5));
    DRV_SetReg32 (CEVA_MEM_PDN, u4Val);

	/* CEVA MTCMOS */	
    //printf("CEVA MTCMOS\n");
	DRV_SetReg16(ISO_EN, 1<<5);
	DRV_SetReg16(IN_ISO_EN, 1<<5 );    
	DRV_SetReg16(PWR_OFF, 1<<5 );
	DRV_SetReg16(ACK_CLR, 0x1); 

#if 1
    /* Stop CEVA PLL*/
    DRV_ClrReg32(CEVAPLL2,1<<0);
#endif

    /* Stop UPLL*/
    DRV_ClrReg32(PDN_CON,1<<4);
    /* Stop DPLL*/
#if 0
	printf("DPLL Power Down\n");
    DRV_ClrReg32(PDN_CON,1<<3);
#endif

#endif
}    


#endif
static void mt6577_pinmux_init(void)
{
	mt_gpio_set_default();
}

/*****************************************
 * Routine: board_init
 * Description: Early hardware init.
 *****************************************/
int board_init (void)
{
  /*Warning: DO NOT use "printf" before serial initialize*/
#ifdef CFG_UBOOT_PROFILING    
    unsigned int time_disp_init;
    unsigned int time_led_init;
    unsigned int time_pmic6329_init;
    unsigned int time_gpio_init;
    unsigned int time_wdt_init;
    unsigned int time_serial_init;
#endif    
  
  mtk_serial_init();     

  mtk_wdt_init(); // Modify mtk_wdt.h can select dummy function.
  mt6577_pinmux_init();
  gd->bd->bi_arch_number = MACH_TYPE_MT6577;	/* board id for linux */
  gd->bd->bi_boot_params = CFG_BOOTARGS_ADDR; /* address of boot parameters */
  

    gd->fb_base = memory_size() - mt65xx_disp_get_vram_size();

#ifdef CFG_UBOOT_PROFILING
      time_led_init = get_timer(0);
#endif    
    leds_init();
    isink0_init(); //turn on isink0, HW connection must be floating or pull low
#ifdef CFG_UBOOT_PROFILING
      printf("[PROFILE] ------- led init takes %d ms -------- \n", get_timer(time_led_init));
#endif        

    #ifdef CFG_LCD
#ifdef CFG_UBOOT_PROFILING
      time_disp_init = get_timer(0);
#endif
    mt65xx_disp_init((void*)gd->fb_base);
    UBOOT_TRACER;
#ifdef CFG_UBOOT_PROFILING
      printf("[PROFILE] ------- disp init takes %d ms -------- \n", get_timer(time_disp_init));
#endif    
    #endif 


	
#ifdef CFG_UBOOT_PROFILING
      time_pmic6329_init = get_timer(0);
#endif
   	
	pmic6329_init();
//add by txy ,for vibr when boot << 
pmic_config_interface((kal_uint8)(0xB5), (kal_uint8)(3),(kal_uint8)(7), (kal_uint8)(4));
pmic_config_interface((kal_uint8)(0xB5), (kal_uint8)(1),(kal_uint8)(1), (kal_uint8)(0));
//>> add by txy

#ifdef CFG_UBOOT_PROFILING
      printf("[PROFILE] ------- pmic6329_init takes %d ms -------- \n", get_timer(time_pmic6329_init));
#endif   	

    printf("[CHIP]: %x-%x\n", get_chip_eco_ver(), get_chip_ver());
//add by txy ,for vibr when boot << 
mdelay(500);
pmic_config_interface((kal_uint8)(0xB5), (kal_uint8)(0),(kal_uint8)(1), (kal_uint8)(0));
//>> add by txy
    return 0;
}

/*******************************************************
 * Routine: misc_init_r
 * Description: Init ethernet (done here so udelay works)
 ********************************************************/
int misc_init_r (void)
{
    int ret = 0;
#ifdef CFG_UBOOT_PROFILING    
    unsigned int time_load_logo;
    unsigned int time_boot_mode;
    unsigned int time_part_init;
#endif
//Ivan    
    ulong begin;

#ifdef CFG_UBOOT_PROFILING
      time_part_init = get_timer(0);
#endif
    mt6577_part_init(BLK_NUM(16 * GB));
   // mt6577_part_dump();
#ifdef CFG_UBOOT_PROFILING
      printf("[PROFILE] ------- part_init takes %d ms -------- \n", get_timer(time_part_init));
#endif        

    //*****************
    //* check mode (1) factory mode (2) meta mode ...
#ifdef CFG_UBOOT_PROFILING
      time_boot_mode = get_timer(0);
#endif

//Ivan added
#ifdef MENU_BOOT_ENABLE
    if (!meta_detection())
    {
		boot_mode_dkey_check();
		if (g_boot_mode == MENU_BOOT)
		{
			/* clean console screen */
		    video_clean_screen();
		    mt65xx_disp_fill_rect(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT, 0x0);

		    video_set_cursor(2,0);
		    video_printf("Recovery Mode: Volume Up\r\n");
		    video_set_cursor(4,0);
		    video_printf("Factory Mode: Volume Down\r\n");
		    mt65xx_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
		    mt65xx_backlight_on();
	    video_set_cursor(49,0);	

		    while ((mt65XX_get_key() != 0xFFFF) || (mt6577_detect_homekey()==1));
		    printf(" > Key release!!!\n");
//Ivan added 
		    mtk_wdt_restart();
		    begin = get_timer(0);
		    
		    while (g_boot_mode == MENU_BOOT)
		    {
				factory_detection();
#ifdef CFG_RECOVERY_MODE
		    	if(recovery_detection())
		    	{
					if(DRV_Reg32(0x40002300)==0xE92D4800)
					{
						printf(" > do recovery_check\n");
						//jump(0x40002300);
					}
					else
					{
						printf(" > bypass recovery_check\n");
					}
				}
#endif
//Ivan add 20s time limit
			    if (get_timer(begin) < 20000)
				mtk_wdt_restart();
			}
			video_clean_screen();
    		mt65xx_disp_fill_rect(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT, 0x0);
			mt65xx_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);
	video_set_cursor(49,0);	
   	     mt65xx_disp_wait_idle();

		}
    }
#else
    boot_mode_select();
#endif
#ifdef CFG_UBOOT_PROFILING
      printf("[PROFILE] ------- boot mode select takes %d ms -------- \n", get_timer(time_boot_mode));
#endif      
    

#ifdef CONFIG_MMC
    mmc_legacy_init(1);
 mt6577_part_dump();
#endif

#ifdef CFG_UBOOT_PROFILING
      time_load_logo = get_timer(0);
#endif    
    ret = mboot_common_load_logo((unsigned long)mt65xx_get_logo_db_addr(),
                    CFG_LOGO_NAME);

	printf("Show BLACK_PICTURE\n");
	mt65xx_disp_fill_rect(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT, 0x0);
	mt65xx_disp_power(TRUE);
	mt65xx_disp_update(0, 0, CFG_DISPLAY_WIDTH, CFG_DISPLAY_HEIGHT);

#ifdef CFG_UBOOT_PROFILING
      printf("[PROFILE] ------- load_logo takes %d ms -------- \n", get_timer(time_load_logo));
#endif  
                    
    if (ret <= 0)
    {
        printf("[ERROR] load logo partition failed, ret: %d\n", ret);
    }    

    return 0;
 
}

u32 get_board_rev(void)
{
        u32 btype = 0x6573;
        u32 hw_ver = 0x8A00;
        u32 fw_ver = 0x8A00;

        return (btype << 16 | hw_ver << 8 | fw_ver);
}

