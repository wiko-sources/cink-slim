/*****************************************************************************/
/* Copyright (c) 2009 NXP Semiconductors BV                                  */
/*                                                                           */
/* This program is free software; you can redistribute it and/or modify      */
/* it under the terms of the GNU General Public License as published by      */
/* the Free Software Foundation, using version 2 of the License.             */
/*                                                                           */
/* This program is distributed in the hope that it will be useful,           */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the              */
/* GNU General Public License for more details.                              */
/*                                                                           */
/* You should have received a copy of the GNU General Public License         */
/* along with this program; if not, write to the Free Software               */
/* Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307       */
/* USA.                                                                      */
/*                                                                           */
/*****************************************************************************/
#if defined(MTK_HDMI_SUPPORT)
#define TMFL_TDA19989

#define _tx_c_
#include <linux/autoconf.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/earlysuspend.h>
#include <linux/kthread.h>
#include <linux/rtpm_prio.h>
#include <linux/vmalloc.h>
#include <linux/disp_assert_layer.h>

#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/mach-types.h>
#include <asm/cacheflush.h>
#include <asm/io.h>

#include <mach/dma.h>
#include <mach/irqs.h>

#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/cdev.h>
#include <asm/tlbflush.h>
#include <asm/page.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <linux/slab.h>

#include <linux/autoconf.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/fb.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/earlysuspend.h>
#include <linux/kthread.h>
#include <linux/rtpm_prio.h>
#include <linux/list.h>

#include <asm/uaccess.h>
#include <asm/atomic.h>
#include <asm/mach-types.h>
#include <asm/cacheflush.h>
#include <asm/io.h>

#include <mach/dma.h>
#include <mach/irqs.h>
#include <linux/vmalloc.h>

#include <asm/uaccess.h>

#include "hdmi_drv.h"

#include "lcd_drv.h"
#include "lcd_reg.h"
#include "dsi_reg.h"
#include "dpi_drv.h"
#include "dpi_reg.h"
#include "mach/eint.h"
#include "mach/irqs.h"
#include "hdmitx.h"

#include <linux/switch.h>
#if defined(CONFIG_ARCH_MT6575)
	#include <mach/mt6575_m4u.h>
#elif defined(CONFIG_ARCH_MT6577)
	#include <mach/m4u.h>
#else
    #error "unknown arch"
#endif

#undef OUTREG32
#define OUTREG32(x, y) {/*printk("[hdmi]write 0x%08x to 0x%08x\n", (y), (x)); */__OUTREG32((x),(y))}
#define __OUTREG32(x,y) {*(unsigned int*)(x)=(y);}

#define RETIF(cond, rslt)       if ((cond)){HDMI_LOG("return in %d\n",__LINE__);return (rslt);}
#define RET_VOID_IF(cond)       if ((cond)){HDMI_LOG("return in %d\n",__LINE__);return;}
#define RETIF_NOLOG(cond, rslt)       if ((cond)){return (rslt);}
#define RET_VOID_IF_NOLOG(cond)       if ((cond)){return;}
#define RETIFNOT(cond, rslt)    if (!(cond)){HDMI_LOG("return in %d\n",__LINE__);return (rslt);}



#ifdef I2C_DBG
#include "tmbslHdmiTx_types.h"
#include "tmbslTDA9989_local.h"
#endif

#if defined(CONFIG_ARCH_MT6575)
    #include <mach/mt6575_typedefs.h>
    #include <mach/mt6575_gpio.h>
    #include <mach/mt6575_m4u.h>
    #include <mach/mt6575_clock_manager.h>
    #include <mach/mt6575_m4u.h>
    #include <mach/mt6575_mdp.h>
    #include <mach/mt6575_reg_base.h>
#elif defined(CONFIG_ARCH_MT6577)
    #include <mach/mt6577_typedefs.h>
    #include <mach/mt6577_gpio.h>
    #include <mach/mt6577_clock_manager.h>
    #include <mach/mt6577_mdp.h>
    #include <mach/mt6577_reg_base.h>
#endif

static PDSI_PHY_REGS const DSI_PHY_REG_DPI = (PDSI_PHY_REGS)(MIPI_CONFG_BASE + 0x800);
extern int m4u_config_port(M4U_PORT_STRUCT* pm4uport);
extern void HDMI_DBG_Init(void);

extern bool mtkfb_is_suspend(void);


extern UINT32 DISP_GetScreenHeight(void);
extern UINT32 DISP_GetScreenWidth(void);

static size_t hdmi_log_on = true;
static struct switch_dev hdmi_switch_data;
#define HDMI_LOG(fmt, arg...) \
	do { \
		if (hdmi_log_on) {printk("[hdmi]%s,#%d ", __func__, __LINE__); printk(fmt, ##arg);} \
	}while (0)

#define HDMI_FUNC()	\
    do { \
        if(hdmi_log_on) printk("[hdmi] %s\n", __func__); \
    }while (0)

#define HDMI_LINE()	\
    do { \
        if (hdmi_log_on) {printk("[hdmi]%s,%d ", __func__, __LINE__); printk(fmt, ##arg);} \
    }while (0)

extern int pll_fsel(enum mt65xx_pll_id id, unsigned int pll_value);
#define HDMI_DEVNAME "hdmitx"
HDMI_PARAMS _s_hdmi_params = {0};
HDMI_PARAMS *hdmi_params = &_s_hdmi_params;
HDMI_DRIVER *hdmi_drv = NULL;

void hdmi_log_enable(int enable)
{
	printk("hdmi log %s\n", enable?"enabled":"disabled");
	hdmi_log_on = enable;
	hdmi_drv->log_enable(enable);
}

static DEFINE_SEMAPHORE(hdmi_update_mutex);
typedef struct{
    bool is_ipo_poweroff;   // IPO power off or not, like hdmi_suspend(), hdmi_resume()
    bool is_force_fullscreen;    // whether memory is forced to be rotated in Camera, no need to set orientation again
    bool is_force_portrait; // whether forced portrait mode, priority is higher than is_force_fullscreen
    bool is_reconfig_needed;    // whether need to reset HDMI memory
    bool is_enabled;    // whether HDMI is enabled or disabled by user
    //bool is_active;     // whether HDMI is actived with cable pluged in.
    bool is_force_disable; 		//used for camera scenario.
    bool is_clock_on;   // DPI is running or not
    atomic_t state; // HDMI_POWER_STATE state
    bool is_audio_avaliable;
    int 	lcm_width;  // LCD write buffer width
    int		lcm_height; // LCD write buffer height
    int		hdmi_width; // DPI read buffer width
    int		hdmi_height; // DPI read buffer height
    HDMI_VIDEO_RESOLUTION		output_video_resolution;
    HDMI_AUDIO_FORMAT           output_audio_format;
    int		orientation;    // MDP's orientation, 0 means 0 degree, 1 means 90 degree, 2 means 180 degree, 3 means 270 degree
    int     orientation_store;    // Store orientation setting when HDMI driver is_force_fullscreen
    int     orientation_store_portrait;	    // Store orientation setting when HDMI driver is_force_portrait
    HDMI_OUTPUT_MODE    output_mode;
}_t_hdmi_context;

struct hdmi_video_buffer_list {
    struct hdmi_video_buffer_info buffer_info;
    pid_t  pid;
    void*  file_addr;
    unsigned int buffer_mva;
    struct list_head list;
};

static struct list_head hdmi_video_mode_buffer_list;
static struct list_head *hdmi_video_buffer_list_head = &hdmi_video_mode_buffer_list;
DEFINE_SEMAPHORE(hdmi_video_mode_mutex);
static atomic_t hdmi_video_mode_flag = ATOMIC_INIT(0);
static int hdmi_add_video_buffer(struct hdmi_video_buffer_info *buffer_info, struct file *file);
static struct hdmi_video_buffer_list* hdmi_search_video_buffer(struct hdmi_video_buffer_info *buffer_info, struct file *file);
static void hdmi_destory_video_buffer(void);
#define IS_HDMI_IN_VIDEO_MODE()        atomic_read(&hdmi_video_mode_flag)
#define SET_HDMI_TO_VIDEO_MODE()       atomic_set(&hdmi_video_mode_flag, 1)
#define SET_HDMI_LEAVE_VIDEO_MODE()    atomic_set(&hdmi_video_mode_flag, 0)
static wait_queue_head_t hdmi_video_mode_wq;
static atomic_t hdmi_video_mode_event = ATOMIC_INIT(0);
static atomic_t hdmi_video_mode_dpi_change_address = ATOMIC_INIT(0);
#define IS_HDMI_VIDEO_MODE_DPI_IN_CHANGING_ADDRESS()    atomic_read(&hdmi_video_mode_dpi_change_address)
#define SET_HDMI_VIDEO_MODE_DPI_CHANGE_ADDRESS()        atomic_set(&hdmi_video_mode_dpi_change_address, 1)
#define SET_HDMI_VIDEO_MODE_DPI_CHANGE_ADDRESS_DONE()   atomic_set(&hdmi_video_mode_dpi_change_address, 0)


static _t_hdmi_context hdmi_context;
static _t_hdmi_context *p = &hdmi_context;

#define IS_HDMI_ON()			(HDMI_POWER_STATE_ON == atomic_read(&p->state))
#define IS_HDMI_OFF()			(HDMI_POWER_STATE_OFF == atomic_read(&p->state))
#define IS_HDMI_STANDBY()	    (HDMI_POWER_STATE_STANDBY == atomic_read(&p->state))

#define IS_HDMI_NOT_ON()		(HDMI_POWER_STATE_ON != atomic_read(&p->state))
#define IS_HDMI_NOT_OFF()		(HDMI_POWER_STATE_OFF != atomic_read(&p->state))
#define IS_HDMI_NOT_STANDBY()	(HDMI_POWER_STATE_STANDBY != atomic_read(&p->state))

#define SET_HDMI_ON()	        atomic_set(&p->state, HDMI_POWER_STATE_ON)
#define SET_HDMI_OFF()	        atomic_set(&p->state, HDMI_POWER_STATE_OFF)
#define SET_HDMI_STANDBY()	    atomic_set(&p->state, HDMI_POWER_STATE_STANDBY)

extern int m4u_alloc_mva_stub(M4U_MODULE_ID_ENUM eModuleID, const unsigned int BufAddr, const unsigned int BufSize, unsigned int *pRetMVABuf);
extern int m4u_dealloc_mva_stub(M4U_MODULE_ID_ENUM eModuleID, const unsigned int BufAddr, const unsigned int BufSize, const unsigned int MVA);
extern int m4u_config_port_stub(M4U_PORT_STRUCT* pM4uPort);

extern int m4u_insert_tlb_range_stub(M4U_MODULE_ID_ENUM eModuleID, unsigned int MVAStart, const unsigned int MVAEnd, M4U_RANGE_PRIORITY_ENUM ePriority, unsigned int entryCount);
extern int m4u_invalid_tlb_range_stub(M4U_MODULE_ID_ENUM eModuleID, unsigned int MVAStart, unsigned int MVAEnd);
static unsigned int temp_mva, temp_va, hdmi_va, hdmi_mva /*, fb_pa, fb_va, fb_size*/;
static dev_t hdmi_devno;
static struct cdev *hdmi_cdev;
static struct class *hdmi_class = NULL;

static UINT32 const DPI_PAD_CON = 0xf2080900;
static UINT32 const NLI_ARB_CS = 0xf100d014;

static int hdmi_default_width = 1280;
static int hdmi_default_height = 720;

static int hdmi_buffer_write_id = 0;
static int hdmi_buffer_read_id = 0;
static int hdmi_buffer_lcdw_id = 0;

static DPI_POLARITY clk_pol, de_pol, hsync_pol, vsync_pol;
static unsigned int dpi_clk_div, dpi_clk_duty, hsync_pulse_width, hsync_back_porch, hsync_front_porch, vsync_pulse_width, vsync_back_porch, vsync_front_porch, intermediat_buffer_num;//mipi_pll_clk_ref, mipi_pll_clk_div1, mipi_pll_clk_div2, 
static HDMI_COLOR_ORDER rgb_order;


static struct task_struct *hdmi_update_task = NULL;

static wait_queue_head_t hdmi_update_wq;
static atomic_t hdmi_update_event = ATOMIC_INIT(0);

static unsigned int hdmi_resolution_param_table[][3] =
{
        {720,   480,    60},
        {1280,  720,    60},
        {1920,  1080,   60},
};


static void hdmi_udelay(unsigned int us)
{
	udelay(us);
}

static void hdmi_mdelay(unsigned int ms)
{
	msleep(ms);
}

static unsigned int hdmi_get_width(HDMI_VIDEO_RESOLUTION r)
{
    ASSERT(r < HDMI_VIDEO_RESOLUTION_NUM);
    return hdmi_resolution_param_table[r][0];
}

static unsigned int hdmi_get_height(HDMI_VIDEO_RESOLUTION r)
{
    ASSERT(r < HDMI_VIDEO_RESOLUTION_NUM);
    return hdmi_resolution_param_table[r][1];
}


static atomic_t hdmi_fake_in = ATOMIC_INIT(false);
#define IS_HDMI_FAKE_PLUG_IN()  (true == atomic_read(&hdmi_fake_in))
#define SET_HDMI_FAKE_PLUG_IN() (atomic_set(&hdmi_fake_in, true))
#define SET_HDMI_NOT_FAKE()     (atomic_set(&hdmi_fake_in, false))

// For Debugfs
void hdmi_cable_fake_plug_in(void)
{
    SET_HDMI_FAKE_PLUG_IN();
    HDMI_LOG("[HDMIFake]Cable Plug In\n");
    if(p->is_force_disable == false)
    {
        if (IS_HDMI_STANDBY())
        {
            hdmi_resume( );
            msleep(1000);
            switch_set_state(&hdmi_switch_data, HDMI_STATE_ACTIVE);
        }
    }
}

// For Debugfs
void hdmi_cable_fake_plug_out(void)
{
    SET_HDMI_NOT_FAKE();
    HDMI_LOG("[HDMIFake]Disable\n");
    if(p->is_force_disable == false)
    {
        if (IS_HDMI_ON())
        {
            if (hdmi_drv->get_state() != HDMI_STATE_ACTIVE)
            {
                hdmi_suspend( );
                switch_set_state(&hdmi_switch_data, HDMI_STATE_NO_DEVICE);
            }
        }
    }
}

extern const HDMI_DRIVER* HDMI_GetDriver(void);

#if 0
static void freq_meter(void)
{
	unsigned int temp = 0;
	unsigned int temp1 = 0;
	__OUTREG32(0xF0007400, 0xC800);
	__OUTREG32(0xF0007404, 0x000B);
	__OUTREG32(0xF0007400, 0x8800);
	while(INREG32(0xF0007404) & 0x8000);
	temp = INREG32(0xF0007410);
	//printk("tvdss clock=%d, %dMhz\n", temp, (temp+1)*26/2048);

	__OUTREG32(0xF0007400, 0xC800);
	__OUTREG32(0xF0007404, 0x0008);
	__OUTREG32(0xF0007400, 0x8800);
	while(INREG32(0xF0007404) & 0x8000);
	temp1 = INREG32(0xF0007410);
	//printk("upll clock=%d, %dMhz\n", temp, (temp+1)*26/2048);
	printk("[hdmi]upll=%dMhz, tvdss=%dMhz\n", (temp1+1)*26/2048, (temp+1)*26/2048);
}

static long int get_current_time_us(void)
{
	struct timeval t;
	do_gettimeofday(&t);
	return (t.tv_sec & 0xFFF) * 1000000 + t.tv_usec;
}
#endif



extern unsigned int DPI_GetCurrentFB(void);

#if 0
static bool is_hdmi_clock_on(void)
{
	return p->is_clock_on;
}
#endif

/* Will be called in LCD Interrupt handler to check whether HDMI is actived */
bool is_hdmi_active(void)
{
	return IS_HDMI_ON();
}


void hdmi_set_mode(unsigned char ucMode)
{
    HDMI_FUNC();

    hdmi_drv->set_mode(ucMode);

    return;
}

void hdmi_reg_dump(void)
{
	hdmi_drv->dump();
}

void hdmi_read_reg(unsigned char u8Reg)
{
	hdmi_drv->read(u8Reg);
}

void hdmi_write_reg(unsigned char u8Reg, unsigned char u8Data)
{
	hdmi_drv->write(u8Reg,u8Data);
}

/* Used for HDMI Driver update */
static int hdmi_update_kthread(void *data)
{
	struct sched_param param = { .sched_priority = RTPM_PRIO_SCRN_UPDATE };
	sched_setscheduler(current, SCHED_RR, &param);

    for( ;; ) {
        wait_event_interruptible(hdmi_update_wq, atomic_read(&hdmi_update_event));
		//HDMI_LOG("wq wakeup\n");
        //hdmi_update_impl();

		atomic_set(&hdmi_update_event,0);
        hdmi_update_impl();
        if (kthread_should_stop())
            break;
    }

    return 0;
}

/* Switch LCD write buffer, will be called in LCD Interrupt handler */
void hdmi_source_buffer_switch(void)
{
    //printk("lcd write buffer:%d\n", hdmi_buffer_lcdw_id);

    RET_VOID_IF_NOLOG(IS_HDMI_NOT_ON());
    RET_VOID_IF_NOLOG(p->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS);
    RET_VOID_IF_NOLOG(IS_HDMI_IN_VIDEO_MODE());

    hdmi_buffer_lcdw_id = (hdmi_buffer_lcdw_id+1)%hdmi_params->intermediat_buffer_num;

    LCD_CHECK_RET(LCD_FBSetAddress(LCD_FB_0, temp_mva + p->lcm_width*p->lcm_height*3*hdmi_buffer_lcdw_id));

}

/* Switch DPI read buffer, will be called in DPI Interrupt handler */
void hdmi_update_buffer_switch(void)
{
    //HDMI_LOG("DPI read buffer:%d\n", hdmi_buffer_read_id);

    RET_VOID_IF_NOLOG(IS_HDMI_NOT_ON());
    RET_VOID_IF_NOLOG(p->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS);

    if(IS_HDMI_IN_VIDEO_MODE())
    {
        if (IS_HDMI_VIDEO_MODE_DPI_IN_CHANGING_ADDRESS())
        {
            if (0 == *((volatile unsigned int*)(0xF208C020)))
            {
                SET_HDMI_VIDEO_MODE_DPI_CHANGE_ADDRESS_DONE();
                atomic_set(&hdmi_video_mode_event, 1);
                wake_up_interruptible(&hdmi_video_mode_wq);
            }
        }
    }
    else
    {
        //DPI_CHECK_RET(DPI_FBSetAddress(DPI_FB_0, hdmi_mva + p->hdmi_width*p->hdmi_height*3*hdmi_buffer_read_id));
    }
}

extern void DBG_OnTriggerHDMI(void);
extern void DBG_OnHDMIDone(void);

/* hdmi update api, will be called in LCD Interrupt handler */
void hdmi_update(void)
{
    //HDMI_FUNC();
#if 1
    RET_VOID_IF(IS_HDMI_NOT_ON());
    RET_VOID_IF_NOLOG(p->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS);

    atomic_set(&hdmi_update_event, 1);
    wake_up_interruptible(&hdmi_update_wq); //wake up hdmi_update_kthread() to do update
#else
    hdmi_update_impl();
#endif
}
void hdmi_update_impl(void)
{
    //int i = 0;
    int ret = 0;
    //int nextfb = 0;
    MdpkBitbltConfig pmdp;

    RET_VOID_IF_NOLOG(p->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS);

    if(p->hdmi_width * p->hdmi_height == 0)
    {
        HDMI_LOG("ignored[resolution is null]\n");
        return;
    }

    //HDMI_FUNC();
    if(down_interruptible(&hdmi_update_mutex))
    {
        HDMI_LOG("[HDMI] can't get semaphore in\n");
        return;
    }

    if (IS_HDMI_NOT_ON())
    {
        goto done;
    }
    if(IS_HDMI_IN_VIDEO_MODE())
    {
        goto done;
    }

    DBG_OnTriggerHDMI();
    //LCD_WaitForNotBusy();
    hdmi_buffer_write_id = (hdmi_buffer_write_id+1)%hdmi_params->intermediat_buffer_num;

    if(p->is_reconfig_needed)
    {
        memset((void*)hdmi_va, 0, p->hdmi_width*p->hdmi_height*3*hdmi_params->intermediat_buffer_num);
        p->is_reconfig_needed = false;
    }

    memset((void*)&pmdp, 0, sizeof(MdpkBitbltConfig));
    pmdp.srcX = pmdp.srcY = 0;
    pmdp.srcW = p->lcm_width;
    pmdp.srcH	= p->lcm_height;
    pmdp.srcWStride = p->lcm_width;
    pmdp.srcHStride = p->lcm_height;
    pmdp.srcAddr = temp_va + p->lcm_width*p->lcm_height*3*((hdmi_buffer_lcdw_id+(hdmi_params->intermediat_buffer_num-1))%hdmi_params->intermediat_buffer_num);
    pmdp.srcFormat = MDPK_FORMAT_RGB_888;
    pmdp.srcBufferSize = p->lcm_width*p->lcm_height*3;
    pmdp.srcMemType =  MDPK_MEMTYPE_VMALLOC;

    pmdp.orientation = p->orientation;
    switch(pmdp.orientation)
    {
        case 0:
        case 2:
        {
            // make sure in portrait mode, MDP resize must be as propotion
            // that is p.dstW/p.dstH == lcm_width/lcm_height
            pmdp.dstW = p->lcm_width * p->hdmi_height * 95 / 100 / p->lcm_height;
            pmdp.dstH = p->hdmi_height * 95 / 100;
            // setting memory write start address
            pmdp.dstAddr = hdmi_va + hdmi_buffer_write_id * p->hdmi_width * p->hdmi_height * 3 + 
                           (p->hdmi_height - pmdp.dstH) / 2 * p->hdmi_width * 3 +
                           (p->hdmi_width - pmdp.dstW) / 2 * 3;
            pmdp.dstFormat = MDPK_FORMAT_RGB_888;
            pmdp.pitch = p->hdmi_width;
            pmdp.dstBufferSize = p->hdmi_width*p->hdmi_height*3;
            pmdp.dstMemType =  MDPK_MEMTYPE_VMALLOC;
            break;
        }
        case 1:
        case 3:
        {
            pmdp.dstW = p->hdmi_width * 95 / 100;
            pmdp.dstH = p->hdmi_height * 95 /100;
            pmdp.dstAddr = hdmi_va + hdmi_buffer_write_id * p->hdmi_width * p->hdmi_height * 3 + 
                           (p->hdmi_height - pmdp.dstH) / 2 * p->hdmi_width * 3 +
                           (p->hdmi_width - pmdp.dstW) / 2 * 3;
            pmdp.dstFormat = MDPK_FORMAT_RGB_888;
            pmdp.pitch = p->hdmi_width;
            pmdp.dstBufferSize = p->hdmi_width*p->hdmi_height*3;
            pmdp.dstMemType =  MDPK_MEMTYPE_VMALLOC;
            break;
        }
    }

    //HDMI_LOG("dstw=%d, dsth=%d, ori=%d\n", p.dstW, p.dstH, p.orientation);

    MDPK_Bitblt_Config(MDPK_BB_CH_HDMI_0, &pmdp);

    //long int t = get_current_time_us();
    ret = MDPK_Bitblt(0);
    if(ret)
    {
        HDMI_LOG("MDPK_Bitblt fail!\n");
    }

    DBG_OnHDMIDone();
    //HDMI_LOG("cost %d us\n", get_current_time_us() - t);

    hdmi_buffer_read_id = hdmi_buffer_write_id;

    if(p->is_clock_on)
    {
        //Wait for change buffer
        DPI_CHECK_RET(DPI_FBSetAddress(DPI_FB_0, hdmi_mva + p->hdmi_width*p->hdmi_height*3*hdmi_buffer_read_id));
    }

    done:
    up(&hdmi_update_mutex);

    return;
}


/* Allocate memory, set M4U, LCD, MDP, DPI */
/* LCD overlay to memory -> MDP resize and rotate to memory -> DPI read to HDMI */
/* Will only be used in ioctl(MTK_HDMI_AUDIO_VIDEO_ENABLE) */
static HDMI_STATUS hdmi_drv_init(void)
{
	int ret = 0;
	int lcm_width, lcm_height;
	M4U_PORT_STRUCT m4uport;

    HDMI_FUNC();

	RETIF(p->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS, 0);

	p->hdmi_width = hdmi_get_width(hdmi_params->init_config.vformat);
	p->hdmi_height = hdmi_get_height(hdmi_params->init_config.vformat);

	lcm_width = DISP_GetScreenWidth();
	lcm_height = DISP_GetScreenHeight();

	//printk("[hdmi]%s, hdmi_width=%d, hdmi_height=%d\n", __func__, p->hdmi_width, p->hdmi_height);
	printk("[hdmi]%s, lcm_width=%d, lcm_height=%d\n", __func__, lcm_width, lcm_height);


	memset((void*)&m4uport, 0, sizeof(M4U_PORT_STRUCT));
	m4uport.ePortID = M4U_PORT_DPI;
	m4uport.Virtuality = 1;
	m4uport.Security = 0;
	m4uport.Distance = 1;
	m4uport.Direction = 0;

	if(m4u_config_port_stub(&m4uport))
	{
		printk("[hdmi]config dpi port failed\n");
		return -1;
	}

	memset((void*)&m4uport, 0, sizeof(M4U_PORT_STRUCT));
	m4uport.ePortID = M4U_PORT_LCD_W;
	m4uport.Virtuality = 1;
	m4uport.Security = 0;
	m4uport.Distance = 1;
	m4uport.Direction = 0;

	if(m4u_config_port_stub(&m4uport))
	{
		printk("[hdmi]config dpi port failed\n");
		return -1;
	}

	temp_va = (unsigned int)vmalloc(lcm_width*lcm_height*3*hdmi_params->intermediat_buffer_num);
	if(((void*)temp_va) == NULL)
	{
		HDMI_LOG("vmalloc fail\n");
		return -1;
	}

	ret = m4u_alloc_mva_stub(M4U_CLNTMOD_LCDC, temp_va, lcm_width*lcm_height*3*hdmi_params->intermediat_buffer_num, &temp_mva);
	if(ret)
	{
		printk("[hdmi]allocate mva for dpi fail\n");
		return -1;
	}

	printk("[hdmi]%s, temp_va=0x%08x, temp_mva=0x%08x\n", __func__, temp_va, temp_mva);

	m4u_insert_tlb_range_stub(M4U_CLNTMOD_LCDC, temp_mva,temp_mva+lcm_width*lcm_height*3*hdmi_params->intermediat_buffer_num, RT_RANGE_HIGH_PRIORITY, 0);

    LCD_CHECK_RET(LCD_FBSetFormat(LCD_FB_FORMAT_RGB888));
	LCD_CHECK_RET(LCD_FBSetPitch(lcm_width*3));
	LCD_CHECK_RET(LCD_FBSetStartCoord(0, 0));

	LCD_CHECK_RET(LCD_FBSetAddress(LCD_FB_0, temp_mva));
	LCD_CHECK_RET(LCD_FBEnable(LCD_FB_0, TRUE));

	hdmi_va = (unsigned int)vmalloc(hdmi_default_width*hdmi_default_height*3*hdmi_params->intermediat_buffer_num);
	if(((void*)hdmi_va) == NULL)
	{
		printk("[hdmi]kmalloc 0x%08x fail!!!\n", hdmi_default_width*hdmi_default_height*3*hdmi_params->intermediat_buffer_num);
		return -1;
	}

	memset((void*)hdmi_va, 0, hdmi_default_width*hdmi_default_height*3*hdmi_params->intermediat_buffer_num);

	ret = m4u_alloc_mva_stub(M4U_CLNTMOD_DPI, hdmi_va, hdmi_default_width*hdmi_default_height*3*hdmi_params->intermediat_buffer_num, &hdmi_mva);
	if(ret)
	{
		printk("[hdmi]allocate mva for dpi fail\n");
		return -1;
	}

	m4u_insert_tlb_range_stub(M4U_CLNTMOD_DPI, hdmi_mva, hdmi_mva+hdmi_default_width*hdmi_default_height*3*hdmi_params->intermediat_buffer_num, RT_RANGE_HIGH_PRIORITY, 0);
	printk("[hdmi]%s, hdmi_va=0x%08x, hdmi_mva=0x%08x\n", __func__, hdmi_va, hdmi_mva);

	p->lcm_width = lcm_width;
	p->lcm_height = lcm_height;
    p->output_video_resolution = hdmi_params->init_config.vformat;
    p->output_audio_format = hdmi_params->init_config.aformat;
    hdmi_dpi_config_clock();    // configure dpi clock
    hdmi_dpi_power_switch(false);   // but dpi power is still off
    //hdmi_drv->suspend();
    LCD_WaitForNotBusy();
    LCD_SetOutputMode(3);   // LCD write to memory and LCM

	return HDMI_STATUS_OK;
}

//free IRQ
/*static*/ void hdmi_dpi_free_irq(void)
{
    RET_VOID_IF(p->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS);

    DPI_CHECK_RET(DPI_FreeIRQ());
}


/* Release memory */
/* Will only be used in ioctl(MTK_HDMI_AUDIO_VIDEO_ENABLE) */
/*static*/ HDMI_STATUS hdmi_drv_deinit(void)
{
	int ret = 0;
    int temp_va_size;
    int hdmi_va_size; 

	RETIF(p->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS, 0);

    LCD_WaitForNotBusy();
    LCD_SetOutputMode(LCD_OUTPUT_TO_LCM);
    hdmi_dpi_power_switch(false);

    printk("[hdmi]%s, temp_va=0x%08x, temp_mva=0x%08x\n", __func__, temp_va, temp_mva);
    printk("[hdmi]%s, hdmi_va=0x%08x, hdmi_mva=0x%08x\n", __func__, hdmi_va, hdmi_mva);

    //free temp_va & temp_mva 
    temp_va_size = p->lcm_width * p->lcm_height * 3 * hdmi_params->intermediat_buffer_num;
	m4u_invalid_tlb_range_stub(M4U_CLNTMOD_LCDC, temp_mva, temp_mva + temp_va_size);
	ret = m4u_dealloc_mva_stub(M4U_CLNTMOD_LCDC, temp_va, temp_va_size, temp_mva);
	if(ret)
	{
		printk("[hdmi]allocate mva for dpi fail\n");
		return -1;
	}

    vfree((void*)temp_va);
    temp_va = 0;


    printk("[hdmi]%s, temp_va=0x%08x, temp_mva=0x%08x\n", __func__, temp_va, temp_mva);
                   printk("[hdmi]%s, hdmi_va=0x%08x, hdmi_mva=0x%08x\n", __func__, hdmi_va, hdmi_mva);

    //free hdmi_va & hdmi_mva
    hdmi_va_size = hdmi_default_width * hdmi_default_height * 3 * hdmi_params->intermediat_buffer_num;
    m4u_invalid_tlb_range_stub(M4U_CLNTMOD_DPI, hdmi_mva, hdmi_mva + hdmi_va_size);
	ret = m4u_dealloc_mva_stub(M4U_CLNTMOD_DPI, hdmi_va, hdmi_va_size, hdmi_mva);
	if(ret)
	{
		printk("[hdmi]allocate mva for dpi fail\n");
		return -1;
	}

    vfree((void*)hdmi_va);
    hdmi_va = 0;

    hdmi_dpi_free_irq(); 
	return HDMI_STATUS_OK;
}

/* Will only be used in hdmi_drv_init(), this means that will only be use in ioctl(MTK_HDMI_AUDIO_VIDEO_ENABLE) */
/*static*/ void hdmi_dpi_config_clock(void)
{
    int ret = 0;

    RET_VOID_IF(p->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS);

    ret = enable_pll(MT65XX_TVDDS, "HDMI");
    if(ret)
    {
        HDMI_LOG("enable_pll fail!!\n");
    }

    switch(hdmi_params->init_config.vformat)
    {
        case HDMI_VIDEO_720x480p_60Hz:
        {
            printk("[hdmi]480p\n");
            ret = pll_fsel(MT65XX_TVDDS, 0x1C7204C7);
            ASSERT(!ret);

            dpi_clk_div = 2;
            dpi_clk_duty = 1;

            OUTREG32(0xF2080920, 1);
            OUTREG32(0xF2080900, 1);
            break;
        }
        case HDMI_VIDEO_1280x720p_60Hz:
        {
            printk("[hdmi]720p\n");
            ret = pll_fsel(MT65XX_TVDDS, 0xDBCD0119);
            ASSERT(!ret);

            dpi_clk_div = 2;
            dpi_clk_duty = 1;

            OUTREG32(0xF2080920, 1);
            OUTREG32(0xF2080900, 1);
            break;
        }
        default:
        {
            printk("[hdmi] not supported format, %s, %d, format = %d\n", __func__, __LINE__, hdmi_params->init_config.vformat);
            break;
        }
    }

    clk_pol     = hdmi_params->clk_pol;
    de_pol      = hdmi_params->de_pol;
    hsync_pol   = hdmi_params->hsync_pol;
    vsync_pol   = hdmi_params->vsync_pol;;

    hsync_pulse_width   = hdmi_params->hsync_pulse_width;
    vsync_pulse_width   = hdmi_params->vsync_pulse_width;
    hsync_back_porch    = hdmi_params->hsync_back_porch;
    vsync_back_porch    = hdmi_params->vsync_back_porch;
    hsync_front_porch   = hdmi_params->hsync_front_porch;
    vsync_front_porch   = hdmi_params->vsync_front_porch;

    rgb_order           = hdmi_params->rgb_order;
    intermediat_buffer_num = hdmi_params->intermediat_buffer_num;

    // dpi clock configuration using MIPITX

    DPI_CHECK_RET(DPI_Init(FALSE));

    DPI_CHECK_RET(DPI_ConfigPixelClk(clk_pol, dpi_clk_div, dpi_clk_duty));

    DPI_CHECK_RET(DPI_ConfigDataEnable(de_pol));

    DPI_CHECK_RET(DPI_ConfigHsync(hsync_pol, hsync_pulse_width, hsync_back_porch, hsync_front_porch));

    DPI_CHECK_RET(DPI_ConfigVsync(vsync_pol, vsync_pulse_width, vsync_back_porch, vsync_front_porch));

    DPI_CHECK_RET(DPI_FBSetSize(p->hdmi_width, p->hdmi_height));

    {
        DPI_CHECK_RET(DPI_FBSetAddress(DPI_FB_0, hdmi_mva));
        DPI_CHECK_RET(DPI_FBSetPitch(DPI_FB_0, p->hdmi_width*3));
        DPI_CHECK_RET(DPI_FBEnable(DPI_FB_0, TRUE));
    }

    OUTREG32(0xF208C090, 0x41);
    DPI_CHECK_RET(DPI_FBSetFormat(DPI_FB_FORMAT_RGB888));

    if (LCM_COLOR_ORDER_BGR == rgb_order)
    {
        DPI_CHECK_RET(DPI_SetRGBOrder(DPI_RGB_ORDER_RGB, DPI_RGB_ORDER_BGR));
    }
    else
    {
        DPI_CHECK_RET(DPI_SetRGBOrder(DPI_RGB_ORDER_RGB, DPI_RGB_ORDER_RGB));
    }

    DPI_CHECK_RET(DPI_EnableClk());

    p->is_clock_on = true;
}

/* Switch DPI Power for HDMI Driver */
/*static*/ void hdmi_dpi_power_switch(bool enable)
{
    int ret;

    HDMI_LOG("DPI clock:%d\n", enable);

    RET_VOID_IF(p->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS);

    if(enable)
    {
        if(p->is_clock_on == true)
        {
            HDMI_LOG("power on request while already powered on!\n");
            return;
        }

        ret = enable_pll(MT65XX_TVDDS, "HDMI");
        if(ret)
        {
            HDMI_LOG("enable_pll fail!!\n");
            return;
        }

        DPI_PowerOn();
				DPI_EnableIrq();
				
        OUTREG32(0xF2080920, 1);
        OUTREG32(0xF2080900, 1);
        DPI_CHECK_RET(DPI_EnableClk());
        p->is_clock_on = true;
    }
    else
    {
        if(p->is_clock_on == false)
        {
            HDMI_LOG("power off request while already powered off!\n");
            return;
        }

        p->is_clock_on = false;
		DPI_DisableIrq();

        DPI_DisableClk();
        DPI_PowerOff();
        ret = disable_pll(MT65XX_TVDDS, "HDMI");
        if(ret)
        {
            HDMI_LOG("disable_pll fail!!\n");
            //return;
        }

        if (IS_HDMI_VIDEO_MODE_DPI_IN_CHANGING_ADDRESS())
        {
            SET_HDMI_VIDEO_MODE_DPI_CHANGE_ADDRESS_DONE();
            atomic_set(&hdmi_video_mode_event, 1);
            wake_up_interruptible(&hdmi_video_mode_wq);
        }
    }
}

/* Configure video attribute */
static int hdmi_video_config(HDMI_VIDEO_RESOLUTION vformat, HDMI_VIDEO_INPUT_FORMAT vin, HDMI_VIDEO_OUTPUT_FORMAT vout)
{
	HDMI_FUNC();
	RETIF(IS_HDMI_NOT_ON(), 0);

	return hdmi_drv->video_config(vformat, vin, vout);
}

/* Configure audio attribute, will be called by audio driver */
int hdmi_audio_config(int samplerate)
{
    HDMI_FUNC();
	RETIF(!p->is_enabled, 0);
	RETIF(IS_HDMI_NOT_ON(), 0);

    HDMI_LOG("sample rate=%d\n", samplerate);
    if(samplerate == 48000)
    {
        p->output_audio_format = HDMI_AUDIO_PCM_16bit_48000;
    }
    else if(samplerate == 44100)
    {
        p->output_audio_format = HDMI_AUDIO_PCM_16bit_44100;
    }
    else if(samplerate == 32000)
	{
        p->output_audio_format = HDMI_AUDIO_PCM_16bit_32000;
    }
    else
    {
        HDMI_LOG("samplerate not support:%d\n", samplerate);
    }


    hdmi_drv->audio_config(p->output_audio_format);

    return 0;
}

/* No one will use this function */
/*static*/ int hdmi_video_enable(bool enable)
{
    HDMI_FUNC();


	return hdmi_drv->video_enable(enable);
}

/* No one will use this function */
/*static*/ int hdmi_audio_enable(bool enable)
{
    HDMI_FUNC();


	return hdmi_drv->audio_enable(enable);
}

struct timer_list timer;
void __timer_isr(unsigned long n)
{
    HDMI_FUNC();
    if(hdmi_drv->audio_enable) hdmi_drv->audio_enable(true);

    del_timer(&timer);
}

int hdmi_audio_delay_mute(int latency)
{
    HDMI_FUNC();
    memset((void*)&timer, 0, sizeof(timer));
    timer.expires = jiffies +  ( latency * HZ / 1000 );
    timer.function = __timer_isr;
    init_timer(&timer);
    add_timer(&timer);
    if(hdmi_drv->audio_enable) hdmi_drv->audio_enable(false);
    return 0;
}

/* Reset HDMI Driver state */
static void hdmi_state_reset(void)
{
    HDMI_FUNC();

    if(hdmi_drv->get_state() == HDMI_STATE_ACTIVE)
    {
        switch_set_state(&hdmi_switch_data, HDMI_STATE_ACTIVE);
    }
    else
    {
        switch_set_state(&hdmi_switch_data, HDMI_STATE_NO_DEVICE);
    }
}

/* HDMI Driver state callback function */
static void hdmi_state_callback(HDMI_STATE state)
{
    //int ret, i;
    //static int is_first_time = 1;

    printk("[hdmi]%s, state = %d\n", __func__, state);

    RET_VOID_IF((p->is_force_disable == true));
    RET_VOID_IF(IS_HDMI_FAKE_PLUG_IN());

    switch(state)
    {
        case HDMI_STATE_NO_DEVICE:
        {
            hdmi_suspend();
            switch_set_state(&hdmi_switch_data, HDMI_STATE_NO_DEVICE);

            break;
        }
        case HDMI_STATE_ACTIVE:
        {
            hdmi_resume();
            //force update screen
            DISP_UpdateScreen(0, 0, DISP_GetScreenWidth(), DISP_GetScreenHeight());
            if (HDMI_OUTPUT_MODE_LCD_MIRROR == p->output_mode) 
            {
                msleep(1000);
            }
            switch_set_state(&hdmi_switch_data, HDMI_STATE_ACTIVE); 

            break;
        }
        default:
        {
            printk("[hdmi]%s, state not support\n", __func__);
            break;
        }
    }

    return;
}

/*static*/ void hdmi_power_on(void)
{
    HDMI_FUNC();

    RET_VOID_IF(IS_HDMI_NOT_OFF());

	if (down_interruptible(&hdmi_update_mutex)) {
			printk("[hdmi][HDMI] can't get semaphore in %s()\n", __func__);
			return;
	}

	// Why we set power state before calling hdmi_drv->power_on()?
	// Because when power on, the hpd irq will come immediately, that means hdmi_resume will be called before hdmi_drv->power_on() retuen here.
	// So we have to ensure the power state is STANDBY before hdmi_resume() be called.
	SET_HDMI_STANDBY();

    hdmi_drv->power_on();

	// When camera is open, the state will only be changed when camera exits.
	// So we bypass state_reset here, if camera is open.
	// The related scenario is: suspend in camera with hdmi enabled.
	// Why need state_reset() here?
	// When we suspend the phone, and then plug out hdmi cable, the hdmi chip status will change immediately
	// But when we resume the phone and check hdmi status, the irq will never come again
	// So we have to reset hdmi state manually, to ensure the status is the same between the host and hdmi chip.
    if(p->is_force_disable == false)
    {
        if (IS_HDMI_FAKE_PLUG_IN())
        {
            hdmi_resume();
            msleep(1000);
            switch_set_state(&hdmi_switch_data, HDMI_STATE_ACTIVE);
        }
        else
        {
            hdmi_state_reset();
            // this is just a ugly workaround for some tv sets...
            //if(hdmi_drv->get_state() == HDMI_STATE_ACTIVE)
            //	hdmi_resume();
        }
    }

    up(&hdmi_update_mutex);

    return;
}

/*static*/ void hdmi_power_off(void)
{
    HDMI_FUNC();

    RET_VOID_IF(IS_HDMI_OFF());

	if (down_interruptible(&hdmi_update_mutex)) {
			printk("[hdmi][HDMI] can't get semaphore in %s()\n", __func__);
			return;
	}

	hdmi_drv->power_off();
        hdmi_dpi_power_switch(false);
	SET_HDMI_OFF();
	up(&hdmi_update_mutex);

    return;
}

/*static*/ void hdmi_suspend(void)
{
    HDMI_FUNC();

    RET_VOID_IF(IS_HDMI_NOT_ON());

    hdmi_dpi_power_switch(false);
    hdmi_drv->suspend();
    SET_HDMI_STANDBY();
}

/*static*/ void hdmi_resume(void)
{
    HDMI_FUNC();

    RET_VOID_IF(IS_HDMI_NOT_STANDBY());
    SET_HDMI_ON();

    hdmi_dpi_power_switch(true);
    hdmi_drv->resume();

    hdmi_video_config(p->output_video_resolution, HDMI_VIN_FORMAT_RGB888, HDMI_VOUT_FORMAT_RGB888);
    hdmi_audio_config(44100);
}

/* Set HDMI orientation, will be called in mtkfb_ioctl(SET_ORIENTATION) */
/*static*/ void hdmi_setorientation(int orientation)
{
	HDMI_FUNC();
    RET_VOID_IF(!p->is_enabled);

	if(down_interruptible(&hdmi_update_mutex))
	{
		printk("[hdmi][HDMI] can't get semaphore in %s\n", __func__);
		return;
	}

	p->orientation = orientation;
	p->is_reconfig_needed = true;

//done:
	up(&hdmi_update_mutex);
}

static int hdmi_release(struct inode *inode, struct file *file)
{
	return 0;
}

static int hdmi_open(struct inode *inode, struct file *file)
{
	return 0;
}

static BOOL hdmi_drv_init_context(void);

static char* _hdmi_ioctl_spy(unsigned int cmd)
{
	switch(cmd)
	{
		case MTK_HDMI_AUDIO_VIDEO_ENABLE:
			return "MTK_HDMI_AUDIO_VIDEO_ENABLE";
		case MTK_HDMI_AUDIO_ENABLE:
			return "MTK_HDMI_AUDIO_ENABLE";
		case MTK_HDMI_VIDEO_ENABLE:
			return "MTK_HDMI_VIDEO_ENABLE";
		case MTK_HDMI_GET_CAPABILITY:
			return "MTK_HDMI_GET_CAPABILITY";
		case MTK_HDMI_GET_DEVICE_STATUS:
			return "MTK_HDMI_GET_DEVICE_STATUS";
		case MTK_HDMI_VIDEO_CONFIG:
			return "MTK_HDMI_VIDEO_CONFIG";
		case MTK_HDMI_AUDIO_CONFIG:
			return "MTK_HDMI_AUDIO_CONFIG";
		case MTK_HDMI_FORCE_FULLSCREEN_ON:
			return "MTK_HDMI_FORCE_FULLSCREEN_ON";
		case MTK_HDMI_FORCE_FULLSCREEN_OFF:
			return "MTK_HDMI_FORCE_FULLSCREEN_OFF";
		case MTK_HDMI_IPO_POWEROFF:
			return "MTK_HDMI_IPO_POWEROFF";
		case MTK_HDMI_IPO_POWERON:
			return "MTK_HDMI_IPO_POWERON";
		case MTK_HDMI_POWER_ENABLE:
			return "MTK_HDMI_POWER_ENABLE";
		case MTK_HDMI_PORTRAIT_ENABLE:
			return "MTK_HDMI_PORTRAIT_ENABLE";
		case MTK_HDMI_FORCE_OPEN:
			return "MTK_HDMI_FORCE_OPEN";
		case MTK_HDMI_FORCE_CLOSE:
			return "MTK_HDMI_FORCE_CLOSE";
        case MTK_HDMI_IS_FORCE_AWAKE:
            return "MTK_HDMI_IS_FORCE_AWAKE";
        case MTK_HDMI_ENTER_VIDEO_MODE:
            return "MTK_HDMI_ENTER_VIDEO_MODE";
        case MTK_HDMI_LEAVE_VIDEO_MODE:
            return "MTK_HDMI_LEAVE_VIDEO_MODE";
        case MTK_HDMI_REGISTER_VIDEO_BUFFER:
            return "MTK_HDMI_REGISTER_VIDEO_BUFFER";
        case MTK_HDMI_POST_VIDEO_BUFFER:
            return "MTK_HDMI_POST_VIDEO_BUFFER";
		default:
			return "unknown ioctl command";
	}
}

#if 0
static void hdmi_video_config_ioctl(int arg)
{
    HDMI_LOG("video resolution configuration, arg=%d\n", arg);
    RETIF(!p->is_enabled, 0);
    RETIF(IS_HDMI_NOT_ON(),0);

    if(down_interruptible(&hdmi_update_mutex))
    {
        HDMI_LOG("[HDMI] can't get semaphore in\n");
        return;
    }

    if (HDMI_OUTPUT_MODE_DPI_BYPASS == p->output_mode)
    {
        if (1 == arg || 0 == arg) // 720p or 480p
        {
            p->output_video_resolution = arg;
        }
        else
        {
            p->output_video_resolution = HDMI_VIDEO_1920x1080p_30Hz;
        }
        hdmi_video_config(p->output_video_resolution, HDMI_VIN_FORMAT_RGB888, HDMI_VOUT_FORMAT_RGB888);
        up(&hdmi_update_mutex);
        return;
    }

    if(arg == 2 || arg == 3 || arg == 1) // auto or 720p
    {
        HDMI_LOG("video reso: auto\n");
        if(p->hdmi_width == 1280)
        {
            HDMI_LOG("reso is the same\n");
            up(&hdmi_update_mutex);
            return;
        }

        pll_fsel(MT65XX_TVDDS, 0xDBCD0119);
        clk_pol     = HDMI_POLARITY_FALLING;
        de_pol      = HDMI_POLARITY_RISING;
        hsync_pol   = HDMI_POLARITY_RISING;
        vsync_pol   = HDMI_POLARITY_RISING;;

        dpi_clk_div = 2;

        hsync_front_porch   = 110;
        hsync_pulse_width   = 40;
        hsync_back_porch    = 220;
        vsync_front_porch   = 5;
        vsync_pulse_width   = 5;
        vsync_back_porch    = 20;
        p->hdmi_width = 1280;
        p->hdmi_height = 720;
        p->output_video_resolution = HDMI_VIDEO_1280x720p_60Hz;
    }
    else if(arg == 0) // 480p
    {
        HDMI_LOG("video reso: 480p\n");
        if(p->hdmi_width == 720)
        {
            HDMI_LOG("reso is the same\n");
            up(&hdmi_update_mutex);
            return;
        }
        pll_fsel(MT65XX_TVDDS, 0x1C7204C7);
        clk_pol     = HDMI_POLARITY_FALLING;
        de_pol      = HDMI_POLARITY_RISING;
        hsync_pol   = HDMI_POLARITY_FALLING;
        vsync_pol   = HDMI_POLARITY_FALLING;;
        dpi_clk_div = 2;

        hsync_front_porch   = 16;
        hsync_pulse_width   = 62;
        hsync_back_porch    = 60;
        vsync_front_porch   = 9;
        vsync_pulse_width   = 6;
        vsync_back_porch    = 30;
        p->hdmi_width = 720;
        p->hdmi_height = 480;
        p->output_video_resolution = HDMI_VIDEO_720x480p_60Hz;
    }

    memset((void*)hdmi_va, 0, hdmi_default_width*hdmi_default_height*3*hdmi_params->intermediat_buffer_num);

    hdmi_video_config(p->output_video_resolution, HDMI_VIN_FORMAT_RGB888, HDMI_VOUT_FORMAT_RGB888);

    DPI_CHECK_RET(DPI_DisableClk());
    DPI_CHECK_RET(DPI_ConfigPixelClk(clk_pol, dpi_clk_div, dpi_clk_duty));

    DPI_CHECK_RET(DPI_ConfigHsync(hsync_pol, hsync_pulse_width, hsync_back_porch, hsync_front_porch));

    DPI_CHECK_RET(DPI_ConfigVsync(vsync_pol, vsync_pulse_width, vsync_back_porch, vsync_front_porch));

    DPI_CHECK_RET(DPI_FBSetSize(p->hdmi_width, p->hdmi_height));

    DPI_CHECK_RET(DPI_FBSetPitch(DPI_FB_0, p->hdmi_width*3));
    DPI_CHECK_RET(DPI_EnableClk());
    up(&hdmi_update_mutex);
}
#endif

static long hdmi_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;

	int r = 0;

	HDMI_LOG("cmd=%s, arg=0x%08lx\n", _hdmi_ioctl_spy(cmd), arg);

    switch(cmd)
    {
    	case MTK_HDMI_AUDIO_VIDEO_ENABLE:
        {
            if (arg)
            {
                if(p->is_enabled)
                    return 0;

                HDMI_CHECK_RET(hdmi_drv_init());
                hdmi_drv->enter();
                hdmi_power_on();
                p->is_enabled = true;
            }
            else
            {
                if(!p->is_enabled)
                    return 0;

                //when disable hdmi, HPD is disabled
                switch_set_state(&hdmi_switch_data, HDMI_STATE_NO_DEVICE);

                hdmi_power_off();

                //wait hdmi finish update
                if (down_interruptible(&hdmi_update_mutex)) {
                    printk("[hdmi][HDMI] can't get semaphore in %s()\n", __func__);
                    return -EFAULT;
                }
                HDMI_CHECK_RET(hdmi_drv_deinit());
                up(&hdmi_update_mutex);

                hdmi_drv->exit();
                p->is_enabled = false;
            }

            break;
        }
		case MTK_HDMI_FORCE_FULLSCREEN_ON:
        case MTK_HDMI_FORCE_CLOSE:
        {
            RETIF(p->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS, 0);
            RETIF(!p->is_enabled, 0);
            RETIF(IS_HDMI_OFF(), 0);

            if(p->is_force_disable == true)
                break;

            if (IS_HDMI_FAKE_PLUG_IN())
            {
                hdmi_suspend();
                switch_set_state(&hdmi_switch_data, HDMI_STATE_NO_DEVICE);
            }
            else
            {
                if(hdmi_drv->get_state() == HDMI_STATE_ACTIVE)
                {
                    hdmi_suspend();
                    switch_set_state(&hdmi_switch_data, HDMI_STATE_NO_DEVICE);
                }
            }

            p->is_force_disable = true;

            break;
	    }
		case MTK_HDMI_FORCE_FULLSCREEN_OFF:
        case MTK_HDMI_FORCE_OPEN:
        {
            RETIF(p->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS, 0);
            RETIF(!p->is_enabled, 0);
            RETIF(IS_HDMI_OFF(), 0);

            if(p->is_force_disable == false)
                break;

            if (IS_HDMI_FAKE_PLUG_IN())
            {
                hdmi_resume();
                msleep(1000);
                switch_set_state(&hdmi_switch_data, HDMI_STATE_ACTIVE);
            }
            else
            {
                if(hdmi_drv->get_state() == HDMI_STATE_ACTIVE)
                {
                    hdmi_resume();
                    msleep(1000);
                    switch_set_state(&hdmi_switch_data, HDMI_STATE_ACTIVE);
                }
            }
            p->is_force_disable = false;

            break;
        }
    #if 0
	case MTK_HDMI_PORTRAIT_ENABLE:
        {
            if(down_interruptible(&hdmi_update_mutex))
            {
                HDMI_LOG("[HDMI] can't get semaphore in\n");
                return;
            }

            if (arg)
            {
                HDMI_LOG("[HDMIPortrait] hdmi force portrait on\n");
                p->is_force_portrait = true;
                p->orientation_store_portrait = p->orientation; //backup original orientation
                p->orientation = 0;
                p->is_reconfig_needed = true;
            }
            else
            {
                HDMI_LOG("[HDMIPortrait] hdmi force portrait off\n");
                p->is_force_portrait = false;
                p->orientation = p->orientation_store_portrait; //restore original orientation
                p->is_reconfig_needed = true;
            }
            up(&hdmi_update_mutex);
            break;
        }
   #endif
        /* Shutdown thread(No matter IPO), system suspend/resume will go this way... */
        case MTK_HDMI_POWER_ENABLE:
        {
			RETIF(!p->is_enabled, 0);

            if (arg)
            {
                hdmi_power_on();
            }
            else
            {
                hdmi_power_off();
				//why add this?
                //memset((void*)hdmi_va, 0, hdmi_default_width*hdmi_default_height*3*hdmi_params->intermediat_buffer_num);
            }
            break;
        }
        case MTK_HDMI_AUDIO_ENABLE:
        {
			RETIF(!p->is_enabled, 0);

            if (arg)
            {
                HDMI_CHECK_RET(hdmi_audio_enable(true));
            }
            else
            {
                HDMI_CHECK_RET(hdmi_audio_enable(false));
            }

            break;
        }
        case MTK_HDMI_VIDEO_ENABLE:
        {
			RETIF(!p->is_enabled, 0);
            break;
        }
        case MTK_HDMI_VIDEO_CONFIG:
        {
            HDMI_LOG("video resolution configuration, arg=%ld\n", arg);
            RETIF(!p->is_enabled, 0);
            RETIF(IS_HDMI_NOT_ON(),0);

            if(down_interruptible(&hdmi_update_mutex))
            {
                HDMI_LOG("[HDMI] can't get semaphore in\n");
                return EAGAIN;
            }

            if (HDMI_OUTPUT_MODE_DPI_BYPASS == p->output_mode)
            {
                if (1 == arg || 0 == arg) // 720p or 480p
                {
                    p->output_video_resolution = arg;
                }
                else
                {
                    p->output_video_resolution = HDMI_VIDEO_1920x1080p_30Hz;
                }
                hdmi_video_config(p->output_video_resolution, HDMI_VIN_FORMAT_RGB888, HDMI_VOUT_FORMAT_RGB888);
                up(&hdmi_update_mutex);
                break;
            }

            if(arg == 2 || arg == 3 || arg == 1) // auto or 720p
            {
                HDMI_LOG("video reso: auto\n");
                if(p->hdmi_width == 1280)
                {
                    HDMI_LOG("reso is the same\n");
                    up(&hdmi_update_mutex);
                    break;
                }

				pll_fsel(MT65XX_TVDDS, 0xDBCD0119);
			    clk_pol     = HDMI_POLARITY_FALLING;
			    de_pol      = HDMI_POLARITY_RISING;
			    hsync_pol   = HDMI_POLARITY_RISING;
			    vsync_pol   = HDMI_POLARITY_RISING;;

                dpi_clk_div = 2;

                hsync_front_porch   = 110;
                hsync_pulse_width   = 40;
                hsync_back_porch    = 220;
                vsync_front_porch   = 5;
                vsync_pulse_width   = 5;
                vsync_back_porch    = 20;
                p->hdmi_width = 1280;
                p->hdmi_height = 720;
                p->output_video_resolution = HDMI_VIDEO_1280x720p_60Hz;
            }
            else if(arg == 0) // 480p
            {
                HDMI_LOG("video reso: 480p\n");
                if(p->hdmi_width == 720)
                {
                    HDMI_LOG("reso is the same\n");
                    up(&hdmi_update_mutex);
                    break;
                }
                pll_fsel(MT65XX_TVDDS, 0x1C7204C7);
			    clk_pol     = HDMI_POLARITY_FALLING;
			    de_pol      = HDMI_POLARITY_RISING;
			    hsync_pol   = HDMI_POLARITY_FALLING;
			    vsync_pol   = HDMI_POLARITY_FALLING;;
                dpi_clk_div = 2;

                hsync_front_porch   = 16;
                hsync_pulse_width   = 62;
                hsync_back_porch    = 60;
                vsync_front_porch   = 9;
                vsync_pulse_width   = 6;
                vsync_back_porch    = 30;
                p->hdmi_width = 720;
                p->hdmi_height = 480;
                p->output_video_resolution = HDMI_VIDEO_720x480p_60Hz;
            }

            memset((void*)hdmi_va, 0, hdmi_default_width*hdmi_default_height*3*hdmi_params->intermediat_buffer_num);

            hdmi_video_config(p->output_video_resolution, HDMI_VIN_FORMAT_RGB888, HDMI_VOUT_FORMAT_RGB888);

            DPI_CHECK_RET(DPI_DisableClk());
            DPI_CHECK_RET(DPI_ConfigPixelClk(clk_pol, dpi_clk_div, dpi_clk_duty));

            DPI_CHECK_RET(DPI_ConfigHsync(hsync_pol, hsync_pulse_width, hsync_back_porch, hsync_front_porch));

            DPI_CHECK_RET(DPI_ConfigVsync(vsync_pol, vsync_pulse_width, vsync_back_porch, vsync_front_porch));

            DPI_CHECK_RET(DPI_FBSetSize(p->hdmi_width, p->hdmi_height));

            DPI_CHECK_RET(DPI_FBSetPitch(DPI_FB_0, p->hdmi_width*3));
            DPI_CHECK_RET(DPI_EnableClk());
            up(&hdmi_update_mutex);
            break;
        }
        case MTK_HDMI_AUDIO_CONFIG:
        {
			RETIF(!p->is_enabled, 0);

            break;
        }
        case MTK_HDMI_IS_FORCE_AWAKE:
        {
            if (!hdmi_drv_init_context())
	        {
	            printk("[hdmi]%s, hdmi_drv_init_context fail\n", __func__);
		        return HDMI_STATUS_NOT_IMPLEMENTED;
	        }
            r = copy_to_user(argp, &hdmi_params->is_force_awake, sizeof(hdmi_params->is_force_awake)) ? -EFAULT : 0;
            break;
        }
        case MTK_HDMI_ENTER_VIDEO_MODE:
        {
			RETIF(!p->is_enabled, 0);
			RETIF(HDMI_OUTPUT_MODE_VIDEO_MODE != p->output_mode, 0);
            //FIXME
            break;
        }
        case MTK_HDMI_REGISTER_VIDEO_BUFFER:
        {
            struct hdmi_video_buffer_info video_buffer_info;
			RETIF(!p->is_enabled, 0);
			RETIF(HDMI_OUTPUT_MODE_VIDEO_MODE != p->output_mode, 0);
            if(copy_from_user(&video_buffer_info, (void __user *)argp, sizeof(video_buffer_info)))
            {
                HDMI_LOG("copy_from_user failed! line\n");
                r = -EFAULT;
                break;
            }
			if(video_buffer_info.src_vir_addr == 0)
			{
				HDMI_LOG("[Error]HDMI_REGISTER_VIDEO_BUFFER VA should not be 0x00000000\n");
                break;
			}
            if(down_interruptible(&hdmi_video_mode_mutex))
            {
                HDMI_LOG("[HDMI] can't get semaphore in\n");
                break;
            }
            if(hdmi_add_video_buffer(&video_buffer_info, file) < 0)
            {
                r = -ENOMEM;
            }
            up(&hdmi_video_mode_mutex);

            break;

        }
        case MTK_HDMI_POST_VIDEO_BUFFER:
        {
            struct hdmi_video_buffer_info video_buffer_info;
            struct hdmi_video_buffer_list* buffer_list;
			RETIF(!p->is_enabled, 0);
			RETIF(HDMI_OUTPUT_MODE_VIDEO_MODE != p->output_mode, 0);
            RETIF(IS_HDMI_NOT_ON(),0);
            if(copy_from_user(&video_buffer_info, (void __user *)argp, sizeof(video_buffer_info)))
            {
                HDMI_LOG("copy_from_user failed! line\n");
                r = -EFAULT;
                break;
            }
            if(down_interruptible(&hdmi_update_mutex))
            {
                HDMI_LOG("[HDMI] can't get semaphore in\n");
                break;
            }
            if(down_interruptible(&hdmi_video_mode_mutex))
            {
                HDMI_LOG("[HDMI] can't get semaphore in\n");
                up(&hdmi_update_mutex);
                break;
            }

            if(NULL == (buffer_list = hdmi_search_video_buffer(&video_buffer_info, file)))
            {
                up(&hdmi_video_mode_mutex);
                up(&hdmi_update_mutex);
                break;
            }

            if(IS_HDMI_IN_VIDEO_MODE())
            {
                //FIXME
            }
            else //Enter video mode the first time
            {
                SET_HDMI_TO_VIDEO_MODE();
            }
            up(&hdmi_update_mutex);

            DBG_OnTriggerHDMI();
            if(p->is_clock_on)
            {
                //Wait for change buffer
                DPI_CHECK_RET(DPI_FBSetAddress(DPI_FB_0, buffer_list->buffer_mva));
                *((volatile unsigned int*)(0xF208C020)) = 1;
                SET_HDMI_VIDEO_MODE_DPI_CHANGE_ADDRESS();

                wait_event_interruptible(hdmi_video_mode_wq, atomic_read(&hdmi_video_mode_event));
                atomic_set(&hdmi_video_mode_event, 0);
            }
            DBG_OnHDMIDone();

            up(&hdmi_video_mode_mutex);
            break;
        }
        case MTK_HDMI_LEAVE_VIDEO_MODE:
        {
			RETIF(!p->is_enabled, 0);
			RETIF(HDMI_OUTPUT_MODE_VIDEO_MODE != p->output_mode, 0);
            if(down_interruptible(&hdmi_update_mutex))
            {
                HDMI_LOG("[HDMI] can't get semaphore in\n");
                break;
            }
            //clear UI buffer
            memset((void*)hdmi_va, 0, p->hdmi_width*p->hdmi_height*3*hdmi_params->intermediat_buffer_num);

            if(down_interruptible(&hdmi_video_mode_mutex))
            {
                HDMI_LOG("[HDMI] can't get semaphore in\n");
                up(&hdmi_update_mutex);
                break;
            }
            if(p->is_clock_on)
            {
                //Wait for change buffer
                DPI_CHECK_RET(DPI_FBSetAddress(DPI_FB_0, hdmi_mva + p->hdmi_width*p->hdmi_height*3*hdmi_buffer_read_id));
                *((volatile unsigned int*)(0xF208C020)) = 1;
                SET_HDMI_VIDEO_MODE_DPI_CHANGE_ADDRESS();

                wait_event_interruptible(hdmi_video_mode_wq, atomic_read(&hdmi_video_mode_event));
                atomic_set(&hdmi_video_mode_event, 0);
            }
            SET_HDMI_LEAVE_VIDEO_MODE();
            //Destory video buffer mva
            hdmi_destory_video_buffer();
            up(&hdmi_video_mode_mutex);
            up(&hdmi_update_mutex);
            break;
        }
        default:
        {
            printk("[hdmi][HDMI] arguments error\n");
            break;
        }
    }

	return r;
}

static int hdmi_add_video_buffer(struct hdmi_video_buffer_info *buffer_info, struct file *file)
{
    struct hdmi_video_buffer_list *buffer_list;
    int r = 0;
    /*{
         struct hdmi_video_buffer_list *buffer_list;
         struct list_head *temp;
         HDMI_LOG("before add a node,list node Addr:\n");
         list_for_each(temp, hdmi_video_buffer_list_head)
         {
             buffer_list = list_entry(temp, struct hdmi_video_buffer_list, list);
             HDMI_LOG("0x%08x,    ", (unsigned int)temp);
         }
         HDMI_LOG("\n");
    }*/
    buffer_list = (struct hdmi_video_buffer_list*)kmalloc(sizeof(struct hdmi_video_buffer_list), GFP_KERNEL);
    if(!buffer_list)
    {
        HDMI_LOG("kmalloc failed\n");
        r = -1;
        goto EXIT;
    }
    {
        int ret = m4u_alloc_mva_stub(M4U_CLNTMOD_DPI, \
                                    (unsigned int)(buffer_info->src_vir_addr), \
                                    buffer_info->src_size, \
                                    &buffer_list->buffer_mva);
        if(ret)
        {
            HDMI_LOG("allocate mva for dpi fail\n");
            r = -2;
            goto EXIT;
        }
    }
    m4u_insert_tlb_range_stub(M4U_CLNTMOD_DPI, \
                              buffer_list->buffer_mva, \
                              buffer_list->buffer_mva + buffer_info->src_size - 1, \
                              RT_RANGE_HIGH_PRIORITY, \
                              0);
    HDMI_LOG("HDMI_ADD_VIDEO_BUFFER,hdmi_va=0x%08x, hdmi_mva=0x%08x\n", (unsigned int)(buffer_info->src_vir_addr), buffer_list->buffer_mva);

    buffer_list->buffer_info = *buffer_info;
    buffer_list->pid = current->tgid;
    buffer_list->file_addr = (void*)file;
    //HDMI_LOG("[current process ID = %d, current process name:%s]\n", current->tgid, current->comm);
    list_add_tail(&buffer_list->list, hdmi_video_buffer_list_head);
    /*{
        struct hdmi_video_buffer_list *temp;
        HDMI_LOG("after add a node,list node Addr:\n");
        list_for_each_entry(temp, hdmi_video_buffer_list_head, list)
        {
            HDMI_LOG("0x%x,    ", (unsigned int)temp);
        }
        HDMI_LOG("\n");
    }*/

EXIT:
    return r;
}
static struct hdmi_video_buffer_list* hdmi_search_video_buffer(struct hdmi_video_buffer_info *buffer_info, struct file *file)
{
    struct hdmi_video_buffer_list *temp;
    list_for_each_entry(temp, hdmi_video_buffer_list_head, list)
    {
        if(temp->buffer_info.src_vir_addr == buffer_info->src_vir_addr && \
           temp->pid == current->tgid \
          )
        {
            return temp;
        }
    }
    HDMI_LOG("Error: Cannot find matched VA:0x%08x in buffer list\n", (unsigned int)(buffer_info->src_vir_addr));
    return NULL;
}

static void hdmi_destory_video_buffer(void)
{
    struct hdmi_video_buffer_list *temp, *next;
    int ret;
    list_for_each_entry_safe(temp, next, hdmi_video_buffer_list_head, list)
    {
        m4u_invalid_tlb_range_stub(M4U_CLNTMOD_DPI, \
                                   temp->buffer_mva, \
                                   temp->buffer_mva + temp->buffer_info.src_size - 1);
        ret = m4u_dealloc_mva_stub(M4U_CLNTMOD_DPI, \
                                   (unsigned int)(temp->buffer_info.src_vir_addr), \
                                   temp->buffer_info.src_size, \
                                   temp->buffer_mva);
        if (ret < 0)
        {
            HDMI_LOG("Invlid M4U Error, vir_addr = 0x%08x, mva = 0x%08x\n", (unsigned int)(temp->buffer_info.src_vir_addr), temp->buffer_mva);
        }

        list_del(&temp->list);
        kfree(temp);
    }
}

static int hdmi_remove(struct platform_device *pdev)
{
	return 0;
}

static BOOL hdmi_drv_init_context(void)
{
	static const HDMI_UTIL_FUNCS hdmi_utils =
	{
		.udelay             	= hdmi_udelay,
		.mdelay             	= hdmi_mdelay,
		.state_callback			= hdmi_state_callback,
	};

	if (hdmi_drv != NULL)
		return TRUE;


    hdmi_drv = (HDMI_DRIVER*)HDMI_GetDriver();

	if (NULL == hdmi_drv) return FALSE;

	hdmi_drv->set_util_funcs(&hdmi_utils);
	hdmi_drv->get_params(hdmi_params);

	return TRUE;
}

static void __exit hdmi_exit(void)
{
	device_destroy(hdmi_class, hdmi_devno);
	class_destroy(hdmi_class);
	cdev_del(hdmi_cdev);
	unregister_chrdev_region(hdmi_devno, 1);

}

struct file_operations hdmi_fops = {
	.owner   = THIS_MODULE,
	.unlocked_ioctl   = hdmi_ioctl,
	.open    = hdmi_open,
	.release = hdmi_release,
};

static int hdmi_probe(struct platform_device *pdev)
{
    int ret = 0;
	struct class_device *class_dev = NULL;

    printk("[hdmi]%s\n", __func__);

    /* Allocate device number for hdmi driver */
	ret = alloc_chrdev_region(&hdmi_devno, 0, 1, HDMI_DEVNAME);
	if(ret)
	{
		printk("[hdmi]alloc_chrdev_region fail\n");
		return -1;
	}

    /* For character driver register to system, device number binded to file operations */
	hdmi_cdev = cdev_alloc();
	hdmi_cdev->owner = THIS_MODULE;
	hdmi_cdev->ops = &hdmi_fops;
	ret = cdev_add(hdmi_cdev, hdmi_devno, 1);

	/* For device number binded to device name(hdmitx), one class is corresponeded to one node */
	hdmi_class = class_create(THIS_MODULE, HDMI_DEVNAME);
	/* mknod /dev/hdmitx */
	class_dev = (struct class_device *)device_create(hdmi_class, NULL, hdmi_devno,	NULL, HDMI_DEVNAME);

	printk("[hdmi][%s] current=0x%08x\n", __func__, (unsigned int)current);

    if (!hdmi_drv_init_context())
	{
	    printk("[hdmi]%s, hdmi_drv_init_context fail\n", __func__);
		return HDMI_STATUS_NOT_IMPLEMENTED;
	}

    init_waitqueue_head(&hdmi_update_wq);

    init_waitqueue_head(&hdmi_video_mode_wq);
    INIT_LIST_HEAD(&hdmi_video_mode_buffer_list);

    hdmi_update_task = kthread_create(hdmi_update_kthread, NULL, "hdmi_update_kthread");
    wake_up_process(hdmi_update_task);

    return 0;
}

static struct platform_driver hdmi_driver = {
    .probe  = hdmi_probe,
    .remove = hdmi_remove,
    .driver = { .name = HDMI_DEVNAME }
};

static int __init hdmi_init(void)
{
	int ret = 0;
	printk("[hdmi]%s\n", __func__);


    if (platform_driver_register(&hdmi_driver))
    {
        printk("[hdmi]failed to register mtkfb driver\n");
        return -1;
    }

	memset((void*)&hdmi_context, 0, sizeof(_t_hdmi_context));
	SET_HDMI_OFF();


    if (!hdmi_drv_init_context())
	{
	    printk("[hdmi]%s, hdmi_drv_init_context fail\n", __func__);
		return HDMI_STATUS_NOT_IMPLEMENTED;
	}

    p->output_mode = hdmi_params->output_mode;
    hdmi_drv->init();
    HDMI_LOG("Output mode is %s\n", (hdmi_params->output_mode==HDMI_OUTPUT_MODE_DPI_BYPASS)?"HDMI_OUTPUT_MODE_DPI_BYPASS":"HDMI_OUTPUT_MODE_LCD_MIRROR");

    if(hdmi_params->output_mode == HDMI_OUTPUT_MODE_DPI_BYPASS) 
    {
        p->output_video_resolution = HDMI_VIDEO_RESOLUTION_NUM;
    }
    
        HDMI_DBG_Init();

    if (hdmi_params->is_force_landscape) {
        hdmi_switch_data.name = "hdmi";
    } else {
        hdmi_switch_data.name = "mtk_hdmi";
    }
	hdmi_switch_data.index = 0;
	hdmi_switch_data.state = NO_DEVICE;

    // for support hdmi hotplug, inform AP the event
	ret = switch_dev_register(&hdmi_switch_data);
	if(ret)
	{
		printk("[hdmi][HDMI]switch_dev_register returned:%d!\n", ret);
		return 1;
	}

	return 0;
}


module_init(hdmi_init);
module_exit(hdmi_exit);
MODULE_AUTHOR("Xuecheng, Zhang <xuecheng.zhang@mediatek.com>");
MODULE_DESCRIPTION("HDMI Driver");
MODULE_LICENSE("GPL");

#endif
