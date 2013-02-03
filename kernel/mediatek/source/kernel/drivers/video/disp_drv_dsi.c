#ifdef BUILD_UBOOT
#include <asm/arch/disp_drv_platform.h>
#else

#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/semaphore.h>
#include "disp_drv_log.h"

#include "disp_drv_platform.h"
#include "lcd_drv.h"
#include "dpi_drv.h"
#include "dsi_drv.h"

#include "lcm_drv.h"
#endif
// ---------------------------------------------------------------------------
//  Private Variables
// ---------------------------------------------------------------------------
static UINT32 dsiTmpBufBpp = 0;
extern LCM_DRIVER *lcm_drv;
extern LCM_PARAMS *lcm_params;

typedef struct
{
    UINT32 pa;
    UINT32 pitchInBytes;
} TempBuffer;

static TempBuffer s_tmpBuffers[3];
static bool	dsi_vdo_streaming = false;

// ---------------------------------------------------------------------------
//  Private Functions
// ---------------------------------------------------------------------------

static void init_lcd_te_control(void)
{
    const LCM_DBI_PARAMS *dbi = &(lcm_params->dbi);

    LCD_CHECK_RET(LCD_TE_Enable(FALSE));
	if(!DISP_IsLcmFound())
        return;
#ifdef BUILD_UBOOT
    {
        extern BOOTMODE g_boot_mode;
        printf("boot_mode = %d\n",g_boot_mode);
        if(g_boot_mode == META_BOOT)
            return;
    }
#endif
    if (LCM_DBI_TE_MODE_DISABLED == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_Enable(FALSE));
        return;
    }

    if (LCM_DBI_TE_MODE_VSYNC_ONLY == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_SetMode(LCD_TE_MODE_VSYNC_ONLY));
    } else if (LCM_DBI_TE_MODE_VSYNC_OR_HSYNC == dbi->te_mode) {
        LCD_CHECK_RET(LCD_TE_SetMode(LCD_TE_MODE_VSYNC_OR_HSYNC));
        LCD_CHECK_RET(LCD_TE_ConfigVHSyncMode(dbi->te_hs_delay_cnt,
                                              dbi->te_vs_width_cnt,
                     (LCD_TE_VS_WIDTH_CNT_DIV)dbi->te_vs_width_cnt_div));
    } else ASSERT(0);

    LCD_CHECK_RET(LCD_TE_SetEdgePolarity(dbi->te_edge_polarity));
    LCD_CHECK_RET(LCD_TE_Enable(TRUE));
}

__inline DPI_FB_FORMAT get_dsi_tmp_buffer_format(void)
{
    switch(lcm_params->dsi.data_format.format)
    {
    case LCM_DSI_FORMAT_RGB565 : return 0;
    case LCM_DSI_FORMAT_RGB666 :
    case LCM_DSI_FORMAT_RGB888 : return 1;
    default : ASSERT(0);
    }
    return 1;
}


__inline UINT32 get_dsi_tmp_buffer_bpp(void)
{
    static const UINT32 TO_BPP[] = {2, 3};
    return TO_BPP[get_dsi_tmp_buffer_format()];
}


__inline LCD_FB_FORMAT get_lcd_tmp_buffer_format(void)
{
    static const UINT32 TO_LCD_FORMAT[] = {
        LCD_FB_FORMAT_RGB565,
        LCD_FB_FORMAT_RGB888
    };
    
    return TO_LCD_FORMAT[get_dsi_tmp_buffer_format()];
}

static __inline LCD_IF_WIDTH to_lcd_if_width(LCM_DBI_DATA_WIDTH data_width)
{
    switch(data_width)
    {
    case LCM_DBI_DATA_WIDTH_8BITS  : return LCD_IF_WIDTH_8_BITS;
    case LCM_DBI_DATA_WIDTH_9BITS  : return LCD_IF_WIDTH_9_BITS;
    case LCM_DBI_DATA_WIDTH_16BITS : return LCD_IF_WIDTH_16_BITS;
    case LCM_DBI_DATA_WIDTH_18BITS : return LCD_IF_WIDTH_18_BITS;
    case LCM_DBI_DATA_WIDTH_24BITS : return LCD_IF_WIDTH_24_BITS;
    default : ASSERT(0);
    }
    return LCD_IF_WIDTH_18_BITS;
}

static BOOL disp_drv_dsi_init_context(void)
{
    if (lcm_drv != NULL && lcm_params != NULL){
		dsiTmpBufBpp=get_dsi_tmp_buffer_bpp();
		return TRUE;
	}

    if (NULL == lcm_drv) {
        return FALSE;
    }

    lcm_drv->get_params(lcm_params);

	dsiTmpBufBpp=get_dsi_tmp_buffer_bpp();
    
    return TRUE;
}

static void init_intermediate_buffers(UINT32 fbPhysAddr)
{
    UINT32 tmpFbStartPA = fbPhysAddr;

    UINT32 tmpFbPitchInBytes = DISP_GetScreenWidth() * dsiTmpBufBpp;
    UINT32 tmpFbSizeInBytes  = tmpFbPitchInBytes * DISP_GetScreenHeight();

    UINT32 i;

	DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", "init_intermediate_buffers \n");
	DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", "tmpFbStartPA=%x, fbPhysAddr=%x \n", tmpFbStartPA, fbPhysAddr);
    
    for (i = 0; i < lcm_params->dsi.intermediat_buffer_num; ++ i)
    {
        TempBuffer *b = &s_tmpBuffers[i];
        #ifdef BUILD_UBOOT
        // clean the intermediate buffers as black to prevent from noise display
        memset(tmpFbStartPA, 0, tmpFbSizeInBytes);
        #endif
        b->pitchInBytes = tmpFbPitchInBytes;
        b->pa = tmpFbStartPA;
        ASSERT((tmpFbStartPA & 0x7) == 0);  // check if 8-byte-aligned
        tmpFbStartPA += tmpFbSizeInBytes;
    }

}

static void init_lcd(void)
{
	UINT32 i;		

	// Config LCD Controller
	LCD_CHECK_RET(LCD_LayerEnable(LCD_LAYER_ALL, FALSE));
	LCD_CHECK_RET(LCD_LayerSetTriggerMode(LCD_LAYER_ALL, LCD_SW_TRIGGER));
	LCD_CHECK_RET(LCD_EnableHwTrigger(FALSE));

	LCD_CHECK_RET(LCD_SetBackgroundColor(0));

	if(lcm_params->dsi.mode == CMD_MODE)
		LCD_CHECK_RET(LCD_SetRoiWindow(0, 0, lcm_params->width, lcm_params->height));
	else
		LCD_CHECK_RET(LCD_SetRoiWindow(0, 0, DISP_GetScreenWidth(), DISP_GetScreenHeight()));

	if(lcm_params->dsi.mode == CMD_MODE) {
		LCD_CHECK_RET(LCD_ConfigDSIIfFormat(lcm_params->dsi.data_format.color_order,
								lcm_params->dsi.data_format.trans_seq,
								lcm_params->dsi.data_format.padding,
								lcm_params->dsi.data_format.format,
								lcm_params->dsi.packet_size,
								TRUE));

		LCD_CHECK_RET(LCD_SetOutputMode(LCD_OUTPUT_TO_LCM));
		LCD_CHECK_RET(LCD_WaitDPIIndication(FALSE));
	    LCD_CHECK_RET(LCD_FBEnable(LCD_FB_0, FALSE));
	    LCD_CHECK_RET(LCD_FBEnable(LCD_FB_1, FALSE));
	    LCD_CHECK_RET(LCD_FBEnable(LCD_FB_2, FALSE));
	} else {
		LCD_CHECK_RET(LCD_FBSetFormat(get_lcd_tmp_buffer_format()));
		LCD_CHECK_RET(LCD_FBSetPitch(s_tmpBuffers[0].pitchInBytes));
		LCD_CHECK_RET(LCD_FBSetStartCoord(0, 0));

		for (i = 0; i < lcm_params->dsi.intermediat_buffer_num; ++ i)
		{
			LCD_CHECK_RET(LCD_FBSetAddress(LCD_FB_0 + i, s_tmpBuffers[i].pa));
			LCD_CHECK_RET(LCD_FBEnable(LCD_FB_0 + i, TRUE));
		}
	
		LCD_CHECK_RET(LCD_SetOutputMode(LCD_OUTPUT_TO_MEM));
		LCD_CHECK_RET(LCD_WaitDPIIndication(TRUE));
	}
}


static void init_dpi(BOOL isDpiPoweredOn)
{
    const LCM_DSI_PARAMS *dsi = &(lcm_params->dsi);
    UINT32 i;

    DPI_CHECK_RET(DPI_Init(isDpiPoweredOn));

    DPI_CHECK_RET(DPI_EnableSeqOutput(FALSE));

    DPI_CHECK_RET(DPI_FBSetSize(DISP_GetScreenWidth(), DISP_GetScreenHeight()));
    
    for (i = 0; i < lcm_params->dsi.intermediat_buffer_num; ++ i)
    {
        DPI_CHECK_RET(DPI_FBSetAddress(DPI_FB_0 + i, s_tmpBuffers[i].pa));
        DPI_CHECK_RET(DPI_FBSetPitch(DPI_FB_0 + i, s_tmpBuffers[i].pitchInBytes));
        DPI_CHECK_RET(DPI_FBEnable(DPI_FB_0 + i, TRUE));
    }
    DPI_CHECK_RET(DPI_FBSetFormat(get_dsi_tmp_buffer_format()));
    DPI_CHECK_RET(DPI_FBSyncFlipWithLCD(TRUE));

    if (LCM_COLOR_ORDER_BGR == dsi->data_format.color_order) {
        DPI_CHECK_RET(DPI_SetRGBOrder(DPI_RGB_ORDER_RGB, DPI_RGB_ORDER_BGR));
    } else {
        DPI_CHECK_RET(DPI_SetRGBOrder(DPI_RGB_ORDER_RGB, DPI_RGB_ORDER_RGB));
    }

	DPI_CHECK_RET(DPI_SetDSIMode(TRUE));

}


void init_dsi(BOOL isDsiPoweredOn)
{
    DSI_CHECK_RET(DSI_Init(isDsiPoweredOn));

    DSI_CHECK_RET(DSI_TXRX_Control(TRUE,                    //cksm_en
                                   TRUE,                    //ecc_en
                                   lcm_params->dsi.LANE_NUM, //ecc_en
                                   0,                       //vc_num
                                   FALSE,                   //null_packet_en
                                   FALSE,                   //err_correction_en
                                   FALSE,                   //dis_eotp_en
                                   0));                     //max_return_size

    
	//initialize DSI_PHY
	DSI_PHY_clk_switch(TRUE);
	DSI_PHY_TIMCONFIG(lcm_params);
	DSI_PHY_clk_setting(lcm_params->dsi.pll_div1, lcm_params->dsi.pll_div2, lcm_params->dsi.LANE_NUM);

	if(lcm_params->dsi.mode != CMD_MODE)
	{
		DSI_Config_VDO_Timing(lcm_params);
        DSI_CHECK_RET(DSI_PS_Control(lcm_params->dsi.PS, lcm_params->width * dsiTmpBufBpp));
    }
	
    DSI_CHECK_RET(DSI_enable_MIPI_txio(TRUE));

	
}

#ifndef BUILD_UBOOT
extern spinlock_t g_handle_esd_lock;
#endif

// ---------------------------------------------------------------------------
//  DBI Display Driver Public Functions
// ---------------------------------------------------------------------------

bool DDMS_capturing=0;

static DISP_STATUS dsi_init(UINT32 fbVA, UINT32 fbPA, BOOL isLcmInited)
{
	if (!disp_drv_dsi_init_context()) 
		return DISP_STATUS_NOT_IMPLEMENTED;

	if(lcm_params->dsi.mode == CMD_MODE) {
	
		init_lcd();
		init_dsi(isLcmInited);

		if (NULL != lcm_drv->init && !isLcmInited) 
		{
			lcm_drv->init();
		}

		DSI_clk_HS_mode(0);

		DSI_SetMode(lcm_params->dsi.mode);

		DPI_PowerOn();
		DPI_PowerOff();

		init_lcd_te_control();
	}
	else {

#ifndef BUILD_UBOOT
		spin_lock(&g_handle_esd_lock);
#endif

		init_intermediate_buffers(fbPA);
		
	    init_lcd();
		init_dpi(isLcmInited);
		init_dsi(isLcmInited);

		if (NULL != lcm_drv->init && !isLcmInited) {
			lcm_drv->init();
		}

		DSI_SetMode(lcm_params->dsi.mode);

#ifndef BUILD_UBOOT	
		if(lcm_params->dsi.lcm_ext_te_monitor)
		{
			dsi_vdo_streaming = false;
			LCD_TE_SetMode(LCD_TE_MODE_VSYNC_ONLY);
			LCD_TE_SetEdgePolarity(LCM_POLARITY_RISING);
			LCD_TE_Enable(FALSE);
		}

		if(lcm_params->dsi.noncont_clock)
			DSI_set_noncont_clk(false, lcm_params->dsi.noncont_clock_period);

		if(lcm_params->dsi.lcm_int_te_monitor)
			DSI_set_int_TE(false, lcm_params->dsi.lcm_int_te_period);			

		spin_unlock(&g_handle_esd_lock);
#endif			
	}

	return DISP_STATUS_OK;
}


// protected by sem_early_suspend, sem_update_screen
static DISP_STATUS dsi_enable_power(BOOL enable)
{
	disp_drv_dsi_init_context();
	
	if(lcm_params->dsi.mode == CMD_MODE) {

		if (enable) {
			// FIX ME
			// Switch bus to MIPI TX.
			DSI_CHECK_RET(DSI_enable_MIPI_txio(TRUE));
			DSI_PHY_clk_switch(1);
			DSI_PHY_clk_setting(lcm_params->dsi.pll_div1, lcm_params->dsi.pll_div2, lcm_params->dsi.LANE_NUM);
			DSI_CHECK_RET(DSI_PowerOn());
			DSI_WaitForNotBusy();		
			DSI_clk_HS_mode(0);
			DSI_clk_ULP_mode(0);			
			DSI_lane0_ULP_mode(0);	
			DSI_Reset();
			LCD_CHECK_RET(LCD_PowerOn());
		} else {
			LCD_CHECK_RET(LCD_PowerOff());
			DSI_clk_HS_mode(0);
			DSI_lane0_ULP_mode(1);
			DSI_clk_ULP_mode(1);
			DSI_CHECK_RET(DSI_PowerOff());
			DSI_PHY_clk_switch(0);
			// FIX ME
			// Switch bus to GPIO, then power level will be decided by GPIO setting.
			DSI_CHECK_RET(DSI_enable_MIPI_txio(FALSE));
		}
	} else {

#ifndef BUILD_UBOOT
		spin_lock(&g_handle_esd_lock);
#endif
	    if (enable) {
			// FIX ME
			// Switch bus to MIPI TX.
			DSI_CHECK_RET(DSI_enable_MIPI_txio(TRUE));
			DSI_PHY_clk_switch(1);
			DSI_PHY_clk_setting(lcm_params->dsi.pll_div1, lcm_params->dsi.pll_div2, lcm_params->dsi.LANE_NUM);
			DSI_CHECK_RET(DSI_PowerOn());			
			DSI_clk_ULP_mode(0);			
			DSI_lane0_ULP_mode(0);
			DSI_clk_HS_mode(0);	
			DSI_Reset();
			DPI_CHECK_RET(DPI_PowerOn());
			LCD_CHECK_RET(LCD_PowerOn());

	    } else {

#ifndef BUILD_UBOOT
			dsi_vdo_streaming = false;

			if(lcm_params->dsi.noncont_clock)
				DSI_set_noncont_clk(false, lcm_params->dsi.noncont_clock_period);
			
			if(lcm_params->dsi.lcm_int_te_monitor)
				DSI_set_int_TE(false, lcm_params->dsi.lcm_int_te_period);
#endif
			LCD_CHECK_RET(LCD_PowerOff());		
			DPI_CHECK_RET(DPI_PowerOff());

			DSI_lane0_ULP_mode(1);
			DSI_clk_ULP_mode(1);	
			DSI_CHECK_RET(DSI_PowerOff());

			DSI_PHY_clk_switch(0);
			// FIX ME
			// Switch bus to GPIO, then power level will be decided by GPIO setting.
			DSI_CHECK_RET(DSI_enable_MIPI_txio(FALSE));
	    }

#ifndef BUILD_UBOOT
		spin_unlock(&g_handle_esd_lock);
#endif		
	}

    return DISP_STATUS_OK;
}


// protected by sem_flipping, sem_early_suspend, sem_overlay_buffer, sem_update_screen
static DISP_STATUS dsi_update_screen(void)
{
	disp_drv_dsi_init_context();

    DSI_CHECK_RET(DSI_enable_MIPI_txio(TRUE));

	//DSI_CHECK_RET(DSI_handle_TE());

	DSI_SetMode(lcm_params->dsi.mode);

	LCD_CHECK_RET(LCD_StartTransfer(FALSE));

	if (lcm_params->type==LCM_TYPE_DSI && lcm_params->dsi.mode == CMD_MODE && !DDMS_capturing) {
		DSI_clk_HS_mode(1);
		DSI_CHECK_RET(DSI_EnableClk());
	}
	else if (lcm_params->type==LCM_TYPE_DSI && lcm_params->dsi.mode != CMD_MODE && !DDMS_capturing)
	{
#ifndef BUILD_UBOOT
		spin_lock(&g_handle_esd_lock);
#endif	
		DSI_clk_HS_mode(1);
		DPI_CHECK_RET(DPI_EnableClk());
		DSI_CHECK_RET(DSI_EnableClk());

#ifndef BUILD_UBOOT
		dsi_vdo_streaming = true;
		
		if(lcm_params->dsi.noncont_clock)
			DSI_set_noncont_clk(true, lcm_params->dsi.noncont_clock_period);
	
		if(lcm_params->dsi.lcm_int_te_monitor)
			DSI_set_int_TE(true, lcm_params->dsi.lcm_int_te_period);

		spin_unlock(&g_handle_esd_lock);
#endif		
	}

	if (DDMS_capturing)
		DISP_LOG_PRINT(ANDROID_LOG_INFO, "DSI", "[DISP] kernel - dsi_update_screen. DDMS is capturing. Skip one frame. \n");		

	return DISP_STATUS_OK;
}


static UINT32 dsi_get_working_buffer_size(void)
{
    disp_drv_dsi_init_context();

	if(lcm_params->dsi.mode != CMD_MODE) {
        
            return 
            DISP_GetScreenWidth() *
            DISP_GetScreenHeight() *
            dsiTmpBufBpp *
            lcm_params->dsi.intermediat_buffer_num;
	}
    
    return 0;
}

static UINT32 dsi_get_working_buffer_bpp(void)
{
    disp_drv_dsi_init_context();

	if(lcm_params->dsi.mode != CMD_MODE) {
            return dsiTmpBufBpp;
	}
    
    return 0;
}

static PANEL_COLOR_FORMAT dsi_get_panel_color_format(void)
{
    disp_drv_dsi_init_context();

	{

	    switch(lcm_params->dsi.data_format.format)
	    {
		    case LCM_DSI_FORMAT_RGB565 : return PANEL_COLOR_FORMAT_RGB565;
		    case LCM_DSI_FORMAT_RGB666 : return PANEL_COLOR_FORMAT_RGB666;
		    case LCM_DSI_FORMAT_RGB888 : return PANEL_COLOR_FORMAT_RGB888;
		    default : ASSERT(0);
	    }
		
	}
}

static UINT32 dsi_get_dithering_bpp(void)
{
	return PANEL_COLOR_FORMAT_TO_BPP(dsi_get_panel_color_format());
}


// protected by sem_early_suspend
DISP_STATUS dsi_capture_framebuffer(UINT32 pvbuf, UINT32 bpp)
{
	DSI_CHECK_RET(DSI_WaitForNotBusy());

	DDMS_capturing=1;

	if(lcm_params->dsi.mode == CMD_MODE)
	{
        LCD_CHECK_RET(LCD_EnableDCtoDsi(FALSE));

	    LCD_CHECK_RET(LCD_Capture_Framebuffer(pvbuf, bpp));
	}
	else
	{
	    DPI_CHECK_RET(DPI_Capture_Framebuffer(pvbuf, bpp));
	}


	if(lcm_params->dsi.mode == CMD_MODE)
	{
        LCD_CHECK_RET(LCD_EnableDCtoDsi(TRUE));
	}

	DDMS_capturing=0;

	return DISP_STATUS_OK;	
}


// called by "esd_recovery_kthread"
// protected by sem_early_suspend, sem_update_screen
BOOL dsi_esd_check(void)
{
	BOOL result = false;

	if(lcm_params->dsi.mode == CMD_MODE || !dsi_vdo_streaming)
		return false;
	else
	{
#ifndef BUILD_UBOOT
		if(lcm_params->dsi.lcm_int_te_monitor)
			result = DSI_esd_check();

		if(result)
			return true;

		if(lcm_params->dsi.lcm_ext_te_monitor)
			result = LCD_esd_check();

		return result;
#endif	
	}

}


// called by "esd_recovery_kthread"
// protected by sem_early_suspend, sem_update_screen
void dsi_esd_reset(void)
{
     /// we assume the power is on here
    ///  what we need is some setting for LCM init
    if(lcm_params->dsi.mode == CMD_MODE) {
        DSI_clk_HS_mode(0);
        DSI_clk_ULP_mode(0);            
        DSI_lane0_ULP_mode(0);  
    }
	else {

		DSI_SetMode(CMD_MODE);
        DSI_clk_HS_mode(0);
		// clock/data lane go to Ideal
		DSI_Reset();
		DPI_CHECK_RET(DPI_DisableClk());

	}
	
}

const DISP_DRIVER *DISP_GetDriverDSI()
{
    static const DISP_DRIVER DSI_DISP_DRV =
    {
        .init                   = dsi_init,
        .enable_power           = dsi_enable_power,
        .update_screen          = dsi_update_screen,       
        .get_working_buffer_size = dsi_get_working_buffer_size,

        .get_panel_color_format = dsi_get_panel_color_format,
        .get_working_buffer_bpp = dsi_get_working_buffer_bpp,
        .init_te_control        = init_lcd_te_control,
        .get_dithering_bpp	= dsi_get_dithering_bpp,
        .capture_framebuffer	= dsi_capture_framebuffer,
        .esd_reset              = dsi_esd_reset,
        .esd_check				= dsi_esd_check,
    };

    return &DSI_DISP_DRV;
}

