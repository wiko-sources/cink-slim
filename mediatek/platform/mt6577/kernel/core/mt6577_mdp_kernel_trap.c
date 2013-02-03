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
#include <linux/vmalloc.h>  //For vmalloc


#include <linux/xlog.h>  //Prevent include by user space (no good)
//Arch dependent files
#include <mach/sync_write.h>
#include <mach/mt6577_mdp.h>
#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_clock_manager.h>
//#include <mach/mt6577_pll.h>
#include <mach/irqs.h>
//#include <asm/tcm.h> /*MT6577 do not have tcm*/

#define MDPKBITBLIT_TIMEOUT     ( 3000 )


typedef struct 
{
    spinlock_t          lock;               /*void spin_lock_init(spinlock_t *lock);*/
    wait_queue_head_t   request_wait_queue; /*Wait by daemon*/
    wait_queue_head_t   done_wait_queue;    /*Wait by kernel request client;*/
    unsigned long       request_flag;
    int                 b_src_addr_dirty[ MDPKBITBLIT_CHNL_COUNT ];   /*if b_addr_dirty , need to remap*/
    int                 b_dst_addr_dirty[ MDPKBITBLIT_CHNL_COUNT ];   /*if b_addr_dirty , need to remap*/
    MdpkBitbltConfig    config[ MDPKBITBLIT_CHNL_COUNT ];
    int                 result[ MDPKBITBLIT_CHNL_COUNT ];
    
} MdpkBitbltChannels;


static MdpkBitbltChannels g_mdpk_bitblt_chnls;




int MDPK_Bitblt_Init( void )
{
    int i;

    spin_lock_init( &g_mdpk_bitblt_chnls.lock );
    init_waitqueue_head( &g_mdpk_bitblt_chnls.request_wait_queue );
    init_waitqueue_head( &g_mdpk_bitblt_chnls.done_wait_queue );
    g_mdpk_bitblt_chnls.request_flag = 0;

    for( i = 0; i < MDPKBITBLIT_CHNL_COUNT; i++ )
    {
         g_mdpk_bitblt_chnls.b_src_addr_dirty[i] = 0;
         g_mdpk_bitblt_chnls.b_dst_addr_dirty[i] = 0;
    }

    return 0;
}



/*int Mt6577_mHalBitblt(void *a_pInBuffer) mt6577 prefix should not add, this should be platform independent implement*/
int MDPK_Bitblt_Config( int channel, MdpkBitbltConfig* pParam )
{
    unsigned long irq_state;

    if( ( channel >= MDPKBITBLIT_CHNL_COUNT) || (channel < 0) )    {
        MDPK_ERROR("mdpk bitblt channel illegal(=%d), max = %d\n", channel, MDPKBITBLIT_CHNL_COUNT-1 );
        return -1;
    }

    if( pParam == NULL )    {
        MDPK_ERROR("mdpk bitblt channel config parameter is NULL\n");
        return -1;
    }

    /*Check if this channel busy*/
    /*Critical Section*/ 
    spin_lock_irqsave(&g_mdpk_bitblt_chnls.lock, irq_state );
    {
        if( (g_mdpk_bitblt_chnls.request_flag & ( 0x1 << channel )) != 0 )
        {
            MDPK_ERROR("MDPK Bitblt Ch:%d (0x%08X) is busy. Config fail\n", channel, (unsigned int)g_mdpk_bitblt_chnls.request_flag );
            spin_unlock_irqrestore(&g_mdpk_bitblt_chnls.lock, irq_state );
            return -1;
        }
    }
    spin_unlock_irqrestore(&g_mdpk_bitblt_chnls.lock, irq_state );
    /*~Critical Section*/    
    

    /*Critical Section*/    
    spin_lock_irqsave(&g_mdpk_bitblt_chnls.lock, irq_state );
    {
        g_mdpk_bitblt_chnls.b_src_addr_dirty[channel] = 1;
        g_mdpk_bitblt_chnls.b_dst_addr_dirty[channel] = 1;
        memcpy( &(g_mdpk_bitblt_chnls.config[channel]), pParam, sizeof(MdpkBitbltConfig)  );
    }
    spin_unlock_irqrestore(&g_mdpk_bitblt_chnls.lock, irq_state );
    /*~Critical Section*/    


    return 0;
    
}



int MDPK_Bitblt( int channel )
{
    const unsigned long kTIMEOUT = 3000;    //ms
    unsigned long   irq_state;
    int             ret_of_wait;
    
    if( ( channel >= MDPKBITBLIT_CHNL_COUNT) || (channel < 0) )    {
        MDPK_ERROR("mdpk bitblt channel illegal(=%d), max = %d\n", channel, MDPKBITBLIT_CHNL_COUNT-1 );
        return -1;
    }
    
    /*Critical Section*/    
    spin_lock_irqsave(&g_mdpk_bitblt_chnls.lock, irq_state );
    {
        if( (g_mdpk_bitblt_chnls.request_flag & ( 0x1 << channel )) != 0 )
        {
            MDPK_ERROR("MDPK Bitblt Ch:%d (0x%08X) is busy. Config fail\n", channel, (unsigned int)g_mdpk_bitblt_chnls.request_flag );
            spin_unlock_irqrestore(&g_mdpk_bitblt_chnls.lock, irq_state );
            return -1;
        }

        /*Set request flag*/
        g_mdpk_bitblt_chnls.request_flag |= ( 0x1 << channel );

        MDPK_BBINFO("MDPK Bitblt Request Ch:%d (0x%08X)\n", channel, (unsigned int)g_mdpk_bitblt_chnls.request_flag );
    }
    spin_unlock_irqrestore(&g_mdpk_bitblt_chnls.lock, irq_state );
    /*~Critical Section*/    

    

    /*Wake-up Request Wait Queue for daemon*/
    mb();   /*Add memory barrier before the other CPU (may) wakeup*/
    wake_up_interruptible( &(g_mdpk_bitblt_chnls.request_wait_queue) );

    /*Wait Done Wait Queue from daemon*/
    ret_of_wait = wait_event_interruptible_timeout( g_mdpk_bitblt_chnls.done_wait_queue,
                                                    ( g_mdpk_bitblt_chnls.request_flag & ( 0x1 << channel ) ) == 0,
                                                    MDPK_Util_MsToJiffies(kTIMEOUT) );
    if( ret_of_wait == 0 )
    {
        MDPK_ERROR("MDPK Timeout. Ch:%d (0x%08X) timeout = %dms\n", channel, (unsigned int)g_mdpk_bitblt_chnls.request_flag, (int)kTIMEOUT );
        return -1;
    }

    return g_mdpk_bitblt_chnls.result[ channel ];
    
}




int MDPK_Bitblt_IOCTL_Wait_Request( unsigned long ioctl_user_param  )
{
    int                         ret_of_wait;
    MDPIOCTL_MdpkBitbltConfig   param;
    int                         ch;
    unsigned long               irq_state;

    
    /*wait until request flag is raised*/
    ret_of_wait = wait_event_interruptible( g_mdpk_bitblt_chnls.request_wait_queue , 
                                            g_mdpk_bitblt_chnls.request_flag != 0  );

    if( ret_of_wait != 0 ){
        return -ERESTARTSYS;
    }

    
    /*Critical Section*/    
    spin_lock_irqsave(&g_mdpk_bitblt_chnls.lock, irq_state );
    {
        int b_found = 0;
        /*find the first request flag*/
        for( ch = 0; ch < MDPKBITBLIT_CHNL_COUNT; ch++ ){
            if( ( g_mdpk_bitblt_chnls.request_flag & ( 0x1 << ch ) ) != 0 ){
                b_found = 1;
                break;
            }
        }

        if( b_found == 0 ){
            MDPK_ERROR("MDPK Bitblt cannot found request flag\n");
            spin_unlock_irqrestore(&g_mdpk_bitblt_chnls.lock, irq_state );
            return -ERESTARTSYS;
        }

        MDPK_BBINFO("Process MDPK Bitblt Channel : %d ( request flag = 0x%08X )\n", ch, (unsigned int)g_mdpk_bitblt_chnls.request_flag );

        param.out_channel          = ch;
        param.out_b_src_addr_dirty = g_mdpk_bitblt_chnls.b_src_addr_dirty[ch];
        param.out_b_dst_addr_dirty = g_mdpk_bitblt_chnls.b_dst_addr_dirty[ch];
        memcpy( &param.out_config, &(g_mdpk_bitblt_chnls.config[ch]), sizeof(MdpkBitbltConfig)  );

        /*Clear address dirty bit,once user retrieve config parameter*/
        g_mdpk_bitblt_chnls.b_src_addr_dirty[ch] = 0;
        g_mdpk_bitblt_chnls.b_dst_addr_dirty[ch] = 0;
    }
    spin_unlock_irqrestore(&g_mdpk_bitblt_chnls.lock, irq_state );
    /*~Critical Section*/    


    if(copy_to_user((void __user *) ioctl_user_param , (void*)&param , sizeof(MDPIOCTL_MdpkBitbltConfig)))
    {
        MDPK_ERROR("MDPK_Bitblt_IOCTL_Config_Get() copy to user failed!!\n");
        return -EFAULT;
    }

    return 0;
}


  

int MDPK_Bitblt_IOCTL_Inform_Done( unsigned long ioctl_user_param  )
{
    int                             channel;
    int                             result;
    unsigned long                   irq_state;
    MDPIOCTL_MdpkBitbltInformDone   param;
        
    if(copy_from_user(&param, (const void __user *)ioctl_user_param , sizeof(MDPIOCTL_MdpkBitbltInformDone)))
    {
        MDPK_ERROR("MDPK_Bitblt_IOCTL_Inform_Done(), Copy from user failed !!\n");
        return -EFAULT;
    }

    channel = param.in_channel;
    result  = param.in_ret_val;

    
    if( ( channel >= MDPKBITBLIT_CHNL_COUNT) || (channel < 0) )    {
        MDPK_ERROR("mdpk bitblt channel illegal(=%d), max = %d\n", channel, MDPKBITBLIT_CHNL_COUNT-1 );
        return -1;
    }

    /*Clear Request Flag and get return value*/
    /*Critical Section*/ 
    spin_lock_irqsave(&g_mdpk_bitblt_chnls.lock, irq_state );
    {
        if( (g_mdpk_bitblt_chnls.request_flag & ( 0x1 << channel )) == 0 )
        {
            MDPK_ERROR("MDPK Bitblt Ch:%d (0x%08X) is free, but a done signal received.\n", channel, (unsigned int)g_mdpk_bitblt_chnls.request_flag );
            spin_unlock_irqrestore(&g_mdpk_bitblt_chnls.lock, irq_state );
            return -1;
        }

        /*Store return value*/
        g_mdpk_bitblt_chnls.result[ channel ] = result;

        /*Clear request flag*/
        g_mdpk_bitblt_chnls.request_flag &= ~( 0x1 << channel );

    }
    spin_unlock_irqrestore(&g_mdpk_bitblt_chnls.lock, irq_state );
    /*~Critical Section*/    


    /*Wake-up Done Wait Queue for Kernel*/
    mb();   /*Add memory barrier before the other CPU (may) wakeup*/
    wake_up_interruptible( &(g_mdpk_bitblt_chnls.done_wait_queue) );

    return 0;
    
}



int MDPK_Util_Map_Vmalloc_to_User( struct vm_area_struct* p_vma )
{
    int             ret;
    unsigned long   pfn;
    void*           vmalloc_addr    = (void*)( p_vma->vm_pgoff * PAGE_SIZE );
    unsigned long   start           = p_vma->vm_start;
    long            size            = (long)(p_vma->vm_end - p_vma->vm_start);

    
    MDPK_BBINFO("Map Kernel Vmalloc (0x%08X) to User Space (0x%08X) (szie=0x%08X)-------\n", 
                (unsigned int)vmalloc_addr, (unsigned int)start, (unsigned int)size );

    
    while( size > 0 )
    {
        pfn = vmalloc_to_pfn( vmalloc_addr );

        /*Debug Info*/
        MDPK_BBINFO("kernel vm=0x%08X pfn==0x%08X\n", (unsigned int)vmalloc_addr, (unsigned int)pfn);

        if( (ret = remap_pfn_range( p_vma, start, pfn, PAGE_SIZE, PAGE_SHARED )) < 0 )
        {
            return ret;
        }

        start           += PAGE_SIZE;
        vmalloc_addr    += PAGE_SIZE;
        size            -= PAGE_SIZE;
        
    }

    return 0;

}

EXPORT_SYMBOL(MDPK_Bitblt);
EXPORT_SYMBOL(MDPK_Bitblt_Config);





/*-----------------------------------------------------------------------------
    Test Harness
  -----------------------------------------------------------------------------*/
void MDPK_TestFunc_1( unsigned long channel)
{

    static void* src_buffer[MDPKBITBLIT_CHNL_COUNT] = { 0 };
    unsigned long src_w = 100;
    unsigned long src_h = 100;
    unsigned long src_size = src_w * src_h * 4;

    
    static void* dst_buffer[MDPKBITBLIT_CHNL_COUNT] = { 0 };
    unsigned long dst_w = 200;
    unsigned long dst_h = 200;
    unsigned long dst_size = dst_w * dst_h * 4;

    MdpkBitbltConfig    config;


    if(  src_buffer[channel] != NULL )   vfree( src_buffer[channel] );
    src_buffer[channel] = vmalloc( src_size );
    MDPK_DEBUG("src_buffer[%lu] = 0x%08X\n", channel, (unsigned int)src_buffer[channel] );

    if(  dst_buffer[channel] != NULL )   vfree( dst_buffer[channel] );
    dst_buffer[channel] = vmalloc( dst_size );
    MDPK_DEBUG("dst_buffer[%lu] = 0x%08X\n", channel, (unsigned int)dst_buffer[channel] );


    
    {
        MDPK_DEBUG("MDPK_Bitblt_Config()\n");

        config.srcX = 0;
        config.srcY = 0;
        config.srcW = src_w;
        config.srcWStride = src_w;
        config.srcH = src_h;
        config.srcHStride = src_h;
        config.srcAddr = (unsigned int)src_buffer[channel];
        config.srcFormat = MDPK_FORMAT_ARGB_8888;
        config.srcBufferSize = src_size; //Note:Kernel Mdpk Api Only
        config.srcMemType = MDPK_MEMTYPE_VMALLOC;

        config.dstW = dst_w;
        config.dstH = dst_h;
        config.dstAddr = (unsigned int)dst_buffer[channel];
        config.dstFormat = MDPK_FORMAT_ARGB_8888;
        config.pitch = dst_w;
        config.dstBufferSize = dst_size; //Note:Kernel Mdpk Api Only
        config.dstMemType = MDPK_MEMTYPE_VMALLOC;

        config.orientation = 0;

        config.u4SrcOffsetXFloat = 0;//0x100 stands for 1, 0x40 stands for 0.25 , etc...
        config.u4SrcOffsetYFloat = 0;//0x100 stands for 1, 0x40 stands for 0.25 , etc...
        
        MDPK_Bitblt_Config( channel, &config );
    }


    {
        int ret = 0;

        MDPK_DEBUG("MDPK_Bitblt()\n");

        ret = MDPK_Bitblt( channel );

        MDPK_DEBUG("MDPK_Bitblt() return %d\n", ret );
        
    }
    


    
}

extern unsigned long    max_pfn;
#define MAX_PFN        ((max_pfn << PAGE_SHIFT) + PHYS_OFFSET)

#define PMEM_MM_START  (MAX_PFN)
#define PMEM_MM_SIZE   (CONFIG_RESERVED_MEM_SIZE_FOR_PMEM)

//#define FB_START       (PMEM_MM_START + PMEM_MM_SIZE)
#define FB_START PMEM_MM_START
#define FB_SIZE        (RESERVED_MEM_SIZE_FOR_FB)


void MDPK_TestFunc_2( unsigned long channel)
{

    static void* src_buffer[MDPKBITBLIT_CHNL_COUNT] = { 0 };
    unsigned long src_w = 100;
    unsigned long src_h = 100;
    unsigned long src_size = src_w * src_h * 4;

    
    static void* dst_buffer[MDPKBITBLIT_CHNL_COUNT] = { 0 };
    unsigned long dst_w = 200;
    unsigned long dst_h = 200;
    unsigned long dst_size = dst_w * dst_h * 4;

    MdpkBitbltConfig    config;

    unsigned long src_pa, dst_pa;


    if( src_buffer[channel] != NULL ){
        iounmap( src_buffer[channel] );
    }
    src_pa = FB_START;
    src_buffer[channel] = ioremap( src_pa, src_size );
    MDPK_DEBUG("src_buffer[%lu] = 0x%08X(PA) ==> 0x%08X\n", channel, (unsigned int)src_pa, (unsigned int)src_buffer[channel] );

    if( dst_buffer[channel] != NULL ){
        iounmap( dst_buffer[channel] );
    }
    dst_pa = ( (FB_START + src_size) +  0xFFF ) & (~0xFFF);
    dst_buffer[channel] = ioremap( dst_pa, dst_size );
    MDPK_DEBUG("dst_buffer[%lu] = 0x%08X(PA) ==> 0x%08X\n", channel, (unsigned int)dst_pa, (unsigned int)dst_buffer[channel] );


    
    {
        MDPK_DEBUG("MDPK_Bitblt_Config()\n");

        config.srcX = 0;
        config.srcY = 0;
        config.srcW = src_w;
        config.srcWStride = src_w;
        config.srcH = src_h;
        config.srcHStride = src_h;
        config.srcAddr = (unsigned int)src_buffer[channel];
        config.srcFormat = MDPK_FORMAT_ARGB_8888;
        config.srcBufferSize = src_size; //Note:Kernel Mdpk Api Only
        config.srcMemType = MDPK_MEMTYPE_PHYMEM;

        config.dstW = dst_w;
        config.dstH = dst_h;
        config.dstAddr = (unsigned int)dst_buffer[channel];
        config.dstFormat = MDPK_FORMAT_ARGB_8888;
        config.pitch = dst_w;
        config.dstBufferSize = dst_size; //Note:Kernel Mdpk Api Only
        config.dstMemType = MDPK_MEMTYPE_PHYMEM;

        config.orientation      = 2;
        config.doImageProcess   = 1;

        config.u4SrcOffsetXFloat = 0;//0x100 stands for 1, 0x40 stands for 0.25 , etc...
        config.u4SrcOffsetYFloat = 0;//0x100 stands for 1, 0x40 stands for 0.25 , etc...
        
        MDPK_Bitblt_Config( channel, &config );
    }


    {
        int ret = 0;

        MDPK_DEBUG("MDPK_Bitblt()\n");

        ret = MDPK_Bitblt( channel );

        MDPK_DEBUG("MDPK_Bitblt() return %d\n", ret );
        
    }
    


    
}



