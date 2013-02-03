#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/earlysuspend.h>
#include <linux/mm.h>
#include <linux/slab.h>     //kmalloc
#include <linux/proc_fs.h>  //For proc filesystem


#include <linux/xlog.h>  //Prevent include by user space (no good)
//Arch dependent files
#include <mach/sync_write.h>
#include <mach/mt6577_mdp.h>
#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_clock_manager.h>
//#include <mach/mt6577_pll.h>
#include <mach/irqs.h>
//#include <asm/tcm.h> /*Mt6577 do not have TCM*/


//--------------------------------------------Global Variable (Log Switch)---------------------------------------//
int b_mdp_show_info       = 0;
int b_mdp_show_warning    = 1;
int b_mdp_show_error      = 1;   //Always kepp this on
int b_mdp_show_isrinfo    = 0;
int b_mdp_show_powerinfo  = 0;
int b_mdp_show_zoominfo   = 0;
int b_mdp_show_fpsinfo    = 0;
int b_mdp_show_bbinfo     = 0;
int b_mdp_show_zsdinfo    = 0;




int b_mdp_timeout_dumpreg = 0;

/*Clock switch*/
int b_mdp_clock_all_always_on       =0;  /*General switch*/
int b_mdp_clk_jpeg_dec_always_on    =1;
int b_mdp_clk_jpeg_enc_always_on    =1;
int b_mdp_clk_cam_always_on         =1;
int b_mdp_clk_brz_always_on         =1;
int b_mdp_clk_crz_always_on         =1;
int b_mdp_clk_prz0_always_on        =1;
int b_mdp_clk_prz1_always_on        =1;
int b_mdp_clk_vrz0_always_on        =1;
int b_mdp_clk_vrz1_always_on        =1;
int b_mdp_clk_r_dma0_always_on      =1;
int b_mdp_clk_r_dma1_always_on      =1;
int b_mdp_clk_ovl_always_on         =1;
int b_mdp_clk_ipp_always_on         =1;
int b_mdp_clk_mout_always_on        =1;
int b_mdp_clk_lcd_always_on         =1;
int b_mdp_clk_jpeg_dma_always_on    =1;
int b_mdp_clk_eis_always_on         =1;
int b_mdp_clk_rgb_rot0_always_on    =1;
int b_mdp_clk_rgb_rot1_always_on    =1;
int b_mdp_clk_rgb_rot2_always_on    =1;
int b_mdp_clk_vdo_rot0_always_on    =1;
int b_mdp_clk_vdo_rot1_always_on    =1;
int b_mdp_clk_tv_rot_always_on      =1;





int b_mdp_pd_path = 0;




//--------------------------------------------Global Variable (Resource Lock)---------------------------------------//
struct MdpResourceTbl
{
    spinlock_t      spin_lock;
    unsigned long   resource_in_use_table;          /*record the resource that is in use for the time being*/
    unsigned long   resource_in_occupied_table;     /*resource in this talbe is not only in use, but also in "LONG-TIME" occupied.*/
}; 



static struct MdpResourceTbl g_mdp_resource_tbl =
                             {
                                 //.spin_lock                  = SPIN_LOCK_UNLOCKED,
                                 .spin_lock                    =__SPIN_LOCK_UNLOCKED(g_mdp_resource_tbl.spin_lock),
                                 .resource_in_use_table      = 0,   /* 0 : no mdp resource is in used*/
                                 .resource_in_occupied_table = 0,   /* 0 : no mdp resource is occupied*/
                             };

static DECLARE_WAIT_QUEUE_HEAD(g_mdp_resource_wait_queue);


//--------------------------------------------Global Variable (INTERRUPT)------------------------------------------//
struct MdpIrqDoneTbl
{
    spinlock_t      spin_lock;
    unsigned long   irq_done_table;     /*elements that has received done signal through INT, not include error signal*/
    unsigned long   rot_exec_count[5];  /*5 exec count for each rot dma*/

    unsigned long   fps_show_info_period;
    unsigned long   fps_last_rot_exec_count[5];
    struct timeval  fps_last_time[5];
    unsigned long   fps_int_count[5];
    unsigned long   fps_fps_number[5];
    
}; 


static struct MdpIrqDoneTbl g_mdp_irq_tbl =
                        {
                            //.spin_lock        = SPIN_LOCK_UNLOCKED,
                            .spin_lock          =__SPIN_LOCK_UNLOCKED(g_mdp_irq_tbl.spin_lock),
                            .irq_done_table     = 0,    /* 0: no irq flag has been set*/
                            .rot_exec_count     = { 0, 0, 0, 0, 0 },  

                            .fps_show_info_period = 0,
                            .fps_last_rot_exec_count= { 0, 0, 0, 0, 0 },  
                            .fps_last_time      = { {0,0},{0,0},{0,0},{0,0},{0,0} },
                            .fps_int_count      = { 0, 0, 0, 0, 0 },  
                            .fps_fps_number     = { 0, 0, 0, 0, 0 }, 
                            
                        };

static DECLARE_WAIT_QUEUE_HEAD(g_mdp_irq_done_queue);

//--------------------------------------------Global Variable (ZOOM)------------------------------------------//
struct MdpZoomParam
{
    spinlock_t      spin_lock;
    
    unsigned long   is_dirty;

    unsigned long   frame_counter;
    unsigned long   applied_frame;

    stZoomSetting   CRZ_REG;
    
};

static struct MdpZoomParam g_mdp_zoom_param =
                    {
                        //.spin_lock      = SPIN_LOCK_UNLOCKED,
                        .spin_lock          =__SPIN_LOCK_UNLOCKED(g_mdp_zoom_param.spin_lock),
                        .is_dirty     = 0,
                        .frame_counter  = 0,
                        .applied_frame  = 0,
                    };

//--------------------------------------------Global Variable (Power)------------------------------------------//
struct MdpClockStatus
{
    spinlock_t      spin_lock;
    int             ref_count[MDP_ELEMENT_MAX_NUM];
};

static struct MdpClockStatus g_mdp_clock_status =
                    {
                        //.spin_lock      = SPIN_LOCK_UNLOCKED,
                        .spin_lock          =__SPIN_LOCK_UNLOCKED(g_mdp_clock_status.spin_lock),    
                        .ref_count      = { /*23 0's*/
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,0,0,0,0,0,0,0,0,
                                            0,0,0
                                          },
                    };


//--------------------------------------------Global Variable (MVA Loop Memory Tracker)------------------------------------------//
#define MDPMVALOOPMEM_MAX_CLIENT    (20)

struct MdpMvaLoopMemClient
{
    long   statistic_alloc_count;
    long   statistic_free_count;
    long   statistic_count_balance;

    long   statistic_alloc_size;
    long   statistic_free_size;
    long   statistic_size_balance;

    unsigned long   last_tgid;
};

struct MdpMvaLoopMemStatistic
{
    spinlock_t                      spin_lock;
    struct MdpMvaLoopMemClient      client[MDPMVALOOPMEM_MAX_CLIENT];
};

static struct MdpMvaLoopMemStatistic g_mdp_mva_loop_mem_statistic =
                    {
                        //.spin_lock      = SPIN_LOCK_UNLOCKED,
                        .spin_lock      =__SPIN_LOCK_UNLOCKED(g_mdp_mva_loop_mem_statistic.spin_lock),    
                        .client         = { {0} },
                    };

//--------------------------------------------Global Variable (last preview frame for zsd)------------------------------------------//

static unsigned long g_mdp_zsd_update_reg_safty_zone = 0;


//--------------------------------------------Global Variable------------------------------------------//


typedef struct {
    pid_t tgid;
    unsigned long u4OwnResource;    //Record used resource by each process (opened driver)
} stMDPOpLog;


//--------------------------------------------------------------_______________-----------------------//

void MT6577MDP_DUMPREG(void);

/*=============================================================================
    Static Functions
  =============================================================================*/
static inline int MT6577_TimeStamp(unsigned long mdp_id, unsigned long a_u4Cmd , stTimeStamp * a_u4Param )
{
    stTimeStamp         time_stamp_param;
    unsigned long u4Index = 0;
    unsigned long u4RegVal;
    unsigned long       u4BuffCnt;
    unsigned long       pBase;
    int                 rot_index;

    static stTimeStamp  TimeStamp[5];

    if( a_u4Param != NULL )
    {
        if( copy_from_user( &time_stamp_param , a_u4Param , sizeof(stTimeStamp)) )
        {
            MDPK_ERROR("MT6577_TimeStamp copy from user failed!!\n");
            return -EFAULT;
        }
        //If parameter has assigned mdp_id, prefer this
        mdp_id = time_stamp_param.mdp_id;
    }

    
    switch( mdp_id )
    {
        case MID_RGB_ROT0:
            pBase = RGB_ROT0_BASE;
            rot_index = 0;
            break;
        case MID_RGB_ROT1:
            pBase = RGB_ROT1_BASE;
            rot_index = 1;
            break;
        case MID_RGB_ROT2:
            pBase = RGB_ROT2_BASE;
            rot_index = 2;
            break;
        
        case MID_VDO_ROT0:
            pBase = VDO_ROT0_BASE;
            rot_index = 3;
            break;
        case MID_VDO_ROT1:
            pBase = VDO_ROT1_BASE;
            rot_index = 4;
            break;
        
        default:
            MDPK_ERROR("Invalid mdp_id:0x%08X\n", (unsigned int)mdp_id);
            return -EFAULT;
    }


    u4BuffCnt = ioread32( MT6577_ROT_DMA_CFG_ADDR(pBase) );

    u4BuffCnt = ((u4BuffCnt >> 8 ) & 0xF);

    if(0 == a_u4Cmd)
    {
        
        unsigned long long  sec; 
        unsigned long       usec;

        sec = cpu_clock(0);     //ns
        do_div( sec, 1000 );    //usec
        usec = do_div( sec, 1000000);    //sec and usec

        
        u4RegVal = ioread32(MT6577_ROT_DMA_QUEUE_RSTA_ADDR(pBase));
        u4Index = ((0xF00 & u4RegVal) >> 8);
        u4Index = ((u4Index > 0) ? (u4Index - 1) : u4BuffCnt);
        TimeStamp[rot_index].sec[u4Index] = sec;
        TimeStamp[rot_index].usec[u4Index] = usec;

        mb();   /*Add memory barrier for timestamp reverse issue(?)*/
        MDPK_ISRINFO("Update Time Stamp[%d].[%lu]= %08ds %08dus\n", rot_index, u4Index, (int)sec, (int)usec);

        /*Check Timesamp reverse*/
        {
            int b_reverse_detected = 0;
            unsigned long last_index;
            unsigned long last_s, last_us, curr_s, curr_us;

            last_index = ( u4Index == 0 ) ? u4BuffCnt : (u4Index - 1);

            last_s  = TimeStamp[rot_index].sec[last_index];
            last_us = TimeStamp[rot_index].usec[last_index];
            curr_s  = TimeStamp[rot_index].sec[u4Index];
            curr_us = TimeStamp[rot_index].usec[u4Index];

            if( curr_s < last_s )
            {
                b_reverse_detected = 1;
            } else if( ( curr_s == last_s) &&  ( curr_us < last_us) ) 
            {
                b_reverse_detected = 1;
            }

            if( b_reverse_detected )
            {
                MDPK_ERROR("Timestamp Reverse! mdp_id = 0x%08X , sec/us[%lu]=(%lu %lu) < sec/us[%lu]=(%lu %lu)\n", 
                    (unsigned int)mdp_id, u4Index, curr_s, curr_us, last_index, last_s, last_us );
            }
            
        }
    }
    else
    {
        if(copy_to_user((void __user *) a_u4Param , &TimeStamp[rot_index] , sizeof(stTimeStamp)))
        {
            MDPK_ERROR("MDP time stamp copy to user failed!!\n");
            return -1;
        }
    }
    return 0;
}

static const char* MdpIoctlGetCmdStr( unsigned int    a_u4Command  )
{
    #define _IOCTLSTR_SWITCH_CASE_( _iocmd_ ) \
                        case _iocmd_:\
                            return #_iocmd_;\
                        break;
    
    switch(a_u4Command)
    {
    _IOCTLSTR_SWITCH_CASE_(MT6577MDP_T_LOCKRESOURCE);
    _IOCTLSTR_SWITCH_CASE_(MT6577MDP_T_UNLOCKRESOURCE);
    _IOCTLSTR_SWITCH_CASE_(MT6577MDP_X_WAITIRQ);
    _IOCTLSTR_SWITCH_CASE_(MT6577MDP_T_CLRIRQ);
    _IOCTLSTR_SWITCH_CASE_(MT6577MDP_T_DUMPREG);
    _IOCTLSTR_SWITCH_CASE_(MT6577MDP_G_TIMESTAMP);
    _IOCTLSTR_SWITCH_CASE_(MT6577MDP_G_PMEMRANGE);
    _IOCTLSTR_SWITCH_CASE_(MT6577MDP_T_SETZOOM);
    _IOCTLSTR_SWITCH_CASE_(MDPK_BITBLT_G_WAIT_REQUEST);
    _IOCTLSTR_SWITCH_CASE_(MDPK_BITBLT_T_INFORM_DONE);
    _IOCTLSTR_SWITCH_CASE_(MT6577MDP_T_MVALOOPMEMTRACK);
    _IOCTLSTR_SWITCH_CASE_(MDPIO_T_CPUCLOCKGET);

    default :
        return "";
    break;
    }
}


/*=============================================================================
    ISR Functions
  =============================================================================*/
#if defined(MDP_KERNEL_FLAG_1_SUPPORT_INT)

typedef struct {
    spinlock_t SpinLock;
    unsigned long u4Dirty;
    stZSDZoomReg stReg;
} stZSDZoomSetting;

static stZSDZoomSetting g_stMT6577ZSDZoom = 
                    {
                        .SpinLock   = __SPIN_LOCK_UNLOCKED(g_stMT6577ZSDZoom.SpinLock),
                        .u4Dirty    =   0,
                    };

static int MT6577_SetZSDZoom(stZSDZoomReg * a_u4Param)
{
    unsigned long flag;

    spin_lock_irqsave(&g_stMT6577ZSDZoom.SpinLock , flag);
    g_stMT6577ZSDZoom.u4Dirty= 0;
    spin_unlock_irqrestore(&g_stMT6577ZSDZoom.SpinLock , flag);

    if(copy_from_user(&g_stMT6577ZSDZoom.stReg , (struct stZSDZoomReg *)a_u4Param , sizeof(stZSDZoomReg)))
    {
        MDPK_ERROR("MDP set ZSD zoom copy from user failed!!\n");
        return -EFAULT;
    }

    spin_lock_irqsave(&g_stMT6577ZSDZoom.SpinLock , flag);
    g_stMT6577ZSDZoom.u4Dirty = 1;
    spin_unlock_irqrestore(&g_stMT6577ZSDZoom.SpinLock , flag);

    return 0;
}


#define MT6577_SYSRAM_PA 0xC2000000

// last preview frame for zsd
typedef struct {
    spinlock_t SpinLock;
    unsigned long u4Dirty;
    stZSDPreviewReg stReg;
} stZSDPreviewSetting;

static stZSDPreviewSetting g_stMT6577ZSDPreview =
                    {
                        .SpinLock   = __SPIN_LOCK_UNLOCKED(g_stMT6577ZSDPreview.SpinLock),
                        .u4Dirty    =   0,
                    };

static inline int _DescWriteWaitBusy( unsigned long reg_base )
{
    unsigned long u4TimeOut;

    u4TimeOut = 0;
    while(!(ioread32(MT6577_ROT_DMA_QUEUE_WSTA_ADDR(reg_base)) & 0x1))
    {
        u4TimeOut += 1;
        //if(u4TimeOut  > 100000)
        if(u4TimeOut  > 0xFFFFFF)
        {
            return -1;
        }
    }

    return 0;
}


static inline int DescQueueRefill(unsigned long index)
{

    #define _CHECK_Q_BUSY_AND_RETURN_( _seg_name_ ) \
        if( _DescWriteWaitBusy( RGB_ROT0_BASE ) < 0 ){\
            MDPK_ERROR("put " _seg_name_ " command queue timeout\n");\
            return -1; }
    stZSDPreviewReg * pstReg = &g_stMT6577ZSDPreview.stReg;

    /*Fill descriptor command*/
    /*[SEG1]*/
    _CHECK_Q_BUSY_AND_RETURN_("[SEG1]");
    mt65xx_reg_sync_writel(pstReg->u4RGB0Seg1 , MT6577_ROT_DMA_QUEUE_DATA_ADDR(RGB_ROT0_BASE));

    /*[SEG4]*/
    _CHECK_Q_BUSY_AND_RETURN_("[SEG4]");
    mt65xx_reg_sync_writel(pstReg->u4RGB0Seg4 , MT6577_ROT_DMA_QUEUE_DATA_ADDR(RGB_ROT0_BASE));

    /*[SEG5]*/
    _CHECK_Q_BUSY_AND_RETURN_("[SEG5]");
    mt65xx_reg_sync_writel(pstReg->u4RGB0Seg5 , MT6577_ROT_DMA_QUEUE_DATA_ADDR(RGB_ROT0_BASE));

    /*[SEG6]*/
    _CHECK_Q_BUSY_AND_RETURN_("[SEG6]");
    mt65xx_reg_sync_writel(pstReg->u4RGB0Seg6 , MT6577_ROT_DMA_QUEUE_DATA_ADDR(RGB_ROT0_BASE));

    /*[SEG7]*/
    _CHECK_Q_BUSY_AND_RETURN_("[SEG7]");
    mt65xx_reg_sync_writel(pstReg->u4RGB0Seg7 , MT6577_ROT_DMA_QUEUE_DATA_ADDR(RGB_ROT0_BASE));
    /*[SEG8]*/
    _CHECK_Q_BUSY_AND_RETURN_("[SEG8]");
    mt65xx_reg_sync_writel(pstReg->u4RGB0Seg8 , MT6577_ROT_DMA_QUEUE_DATA_ADDR(RGB_ROT0_BASE));

    /*[SEG9]*/
    _CHECK_Q_BUSY_AND_RETURN_("[SEG9]");
    mt65xx_reg_sync_writel(pstReg->u4RGB0Seg9 , MT6577_ROT_DMA_QUEUE_DATA_ADDR(RGB_ROT0_BASE));

    return 0;
}



// TODO: This must sync with CameraIO.h
#define MDP_CAMERA_PREVIEW_WORKING_BUFFER_CNT 3
static inline unsigned long UpdateZsdPreviewReg(void)
{


    unsigned long   reg_q_rsta, reg_q_wsta;
    unsigned long   reg_q_rd_base_pa;
    unsigned long   reg_q_base_pa, reg_q_base_va;

    int             b_empty;
    unsigned long   hw_read_index,hw_r_wrap;  /*RPT*/
    unsigned long   hw_write_index,hw_w_wrap; /*WPT*/
    unsigned long   desc_remain_count;

    //unsigned long   rd_rpt;
    unsigned long   rd_adv_cnt;
    unsigned long   reg_q_depth;
    unsigned long   q_index;
    unsigned long * descriptBase = 0;
    stZSDPreviewReg * pstReg = &g_stMT6577ZSDPreview.stReg;
    unsigned long u4RegVal = 0;
    unsigned long ret = 0xff;


    mt65xx_reg_sync_writel(0x1, MT6577_RZ_LOCK_ADDR( CRZ_BASE ));
    mt65xx_reg_sync_writel(0x1, MT6577_ROT_DMA_LOCK_ADDR( RGB_ROT0_BASE ));

    reg_q_rsta  = ioread32(MT6577_ROT_DMA_QUEUE_RSTA_ADDR( RGB_ROT0_BASE ));
    reg_q_wsta = ioread32(MT6577_ROT_DMA_QUEUE_WSTA_ADDR( RGB_ROT0_BASE ));
    reg_q_base_pa = ioread32(MT6577_ROT_DMA_QUEUE_BASE_ADDR( RGB_ROT0_BASE ));
    reg_q_rd_base_pa = ioread32(MT6577_ROT_DMA_RD_BASE_ADDR( RGB_ROT0_BASE ));

    reg_q_depth = ioread32(MT6577_ROT_DMA_CFG_ADDR(RGB_ROT0_BASE));
    reg_q_depth = ((reg_q_depth & 0xf00) >> 8);
    reg_q_depth += 1;

    if(reg_q_base_pa < MT6577_SYSRAM_PA)
    {
        MDPK_ERROR("RGBROT0 sysram base address is weird 0x%08X\n" , (unsigned int)reg_q_base_pa);
        return ret;
    }
    reg_q_base_va = SYSRAM_BASE + (reg_q_base_pa - MT6577_SYSRAM_PA);

    b_empty         = ( reg_q_rsta & ( 0x1 << 2 ) ) >> 2;
    hw_read_index   = ( reg_q_rsta & ( 0xF << 8 ) ) >> 8;
    hw_r_wrap       = ( reg_q_rsta & ( 0x1 << 12 ) ) >> 12;


    hw_write_index  = ( reg_q_wsta & ( 0xF << 8 ) ) >> 8;
    hw_w_wrap       = ( reg_q_wsta & ( 0x1 << 12 ) ) >> 12;


    if ( b_empty== 1 ) {
        desc_remain_count = 0;
    } else {
        if( hw_r_wrap == hw_w_wrap )    {
            desc_remain_count = hw_write_index - hw_read_index;
        } else  {
            desc_remain_count = MDP_CAMERA_PREVIEW_WORKING_BUFFER_CNT - hw_read_index + hw_write_index;
        }
    }


    MDPK_ZSDINFO("[ZSD] rsta 0x%08x, wsta 0x%08x, rdpa 0x%08x, rdva 0x%08x\n",
        (unsigned int)reg_q_rsta, (unsigned int)reg_q_wsta, (unsigned int)reg_q_base_pa, (unsigned int)reg_q_base_va);
    MDPK_ZSDINFO("[ZSD] empty %d, remain %d\n",
        b_empty, (int)desc_remain_count);



    for(q_index = 0 ; q_index < reg_q_depth ; q_index += 1)
    {
        descriptBase = (unsigned long *)(reg_q_base_va + q_index*7*4);
        if (pstReg->bStopAfterZSDDone == false)
        {
            descriptBase[0] = pstReg->u4RGB0Seg1[q_index];
        }
        else
        {
            descriptBase[0] = pstReg->u4RGB0Seg1[0];
        }
        descriptBase[1] = pstReg->u4RGB0Seg4;
        descriptBase[2] = pstReg->u4RGB0Seg5;
        descriptBase[3] = pstReg->u4RGB0Seg6;
        descriptBase[4] = pstReg->u4RGB0Seg7;
        descriptBase[5] = pstReg->u4RGB0Seg8;
        descriptBase[6] = pstReg->u4RGB0Seg9;

        MDPK_ZSDINFO("[ZSD] Descriptor(0x%08x) New -> 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x 0x%08x\n",
            (unsigned int)descriptBase,
            (unsigned int)descriptBase[0],
            (unsigned int)descriptBase[1],
            (unsigned int)descriptBase[2],
            (unsigned int)descriptBase[3],
            (unsigned int)descriptBase[4],
            (unsigned int)descriptBase[5],
            (unsigned int)descriptBase[6]);
    }

    pstReg->u4DescRemain = desc_remain_count;
    pstReg->u4DescCnt = reg_q_depth;
    pstReg->u4DescHwReadIndex = hw_read_index;
    pstReg->u4DescHwWriteIndex = hw_write_index;

    if (pstReg->bStopAfterZSDDone == TRUE)
    {
        //  >=2 means there is at least ONE descriptor in the queue NOT fetched by engine
        if (desc_remain_count >= 2)
        {

            // To make sure the next frame will be the last frame
            for (rd_adv_cnt = desc_remain_count; rd_adv_cnt > 2; rd_adv_cnt--)
            {
            mt65xx_reg_sync_writel(0x1, MT6577_ROT_DMA_RPT_ADVANCE_ADDR(RGB_ROT0_BASE));
            }

        }

        // =1 there is no descriptor in the queue, we need to queue a new one
        else if (desc_remain_count == 1)
        {
            // Queue a new descriptor
            MDPK_ZSDINFO("[ZSD] ROTDMA  remain %d desc, queue refill\n", (int)desc_remain_count);
            //DescQueueRefill(hw_write_index);
            DescQueueRefill(0);

        }

        else
        {
            // Queue new descriptor
            //DescQueueRefill(hw_write_index);
            //DescQueueRefill(hw_write_index == reg_q_depth-1 ? 0: hw_write_index++);
            DescQueueRefill(0);
            DescQueueRefill(0);

            MDPK_ZSDINFO("[ZSD] ROTDMA  remain %d desc, queue refill\n", (int)desc_remain_count);
        }
    }



    // Update CRZ

    u4RegVal = ioread32(MT6577_RZ_CFG_ADDR(CRZ_BASE));
    u4RegVal &= (~(0x3FF << 16));
    u4RegVal |= (pstReg->u4CRZLBMAX << 16);
    mt65xx_reg_sync_writel(u4RegVal , MT6577_RZ_CFG_ADDR(CRZ_BASE));
    mt65xx_reg_sync_writel(pstReg->u4CRZSrcSZ , MT6577_RZ_SRCSZ_ADDR(CRZ_BASE));
    mt65xx_reg_sync_writel(pstReg->u4CRZTarSZ , MT6577_RZ_TARSZ_ADDR(CRZ_BASE));
    mt65xx_reg_sync_writel(pstReg->u4CRZHRatio , MT6577_RZ_HRATIO_ADDR(CRZ_BASE));
    mt65xx_reg_sync_writel(pstReg->u4CRZVRatio , MT6577_RZ_VRATIO_ADDR(CRZ_BASE));
    mt65xx_reg_sync_writel(pstReg->u4CRZCropLR , MT6577_RZ_CROPLR_ADDR(CRZ_BASE));
    mt65xx_reg_sync_writel(pstReg->u4CRZCropTB , MT6577_RZ_CROPTB_ADDR(CRZ_BASE));


    mt65xx_reg_sync_writel(0x0, MT6577_RZ_LOCK_ADDR( CRZ_BASE ));
    mt65xx_reg_sync_writel(0x0, MT6577_ROT_DMA_LOCK_ADDR( RGB_ROT0_BASE ));

    return ret;

}



static int MT6577_SetZSDPreview(stZSDPreviewReg * a_u4Param)
{
    unsigned long   flag;

    spin_lock_irqsave(&g_stMT6577ZSDPreview.SpinLock , flag);
    g_stMT6577ZSDPreview.u4Dirty= 0;
    spin_unlock_irqrestore(&g_stMT6577ZSDPreview.SpinLock , flag);

    if(copy_from_user(&g_stMT6577ZSDPreview.stReg , (struct stZSDPreviewReg *)a_u4Param , sizeof(stZSDPreviewReg)))
    {
        MDPK_ERROR("[ZSD] MDP set ZSD Preview copy from user failed!!\n");
        return -EFAULT;
    }

    // Clear RGBROT0 interrupt table
    spin_lock_irqsave( &g_mdp_irq_tbl.spin_lock , flag );
    g_mdp_irq_tbl.irq_done_table &= ~MID_RGB_ROT0;
    spin_unlock_irqrestore(&g_mdp_irq_tbl.spin_lock , flag);

    /*
        Safty zone: between CRZ SOF and ROT done INT.
            In this zone, both the ROT descriptor and CRZ register will be updated in the next frame.
        Unsafty zone: between ROT done INT & CRZ SOF.
            In this zone, CRZ will updated new but ROT will not in next frame. Thus, we update these registers in CRZ SOF.
    */

    if (g_mdp_zsd_update_reg_safty_zone)
    {
        unsigned long y_cnt = (ioread32(MT6577_RZ_INFO0(CRZ_BASE))>>16) & 0x1FFF ;
        //unsigned long outcnt = ioread32(MT6575_RZ_INFO1(CRZ_BASE));
        unsigned long y_size = (ioread32(MT6577_RZ_SRCSZ_ADDR(CRZ_BASE))>>16) & 0xFFFF;

        if (y_cnt > y_size - 50)
        {
            spin_lock_irqsave(&g_stMT6577ZSDPreview.SpinLock , flag);
            g_stMT6577ZSDPreview.u4Dirty = 1;
            spin_unlock_irqrestore(&g_stMT6577ZSDPreview.SpinLock , flag);
            g_stMT6577ZSDPreview.stReg.u4DescUpdateZone = 0;
            MDPK_PRINTF("[ZSD] update zone unsafe y_cnt %d y_size %d, will update in CRZ SOF\n",
                (int)y_cnt, (int)y_size);

        }
        else
        {
            UpdateZsdPreviewReg();
            g_stMT6577ZSDPreview.stReg.u4DescUpdateZone = 1;
            MDPK_PRINTF("[ZSD] update zone safe, update immediately y_cnt %d y_size %d\n",
                (int)y_cnt, (int)y_size);
        }

    }
    else
    {
        spin_lock_irqsave(&g_stMT6577ZSDPreview.SpinLock , flag);
        g_stMT6577ZSDPreview.u4Dirty = 1;
        spin_unlock_irqrestore(&g_stMT6577ZSDPreview.SpinLock , flag);
        g_stMT6577ZSDPreview.stReg.u4DescUpdateZone = 0;
        MDPK_PRINTF("[ZSD] update zone unsafe, will update in CRZ SOF\n");
    }
    if(copy_to_user((struct stZSDPreviewReg *)a_u4Param, &g_stMT6577ZSDPreview.stReg,
                            sizeof(stZSDPreviewReg))){
        MDPK_ERROR("[ZSD] MDP set ZSD Preview  Copy to user failed !!\n");
        return -EFAULT;
    }
    return 0;

}

//This routine is executed in interrupt service routine content
static inline void UpdateZsdZoom(void)
{
    unsigned long u4RegVal = 0;
    unsigned long u4QueueBaseAddrRegVal = 0;
    unsigned long u4Index = 0;
    unsigned long * pu4DescriptBase = 0;
    unsigned long u4QDepth = 0;
    stZSDZoomReg * pstReg = &g_stMT6577ZSDZoom.stReg;

    if(g_stMT6577ZSDZoom.u4Dirty)
    {
        /*Check VDO1 SYSRAM validity*/
        u4QueueBaseAddrRegVal = ioread32(MT6577_ROT_DMA_QUEUE_BASE_ADDR(VDO_ROT1_BASE));
        if(u4QueueBaseAddrRegVal < MT6577_SYSRAM_PA)
        {
            MDPK_ERROR("VDOROT1 sysram base address is weird 0x%08X\n" , (unsigned int)u4QueueBaseAddrRegVal);
            return;
        }

        mt65xx_reg_sync_writel(1 , MT6577_RZ_LOCK_ADDR(CRZ_BASE));
        mt65xx_reg_sync_writel(1 , MT6577_RZ_LOCK_ADDR(PRZ0_BASE));
        mt65xx_reg_sync_writel(1 , MT6577_ROT_DMA_LOCK_ADDR(VDO_ROT1_BASE));

        //Config VRZ first, since it is most time critical
/*
        u4RegVal = ioread32(MT6577_RZ_STA_ADDR(VRZ0_BASE));
        if(u4RegVal)
        {
            printk("Error ! VRZ is still busy on something : %d\n" , u4RegVal);
            return;
        }
*/

        mt65xx_reg_sync_writel(pstReg->u4VRZSrcSZ , MT6577_RZ_SRCSZ_ADDR(VRZ0_BASE));
        mt65xx_reg_sync_writel(pstReg->u4VRZTarSZ , MT6577_RZ_TARSZ_ADDR(VRZ0_BASE));
        mt65xx_reg_sync_writel(pstReg->u4VRZHRatio , MT6577_RZ_HRATIO_ADDR(VRZ0_BASE));
        mt65xx_reg_sync_writel(pstReg->u4VRZVRatio , MT6577_RZ_VRATIO_ADDR(VRZ0_BASE));
        mt65xx_reg_sync_writel(pstReg->u4VRZHRes , MT6577_VRZ_HRES_ADDR(VRZ0_BASE));
        mt65xx_reg_sync_writel(pstReg->u4VRZVRes , MT6577_VRZ_VRES_ADDR(VRZ0_BASE));

        //Config CRZ
        u4RegVal = ioread32(MT6577_RZ_CFG_ADDR(CRZ_BASE));
        u4RegVal &= (~(0x3FF << 16));
        u4RegVal |= (pstReg->u4CRZLBMAX << 16);
        mt65xx_reg_sync_writel(u4RegVal , MT6577_RZ_CFG_ADDR(CRZ_BASE));
        mt65xx_reg_sync_writel(pstReg->u4CRZSrcSZ , MT6577_RZ_SRCSZ_ADDR(CRZ_BASE));
        mt65xx_reg_sync_writel(pstReg->u4CRZTarSZ , MT6577_RZ_TARSZ_ADDR(CRZ_BASE));
        mt65xx_reg_sync_writel(pstReg->u4CRZHRatio , MT6577_RZ_HRATIO_ADDR(CRZ_BASE));
        mt65xx_reg_sync_writel(pstReg->u4CRZVRatio , MT6577_RZ_VRATIO_ADDR(CRZ_BASE));
        mt65xx_reg_sync_writel(pstReg->u4CRZCropLR , MT6577_RZ_CROPLR_ADDR(CRZ_BASE));
        mt65xx_reg_sync_writel(pstReg->u4CRZCropTB , MT6577_RZ_CROPTB_ADDR(CRZ_BASE));

        //Config PRZ0
        u4RegVal = ioread32(MT6577_RZ_CFG_ADDR(PRZ0_BASE));
        u4RegVal &= (~(0x3FF << 16));
        u4RegVal |= (pstReg->u4PRZ0LBMAX << 16);
        mt65xx_reg_sync_writel(u4RegVal , MT6577_RZ_CFG_ADDR(PRZ0_BASE));
        mt65xx_reg_sync_writel(pstReg->u4PRZ0SrcSZ , MT6577_RZ_SRCSZ_ADDR(PRZ0_BASE));
        mt65xx_reg_sync_writel(pstReg->u4PRZ0TarSZ , MT6577_RZ_TARSZ_ADDR(PRZ0_BASE));
        mt65xx_reg_sync_writel(pstReg->u4PRZ0HRatio , MT6577_RZ_HRATIO_ADDR(PRZ0_BASE));
        mt65xx_reg_sync_writel(pstReg->u4PRZ0VRatio , MT6577_RZ_VRATIO_ADDR(PRZ0_BASE));


        //Config ROTDMA - modify sysram content
        u4QueueBaseAddrRegVal = ( SYSRAM_BASE + (u4QueueBaseAddrRegVal - MT6577_SYSRAM_PA) );//transfer to kernel virtual address
//        pu4DescriptBase = (unsigned long *)(u4RegVal);
        u4QDepth = ioread32(MT6577_ROT_DMA_CFG_ADDR(VDO_ROT1_BASE));
        u4QDepth = ((u4QDepth & 0xf00) >> 8);
        u4QDepth += 1;

#if 0
//Debug - S
printk("Q Addr : 0x%x\n" , u4RegVal);
printk("Q depth : %d\n" , u4QDepth);
printk("Before modify\n");
pu4DescriptBase = (unsigned long *)u4RegVal;
{
    for(u4Index = 0 ; u4Index < (u4QDepth*7) ; u4Index += 1)
    {
        printk("%lu\n" , pu4DescriptBase[u4Index]);
    }
}
//Debug - E
#endif

    for(u4Index = 0 ; u4Index < u4QDepth ; u4Index += 1)
        {
            pu4DescriptBase = (unsigned long *)(u4QueueBaseAddrRegVal + u4Index*7);
            pu4DescriptBase[3] = pstReg->u4VDO1Seg4;
            pu4DescriptBase[4] = pstReg->u4VDO1Seg5;
            pu4DescriptBase[5] = pstReg->u4VDO1Seg6;
            pu4DescriptBase[6] = pstReg->u4VDO1Seg7;
        }

#if 0
//Debug - S
printk("After modify\n");
pu4DescriptBase = (unsigned long *)u4RegVal;
{
    for(u4Index = 0 ; u4Index < (u4QDepth*7) ; u4Index += 1)
    {
        printk("%lu\n" , pu4DescriptBase[u4Index]);
    }
}
//Debug - E
#endif
        mt65xx_reg_sync_writel(0 , MT6577_ROT_DMA_LOCK_ADDR(VDO_ROT1_BASE));
        mt65xx_reg_sync_writel(0 , MT6577_RZ_LOCK_ADDR(PRZ0_BASE));
        mt65xx_reg_sync_writel(0 , MT6577_RZ_LOCK_ADDR(CRZ_BASE));
        g_stMT6577ZSDZoom.u4Dirty = 0;
//Debug - S
//printk("ZSDZoom\n");
//Debug - E
    }
}

// last preview frame for zsd
static inline void UpdateZsdPreviewFrame(void)
{
    if(g_stMT6577ZSDPreview.u4Dirty)
    {
        //MDPK_PRINTF("[ZSD ]Update reg in CRZ SOF\n");
        UpdateZsdPreviewReg();
        g_stMT6577ZSDPreview.u4Dirty = 0;
    }
}

int _IsMdpClockOn( unsigned long mdp_id )
{
    int ret_is_on = 0;
    int bit     = -1;
    int bit2    = -1;
    unsigned long reg_val;
    
    switch( mdp_id )
    {
        case MID_JPEG_DEC:
            bit=27;break;
        case MID_JPEG_ENC:
        	bit=28;break;
        case MID_CAM     :
        	bit=31;break;
        case MID_BRZ     :
        	bit=26;bit2=25;break;
        case MID_CRZ     :
        	bit=23;break;
        case MID_PRZ0    :
        	bit=14;bit2=13;break;
        case MID_PRZ1    :
        	bit=7;break;
        case MID_VRZ0    :
        	bit=11;break;
        case MID_VRZ1    :
        	bit=0;break;
        case MID_R_DMA0  :
        	bit=30;bit2=29;break;
        case MID_R_DMA1  :
        	bit=8;break;
        case MID_OVL     :
        	bit=19;break;
        case MID_IPP     :
        	bit=21;bit2=20;break;
        case MID_MOUT    :
        	bit=16;break;
        case MID_LCD     :
        	bit=5;break;
        case MID_JPEG_DMA:
        	bit=24;break;
        case MID_EIS     :
        	bit=22;break;
        case MID_RGB_ROT0:
        	bit=12;break;
        case MID_RGB_ROT1:
        	bit=10;break;
        case MID_RGB_ROT2:
        	bit=6;break;
        case MID_VDO_ROT0:
        	bit=15;break;
        case MID_VDO_ROT1:
        	bit=9;break;
        case MID_TV_ROT  :
        	bit=4;break;
        default:
            MDPK_ERROR("Unknown mdp_id = 0x%08X\n", (unsigned int)mdp_id);
            return -1;
    }

    
    //MT6575_MMSYS_CG_CON1
    reg_val = ioread32( MMSYS1_CG_CON1_ADDR );

    if( (reg_val & ( 0x1 << bit )) == 0 )
    {
        ret_is_on = 1;
    } else
    {
        ret_is_on = 0;
    }

    return ret_is_on;

}
    


static /*__tcmfunc*/ int MdpIsr_ROT( unsigned long mdp_id )
{
    int ret = 0;
    unsigned long   pBase;
    unsigned long   reg_val;
    int             rot_index;
    unsigned long   drop_counter;
    unsigned long   flag;

    static unsigned long _lcd_too_slow_show_frequency = 0;
    
    

    switch( mdp_id )
    {
        case MID_RGB_ROT0:
            MDPK_ISRINFO("{RGB_ROT0 INT}\n");   
            MDPK_ZSDINFO("3. RGB_ROT0 Done\n");
            pBase = RGB_ROT0_BASE;
            rot_index = 0;
            break;
        case MID_RGB_ROT1:
            MDPK_ISRINFO("{RGB_ROT1 INT}\n");   
            pBase = RGB_ROT1_BASE;
            rot_index = 1;
            break;
        case MID_RGB_ROT2:
            MDPK_ISRINFO("{RGB_ROT2 INT}\n");   
            pBase = RGB_ROT2_BASE;
            rot_index = 2;
            break;
        
        case MID_VDO_ROT0:
            MDPK_ISRINFO("{VDO_ROT0 INT}\n");   
            pBase = VDO_ROT0_BASE;
            rot_index = 3;
            break;
        case MID_VDO_ROT1:
            MDPK_ISRINFO("{VDO_ROT1 INT}\n");   
            pBase = VDO_ROT1_BASE;
            rot_index = 4;
            break;
        
        default:
            MDPK_ERROR("Invalid INT.ROT ID:0x%08X\n", (unsigned int)mdp_id);
            ret = -1;
            return ret;
    }

    #if 0
    {
    unsigned long   desc_exec_counter;
    unsigned long   rpt;

    //Debug:Show Exec Counter
    desc_exec_counter = ioread32( MT6577_ROT_DMA_EXEC_CNT_ADDR(pBase) );
    MDPK_INFO("ROT exe counter = %d\n", desc_exec_counter );

    
    //Debug:Show RPT 
    rpt = ioread32( MT6577_ROT_DMA_QUEUE_RSTA_ADDR(pBase) );
    MDPK_INFO("ROT rpt counter = %d\n", ( rpt & ( 0xF << 8 ) ) >> 8 );
    }
    #endif

    
        

    

    

    //Check error status as well
    reg_val = ioread32( MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );

    //Clear interrupt
    mt65xx_reg_sync_writel( ( 0x33 & reg_val ), MT6577_ROT_DMA_IRQ_FLAG_CLR_ADDR(pBase) );
    
    
    reg_val &= 0xFF;

    /*Done*/
    if( reg_val & 0x1 )
    {
        //Show RGB0 Interrupt period
        if( mdp_id == MID_RGB_ROT0 )
        {
            struct timeval  tv;
            static unsigned long   last_time_us = 0;
            unsigned long   current_time_us;
            unsigned long   elapse_time_us;
            
            do_gettimeofday(&tv);
            
            current_time_us = tv.tv_sec*1000000 + tv.tv_usec;
            elapse_time_us = current_time_us - last_time_us;
            last_time_us = current_time_us;

            MDPK_FPSINFO("RGB0 Int period:%8dus\n", (int)elapse_time_us );
        }

            
        if( g_mdp_irq_tbl.rot_exec_count[rot_index] != ioread32( MT6577_ROT_DMA_EXEC_CNT_ADDR(pBase) ) )
        {
            g_mdp_irq_tbl.rot_exec_count[rot_index] = ioread32( MT6577_ROT_DMA_EXEC_CNT_ADDR(pBase) );

            spin_lock_irqsave( &g_mdp_irq_tbl.spin_lock , flag );
            g_mdp_irq_tbl.irq_done_table |= mdp_id;//complete interrupt
            spin_unlock_irqrestore(&g_mdp_irq_tbl.spin_lock , flag);

            /*Update time stamp*/
            MT6577_TimeStamp( mdp_id, 0, NULL );
            
            if( mdp_id == MID_VDO_ROT1 ){
                //Let's update zoom setting here!!
                UpdateZsdZoom();
            }
            //if ( mdp_id == MID_RGB_ROT0 ) {
                //g_mdp_zsd_update_reg_safty_zone = 0;
            //}
        }
        else
        {
            if(mdp_id == MID_RGB_ROT0)
            {
                drop_counter = ioread32( MT6577_ROT_DMA_DROPPED_FRAME_CNT_ADDR(pBase) );
                //OSS3 request not to print per frame even this condition is incorrect!!
                if((50 > drop_counter) || ((1000 < drop_counter) && (0 == (drop_counter & 0x3FF))))
                    MDPK_WARNING("\tINT:ROTDMA(0x%08X) totally drop %d frame\n" , (unsigned int)mdp_id, (int)drop_counter);
            }
        }
        
        /*.............................................................................
            FPS Calculate
          .............................................................................*/

        g_mdp_irq_tbl.fps_int_count[rot_index]++;
        if( g_mdp_irq_tbl.fps_int_count[rot_index] == 30 /*FPS_CALC_PERIOD*/ )
        {
            struct timeval  tv;
            unsigned long   elapse_time_in_ms;
            unsigned long   elapse_frame;

            do_gettimeofday(&tv);

            elapse_time_in_ms = ( tv.tv_sec - g_mdp_irq_tbl.fps_last_time[rot_index].tv_sec )*1000 +
                                ( tv.tv_usec- g_mdp_irq_tbl.fps_last_time[rot_index].tv_usec )/1000;

            elapse_frame = g_mdp_irq_tbl.rot_exec_count[rot_index] - g_mdp_irq_tbl.fps_last_rot_exec_count[rot_index];

            //FPS
            if( elapse_time_in_ms != 0 ){
            g_mdp_irq_tbl.fps_fps_number[rot_index] =  ( 1000*elapse_frame/elapse_time_in_ms );
            }else
            {
                g_mdp_irq_tbl.fps_fps_number[rot_index] =  0;
            }
            

            //Update fps number
            g_mdp_irq_tbl.fps_int_count[rot_index]           = 0;
            g_mdp_irq_tbl.fps_last_time[rot_index]           = tv;
            g_mdp_irq_tbl.fps_last_rot_exec_count[rot_index] = g_mdp_irq_tbl.rot_exec_count[rot_index];
            
                
            
        }

        g_mdp_irq_tbl.fps_show_info_period++;
        if( (g_mdp_irq_tbl.fps_show_info_period & 0xFF) == 0 )
        {
            MDPK_FPSINFO("RGB_ROT0 FPS = %lu f/s\n",g_mdp_irq_tbl.fps_fps_number[0] );
            MDPK_FPSINFO("RGB_ROT1 FPS = %lu f/s\n",g_mdp_irq_tbl.fps_fps_number[1] );
            MDPK_FPSINFO("RGB_ROT2 FPS = %lu f/s\n",g_mdp_irq_tbl.fps_fps_number[2] );
            MDPK_FPSINFO("VDO_ROT0 FPS = %lu f/s\n",g_mdp_irq_tbl.fps_fps_number[3] );
            MDPK_FPSINFO("VDO_ROT1 FPS = %lu f/s\n",g_mdp_irq_tbl.fps_fps_number[4] );
        }
    }


    /*SW Config error*/
    if( reg_val & 0x2 )
    {
        MDPK_ERROR("\tINT:ROTDMA(0x%08X) SW conf error.Invalid desc mode.\n" , (unsigned int)mdp_id );
        ret = -1;
    }

    
    /*LCD too slow*/
    if( reg_val & 0x10 )
    {
        if( (_lcd_too_slow_show_frequency++%300) == 0 ) {
            MDPK_ERROR("\tINT:ROTDMA(0x%08X) LCD too slow...(%lu)\n", (unsigned int)mdp_id, _lcd_too_slow_show_frequency );
        }
        ret = -1;
    }

    /*SW config error. write to descriptor wehn busy*/
    if( reg_val & 0x20 )
    {    
        MDPK_ERROR("\tINT:ROTDMA(0x%08X) SW conf error.write to descriptor wehn busy.\n" , (unsigned int)mdp_id );
        ret = -1;
    }

    
    return ret;
}

static /*__tcmfunc*/ int MdpIsr_JPEGDMA( void )
{
    int ret = 0;
    unsigned long pBase;
    unsigned long reg_val;
    unsigned long flag;
    
    MDPK_ISRINFO("{JPEG DMA INT}\n");

    pBase = JPG_DMA_BASE;

    reg_val = ioread32( MT6577_JPEG_DMA_INTERRUPT_ADDR( pBase ) );
    
    /*Done*/
    if(reg_val & 0x1)
    {
        /*Clear*/
        mt65xx_reg_sync_writel( 0, MT6577_JPEG_DMA_INTERRUPT_ADDR( pBase ) );

        spin_lock_irqsave( &g_mdp_irq_tbl.spin_lock , flag );
        g_mdp_irq_tbl.irq_done_table |= MID_JPEG_DMA;    
        spin_unlock_irqrestore(&g_mdp_irq_tbl.spin_lock , flag);
    }
    
    return ret;
}

static /*__tcmfunc*/ int MdpIsr_OVL( void )
{
    int ret = 0;
    unsigned long pBase;
    unsigned long reg_val;
    unsigned long   flag;
    
    MDPK_ISRINFO("{OVL INT}\n");

    pBase = OVL_DMA_BASE;

    reg_val = ioread32( MT6577_OVL_DMA_IRQ_FLAG_ADDR( pBase ) );

    //Clear interrupt
    mt65xx_reg_sync_writel( ( 0x33 & reg_val ), MT6577_OVL_DMA_IRQ_FLAG_CLR_ADDR(pBase) );

    reg_val &= 0x33;

    /*Done*/
    if( reg_val & 0x1 )
    {
        spin_lock_irqsave( &g_mdp_irq_tbl.spin_lock , flag );
        g_mdp_irq_tbl.irq_done_table |= MID_OVL;//complete interrupt
        spin_unlock_irqrestore(&g_mdp_irq_tbl.spin_lock , flag);
    }


    /*SW Config error*/
    if( reg_val & 0x2 )
    {
        MDPK_ERROR("OVL SW conf error.Invalid desc mode.\n");
        //printk("OVL SW conf error.Invalid desc mode.\n");
        ret = -1;
    }


    /*SW Config error:errorneous mask size*/
    if( reg_val & 0x10 )
    {
        MDPK_ERROR("OVL SW conf error.Errorneous mask size.\n");
        //printk("OVL SW conf error.Errorneous mask size.\n");
        ret = -1;
    }

    /*SW config error. write to descriptor wehn busy*/
    if( reg_val & 0x20 )
    {    
        MDPK_ERROR("OVL SW conf error.Write desc when busy.\n");
        //printk("OVL SW conf error.Write desc when busy.\n");
        ret = -1;
    } 
    
    return ret;
}

static /*__tcmfunc*/ int MdpIsr_RDMA( unsigned long mdp_id )
{
    int ret = 0;
    unsigned long pBase;
    unsigned long reg_val;
    unsigned long   flag;
    
  
    switch( mdp_id )
{
        case MID_R_DMA0:
            MDPK_ISRINFO("{RDMA0 INT}\n");
            pBase = R_DMA0_BASE;
            break;
        case MID_R_DMA1:
            MDPK_ISRINFO("{RDMA1 INT}\n");
            pBase = R_DMA1_BASE;
            break;
        default:
            MDPK_ERROR("Invalid INT.RDMA ID:0x%08X\n", (unsigned int)mdp_id);
            ret = -1;
            return ret;
    }
    #if 0
    {
        unsigned long desc_exec_counter;
        unsigned long rpt;
        
        //Debug:Show Exec Counter
        desc_exec_counter = ioread32( MT6577_RDMA_EXEC_CNT_ADDR(pBase) );
        MDPK_INFO("RDMA exe counter = %d\n", desc_exec_counter );

        
        //Debug:Show RPT
        rpt = ioread32( MT6577_RDMA_QUEUE_RSTA_ADDR(pBase) );
        MDPK_INFO("RDMA rpt counter = %d\n", ( rpt & ( 0xF << 8 ) ) >> 8 );
    }
    #endif


    
    //Check error status as well
    reg_val = ioread32( MT6577_RDMA_IRQ_FLAG_ADDR(pBase) );
    //Clear interrupt 
    mt65xx_reg_sync_writel( (0x23 & reg_val), MT6577_RDMA_IRQ_FLAG_CLR_ADDR(pBase) );

    reg_val &= 0x23;

    
    if(reg_val & 0x1)
    {
        spin_lock_irqsave( &g_mdp_irq_tbl.spin_lock , flag );
        g_mdp_irq_tbl.irq_done_table |= mdp_id;
        spin_unlock_irqrestore(&g_mdp_irq_tbl.spin_lock , flag);

    } /*Done*/

    if(reg_val & 0x2)
    {     
        MDPK_ERROR("SW configuration error. Invalid descriptor mode\n");
        //printk("SW configuration error. Invalid descriptor mode\n");
        ret = -1;
    }

    if(reg_val & 0x20)
    {    
        MDPK_ERROR("SW configuration error. SW writing to descriptor queue when command queue writer is busy\n");
        //printk("SW configuration error. SW writing to descriptor queue when command queue writer is busy\n");
        ret = -1;
    }

    return ret;
    
}


static /*__tcmfunc*/ int MdpIsr_BRZ( void )
{
    int ret = 0;
    unsigned long pBase;
    unsigned long reg_val;
    unsigned long   flag;

    MDPK_ISRINFO("{BRZ INT}\n");

    pBase = BRZ_BASE;

    reg_val = ioread32( MT6577_BRZ_INT_ADDR( pBase ) );

    
    if( _IsMdpClockOn( MID_BRZ ) != 1 )
    {
        MDPK_ERROR("0x%08X clock is gating and still try to clear interrupt BRZ_INT = 0x%08X!!\n", 
            (unsigned int)MID_BRZ,  (unsigned int)reg_val );
    }
    

    /*Done*/
    if(reg_val)
    {
        /*Clear*/
        mt65xx_reg_sync_writel( ( reg_val | 0x03 ), MT6577_BRZ_INT_ADDR( pBase ) );

        spin_lock_irqsave( &g_mdp_irq_tbl.spin_lock , flag );
        g_mdp_irq_tbl.irq_done_table |= MID_BRZ;    
        spin_unlock_irqrestore(&g_mdp_irq_tbl.spin_lock , flag);
    }

    //Debug
    {
        #define THRESHOLD (50000)
        
        unsigned long long         current_ns;
        static unsigned long long  lt_brz  = 0;
        static int b_en_regdump = 0;
        static int int_count = 0;
        static unsigned long long   start_time = 0;
        
        current_ns = cpu_clock(0);     //ns
        
        if( b_en_regdump == 1 )
        {
            int_count++;
        
            if( (current_ns - start_time) > 200000000 ) /*200ms*/
            {
                MDPK_DEBUG("{BRZ hort INT}:End count. int_count(%d-%d) =  %d\n", (int)start_time,(int)current_ns,int_count);
                
                if( int_count > 200 )
                {
                    MT6577MDP_DUMPREG();
                } 
                int_count= 0;
                b_en_regdump = 0;
                start_time = 0;
            }
        }
        
        if( ( current_ns - lt_brz ) < THRESHOLD )
        {
            MDPK_DEBUG("{BRZ hort INT}:0x%08X\n",reg_val);

            if( (int_count== 0 ) && ( b_en_regdump == 0 ) )
            {
                MDPK_DEBUG("{BRZ hort INT}:Start count\n");
                b_en_regdump = 1;
                start_time = current_ns;
            }


        }
        lt_brz = current_ns;
        
    }

    return ret;
}




#define ABS(a , b) (a > b ? (a-b) : (b-a))
static /*__tcmfunc*/ int MdpIsr_RESZ( unsigned long mdp_id )
{
    int ret = 0;
    unsigned long pBase;
    unsigned long reg_val;
    unsigned long   flag;
    

    switch( mdp_id )
    {
        case MID_CRZ:
            MDPK_ISRINFO("{CRZ INT}\n");   
            pBase = CRZ_BASE;
            break;
        case MID_PRZ0:
            MDPK_ISRINFO("{PRZ0 INT}\n");   
            pBase = PRZ0_BASE;
            break;
        case MID_PRZ1:
            MDPK_ISRINFO("{PRZ1 INT}\n");   
            pBase = PRZ1_BASE;
            break;
        default:
            MDPK_ERROR("Invalid INT.RESZ ID:0x%08X\n", (unsigned int)mdp_id);
            //printk("Invalid INT.RESZ ID:0x%08X\n", (unsigned int)mdp_id);
            ret = -1;
            return ret;
    }


    /*Read Clear*/
    reg_val = ioread32( MT6577_RZ_INT_ADDR(pBase) );

    //DEBUG
    #if 1
    {
        #define THRESHOLD (50000) /*50us*/
        unsigned long long         current_ns;
        static unsigned long long  lt_crz  = 0;
        static unsigned long long  lt_prz0 = 0;
        static unsigned long long  lt_prz1 = 0; 
        static int b_en_regdump = 0;
        static int int_count = 0;
        static unsigned long long   start_time = 0;

        current_ns = cpu_clock(0);     //ns

        if( b_en_regdump == 1 )
        {
            int_count++;
        
            if( (current_ns - start_time) > 200000000 ) /*200ms*/
            {
                MDPK_DEBUG("{RESZ INT}:End count. int_count(%d-%d) =  %d\n", (int)start_time,(int)current_ns,int_count);
                
                if( int_count > 200 )
                {
                    MT6577MDP_DUMPREG();
                } 
                int_count= 0;
                b_en_regdump = 0;
                start_time = 0;
                
                
            }
        }
        
        switch( mdp_id )
        {
            case MID_CRZ:
                if( ( current_ns - lt_crz ) < THRESHOLD )
                {
                    MDPK_DEBUG("{CRZ Short INT}:0x%08X\n",reg_val);

                    if( (int_count== 0 ) && ( b_en_regdump == 0 ) )
                    {
                        MDPK_DEBUG("{CRZ INT}:Start count\n");
                        b_en_regdump = 1;
                        start_time = current_ns;
                    }


                    //MT6577MDP_DUMPREG();
                }
                lt_crz = current_ns;
                break;
            case MID_PRZ0:
                if( ( current_ns - lt_prz0 ) < THRESHOLD ){
                    MDPK_DEBUG("{PRZ0 Short INT}:0x%08X\n",reg_val);  

                    if( (int_count== 0 ) && ( b_en_regdump == 0 ) )
                    {
                        MDPK_DEBUG("{PRZ0 INT}:Start count\n");
                        b_en_regdump = 1;
                        start_time = current_ns;
                    }
                
                }
                lt_prz0 = current_ns;
                break;
            case MID_PRZ1:
                if( ( current_ns - lt_prz1 ) < THRESHOLD )
                {
                    MDPK_DEBUG("{PRZ1 Short INT}:0x%08X\n",reg_val);  

                    if( (int_count== 0 ) && ( b_en_regdump == 0 ) )
                    {
                        MDPK_DEBUG("{PRZ1 INT}:Start count\n");
                        b_en_regdump = 1;
                        start_time = current_ns;
                    }

                }
                lt_prz1 = current_ns;
                break;
        }

    }
    #endif

    
    if( _IsMdpClockOn( mdp_id ) != 1 )
    {
        MDPK_ERROR("0x%08X clock is gating and still try to clear interrupt RZ_INT = 0x%08X!!\n", 
            (unsigned int)mdp_id,  (unsigned int)reg_val );
    }


    /*Frame End.Done*/
    if(reg_val & 0x1)
    {
        MDPK_ISRINFO("\tINT:Frame End\n");   
        if( mdp_id == MID_CRZ )
		{
            g_mdp_zsd_update_reg_safty_zone = 0;
			MDPK_ZSDINFO("2. CRZ:Frame END\n");
		}

        spin_lock_irqsave( &g_mdp_irq_tbl.spin_lock , flag );
        g_mdp_irq_tbl.irq_done_table |= mdp_id;
        spin_unlock_irqrestore(&g_mdp_irq_tbl.spin_lock , flag);
        

        //Debug - S
        #if 0
        if( mdp_id == MID_CRZ )
        {
        printk("CRZ-E\n");
        }

        if( mdp_id == MID_PRZ0 )
        {
        printk("PRZ0-E\n");
        }
        #endif
        //Debug - E

    }

    /*Frame Start.TODO:ZOOM*/
    if(reg_val & 0x2)
    {
        MDPK_ISRINFO("\tINT:Frame Start\n"); 
        
        /*TODO:ZOOM*/
        if( mdp_id == MID_CRZ )
        {
        	MDPK_ZSDINFO("1. CRZ:Frame Start\n");
            g_mdp_zoom_param.frame_counter += 1;

            //Debug - S
            //printk("CRZ-S\n");
            //Debug - E
                
            if(1 == g_mdp_zoom_param.is_dirty && (1 < ABS(g_mdp_zoom_param.frame_counter, g_mdp_zoom_param.applied_frame)))
            {
                //MDPK_ZOOMINFO("Apply CRZ Register!\n");
                //Frame start
                mt65xx_reg_sync_writel(1 , MT6577_RZ_LOCK_ADDR(pBase));
                mt65xx_reg_sync_writel(g_mdp_zoom_param.CRZ_REG.reg_CFG , MT6577_RZ_CFG_ADDR(pBase));
                mt65xx_reg_sync_writel(g_mdp_zoom_param.CRZ_REG.reg_SRCSZ , MT6577_RZ_SRCSZ_ADDR(pBase));
                mt65xx_reg_sync_writel(g_mdp_zoom_param.CRZ_REG.reg_CROPLR , MT6577_RZ_CROPLR_ADDR(pBase));
                mt65xx_reg_sync_writel(g_mdp_zoom_param.CRZ_REG.reg_CROPTB , MT6577_RZ_CROPTB_ADDR(pBase));
                mt65xx_reg_sync_writel(g_mdp_zoom_param.CRZ_REG.reg_HRATIO , MT6577_RZ_HRATIO_ADDR(pBase));
                mt65xx_reg_sync_writel(g_mdp_zoom_param.CRZ_REG.reg_VRATIO , MT6577_RZ_VRATIO_ADDR(pBase));
                mt65xx_reg_sync_writel(0 , MT6577_RZ_LOCK_ADDR(pBase) );
                g_mdp_zoom_param.applied_frame = g_mdp_zoom_param.frame_counter;
                g_mdp_zoom_param.is_dirty= 0;
                
            }
            //last preview frame for zsd
            g_mdp_zsd_update_reg_safty_zone = 1;
            UpdateZsdPreviewFrame();
        }


        //Let's update zoom setting here!!
        //if( mdp_id == MID_PRZ0 ){
        //Debug - S
        //printk("PRZ0-S\n");
        //Debug - E
        //UpdateZsdZoom();
        //}

    }

    /*Check Error bit*/
    if( mdp_id == MID_CRZ )
    {
        unsigned long crz_sta_val;

        /*ERRINT[0]*/
        if(reg_val & 0x10)
        {     
            MDPK_ERROR("\tINT:CRZ Input not enough (Vsync error).\n");
            //printk("\tINT:CRZ Input not enough (Vsync error).\n");
            ret = -1;
        }
        
        /*ERRINT[1]*/
        if(reg_val & 0x20)
        {     
            MDPK_ERROR("\tINT:CRZ Output too slowh (Frame Sync error).\n");
            //printk("\tINT:CRZ Output too slowh (Frame Sync error).\n");
            ret = -1;
        }

        /*Read CRZ_STA*/
        crz_sta_val = ioread32( MT6577_RZ_STA_ADDR(pBase) );

        if( crz_sta_val & (0x1<<3) )    {     
            MDPK_ERROR("\tINT:CRZ EIS error\n");
            //printk("\tINT:CRZ EIS error\n");
            ret = -1;
        }

        if( crz_sta_val & (0x1<<4) )    {     
            MDPK_ERROR("\tINT:CRZ ERR[0] input pixel is not enough\n");
            //printk("\tINT:CRZ ERR[0] input pixel is not enough\n");
            ret = -1;
        }

        if( crz_sta_val & (0x1<<5) )    {     
            MDPK_ERROR("\tINT:CRZ ERR[1] Output too slow\n");
            //printk("\tINT:CRZ ERR[1] Output too slow\n");
            ret = -1;
        }

        if( crz_sta_val & (0x1<<6) )    {     
            MDPK_ERROR("\tINT:CRZ ERR[2] Init signal at output stage\n");
            //printk("\tINT:CRZ ERR[2] Init signal at output stage\n");
            ret = -1;
        }

        if( crz_sta_val & (0x1<<7) )    {     
            MDPK_ERROR("\tINT:CRZ ERR[3] Init signal at input stage\n");
            //printk("\tINT:CRZ ERR[3] Init signal at input stage\n");
            ret = -1;
        }

        /*Clear*/
        mt65xx_reg_sync_writel( (crz_sta_val&0xF8) , MT6577_RZ_STA_ADDR(pBase));

        
    }
    
    return ret;
}

static /*__tcmfunc*/ int MdpIsr_VRZ( unsigned long mdp_id )
{
    int ret = 0;
    unsigned long pBase;
    unsigned long reg_val;
    unsigned long   flag;
    

    switch( mdp_id )
    {
    case MID_VRZ0:
        MDPK_ISRINFO("{VRZ0 INT}\n");   
        pBase = VRZ0_BASE;
        break;
    case MID_VRZ1:
        MDPK_ISRINFO("{VRZ1 INT}\n");   
        pBase = VRZ1_BASE;
        break;
    default:
        MDPK_ERROR("Invalid INT.VRZ ID:0x%08X\n", (unsigned int)mdp_id);
        //printk("Invalid INT.VRZ ID:0x%08X\n", (unsigned int)mdp_id);
        ret = -1;
        return ret;
    }

    /*Read Clear*/
    reg_val = ioread32( MT6577_VRZ_INT_ADDR(pBase) );

    /*Done*/
    if(reg_val & 0x1)
    {
        spin_lock_irqsave( &g_mdp_irq_tbl.spin_lock , flag );
        g_mdp_irq_tbl.irq_done_table |= mdp_id;    
        spin_unlock_irqrestore(&g_mdp_irq_tbl.spin_lock , flag);
    }
    

    return ret;

    
}



static /*__tcmfunc*/ irqreturn_t MdpIsr_Main(int irq, void *dev_id)
{
    unsigned long reg_val;
    
    //MDPK_ISRINFO("\n");
    //MDPK_ISRINFO("INT(%d) happen!\n\r", irq );
    

    /*1. Process ISR*/
    switch( irq )
    {
        case MT6577_CRZ_IRQ_ID://CRZ
            MdpIsr_RESZ( MID_CRZ );
            break;

        case MT6577_ROT_DMA_IRQ_ID://RGB0/RGB1/VDO0/VDO1
            reg_val = ioread32( MMSYS1_ROT_DMA_IRQ_FLAG );
            if( reg_val & (0x1 << 0) )  MdpIsr_ROT( MID_VDO_ROT1 );
            if( reg_val & (0x1 << 1) )  MdpIsr_ROT( MID_RGB_ROT1 );
            if( reg_val & (0x1 << 2) )  MdpIsr_ROT( MID_RGB_ROT0 );
            if( reg_val & (0x1 << 3) )  MdpIsr_ROT( MID_VDO_ROT0 );
            break;
            
        case MT6577_OVL_JPG_DMA_IRQ_ID://OVL/JPEG DMA
            reg_val = ioread32( MMSYS1_OVL_JPG_DMA_IRQ_FLAG );
            if( reg_val & (0x1 << 0) )  MdpIsr_JPEGDMA();
            if( reg_val & (0x1 << 1) )  MdpIsr_OVL();
            break;
        
        case MT6577_R_DMA0_IRQ_ID://RDMA0
            MdpIsr_RDMA( MID_R_DMA0 );
            break;
            
        case MT6577_RESZ_IRQ_ID://PRZ0/VRZ0/BRZ
            reg_val = ioread32( MMSYS1_RESZ_IRQ_FLAG );
            if( reg_val & (0x1 << 0) )  MdpIsr_BRZ();
            if( reg_val & (0x1 << 1) )  MdpIsr_RESZ( MID_PRZ1 );
            if( reg_val & (0x1 << 2) )  MdpIsr_RESZ( MID_PRZ0 );
            if( reg_val & (0x1 << 3) )  MdpIsr_VRZ( MID_VRZ0 );
            break;
            
        case MT6577_DISP_MDP_IRQ_ID://RDMA1/VRZ1/RGB2
            reg_val = ioread32( MMSYS1_DISP_MDP_IRQ_FLAG );
            if( reg_val & (0x1 << 0) )  MdpIsr_ROT( MID_RGB_ROT2 );
            if( reg_val & (0x1 << 1) )  MdpIsr_VRZ( MID_VRZ1 );
            if( reg_val & (0x1 << 2) )  MdpIsr_RDMA( MID_R_DMA1 );
            break;


        default:
            MDPK_ERROR("Unknown IRQ:%d\n\r", irq);
            //printk("Unknown IRQ:%d\n\r", irq);
            break;
    }

    /*2.Wakeup event*/
    mb();   /*Add memory barrier before the other CPU (may) wakeup*/
    wake_up_interruptible(&g_mdp_irq_done_queue);

    return IRQ_HANDLED;
}


#define _REQUEST_IRQ( _IRQ_ ){\
    errno = request_irq( _IRQ_ , (irq_handler_t)MdpIsr_Main, IRQF_TRIGGER_LOW, p_Name , NULL);\
    if( errno != 0 ) {\
        MDPK_ERROR("Request IRQ:" #_IRQ_ " failed.errorno=%d\n",errno);\
        ret = -ENODEV; }\
    else {\
        MDPK_INFO("Request IRQ:" #_IRQ_ " OK.\n"); } }

static int MdpIsr_Register( const char *p_Name )
{
    int ret = 0;
    int errno;

    /*Initial variable before request irq*/
    
        

    /*Request irq.After request irq system will rise irq at any time*/
    _REQUEST_IRQ(MT6577_CRZ_IRQ_ID);
    _REQUEST_IRQ(MT6577_ROT_DMA_IRQ_ID);
    _REQUEST_IRQ(MT6577_OVL_JPG_DMA_IRQ_ID);
    _REQUEST_IRQ(MT6577_R_DMA0_IRQ_ID);
    _REQUEST_IRQ(MT6577_RESZ_IRQ_ID);
    _REQUEST_IRQ(MT6577_DISP_MDP_IRQ_ID);

    return ret;
    
}

#endif /*~MDP_KERNEL_FLAG_1_SUPPORT_INT*/
/*=============================================================================
    IOCTRL Operation
  =============================================================================*/

//return 1 if locked, 0 if not locked.
static int MT6577_TryLockResource(unsigned long request_resource, int b_timeshare , stLockResParam*  p_stParam )
{
    unsigned long irq_state;
    
    spin_lock_irqsave(&g_mdp_resource_tbl.spin_lock, irq_state );

    if( g_mdp_resource_tbl.resource_in_use_table & request_resource )
    {
        p_stParam->out_Table_status_in_use         = g_mdp_resource_tbl.resource_in_use_table;
        p_stParam->out_Table_status_in_occupied    = g_mdp_resource_tbl.resource_in_occupied_table;

        spin_unlock_irqrestore(&g_mdp_resource_tbl.spin_lock, irq_state );
        return 0;//Return 0 : lock failed
    }
    else
    {
        g_mdp_resource_tbl.resource_in_use_table |= request_resource;

        if( !b_timeshare )
        {
            g_mdp_resource_tbl.resource_in_occupied_table |= request_resource;
        }
    }

    p_stParam->out_Table_status_in_use         = g_mdp_resource_tbl.resource_in_use_table;
    p_stParam->out_Table_status_in_occupied    = g_mdp_resource_tbl.resource_in_occupied_table;

    spin_unlock_irqrestore(&g_mdp_resource_tbl.spin_lock, irq_state );

    return 1;//Return 1 : lock success
}

static int PowerManagement_RestInt( unsigned long mdp_id )
{
    unsigned long pBase;
    unsigned long reg_val;


    switch ( mdp_id )
    {
        
        case MID_BRZ:
            pBase = BRZ_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_BRZ_INT_ADDR( pBase ) );
            mt65xx_reg_sync_writel( 0x0, MT6577_BRZ_INT_ADDR( pBase ) );//write 0x0 also clear INT
            //2.Clear INT
            //(Already clear)
            break;
        case MID_CRZ:
            pBase = CRZ_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_RZ_CFG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x3 << 10) );
            mt65xx_reg_sync_writel( reg_val, MT6577_RZ_CFG_ADDR( pBase ) );
            //2.Clear INT
            reg_val = ioread32( MT6577_RZ_INT_ADDR(pBase) );/*Read Clear*/
            break;
        case MID_PRZ0:
            pBase = PRZ0_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_RZ_CFG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x3 << 10) );
            mt65xx_reg_sync_writel( reg_val, MT6577_RZ_CFG_ADDR( pBase ) );
            //2.Clear INT
            reg_val = ioread32( MT6577_RZ_INT_ADDR(pBase) );/*Read Clear*/
            break;
        case MID_PRZ1:
            pBase = PRZ1_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_RZ_CFG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x3 << 10) );
            mt65xx_reg_sync_writel( reg_val, MT6577_RZ_CFG_ADDR( pBase ) );
            //2.Clear INT
            reg_val = ioread32( MT6577_RZ_INT_ADDR(pBase) );/*Read Clear*/
            break;
        case MID_VRZ0:
            pBase = VRZ0_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_VRZ_CFG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x1 << 10) );
            mt65xx_reg_sync_writel( reg_val, MT6577_VRZ_CFG_ADDR( pBase ) );
            //2.Clear INT
            /*Read Clear*/
            reg_val = ioread32( MT6577_VRZ_INT_ADDR(pBase) );
            break;
        case MID_VRZ1:
            pBase = VRZ1_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_VRZ_CFG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x1 << 10) );
            mt65xx_reg_sync_writel( reg_val, MT6577_VRZ_CFG_ADDR( pBase ) );
            //2.Clear INT
            /*Read Clear*/
            reg_val = ioread32( MT6577_VRZ_INT_ADDR(pBase) );
            break;
        case MID_R_DMA0:
            pBase = R_DMA0_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_RDMA_IRQ_FLAG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x23 << 16) );
            mt65xx_reg_sync_writel( reg_val, MT6577_RDMA_IRQ_FLAG_ADDR( pBase ) );
            //2.Clear INT
            reg_val = ioread32( MT6577_RDMA_IRQ_FLAG_ADDR(pBase) );
            mt65xx_reg_sync_writel( (0x23 & reg_val), MT6577_RDMA_IRQ_FLAG_CLR_ADDR(pBase) );
            break;
        case MID_R_DMA1:    
            pBase = R_DMA1_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_RDMA_IRQ_FLAG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x23 << 16) );
            mt65xx_reg_sync_writel( reg_val, MT6577_RDMA_IRQ_FLAG_ADDR( pBase ) );
            //2.Clear INT
            reg_val = ioread32( MT6577_RDMA_IRQ_FLAG_ADDR(pBase) );
            mt65xx_reg_sync_writel( (0x23 & reg_val), MT6577_RDMA_IRQ_FLAG_CLR_ADDR(pBase) );
            break;
        case MID_OVL:
            pBase = OVL_DMA_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_OVL_DMA_IRQ_FLAG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x33 << 16) );
            mt65xx_reg_sync_writel( reg_val, MT6577_OVL_DMA_IRQ_FLAG_ADDR( pBase ) );
            //2.Clear INT
            reg_val = ioread32( MT6577_OVL_DMA_IRQ_FLAG_ADDR(pBase) );
            mt65xx_reg_sync_writel( (0x33 & reg_val), MT6577_OVL_DMA_IRQ_FLAG_ADDR(pBase) );
            break;
        /*        
        case MID_IPP:    
            break;
        case MID_MOUT:    
            break;
        */
        case MID_JPEG_DMA:
            pBase = JPG_DMA_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_JPEG_DMA_CON_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x1) );
            mt65xx_reg_sync_writel( reg_val, MT6577_JPEG_DMA_CON_ADDR( pBase ) );
            //2.Clear INT
            mt65xx_reg_sync_writel( 0, MT6577_JPEG_DMA_INTERRUPT_ADDR( pBase ) );/*Clear*/
            break;
        case MID_RGB_ROT0:
            pBase = RGB_ROT0_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x33 << 16) );
            mt65xx_reg_sync_writel( reg_val, MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            //2.Clear INT
            reg_val = ioread32( MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            mt65xx_reg_sync_writel( ( 0x33 & reg_val ), MT6577_ROT_DMA_IRQ_FLAG_CLR_ADDR(pBase) );
            break;
        case MID_RGB_ROT1:
            pBase = RGB_ROT1_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x33 << 16) );
            mt65xx_reg_sync_writel( reg_val, MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            //2.Clear INT
            reg_val = ioread32( MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            mt65xx_reg_sync_writel( ( 0x33 & reg_val ), MT6577_ROT_DMA_IRQ_FLAG_CLR_ADDR(pBase) );
            break;
        case MID_RGB_ROT2:
            pBase = RGB_ROT2_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x33 << 16) );
            mt65xx_reg_sync_writel( reg_val, MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            //2.Clear INT
            reg_val = ioread32( MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            mt65xx_reg_sync_writel( ( 0x33 & reg_val ), MT6577_ROT_DMA_IRQ_FLAG_CLR_ADDR(pBase) );
            break;
        case MID_VDO_ROT0:
            pBase = VDO_ROT0_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x33 << 16) );
            mt65xx_reg_sync_writel( reg_val, MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            //2.Clear INT
            reg_val = ioread32( MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            mt65xx_reg_sync_writel( ( 0x33 & reg_val ), MT6577_ROT_DMA_IRQ_FLAG_CLR_ADDR(pBase) );
            break;
        case MID_VDO_ROT1:
            pBase = VDO_ROT1_BASE;
            //1.Disable INT
            reg_val = ioread32( MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            reg_val = ( reg_val & ~(0x33 << 16) );
            mt65xx_reg_sync_writel( reg_val, MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            //2.Clear INT
            reg_val = ioread32( MT6577_ROT_DMA_IRQ_FLAG_ADDR( pBase ) );
            mt65xx_reg_sync_writel( ( 0x33 & reg_val ), MT6577_ROT_DMA_IRQ_FLAG_CLR_ADDR(pBase) );
            break;
    }

    return 0;
    
    
}


// 1 : turn on, 0 : turn off
static int PowerManagement(unsigned long u4TurnOn, unsigned long u4ResTbl)
{
#if !defined(MDP_KERNEL_FLAG_0_FPGA)
    
    #define _CLOCK_SWITCH_CASE_( _mdp_id_, _clock_id_, _b_mod_clock_always_on_ ) \
                        case _mdp_id_:\
                        if( u4TurnOn ){\
                                    enable_clock(_clock_id_, "MDP");\
                                    MDPK_POWERINFO("Clock On:<%s> -> %s\n\r",#_mdp_id_,#_clock_id_);\
                                    g_mdp_clock_status.ref_count[_mdp_id_##_i]++;\
                                } else\
                                {\
                                    if( !( (b_mdp_clock_all_always_on==1)&&(_b_mod_clock_always_on_==1)) )\
                                    {\
                                        PowerManagement_RestInt( _mdp_id_ );\
                                        disable_clock(_clock_id_, "MDP");\
                                        MDPK_POWERINFO("Clock Off:<%s> -> %s\n\r",#_mdp_id_,#_clock_id_);\
                                        g_mdp_clock_status.ref_count[_mdp_id_##_i]--;\
                                        if( g_mdp_clock_status.ref_count[_mdp_id_##_i] < 0 ){\
                                            MDPK_ERROR("g_mdp_clock_status.ref_count[%d] = %d< 0\n",(int)_mdp_id_##_i,g_mdp_clock_status.ref_count[_mdp_id_##_i]);}\
                                    }\
                                };

    #define _MORE_CLOCK_SWITCH_( _clock_id_, _b_mod_clock_always_on_ ) \
                        if( u4TurnOn ){\
                                    enable_clock(_clock_id_, "MDP");\
                                    MDPK_POWERINFO("Clock On:    -> %s\n\r",#_clock_id_);\
                                } else\
                                {\
                                    if( !( (b_mdp_clock_all_always_on==1)&&(_b_mod_clock_always_on_==1)) )\
                                    {\
                                        disable_clock(_clock_id_, "MDP");\
                                        MDPK_POWERINFO("Clock Off:   -> %s\n\r",#_clock_id_);\
                                    }\
                                };
#else

    #define _CLOCK_SWITCH_CASE_( _mdp_id_, _clock_id_, _b_mod_clock_always_on_ )
    #define _MORE_CLOCK_SWITCH_( _clock_id_, _b_mod_clock_always_on_ )

#endif/*MDP_KERNEL_FLAG_0_FPGA*/
    
    unsigned long i;
    unsigned long mdp_element_id;
    unsigned long   irq_status;


    /*.............................................................................
        Enter Critical Section
      .............................................................................*/
    spin_lock_irqsave( &g_mdp_clock_status.spin_lock, irq_status );


    MDPK_POWERINFO("\n->MMSYS1_CG_CON1=0x%08X\n",*(unsigned int*)(MMSYS1_CONFIG_BASE+0x04) );


    for( i = 0; i < MDP_ELEMENT_MAX_NUM; i++ )
    {
        mdp_element_id = u4ResTbl & ( 0x1 << i );
        
        if( mdp_element_id == 0)
            continue;

        switch( mdp_element_id )
        {
        _CLOCK_SWITCH_CASE_(  MID_JPEG_DEC, MT65XX_PDN_MM_JPEG_DEC, b_mdp_clk_jpeg_dec_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_JPEG_ENC, MT65XX_PDN_MM_JPEG_ENC, b_mdp_clk_jpeg_enc_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_CAM     , MT65XX_PDN_MM_CAM,      b_mdp_clk_cam_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_BRZ     , MT65XX_PDN_MM_BRZ,      b_mdp_clk_brz_always_on);
            _MORE_CLOCK_SWITCH_(            MT65XX_PDN_MM_BRZ_MOUT, b_mdp_clk_brz_always_on );
            break;
        _CLOCK_SWITCH_CASE_(  MID_CRZ     , MT65XX_PDN_MM_CRZ,      b_mdp_clk_crz_always_on);
            _MORE_CLOCK_SWITCH_(            MT65XX_PDN_MM_RESZ_LB,  b_mdp_clk_crz_always_on );
            _MORE_CLOCK_SWITCH_(            MT65XX_PDN_MM_OVL_DMA_BPS,  b_mdp_clk_crz_always_on );/*WORK AROUND!!!This is for fix after ZSD path operation, JPG decoder will hang*/
            break;
        _CLOCK_SWITCH_CASE_(  MID_PRZ0    , MT65XX_PDN_MM_PRZ0,     b_mdp_clk_prz0_always_on);
            _MORE_CLOCK_SWITCH_(            MT65XX_PDN_MM_PRZ0_MOUT,b_mdp_clk_prz0_always_on );
            _MORE_CLOCK_SWITCH_(            MT65XX_PDN_MM_RESZ_LB,  b_mdp_clk_prz0_always_on );
            break;
        _CLOCK_SWITCH_CASE_(  MID_PRZ1    , MT65XX_PDN_MM_PRZ1,     b_mdp_clk_prz1_always_on);
            _MORE_CLOCK_SWITCH_(            MT65XX_PDN_MM_RESZ_LB,  b_mdp_clk_prz1_always_on );
            break;
        _CLOCK_SWITCH_CASE_(  MID_VRZ0    , MT65XX_PDN_MM_VRZ,      b_mdp_clk_vrz0_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_VRZ1    , MT65XX_PDN_MM_VRZ1,     b_mdp_clk_vrz1_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_R_DMA0  , MT65XX_PDN_MM_R_DMA0,       b_mdp_clk_r_dma0_always_on);
            _MORE_CLOCK_SWITCH_(            MT65XX_PDN_MM_R_DMA0_MOUT,  b_mdp_clk_r_dma0_always_on );
            break;
        _CLOCK_SWITCH_CASE_(  MID_R_DMA1  , MT65XX_PDN_MM_R_DMA1,       b_mdp_clk_r_dma1_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_OVL     , MT65XX_PDN_MM_OVL_DMA,      b_mdp_clk_ovl_always_on );
            _MORE_CLOCK_SWITCH_(            MT65XX_PDN_MM_OVL_DMA_MIMO, b_mdp_clk_ovl_always_on );
            _MORE_CLOCK_SWITCH_(            MT65XX_PDN_MM_OVL_DMA_BPS,  b_mdp_clk_ovl_always_on );/*NOTE:Suspicious bug*/
            break;
        _CLOCK_SWITCH_CASE_(  MID_IPP     , MT65XX_PDN_MM_IPP,      b_mdp_clk_ipp_always_on);
            _MORE_CLOCK_SWITCH_(            MT65XX_PDN_MM_IPP_MOUT, b_mdp_clk_ipp_always_on );
            break;
        _CLOCK_SWITCH_CASE_(  MID_MOUT    , MT65XX_PDN_MM_MOUT,     b_mdp_clk_mout_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_LCD     , MT65XX_PDN_MM_LCD,      b_mdp_clk_lcd_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_JPEG_DMA, MT65XX_PDN_MM_JPEG_DMA, b_mdp_clk_jpeg_dma_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_EIS     , MT65XX_PDN_MM_EIS,      b_mdp_clk_eis_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_RGB_ROT0, MT65XX_PDN_MM_RGB_ROT0, b_mdp_clk_rgb_rot0_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_RGB_ROT1, MT65XX_PDN_MM_RGB_ROT1, b_mdp_clk_rgb_rot1_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_RGB_ROT2, MT65XX_PDN_MM_RGB_ROT2, b_mdp_clk_rgb_rot2_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_VDO_ROT0, MT65XX_PDN_MM_VDO_ROT0, b_mdp_clk_vdo_rot0_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_VDO_ROT1, MT65XX_PDN_MM_VDO_ROT1, b_mdp_clk_vdo_rot1_always_on);
            break;
        _CLOCK_SWITCH_CASE_(  MID_TV_ROT  , MT65XX_PDN_MM_TV_ROT,   b_mdp_clk_tv_rot_always_on);
            break;
        default:
            
            #if !defined(MDP_KERNEL_FLAG_0_FPGA)
            MDPK_ERROR("Unknown mdp element id:0x%X\n\r", (unsigned int)mdp_element_id );
            #endif
            break;
        }
        
    }


    #if 1
    {
        int i;

        for( i = 0 ; i < MDP_ELEMENT_MAX_NUM; i++ )
        {
            MDPK_POWERINFO("g_mdp_clock_status.ref_count[%d] = %d\n", i, g_mdp_clock_status.ref_count[i] );
        }
    }
    #endif

    
    MDPK_POWERINFO("<-MMSYS1_CG_CON1=0x%08X\n\n",*(unsigned int*)(MMSYS1_CONFIG_BASE+0x04) );
    

    spin_unlock_irqrestore( &g_mdp_clock_status.spin_lock, irq_status );
    /*.............................................................................
        Exit Critical Section
      .............................................................................*/
      
    #if !defined(MDP_KERNEL_FLAG_0_FPGA)
    /*if turn on, check Main PLL status*/
    if( u4TurnOn)
    {
        if( ( DRV_Reg32(MAINPLL_CON0) & 0x1 ) == 1 ) //if main pll is off
        {
            MDPK_ERROR("Main Pll is off when enable mdp\n");
            return -1;
        }
    }
    #endif
        


    return 0;
}

unsigned long MDPK_Util_MsToJiffies(unsigned long u4ms)
{
    return ((u4ms*HZ + 512) >> 10);
}

static int MT6577_UnLockResource(unsigned long a_u4ResTbl , stMDPOpLog * a_pstLog)
{
    unsigned long   irq_status;
    
    unsigned int before_rsc_u, before_rsc_o;
    unsigned int after_rsc_u, after_rsc_o;

   
    
    spin_lock_irqsave( &g_mdp_resource_tbl.spin_lock, irq_status );

    
    before_rsc_u = (unsigned int)g_mdp_resource_tbl.resource_in_use_table;
    before_rsc_o = (unsigned int)g_mdp_resource_tbl.resource_in_occupied_table;

    /*Clear resource flag*/
    g_mdp_resource_tbl.resource_in_use_table &= ~a_u4ResTbl;
    g_mdp_resource_tbl.resource_in_occupied_table &= ~a_u4ResTbl;
    
    a_pstLog->u4OwnResource &= (~a_u4ResTbl);

    /*Update thread id*/
    if( a_pstLog->u4OwnResource == 0 ){
        a_pstLog->tgid = 0;
    }
    

    /*Power Down*/
    PowerManagement(0 , a_u4ResTbl);

    after_rsc_u = (unsigned int)g_mdp_resource_tbl.resource_in_use_table;
    after_rsc_o = (unsigned int)g_mdp_resource_tbl.resource_in_occupied_table;

    
    MDPK_INFO("UnLock Resource: (0x%08X/0x%08X)=(0x%08X)=>(0x%08X/0x%08X)\n",
                    before_rsc_u, before_rsc_o,
                    (unsigned int)a_u4ResTbl,
                    after_rsc_u, after_rsc_o );

    mb();   /*Add memory barrier before the other CPU (may) wakeup*/
    wake_up_interruptible( &g_mdp_resource_wait_queue );

    spin_unlock_irqrestore( &g_mdp_resource_tbl.spin_lock, irq_status );

    return 0;
}


static int MT6577_LockResource(unsigned long a_pParam , stMDPOpLog * a_pstLog)
{
    stLockResParam  stParam;
    unsigned long   irq_status;
    long            ret_of_wait_timeout;

    unsigned int before_rsc_u, before_rsc_o;


    //1.Get user parameters
    if(copy_from_user(&stParam, (struct stLockResParam *)a_pParam , sizeof(stLockResParam)))
    {
        MDPK_ERROR("Lock resource, Copy from user failed !!\n");
        return -EFAULT;
    }

    spin_lock_irqsave( &g_mdp_resource_tbl.spin_lock, irq_status );


    before_rsc_u = (unsigned int)g_mdp_resource_tbl.resource_in_use_table;
    before_rsc_o = (unsigned int)g_mdp_resource_tbl.resource_in_occupied_table;

    //Case 1: More than one resource is in used (Lock failed)
    if( g_mdp_resource_tbl.resource_in_use_table & stParam.u4LockResTable ) 
    {
        //Case 1.1: no wait case, return immidiately
        //          case 1 time out = 0 no wait
        //          case 2 resource in occupied( in long-time use case like preview )
        if( ( stParam.u4TimeOutInms == 0 ) || ( g_mdp_resource_tbl.resource_in_occupied_table & stParam.u4LockResTable ) )
        {
            stParam.out_Table_status_in_use         = g_mdp_resource_tbl.resource_in_use_table;
            stParam.out_Table_status_in_occupied    = g_mdp_resource_tbl.resource_in_occupied_table;
            
            spin_unlock_irqrestore( &g_mdp_resource_tbl.spin_lock, irq_status);
            
            MDPK_INFO("Lock Resource Fail: Before:(0x%08X/0x%08X)=Req:(0x%08X)=>After:(0x%08X/0x%08X) time_shared:%d timeout:%d\n",
                        before_rsc_u, before_rsc_o,
                        (unsigned int)stParam.u4LockResTable,
                        (unsigned int)stParam.out_Table_status_in_use, (unsigned int)stParam.out_Table_status_in_occupied,
                        (int)stParam.u4IsTimeShared,    (int)stParam.u4TimeOutInms );
            

            if( copy_to_user( (struct stLockResParam *)a_pParam, &stParam, sizeof(stLockResParam) )) {
                MDPK_ERROR("Lock resource, Copy to user failed !!\n");
                return -EFAULT;
            }

            
            return -EBUSY;
        }

        //Case 1.2: wait until time out or resource free case
        spin_unlock_irqrestore( &g_mdp_resource_tbl.spin_lock, irq_status);

        ret_of_wait_timeout = wait_event_interruptible_timeout( 
                                    g_mdp_resource_wait_queue, 
                                    MT6577_TryLockResource(stParam.u4LockResTable, stParam.u4IsTimeShared , &stParam ) , //If success,realy update resource_in_use_table  
                                    MDPK_Util_MsToJiffies(stParam.u4TimeOutInms) );


        if(0 == ret_of_wait_timeout)
        {
            MDPK_INFO("Lock Resource Fail(Timeout): Before:(0x%08X/0x%08X)=Req:(0x%08X)=>After:(0x%08X/0x%08X) time_shared:%d timeout:%d\n",
                        before_rsc_u, before_rsc_o,
                        (unsigned int)stParam.u4LockResTable,
                        (unsigned int)stParam.out_Table_status_in_use, (unsigned int)stParam.out_Table_status_in_occupied,
                        (int)stParam.u4IsTimeShared,    (int)stParam.u4TimeOutInms );
            
            if( copy_to_user( (struct stLockResParam *)a_pParam, &stParam, sizeof(stLockResParam) )) {
                MDPK_ERROR("Lock resource, Copy to user failed !!\n");
                return -EFAULT;
            }
            
            return -EAGAIN;
        }
        /*.............................................................................
                Lock success
          .............................................................................*/
        //TryLockResource() will do the update of resource table
        a_pstLog->u4OwnResource |= stParam.u4LockResTable;

        //Update thread id
        if( a_pstLog->tgid == 0 ){
            a_pstLog->tgid = current->tgid;
        }else if( a_pstLog->tgid != current->tgid ) {
            MDPK_ERROR("Lock resource tgid inconsistent! %d != %d\n",a_pstLog->tgid,current->tgid );
        }
        

        MDPK_INFO("Lock Resource OK: Before:(0x%08X/0x%08X)=Req:(0x%08X)=>After:(0x%08X/0x%08X) time_shared:%d timeout:%d\n",
                    before_rsc_u, before_rsc_o,
                    (unsigned int)stParam.u4LockResTable,
                    (unsigned int)stParam.out_Table_status_in_use, (unsigned int)stParam.out_Table_status_in_occupied,
                    (int)stParam.u4IsTimeShared,    (int)stParam.u4TimeOutInms );
        
        if( PowerManagement(1 , stParam.u4LockResTable) < 0 )
        {
            MT6577_UnLockResource( stParam.u4LockResTable, a_pstLog );
            
            stParam.out_Table_status_in_use         = g_mdp_resource_tbl.resource_in_use_table;
            stParam.out_Table_status_in_occupied    = g_mdp_resource_tbl.resource_in_occupied_table;
            
            if( copy_to_user( (struct stLockResParam *)a_pParam, &stParam, sizeof(stLockResParam) )) {
                MDPK_ERROR("Lock resource, Copy to user failed !!\n");
                return -EFAULT;
            }
            
            return -EAGAIN;
        }

        
        if( copy_to_user( (struct stLockResParam *)a_pParam, &stParam, sizeof(stLockResParam) )) {
            MDPK_ERROR("Lock resource, Copy to user failed !!\n");
            return -EFAULT;
        }

        return 0;
        
    }

    //Case 2: All request resource are free
    else
    {
        g_mdp_resource_tbl.resource_in_use_table |= stParam.u4LockResTable;
        
        if(!stParam.u4IsTimeShared)
        {
            g_mdp_resource_tbl.resource_in_occupied_table |= stParam.u4LockResTable;
        }

        
        /*.............................................................................
                Lock success
          .............................................................................*/
        a_pstLog->u4OwnResource |= stParam.u4LockResTable;
        
        //Update thread id
        if( a_pstLog->tgid == 0 ){
            a_pstLog->tgid = current->tgid;
        }else if( a_pstLog->tgid != current->tgid ) {
            MDPK_ERROR("Lock resource tgid inconsistent! %d != %d\n",a_pstLog->tgid,current->tgid );
        }
        

        stParam.out_Table_status_in_use         = g_mdp_resource_tbl.resource_in_use_table;
        stParam.out_Table_status_in_occupied    = g_mdp_resource_tbl.resource_in_occupied_table;

        
        MDPK_INFO("Lock Resource OK: Before:(0x%08X/0x%08X)=Req:(0x%08X)=>After:(0x%08X/0x%08X) time_shared:%d timeout:%d\n",
                    before_rsc_u, before_rsc_o,
                    (unsigned int)stParam.u4LockResTable,
                    (unsigned int)stParam.out_Table_status_in_use, (unsigned int)stParam.out_Table_status_in_occupied,
                    (int)stParam.u4IsTimeShared,    (int)stParam.u4TimeOutInms );

        if( PowerManagement(1 , stParam.u4LockResTable) < 0 ){
            spin_unlock_irqrestore( &g_mdp_resource_tbl.spin_lock, irq_status );
            MT6577_UnLockResource( stParam.u4LockResTable, a_pstLog );
            
            stParam.out_Table_status_in_use         = g_mdp_resource_tbl.resource_in_use_table;
            stParam.out_Table_status_in_occupied    = g_mdp_resource_tbl.resource_in_occupied_table;

            if( copy_to_user( (struct stLockResParam *)a_pParam, &stParam, sizeof(stLockResParam) )) {
                MDPK_ERROR("Lock resource, Copy to user failed !!\n");
                return -EFAULT;
            }
            
            return -EAGAIN;
        }
    }


    spin_unlock_irqrestore( &g_mdp_resource_tbl.spin_lock, irq_status );

    if( copy_to_user( (struct stLockResParam *)a_pParam, &stParam, sizeof(stLockResParam) )) {
            MDPK_ERROR("Lock resource, Copy to user failed !!\n");
            return -EFAULT;
    }

    return 0;
}




static int MT6577_WAIT_IRQ(unsigned long a_pParam)
{
    int             ret = 0;
    stWaitIrqParam stParam;
    long iTimeOut;
    unsigned long   flags;


    if( copy_from_user(&stParam, (struct stWaitIrqParam *)a_pParam , sizeof(stWaitIrqParam)) )
    {
        MDPK_ERROR("Wait IRQ: Copy from user failed!!\n");
        return -EFAULT;
    }

    /*wait until irq done*/
    iTimeOut = wait_event_interruptible_timeout(
                    g_mdp_irq_done_queue, 
                    ( (g_mdp_irq_tbl.irq_done_table & stParam.u4IrqNo) == stParam.u4IrqNo ), 
                    MDPK_Util_MsToJiffies( stParam.u4TimeOutInms ) );

    /*wake-up from sleep*/
    spin_lock_irqsave( &g_mdp_irq_tbl.spin_lock , flags );


    if( iTimeOut == 0 )/*wait done failed. time out*/
    {
        MDPK_ERROR("Wait Done Timeout! PID:%d  DoneTbl:0x%08x  WaitFlag:0x%08x  timeout:%lu\n" , 
                        current->pid ,(unsigned int)g_mdp_irq_tbl.irq_done_table, (unsigned int)stParam.u4IrqNo , stParam.u4TimeOutInms );

        if( b_mdp_timeout_dumpreg == 1 ){
            MT6577MDP_DUMPREG();
        }
        ret = -EAGAIN;
        
    }else
    {
        /*Clear IRQ done table*/
        g_mdp_irq_tbl.irq_done_table &= ~stParam.u4IrqNo;
    }

    spin_unlock_irqrestore( &g_mdp_irq_tbl.spin_lock , flags );

    return ret;
}


static int MT6577_CLR_IRQ(unsigned long a_u4IRQFlag)
{
    unsigned long   flags;
    unsigned long   flags2;
    unsigned long   irq_flag;

    #if 0
    if(copy_from_user(&irq_flag, a_u4IRQFlag , sizeof(unsigned long)))
    {
        MDPK_ERROR("clear IRQ, Copy from user failed !!\n");
        return -EFAULT;
    }
    #endif

    irq_flag = a_u4IRQFlag;
    

    spin_lock_irqsave( &g_mdp_irq_tbl.spin_lock , flags );

    /*clear irq done table*/
    g_mdp_irq_tbl.irq_done_table &= (~irq_flag);

    /*Reset exec counter for each rot*/
    if( irq_flag & MID_RGB_ROT0 ) {
        g_mdp_irq_tbl.rot_exec_count[0] = ioread32( MT6577_ROT_DMA_EXEC_CNT_ADDR(RGB_ROT0_BASE) );
    }
    if( irq_flag & MID_RGB_ROT1 ) {
        g_mdp_irq_tbl.rot_exec_count[1] = ioread32( MT6577_ROT_DMA_EXEC_CNT_ADDR(RGB_ROT1_BASE) );
    }
    if( irq_flag & MID_RGB_ROT2 ) {
        g_mdp_irq_tbl.rot_exec_count[2] = ioread32( MT6577_ROT_DMA_EXEC_CNT_ADDR(RGB_ROT2_BASE) );
    }
    if( irq_flag & MID_VDO_ROT0 ) {
        g_mdp_irq_tbl.rot_exec_count[3] = ioread32( MT6577_ROT_DMA_EXEC_CNT_ADDR(VDO_ROT0_BASE) );
    }
    if( irq_flag & MID_VDO_ROT1 ) {
        g_mdp_irq_tbl.rot_exec_count[4] = ioread32( MT6577_ROT_DMA_EXEC_CNT_ADDR(VDO_ROT1_BASE) );
    }

    /*Zoom dirty flag*/
    if( irq_flag & MID_CRZ) {
        spin_lock_irqsave(&g_mdp_zoom_param.spin_lock, flags2);
        g_mdp_zoom_param.is_dirty= 0;
        spin_unlock_irqrestore(&g_mdp_zoom_param.spin_lock , flags2);
    }
    

    spin_unlock_irqrestore( &g_mdp_irq_tbl.spin_lock , flags );


    return 0;
}




extern int is_pmem_range(unsigned long *base, unsigned long size);
extern void get_pmem_range(unsigned long * pu4StartAddr, unsigned long * pu4Size);
static inline int MT6577_CheckPmemRange(stPMEMRange * a_u4Param)
{
    stPMEMRange input;
    unsigned int pageOffset;
    pgd_t *pgd;
    pmd_t *pmd;
    pte_t *pte;

    if( copy_from_user( &input , a_u4Param , sizeof(stPMEMRange)) )
    {
        MDPK_ERROR("MDP check pmem range copy from user failed!!\n");
        return -EFAULT;
    }    


    /*-----------------------------------------------------------------------------
      translate virtual to physical addr
      -----------------------------------------------------------------------------*/
    //MDPK_INFO("Virtual addr : 0x%x , size : %lu\n" , input.u4StartAddr , input.u4Size);
    
    pageOffset = (input.u4StartAddr & (PAGE_SIZE - 1));
    pgd = pgd_offset(current->mm, input.u4StartAddr);
    if(pgd_none(*pgd)||pgd_bad(*pgd))
    {
        //MDPK_WARNING("warning: upper layer pass address va=0x%x, pgd invalid! \n", (unsigned int)input.u4StartAddr);
        input.u4Result = 2;
        goto out;
    }

    pmd = pmd_offset(pgd, input.u4StartAddr);
    if(pmd_none(*pmd)||pmd_bad(*pmd))
    {
        //MDPK_WARNING("warning: upper layer pass address va=0x%x, pmd invalid! \n", (unsigned int)input.u4StartAddr);
        input.u4Result = 2;
        goto out;
    }
        
    pte = pte_offset_map(pmd, input.u4StartAddr);
    if(pte_present(*pte)) 
    { 
        input.u4StartAddr = (pte_val(*pte) & (PAGE_MASK)) | pageOffset;
    }     



    #if 1 /*My Version*/

    input.u4Result = is_pmem_range((unsigned long*) input.u4StartAddr /*phy base?*/, input.u4Size  );

    #else /*Gipi Version*/
    
    get_pmem_range(&u4Addr , &u4Size);

    if((input.u4StartAddr >= u4Addr) && (input.u4StartAddr < (u4Addr + u4Size)))
    {
        //inside of pmem
        if((input.u4StartAddr + input.u4Size) <= (u4Addr + u4Size))
        {
            input.u4Result = 1;
        }
        else
        {
            input.u4Result = 2;
            MDPDB("physical address : 0x%x, size : %lu , pmem addr : 0x%x , pmem size : %lu\n" , (unsigned int)input.u4StartAddr , input.u4Size , (unsigned int)u4Addr , u4Size);
        }
    }
    else
    {
        //outside of pmem
        input.u4Result = 0;
    }

    #endif


    //MDPK_INFO("Physical : 0x%x , result : %d\n" , input.u4StartAddr , input.u4Result);
out:
    if(copy_to_user((void __user *) a_u4Param , &input , sizeof(stPMEMRange)))
    {
        MDPK_ERROR("MDP get pmem range copy to user failed!!\n");
        return -EFAULT;
    }

    return 0;
}


static int MT6577_SetZoom( stZoomSetting *p_param )
{
    
    unsigned long flag;

    spin_lock_irqsave(&g_mdp_zoom_param.spin_lock, flag);
    g_mdp_zoom_param.is_dirty= 0;
    spin_unlock_irqrestore(&g_mdp_zoom_param.spin_lock , flag);

    if(copy_from_user(&g_mdp_zoom_param.CRZ_REG , (struct stZoomSetting *)p_param , sizeof(stZoomSetting)))
    {
        MDPK_ERROR("MDP set zoom copy from user failed!!\n");
        return -EFAULT;
    }    

    spin_lock_irqsave(&g_mdp_zoom_param.spin_lock , flag);
    g_mdp_zoom_param.is_dirty = 1;
    spin_unlock_irqrestore(&g_mdp_zoom_param.spin_lock , flag);

    return 0;
    
}

int MdpMvaLoopMemStatisticDump( int start_index, int end_index )
{
    int i;

    for( i = start_index; ( i <= end_index ) && ( i < MDPMVALOOPMEM_MAX_CLIENT ) ; i++ )
    {
        MDPK_DEBUG("[MVA Loop Mem]Client=%d alloc_#=%d free_#=%d bal_#=%d alloc_z=0x%08X free_z=0x%08X bal_z=0x%08X last_tgid=%d\n",
                    i,
                    (int)g_mdp_mva_loop_mem_statistic.client[i].statistic_alloc_count,
                    (int)g_mdp_mva_loop_mem_statistic.client[i].statistic_free_count,
                    (int)g_mdp_mva_loop_mem_statistic.client[i].statistic_count_balance,
                    (unsigned int)g_mdp_mva_loop_mem_statistic.client[i].statistic_alloc_size,
                    (unsigned int)g_mdp_mva_loop_mem_statistic.client[i].statistic_free_size,
                    (unsigned int)g_mdp_mva_loop_mem_statistic.client[i].statistic_size_balance,
                    (int)g_mdp_mva_loop_mem_statistic.client[i].last_tgid
                    );
        
    }

    return 0;
}


static int MT6577_MvaLoopMemTrack( MDPIOCTL_MvaLoopMemTrack *p_param)
{
    unsigned long flag;
    MDPIOCTL_MvaLoopMemTrack param;
    
    if(copy_from_user(&param , (struct MDPIOCTL_MvaLoopMemTrack *)p_param , sizeof(MDPIOCTL_MvaLoopMemTrack)))
    {
        MDPK_ERROR("MDP MT6577_MvaLoopMemTrack: copy from user failed!!\n");
        return -EFAULT;
    }    

    if( param.in_client_id >= MDPMVALOOPMEM_MAX_CLIENT )
    {
        MDPK_ERROR("invalid client id(0x%08X)!!\n", (unsigned int)param.in_client_id);
        return -EFAULT;
    }

    spin_lock_irqsave(&g_mdp_mva_loop_mem_statistic.spin_lock, flag);

        /*Update Statistic*/
        if( param.in_action == MLMT_ACTION_ALLOC )
        {
            g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_alloc_count++;
            g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_alloc_size += param.in_size;
        }
        else
        {
            g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_free_count++;
            g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_free_size += param.in_size;
        }

        g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_count_balance =
            g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_alloc_count -
            g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_free_count;

        
        g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_size_balance =
            g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_alloc_size-
            g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_free_size;

        g_mdp_mva_loop_mem_statistic.client[param.in_client_id].last_tgid = current->tgid;

        /*Update information to user*/
        param.out_statistic_alloc_count     = g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_alloc_count;
        param.out_statistic_free_count      = g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_free_count;
        param.out_statistic_count_balance   = g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_count_balance;

        param.out_statistic_alloc_size      = g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_alloc_size;
        param.out_statistic_free_size       = g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_free_size;
        param.out_statistic_size_balance    = g_mdp_mva_loop_mem_statistic.client[param.in_client_id].statistic_size_balance;


    spin_unlock_irqrestore(&g_mdp_mva_loop_mem_statistic.spin_lock , flag);

    
    if( copy_to_user( (struct MDPIOCTL_MvaLoopMemTrack *)p_param, &param, sizeof(MDPIOCTL_MvaLoopMemTrack) )) {
        MDPK_ERROR("MDP MT6577_MvaLoopMemTrack: copy to user failed!!\n");
        return -EFAULT;
    }

    return 0;

    
}


static int MDPK_CpuClockGet( MDPIOCTL_CpuClockGet *p_param)
{
    MDPIOCTL_CpuClockGet param;
    unsigned long long  sec; 
    unsigned long       usec;

    sec = cpu_clock(0);     //ns
    do_div( sec, 1000 );    //usec
    usec = do_div( sec, 1000000);    //sec and usec

    param.sec   = sec;
    param.usec  = usec;

    
    if( copy_to_user( (struct MDPIOCTL_CpuClockGet *)p_param, &param, sizeof(MDPIOCTL_CpuClockGet) )) {
        MDPK_ERROR("MDP MDPK_CpuClockGet: copy to user failed!!\n");
        return -EFAULT;
    }

    return 0;
}



/*=============================================================================
    Member function for File operation
        
          static int MT6577_MDP_Open(struct inode * a_pstInode, struct file * a_pstFile)
          static int MT6577_MDP_Release(struct inode * a_pstInode, struct file * a_pstFile)
          static int MT6577_MDP_Flush(struct file * a_pstFile , fl_owner_t a_id)
          static int MT6577_MDP_Ioctl(struct inode * a_pstInode,
                            struct file * a_pstFile,
                            unsigned int a_u4Command,
                            unsigned long a_u4Param)
          static int MT6577_MDP_mmap(struct file * a_pstFile, struct vm_area_struct * a_pstVMArea)
  =============================================================================*/

static int MT6577_MDP_Open(struct inode * a_pstInode, struct file * a_pstFile)
{
    stMDPOpLog * pstLog;

    MDPK_SHOWFUNCTION( 1 );

    //Allocate and initialize private data
    a_pstFile->private_data = kmalloc(sizeof(stMDPOpLog) , GFP_ATOMIC);

    if(NULL == a_pstFile->private_data)
    {
        MDPK_ERROR("Not enough entry for MDP open operation\n");
        return -ENOMEM;
    }

    /*Assigin private data and record pid when each driver instance has been opened*/
    pstLog = (stMDPOpLog *)a_pstFile->private_data;
    //pstLog->pid = current->pid;
    pstLog->tgid = 0; //update tgid(process id) when lock
    pstLog->u4OwnResource = 0;

    return 0;
}

static int MT6577_MDP_Release(struct inode * a_pstInode, struct file * a_pstFile)
{
    stMDPOpLog * pstLog;
    
    MDPK_SHOWFUNCTION( 1 );
    
    pstLog = (stMDPOpLog *)a_pstFile->private_data;

    if(NULL == pstLog)
    {
        MDPK_ERROR("Private data is null in release operation. HOW COULD THIS HAPPEN ??\n");
        return 0;
    }

    if(pstLog->u4OwnResource)
    {
        MDPK_ERROR("Resource : 0x%08X owned by %d(TGID) an not released. Force to release it in release(). current = ( process, pid, tgid ) = ( %s, %d, %d )\n", 
            (unsigned int)pstLog->u4OwnResource , (int)pstLog->tgid,
            current->comm , current->pid, current->tgid );

        //TODO:
        // Step 1 : Check OwnResource and Turn off engines
        //.....

        // Step 2 : Release resource, and turn off clock gate
        MT6577_UnLockResource(pstLog->u4OwnResource , pstLog);
    }
    
    
    
    if(NULL != a_pstFile->private_data)
    {
        kfree(a_pstFile->private_data);
        a_pstFile->private_data = NULL;
    }

    return 0;
}

static int MT6577_MDP_Flush(struct file * a_pstFile , fl_owner_t a_id)
{
    stMDPOpLog * pstLog;

    MDPK_SHOWFUNCTION( 1 );

    pstLog = (stMDPOpLog *)a_pstFile->private_data;

    if(NULL == pstLog)
    {
        MDPK_ERROR("Private data is null in flush operation. HOW COULD THIS HAPPEN ??\n");
        return 0;
    }

    /*.............................................................................
        Process ID protect. 
      .............................................................................*/
    
    if( (pstLog->tgid == 0) && (pstLog->u4OwnResource != 0) )
    {
        MDPK_ERROR("Flush(): pstLog->tgid(0x%08X) == 0 && pstLog->u4OwnResource(0x%08X) != 0\n",
                    (unsigned int)pstLog->tgid, (unsigned int)pstLog->u4OwnResource);
        return 0;
    }
    
    if( (pstLog->tgid != current->tgid) && ( strcmp( current->comm, "binder" ) != 0 )   )
    {
        MDPK_WARNING("FLUSH() by different process. owner:%d, current = ( process, pid, tgid ) = ( %s, %d, %d ) Rsc=0x%08X\n",
                   pstLog->tgid, current->comm , current->pid, current->tgid,
                   (unsigned int)pstLog->u4OwnResource );
        return 0;
    }
    

    if(pstLog->u4OwnResource)
    {
        MDPK_ERROR("Resource : 0x%08X owned by %d(TGID) an not released. Force to release it in flush(). current = ( process, pid, tgid ) = ( %s, %d, %d )\n", 
            (unsigned int)pstLog->u4OwnResource , (int)pstLog->tgid,
            current->comm , current->pid, current->tgid );

        //TODO:
        // Step 1 : Check OwnResource and Turn off engines
        //.....

        // Step 2 : Release resource, and turn off clock gate
        MT6577_UnLockResource(pstLog->u4OwnResource , pstLog);
    }

    return 0;
}


//TODO : design command

static int MDP_DoIoctl( struct inode *  a_pstInode,
                        struct file *   a_pstFile,
                        unsigned int    a_u4Command,
                        unsigned long   a_u4Param )
{
    int i4RetVal = 0;

    stMDPOpLog * pstLog;

    MDPK_SHOWFUNCTION( 1 );
    //MDPK_INFO("ioctl command = 0x%08X\n", (unsigned int)a_u4Command );

    pstLog = (stMDPOpLog *)a_pstFile->private_data;

    if(NULL == pstLog)
    {
        MDPK_ERROR("Private data is null in ioctl operation. HOW COULD THIS HAPPEN ??\n");
        return -1;
    }

    
    /*.............................................................................
        Process ID protect. 
        In principle, only Lock() can be execute when pid is different(exactly pid = 0 ).
        Almost all other io controls will check if pid is consistent.
      .............................................................................*/
    if( ( pstLog->tgid != current->tgid ) 
        && ( a_u4Command != MT6577MDP_T_LOCKRESOURCE) 
        //&& ( a_u4Command != MT6577MDP_X_WAITIRQ)          //Deque will invoke in another thread
        && ( a_u4Command != MT6577MDP_T_DUMPREG)            //Utility
        && ( a_u4Command != MT6577MDP_G_PMEMRANGE)          //Utility
        && ( a_u4Command != MDPK_BITBLT_G_WAIT_REQUEST)     //Kernel-Trap API,use before lock
        && ( a_u4Command != MDPK_BITBLT_T_INFORM_DONE)      //Kernel-Trap API,use before lock
        && ( a_u4Command != MT6577MDP_T_MVALOOPMEMTRACK )   //Register/unregister MVA before lock resource
        && ( a_u4Command != MDPIO_T_CPUCLOCKGET  )          //Get CPU clock
      )
    {
        MDPK_ERROR("IOCTL()-%s by different process. owner pid:%d, current = ( process, pid, tgid ) = ( %s, %d, %d )\n",
                    MdpIoctlGetCmdStr(a_u4Command),
                    pstLog->tgid, current->comm , current->pid, current->tgid );
        return -1;
    }
    

    switch(a_u4Command)
    {
        case MT6577MDP_T_LOCKRESOURCE :
            if( ( pstLog->tgid == 0 ) || ( pstLog->tgid == current->tgid) )
            {
                i4RetVal = MT6577_LockResource(a_u4Param , pstLog);
            } else
            {
                i4RetVal = -1;
                MDPK_ERROR("IOCTL()-lock() by different process. owner pid:%d, current = ( process, pid, tgid ) = ( %s, %d, %d )\n",
                           pstLog->tgid, current->comm , current->pid, current->tgid );
            }
            
            break;

        case MT6577MDP_T_UNLOCKRESOURCE :
            i4RetVal = MT6577_UnLockResource(a_u4Param , pstLog);
            break;

        case MT6577MDP_X_WAITIRQ :
            i4RetVal = MT6577_WAIT_IRQ(a_u4Param);
            break;

        case MT6577MDP_T_CLRIRQ :
            MT6577_CLR_IRQ(a_u4Param);
            break;

        case MT6577MDP_T_DUMPREG :
            MT6577MDP_DUMPREG();
            break;

        case MT6577MDP_G_TIMESTAMP :
            i4RetVal = MT6577_TimeStamp( (unsigned long)NULL, 1 , (stTimeStamp *)a_u4Param);
            break;
        
        case MT6577MDP_G_PMEMRANGE :
            i4RetVal = MT6577_CheckPmemRange((stPMEMRange *)a_u4Param);
            break;

        case MT6577MDP_T_SETZOOM:
            i4RetVal = MT6577_SetZoom((stZoomSetting *)a_u4Param);
            break;

        case MT6577MDP_T_SETZSDZOOM:
            i4RetVal = MT6577_SetZSDZoom((stZSDZoomReg *)a_u4Param);
            break;

// last preview frame for zsd
        case MT6577MDP_T_SETZSDPREVIEWFRAME:
            i4RetVal = MT6577_SetZSDPreview((stZSDPreviewReg *)a_u4Param);
            break;

        case MDPK_BITBLT_G_WAIT_REQUEST:
            i4RetVal = MDPK_Bitblt_IOCTL_Wait_Request( a_u4Param );
            break;

        case MDPK_BITBLT_T_INFORM_DONE:
            i4RetVal = MDPK_Bitblt_IOCTL_Inform_Done( a_u4Param );
            break;

        case MT6577MDP_T_MVALOOPMEMTRACK:
            i4RetVal = MT6577_MvaLoopMemTrack((MDPIOCTL_MvaLoopMemTrack *)a_u4Param);
            break;

        case MDPIO_T_CPUCLOCKGET:
            i4RetVal = MDPK_CpuClockGet((MDPIOCTL_CpuClockGet *)a_u4Param);
            break;

        default :
            MDPK_ERROR("MT6577 MDP ioctl : No such command!!\n");
            i4RetVal = -EINVAL;
        break;
    }


    return i4RetVal;
}

#if defined( MDPK_FLAG_SUPPORT_UNLOCKED_IOCTL )

static long MT6577_MDP_Unlocked_Ioctl(struct file * a_pstFile,
                                      unsigned int a_u4Command,
                                      unsigned long a_u4Param )
{
    long            retval = 0;
    struct inode *  a_pstInode = a_pstFile->f_path.dentry->d_inode;

    retval = (long)MDP_DoIoctl( a_pstInode, a_pstFile, a_u4Command, a_u4Param );

    return retval;
}

#else

static int MT6577_MDP_Ioctl( struct inode * a_pstInode,
                             struct file * a_pstFile,
                             unsigned int a_u4Command,
                             unsigned long a_u4Param )
{
    return MDP_DoIoctl( a_pstInode, a_pstFile, a_u4Command, a_u4Param );
}

#endif


static int MT6577_MDP_mmap(struct file * a_pstFile, struct vm_area_struct * a_pstVMArea)
{
    stMDPOpLog *    pstLog;
    unsigned long   from_addr;

    pstLog     = (stMDPOpLog *)a_pstFile->private_data;
    from_addr  = (unsigned long)( a_pstVMArea->vm_pgoff * PAGE_SIZE );


    if(NULL == pstLog)
    {
        MDPK_ERROR("Private data is null in mmap operation. HOW COULD THIS HAPPEN ??\n");
        return -1;
    }


    #if 0 /*Cancel the protection of mmap, since the strange usage of 3d factory mode*/
    /*.............................................................................
        Process ID protect.
      .............................................................................*/
    if( pstLog->tgid != current->tgid )
    {
        MDPK_ERROR("MMAP() by different process. owner:%d, current = ( process, pid, tgid ) = ( %s, %d, %d )\n",
                   pstLog->tgid, current->comm , current->pid, current->tgid );
        return -1;
    }
    
    #endif



    if( ( from_addr >= VMALLOC_START ) && ( from_addr <= VMALLOC_END ) )
    {
        
        MDPK_DEBUG("from_addr = 0x%08X, vm_start = 0x%08X, VMALLOC_START = 0x%08X,  VMALLOC_END = 0x%08X\n",
                    (unsigned int)from_addr, (unsigned int)a_pstVMArea->vm_start, (unsigned int)VMALLOC_START, (unsigned int)VMALLOC_END);
        MDPK_DEBUG("Call MDPK_Util_Map_Vmalloc_to_User\n");
        if( MDPK_Util_Map_Vmalloc_to_User( a_pstVMArea ) != 0 )
        {
            MDPK_ERROR("MMAP vmalloc failed!!\n");
            return -1;
        }
        
    } 
    else
    {

        a_pstVMArea->vm_page_prot = pgprot_noncached(a_pstVMArea->vm_page_prot);

                                          /*to addr of user space*//*from addr of kernel space*/
        if( remap_pfn_range( a_pstVMArea , a_pstVMArea->vm_start , a_pstVMArea->vm_pgoff , 
    	                    (a_pstVMArea->vm_end - a_pstVMArea->vm_start) , a_pstVMArea->vm_page_prot ) 
    	                   )
        {
            MDPK_ERROR("MMAP failed!!\n");
            return -1;
        }
    }

    return 0;
}


/*=============================================================================
    Register char devices driver
    (Called during probe()@platform_driver )
  =============================================================================*/
static dev_t                        g_mt6577_mdp_device_number = MKDEV(MT6577_MDP_DEV_MAJOR_NUMBER,0);/*No need to set initial major #, because we use dynamic major number allocate*/
static struct cdev*                 g_mt6577_mdp_char_device = NULL;
static const struct file_operations g_mt6577_mdp_file_operations = 
                                    {
                                        .owner      = THIS_MODULE,
                                        .open       = MT6577_MDP_Open,
                                        .release    = MT6577_MDP_Release,
                                        .flush      = MT6577_MDP_Flush,
                                        
                                        #if defined( MDPK_FLAG_SUPPORT_UNLOCKED_IOCTL )
                                        .unlocked_ioctl = MT6577_MDP_Unlocked_Ioctl,
                                        #else
                                        .ioctl          = MT6577_MDP_Ioctl,
                                        #endif
                                        
                                        .mmap       = MT6577_MDP_mmap
                                    };

static inline int RegisterMT6577MDPCharDrv(void)
{
    MDPK_SHOWFUNCTION( 1 );

    /*1. Kernel dynamic allocate major number*/
    if( alloc_chrdev_region(&g_mt6577_mdp_device_number, 0, 1,"mt6575-MDP") ) /* FIXME. mt6575-MDP sync to MHAL and init.rc */
    {
        MDPK_ERROR("Allocate device # failed!\n");
        return -EAGAIN;
    }
    MDPK_INFO("device major:minor number = %d:%d\n\r", MAJOR(g_mt6577_mdp_device_number),MINOR(g_mt6577_mdp_device_number));

    /*2. Allocate a char devices*/
    g_mt6577_mdp_char_device = cdev_alloc();

    if(NULL == g_mt6577_mdp_char_device)
    {
        unregister_chrdev_region(g_mt6577_mdp_device_number, 1);
        MDPK_ERROR("Allocate char device failed\n");
        return -ENOMEM;
    }

    /*3. Fill in char devices with file operations*/
    cdev_init(g_mt6577_mdp_char_device, &g_mt6577_mdp_file_operations);
    g_mt6577_mdp_char_device->owner = THIS_MODULE;

    

    /*4. Tell the kernel this char device*/
    if(cdev_add(g_mt6577_mdp_char_device, g_mt6577_mdp_device_number, 1))
    {
        unregister_chrdev_region(g_mt6577_mdp_device_number, 1);
        MDPK_ERROR("cdev add error.\n");
        return -EAGAIN;
    }

    return 0;

}

static inline void UnregisterMT6577MDPCharDrv(void)
{
    MDPK_SHOWFUNCTION( 1 );
    
    /*Release char device*/
    cdev_del(g_mt6577_mdp_char_device);
    /*Release char device major number*/
    unregister_chrdev_region(g_mt6577_mdp_device_number, 1);
}


/*=============================================================================
    Member function for platform_driver
        
          static int MT6577MDP_probe(struct platform_device *pdev);
          static int MT6577MDP_remove(struct platform_device *pdev);
          static int MT6577MDP_suspend(struct platform_device *pdev, pm_message_t mesg);
          static int MT6577MDP_resume(struct platform_device *pdev);
          
  =============================================================================*/
static struct class *g_p_mt6577_mdp_class = NULL;

/* Probe() will be invoked when "device register" during bsp initial code
   Called to probe if the device really exists. and create the semaphores*/
static int MT6577MDP_probe(struct platform_device *pdev)
{
    struct device* mdp_device = NULL;

    MDPK_SHOWFUNCTION( 1 );

    //Check platform_device parameters
    if(NULL == pdev)
    {
        MDPK_ERROR("platform data missed\n");
        return -ENXIO;
    }

    /*1.Create a char device and register to Kernel */
    /*  proc/devices will have record               */
    if(RegisterMT6577MDPCharDrv())
    {
        dev_err(&pdev->dev,"register char failed\n");
        return -EAGAIN;
    }

    /*2.Create device driver file under /dev/       */
    g_p_mt6577_mdp_class = class_create(THIS_MODULE, "idpdrv");
    if (IS_ERR(g_p_mt6577_mdp_class)) 
    {
        int ret = PTR_ERR(g_p_mt6577_mdp_class);
        MDPK_ERROR("Unable to create class, err = %d\n", ret);
        return ret;            
    }    
    mdp_device = device_create(g_p_mt6577_mdp_class, NULL, g_mt6577_mdp_device_number, NULL, "mt6575-MDP");


    /*3.Register ISR*/ 
    #if defined( MDP_KERNEL_FLAG_1_SUPPORT_INT )
    MDPK_INFO("MDP run in interrupt mode.\n");
    if( MdpIsr_Register( pdev->name ) != 0 )
    {
        return -ENODEV;
    }
    #else
    MDPK_INFO("MDP run in polling mode.\n");
    #endif /*~MDP_KERNEL_FLAG_1_SUPPORT_INT*/
    

    MDPK_INFO("probe MT6577 MDP success\n");

    return 0;
}

// Called when the device is being detached from the driver
static int MT6577MDP_remove(struct platform_device *pdev)
{
    MDPK_SHOWFUNCTION( 1 );
    

    //unregister char driver.
    UnregisterMT6577MDPCharDrv();


    device_destroy(g_p_mt6577_mdp_class, g_mt6577_mdp_device_number);

    class_destroy(g_p_mt6577_mdp_class);
    

    return 0;
}

static int MT6577MDP_suspend(struct platform_device *pdev, pm_message_t mesg)
{
    MDPK_SHOWFUNCTION( 0 );
    
    return 0;
}

static int MT6577MDP_resume(struct platform_device *pdev)
{
    MDPK_SHOWFUNCTION( 0 );
    
    return 0;
}


static struct platform_driver g_mt6577_mdp_platform_driver = 
{
    .probe		= MT6577MDP_probe,
    .remove     = MT6577MDP_remove,
    .suspend	= MT6577MDP_suspend,
    .resume	    = MT6577MDP_resume,
    .driver		= 
    {
        .name	= "mt6575-MDP",  /* FIXME. Sync to MHAL and init.rc */  /*Is this the name of device file under /dev/ ? */
        .owner	= THIS_MODULE,
    }
};
/*=============================================================================
  Dump Register (For both kernel and Proc )
  =============================================================================*/
/*
    out_target: 0: proc use sprintf 1:printk
 */
#define MDPREGDUMP_OUT( _out_target_, _buffer_, _off_current_, _off_start_, _off_end_, _len_, _fmt_, _arg_... )\
            if( _out_target_ == 0 ){\
                int local_len;\
                local_len = sprintf( (_buffer_), _fmt_, ##_arg_);\
                _off_current_ += local_len;\
                if     ( _off_current_ <= _off_start_ )   _len_ += 0;\
                else if( _off_current_ >= _off_end_ ) { _off_current_ -= local_len; *over=1; return len; }\
                else  _len_ += local_len;\
            }else if( _out_target_ == 1 ){\
                printk( _fmt_, ##_arg_);\
            }

static int MdpRegDump(  int out_target , unsigned char* buffer, 
                        unsigned long* p_off_current, unsigned long off_start, unsigned long off_end, int* over, 
                        char* mdp_name, unsigned long addr_start, unsigned long range  )
{
    int len         = 0;
    unsigned long   addr, addr_end;
    unsigned long   irq_state;

    *over = 0;

    addr_end = addr_start + range;

if( out_target == 1 ){
    local_irq_save(irq_state);
}
        
    MDPREGDUMP_OUT( out_target, buffer+len, *p_off_current, off_start, off_end, len,"\n");
    MDPREGDUMP_OUT( out_target, buffer+len, *p_off_current, off_start, off_end, len,"................................................\n");
    MDPREGDUMP_OUT( out_target, buffer+len, *p_off_current, off_start, off_end, len,"<%s>:\n",mdp_name ); 
    MDPREGDUMP_OUT( out_target, buffer+len, *p_off_current, off_start, off_end, len,"Address     3 2 1 0  7 6 5 4  B A 9 8  F E D C\n");
                                           /*0xF0030000 xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx*/

    for( addr = addr_start; addr < addr_end; addr+=16 )
    {
    MDPREGDUMP_OUT( out_target, buffer+len, *p_off_current, off_start, off_end, len,"0x%08X %08X %08X %08X %08X\n",(unsigned int)addr,   
        (unsigned int)*((unsigned long*)(addr)),
        (unsigned int)*((unsigned long*)(addr+4)), 
        (unsigned int)*((unsigned long*)(addr+8)), 
        (unsigned int)*((unsigned long*)(addr+12))
        );
    }
    MDPREGDUMP_OUT( out_target, buffer+len, *p_off_current, off_start, off_end, len,"\n");
    
    MDPREGDUMP_OUT( out_target, buffer+len, *p_off_current, off_start, off_end, len,"................................................\n");
    MDPREGDUMP_OUT( out_target, buffer+len, *p_off_current, off_start, off_end, len,"\n");

if( out_target == 1 )    {
local_irq_restore(irq_state );
}


    return len;
    
}

void MT6577MDP_DUMPREG(void)
{

    /*all this variable are dummy*/
    unsigned long   off_current, off_start, off_end;
    //int byte_out = 0;
    int over;

    off_current = 0;
    off_start = 0;
    off_end = 0;


    //------
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "MMSYS1", MMSYS1_CONFIG_BASE, MT6577_MMSYS1_REG_RANGE );
    //------
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "RDMA0", R_DMA0_BASE, MT6577_RDMA0_REG_RANGE  );
    //------
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "RDMA1", R_DMA1_BASE, MT6577_RDMA1_REG_RANGE  );
    //------
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "BRZ", BRZ_BASE, MT6577_BRZ_REG_RANGE      );
    //------
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "CRZ", CRZ_BASE, MT6577_CRZ_REG_RANGE          );
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "PRZ0", PRZ0_BASE, MT6577_PRZ0_REG_RANGE          );
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "PRZ1", PRZ1_BASE, MT6577_PRZ1_REG_RANGE          );
    //------
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "VRZ0", VRZ0_BASE, MT6577_VRZ0_REG_RANGE          );
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "VRZ1", VRZ1_BASE, MT6577_VRZ1_REG_RANGE          );
    //------
    //MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "IPP", IMGPROC_BASE, MT6577_IPP_REG_RANGE  );
    //------
    //MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "OVL", OVL_DMA_BASE, MT6577_OVL_DMA_REG_RANGE   );
    //------
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "RGB0", RGB_ROT0_BASE, MT6577_RGB_ROT0_REG_RANGE );
    //------
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "RGB1", RGB_ROT1_BASE, MT6577_RGB_ROT1_REG_RANGE );
    //------
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "RGB2", RGB_ROT2_BASE, MT6577_RGB_ROT2_REG_RANGE );
    //------
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "VDO0", VDO_ROT0_BASE, MT6577_VDO_ROT0_REG_RANGE );
    //------    
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "VDO1", VDO_ROT1_BASE, MT6577_VDO_ROT1_REG_RANGE );
    //------
    MdpRegDump( 1 , NULL, &off_current, off_start, off_end, &over, "JPGDMA", JPG_DMA_BASE, MT6577_JPEG_DMA_REG_RANGE  );
   

  
}


/*=============================================================================
  Proc Function
  =============================================================================*/
/*-----------------------------------------------------------------------------
    /proc/driver/mdp_reg
  -----------------------------------------------------------------------------*/
int MdpProc_mdp_reg_read(   char *page, char **start, off_t off,
                            int count,  int *eof, void *data )
{

    //static int content = 0;

    unsigned long   off_current, off_start, off_end;
    int byte_out = 0;
    int over;

    printk("page=0x%X start=0x%X off=%d count=%d eof=%d data=0x%X\n",
              (unsigned int)page, (unsigned int)*start, (int)off, count,  *eof, (unsigned int)data );

    off_current = 0;
    off_start = off;
    off_end = off + count - 80;/*80 is margin*/


    *start  = page;

    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "MMSYS1", MMSYS1_CONFIG_BASE, MT6577_MMSYS1_REG_RANGE );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "RDMA0", R_DMA0_BASE, MT6577_RDMA0_REG_RANGE  );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "RDMA1", R_DMA1_BASE, MT6577_RDMA1_REG_RANGE  );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "BRZ", BRZ_BASE, MT6577_BRZ_REG_RANGE      );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "CRZ", CRZ_BASE, MT6577_CRZ_REG_RANGE          );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "PRZ0", PRZ0_BASE, MT6577_PRZ0_REG_RANGE          );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "PRZ1", PRZ1_BASE, MT6577_PRZ1_REG_RANGE          );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "VRZ0", VRZ0_BASE, MT6577_VRZ0_REG_RANGE          );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "VRZ1", VRZ1_BASE, MT6577_VRZ1_REG_RANGE          );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "IPP", IMGPROC_BASE, MT6577_IPP_REG_RANGE  );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "OVL", OVL_DMA_BASE, MT6577_OVL_DMA_REG_RANGE   );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "RGB0", RGB_ROT0_BASE, MT6577_RGB_ROT0_REG_RANGE );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "RGB1", RGB_ROT1_BASE, MT6577_RGB_ROT1_REG_RANGE );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "RGB2", RGB_ROT2_BASE, MT6577_RGB_ROT2_REG_RANGE );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "VDO0", VDO_ROT0_BASE, MT6577_VDO_ROT0_REG_RANGE );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------    
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "VDO1", VDO_ROT1_BASE, MT6577_VDO_ROT1_REG_RANGE );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;
    //------
    byte_out += MdpRegDump( 0 , page+byte_out, &off_current, off_start, off_end, &over, "JPGDMA", JPG_DMA_BASE, MT6577_JPEG_DMA_REG_RANGE  );
    printk("byte_out=%d  off_current=%lu over=%d\n", byte_out,off_current, over);
    if( over ) goto end;


end:
    if( byte_out == 0 ) 
        *eof=1;

    return byte_out;
  
}



int MdpProc_mdp_reg_write(struct file *file, const char __user *buffer,
                   unsigned long count, void *data)
{

    char cmd_buf[64] = {'\0'}; 
    unsigned long buf_size = (count < (sizeof(cmd_buf) - 1)) ? (count) : (sizeof(cmd_buf) - 1);
    unsigned long addr = 0;
    unsigned long value = 0;

    if (copy_from_user(cmd_buf, buffer, buf_size))
        return -EFAULT;

    if (sscanf(cmd_buf, "%x %x",  (unsigned int *)&addr, (unsigned int *)&value) == 2) {
        #if 0
        iowrite32(value, addr);
        MDPK_MSG("[MDPK] write addr = 0x%08x, value = 0x%08x\n", (unsigned int)addr, (unsigned int)ioread32(addr));
        #else
        MDPK_MSG("[MDPK] write addr function is disabled. write addr = 0x%08x, value = 0x%08x\n", (unsigned int)addr, (unsigned int)ioread32(addr));
        #endif

        
    }
    else if (sscanf(cmd_buf, "%x", (unsigned int *)&addr) == 1) {
        MDPK_MSG("[MDPK] read addr = 0x%08x, value = 0x%08x\n", (unsigned int)addr, (unsigned int)ioread32(addr));
    }

    return count;
    
    
}


/*-----------------------------------------------------------------------------
    /proc/driver/mdp_flag
  -----------------------------------------------------------------------------*/
#define MDPFLAG_DUMPOUT( _target_, _buffer_, _var_, _len_ )\
    if( _target_ == 0 ){\
    _len_ += sprintf( _buffer_, "%s = %d\n", #_var_, (int)_var_ );\
    }else{\
        MDPK_MSG( "[MDPK]Read %s = %d\n", #_var_, (int)_var_ );\
    }
    

int MdpProc_mdp_flag_read(   char *page, char **start, off_t off,
                            int count,  int *eof, void *data )
{
    int len = 0;

    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_show_info , len );
    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_show_warning , len );
    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_show_error , len );
    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_show_isrinfo , len );
    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_show_powerinfo , len );
    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_show_zoominfo , len );
    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_show_fpsinfo , len );
    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_show_bbinfo , len );
    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_show_zsdinfo , len );
    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_timeout_dumpreg , len );
    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clock_all_always_on , len );          //Clock section begin--------------------------
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_jpeg_dec_always_on , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_jpeg_enc_always_on , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_cam_always_on      , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_brz_always_on      , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_crz_always_on      , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_prz0_always_on     , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_prz1_always_on     , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_vrz0_always_on     , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_vrz1_always_on     , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_r_dma0_always_on   , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_r_dma1_always_on   , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_ovl_always_on      , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_ipp_always_on      , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_mout_always_on     , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_lcd_always_on      , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_jpeg_dma_always_on , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_eis_always_on      , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_rgb_rot0_always_on , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_rgb_rot1_always_on , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_rgb_rot2_always_on , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_vdo_rot0_always_on , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_vdo_rot1_always_on , len );
        MDPFLAG_DUMPOUT( 0, page + len, b_mdp_clk_tv_rot_always_on   , len );   //Clock section end--------------------------
    MDPFLAG_DUMPOUT( 0, page + len, b_mdp_pd_path , len );
    

    *eof = 1;
    
    return len;
}


#define MDPFLAG_VARSET( _in_var_str_, _var_, _value_, _flag_)\
    if( strcmp( _in_var_str_, #_var_ ) == 0 ){\
        _var_ = _value_;\
        _flag_ = 1;\
        MDPK_MSG( "[MDPK]Set %s = %d\n", #_var_, (int)_value_ );\
    }


int MdpProc_mdp_flag_write(struct file *file, const char __user *buffer,
                   unsigned long count, void *data)
{

    char            cmd_buf[64] = {'\0'}; 
    unsigned long   buf_size = (count < (sizeof(cmd_buf) - 1)) ? (count) : (sizeof(cmd_buf) - 1);
    char            variable_name[64] = {'\0'}; 
    unsigned long   value = 0;
    int             b_set = 0;
    int             dummy;

    if (copy_from_user(cmd_buf, buffer, buf_size))
        return -EFAULT;

    if (sscanf(cmd_buf, "%s %x",  variable_name, (unsigned int *)&value) == 2) {

        MDPFLAG_VARSET( variable_name, b_mdp_show_info, value, b_set);
        MDPFLAG_VARSET( variable_name, b_mdp_show_warning, value, b_set);
        MDPFLAG_VARSET( variable_name, b_mdp_show_error, value, b_set);
        MDPFLAG_VARSET( variable_name, b_mdp_show_isrinfo, value, b_set);
        MDPFLAG_VARSET( variable_name, b_mdp_show_powerinfo, value, b_set);
        MDPFLAG_VARSET( variable_name, b_mdp_show_zoominfo, value, b_set);
        MDPFLAG_VARSET( variable_name, b_mdp_show_fpsinfo, value, b_set);
        MDPFLAG_VARSET( variable_name, b_mdp_show_bbinfo, value, b_set);
        MDPFLAG_VARSET( variable_name, b_mdp_show_zsdinfo, value, b_set);
        MDPFLAG_VARSET( variable_name, b_mdp_timeout_dumpreg, value, b_set);
        MDPFLAG_VARSET( variable_name, b_mdp_clock_all_always_on, value, b_set);        //Clock section begin--------------------------
            MDPFLAG_VARSET( variable_name, b_mdp_clk_jpeg_dec_always_on, value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_jpeg_enc_always_on, value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_cam_always_on     , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_brz_always_on     , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_crz_always_on     , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_prz0_always_on    , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_prz1_always_on    , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_vrz0_always_on    , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_vrz1_always_on    , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_r_dma0_always_on  , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_r_dma1_always_on  , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_ovl_always_on     , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_ipp_always_on     , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_mout_always_on    , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_lcd_always_on     , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_jpeg_dma_always_on, value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_eis_always_on     , value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_rgb_rot0_always_on, value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_rgb_rot1_always_on, value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_rgb_rot2_always_on, value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_vdo_rot0_always_on, value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_vdo_rot1_always_on, value, b_set);
            MDPFLAG_VARSET( variable_name, b_mdp_clk_tv_rot_always_on  , value, b_set);//Clock section End--------------------------
        MDPFLAG_VARSET( variable_name, b_mdp_pd_path, value, b_set);
        

        if( b_set == 0 ){
            MDPK_ERROR("No such variable to set!\n");
        }
    }
    //else if (sscanf(cmd_buf, "%x", &addr) == 1) {
    else
    {
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_show_info , dummy );
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_show_warning , dummy );
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_show_error , dummy );
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_show_isrinfo , dummy );
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_show_powerinfo , dummy );
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_show_zoominfo , dummy );
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_show_fpsinfo , dummy );
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_show_bbinfo , dummy );
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_show_zsdinfo , dummy );
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_timeout_dumpreg , dummy );
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clock_all_always_on , dummy );          //Clock section begin--------------------------
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_jpeg_dec_always_on , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_jpeg_enc_always_on , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_cam_always_on      , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_brz_always_on      , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_crz_always_on      , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_prz0_always_on     , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_prz1_always_on     , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_vrz0_always_on     , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_vrz1_always_on     , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_r_dma0_always_on   , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_r_dma1_always_on   , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_ovl_always_on      , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_ipp_always_on      , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_mout_always_on     , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_lcd_always_on      , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_jpeg_dma_always_on , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_eis_always_on      , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_rgb_rot0_always_on , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_rgb_rot1_always_on , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_rgb_rot2_always_on , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_vdo_rot0_always_on , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_vdo_rot1_always_on , dummy );
            MDPFLAG_DUMPOUT( 1, NULL, b_mdp_clk_tv_rot_always_on   , dummy );   //Clock section end--------------------------
        MDPFLAG_DUMPOUT( 1, NULL, b_mdp_pd_path , dummy );
        
    }

    return count;
    
    
}


/*-----------------------------------------------------------------------------
    /proc/driver/mdp_test
  -----------------------------------------------------------------------------*/
int MdpProc_mdp_test_read(   char *page, char **start, off_t off,
                            int count,  int *eof, void *data )
{
    int len = 0;

    len += sprintf( page, "mdp_test read\n");

    *eof = 1;
    
    return len;
}



int MdpProc_mdp_test_write(struct file *file, const char __user *buffer,
                   unsigned long count, void *data)
{

    char            cmd_buf[64] = {'\0'}; 
    unsigned long   buf_size = (count < (sizeof(cmd_buf) - 1)) ? (count) : (sizeof(cmd_buf) - 1);
    unsigned long   cmd_no;

    if (copy_from_user(cmd_buf, buffer, buf_size))
        return -EFAULT;

    sscanf(cmd_buf, "%lu",  &cmd_no);

    MDPK_MSG("[MDPK] cmd_no = %lu\n", cmd_no );

    switch( cmd_no )
    {
        case 0:
            MDPK_TestFunc_1( MDPK_BB_CH_HDMI_0 );
            break;

        case 1:
            MDPK_TestFunc_1( MDPK_BB_CH_HDMI_1 );
            break;

        case 2:
            MDPK_TestFunc_2( MDPK_BB_CH_HDMI_2 );
            break;

        case 3:
            MDPK_TestFunc_2( MDPK_BB_CH_HDMI_3 );
            break;

        case 5:
            MdpMvaLoopMemStatisticDump( 0, 3 );
            break;
    }
    

    return count;
    
    
}





int MdpProc_init( void )
{
    int     ret_val = 0;
    struct proc_dir_entry *p_proc_entry;
        
    p_proc_entry = create_proc_entry("driver/mdp_reg", 0, NULL);
    if (p_proc_entry) {
        p_proc_entry->read_proc  = MdpProc_mdp_reg_read;
        p_proc_entry->write_proc = MdpProc_mdp_reg_write;
    }
    else {
        MDPK_ERROR("add /proc/driver/mdp_reg entry fail \n");
        ret_val = -1;
    }

    
    p_proc_entry = create_proc_entry("driver/mdp_flag", 0, NULL);
    if (p_proc_entry) {
        p_proc_entry->read_proc  = MdpProc_mdp_flag_read;
        p_proc_entry->write_proc = MdpProc_mdp_flag_write;
    }
    else {
        MDPK_ERROR("add /proc/driver/mdp_flag entry fail \n");
        ret_val = -1;
    }


    
    p_proc_entry = create_proc_entry("driver/mdp_test", 0, NULL);
    if (p_proc_entry) {
        p_proc_entry->read_proc  = MdpProc_mdp_test_read;
        p_proc_entry->write_proc = MdpProc_mdp_test_write;
    }
    else {
        MDPK_ERROR("add /proc/driver/mdp_test entry fail \n");
        ret_val = -1;
    }



    return ret_val;
    
}


/*=============================================================================
  Kernel Thread Test Code
  =============================================================================*/
#if 0
static DECLARE_WAIT_QUEUE_HEAD(myevent_waitqueue);
rwlock_t myevent_lock;
/*extern*/ unsigned int myevent_id;  /* Holds the identity of the
                                    troubled data structure.
                                    Populated later on */
static int MdpKernelThread(void *unused)
{

    unsigned int event_id = 0;
    DECLARE_WAITQUEUE(wait, current);

    MDPK_MSG("[MDP]Hi! I am Kernel Thread!(%d)\n", current->pid);


    /* Become a kernel thread without attached user resources */
    daemonize("MdpKernelThread");

    /* Request delivery of SIGKILL */
    allow_signal(SIGKILL);

    /* The thread sleeps on this wait queue until it's
    woken up by parts of the kernel in charge of sensing
    the health of data structures of interest */
    add_wait_queue(&myevent_waitqueue, &wait);

    for (;;) 
    {
        MDPK_MSG("[MDP]Kernel Thread Loop!\n");
        
        /* Relinquish the processor until the event occurs */
        set_current_state(TASK_INTERRUPTIBLE);
        MDPK_MSG("[MDP]Kernel Thread Sleep!\n");
        schedule();  /* Allow other parts of the kernel to run */
        MDPK_MSG("[MDP]Kernel Thread Wakeup!\n");
        /* Die if I receive SIGKILL */
        if (signal_pending(current)) break;
        /* Control gets here when the thread is woken up */
        read_lock(&myevent_lock);      /* Critical section starts */
        if (myevent_id) 
        { /* Guard against spurious wakeups */
            event_id = myevent_id;
            read_unlock(&myevent_lock); /* Critical section ends */

            #if 0
            /* Invoke the registered user mode helper and
            pass the identity code in its environment */
            run_umode_handler(event_id); /* Expanded later on */
            #endif
            MDPK_MSG("[MDP]Kernel Thread Execute!\n");
        } 
        else 
        {
            read_unlock(&myevent_lock);
        }
    }
    
    set_current_state(TASK_RUNNING);
    remove_wait_queue(&myevent_waitqueue, &wait);
    return 0;
}

#endif
/*=============================================================================
  Module Init/Exit
  =============================================================================*/

static int __init MT6577_MDP_Init(void)
{
    MDPK_SHOWFUNCTION( 1 );

    if(platform_driver_register(&g_mt6577_mdp_platform_driver))
    {
        MDPK_ERROR("failed to register MT6577 MDP driver\n");
        return -ENODEV;
    }


    /*init mdp proc*/
    MdpProc_init();   

    /*Initial Mdpk Api*/
    MDPK_Bitblt_Init();


    /*Kernel Thread Test Code*/
#if 0
    {
        
        int ret;

        MDPK_INFO("\t[MDP]Create Kernel Thread(%d)\n", current->pid);
        
        ret = kernel_thread(MdpKernelThread, NULL,
            CLONE_FS | CLONE_FILES | CLONE_SIGHAND | SIGCHLD);
    }
#endif
    
    

    return 0;
}

static void __exit MT6577_MDP_Exit(void)
{
    MDPK_SHOWFUNCTION( 1 );
    platform_driver_unregister(&g_mt6577_mdp_platform_driver);
}


/*-----------------------------------------------------------------------------*/
EXPORT_SYMBOL(b_mdp_pd_path);


module_init(MT6577_MDP_Init);
module_exit(MT6577_MDP_Exit);
MODULE_DESCRIPTION("MT6577 MDP driver");
MODULE_AUTHOR("Clouds Lee <clouds.lee@mediatek.com>");
MODULE_LICENSE("GPL");



/*-----------------------------------------------------------------------------
    SPC Utility Code
  -----------------------------------------------------------------------------*/
#if 0
#include <mach/mt6577_spc.h>    // locate at alps/mediatek/platform/mt6577/mt6577/kernel/core/include/mach

int MDPK_Util_Spc_Monitor( int b_on, unsigned long start_addr, unsigned long end_addr )
{
    
    SPC_cfg spc_struct;
    
    spc_struct.RegionID = 1;    /*always 1*/
    spc_struct.Enable = ( b_on == 0 ) ? 0 : 1;      

    if( spc_struct.Enable == 0 )
    {
        spc_struct.EngineMaskLARB0 = 0;//LARB0_ALL_EN;
        spc_struct.EngineMaskLARB1 = 0;//LARB1_ALL_EN; 
        spc_struct.EngineMaskLARB2 = 0;//LARB2_ALL_EN;
        spc_struct.EngineMaskLARB3 = 0;//LARB3_ALL_EN;
        
        spc_struct.ReadEn = 0;//1;                        //monitor read operation
        spc_struct.WriteEn = 0;//1;                       //monitor write operation
        
    } else
    {

        spc_struct.EngineMaskLARB0 = LARB0_ALL_EN;
        spc_struct.EngineMaskLARB1 = LARB1_ALL_EN; 
        spc_struct.EngineMaskLARB2 = LARB2_ALL_EN;
        spc_struct.EngineMaskLARB3 = LARB3_ALL_EN;
        
        spc_struct.ReadEn = 1;                        //monitor read operation
        spc_struct.WriteEn = 1;                       //monitor write operation
    }
    
    spc_struct.StartAddr = start_addr;//MM_SYSRAM_BASE_PA;                     // sysram monitor address
    spc_struct.EndAddr = end_addr;//MM_SYSRAM_BASE_PA + MM_SYSRAM_SIZE -1;
    
    spc_config(&spc_struct);                   

    return 0;
}

#endif



