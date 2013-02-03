#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/autoconf.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/interrupt.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/param.h>
#include <linux/uaccess.h>
#include <linux/sched.h>
#include <linux/slab.h>

#include <linux/xlog.h>

#include <asm/io.h>


#include <mach/irqs.h>

#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_irq.h>
#include <mach/irqs.h>
#include <mach/mt6577_clock_manager.h>

#include "jpeg_drv.h"
#include "jpeg_drv_6577_common.h"

//#include <asm/tcm.h>


#define USE_SYSRAM
#define JPEG_MSG(...)   xlog_printk(ANDROID_LOG_DEBUG, "xlog/jpeg", __VA_ARGS__)
#define JPEG_WRN(...)   xlog_printk(ANDROID_LOG_WARN,  "xlog/jpeg", __VA_ARGS__)
#define JPEG_ERR(...)   xlog_printk(ANDROID_LOG_ERROR, "xlog/jpeg", __VA_ARGS__)
//#define JPEG_MSG printk
#define JPEG_DEVNAME "mtk_jpeg"

#define TABLE_SIZE 4096

#define JPEG_DEC_PROCESS 0x1
#define JPEG_ENC_PROCESS 0x2

//#define FPGA_VERSION
#include "jpeg_drv_6577_reg.h"
//--------------------------------------------------------------------------
// 
//--------------------------------------------------------------------------

// global function
extern kal_uint32 _jpeg_enc_int_status;
extern kal_uint32 _jpeg_dec_int_status;

// device and driver
static dev_t jpeg_devno;
static struct cdev *jpeg_cdev;
static struct class *jpeg_class = NULL;

// decoder
static wait_queue_head_t dec_wait_queue;
static spinlock_t jpeg_dec_lock;
static int dec_status = 0;

static unsigned int table_buffer_pa;
static unsigned char *table_buffer_va;
static unsigned char *dec_src_va;
static unsigned int dec_src_pa;
static unsigned int dec_src_size;

// encoder
static wait_queue_head_t enc_wait_queue;
static spinlock_t jpeg_enc_lock;
static int enc_status = 0;
static unsigned char *dstBufferVA;
static unsigned char *dstUserVA;
static unsigned int dstBufferPA;
static unsigned int dstBufferSize;

//--------------------------------------------------------------------------
// JPEG Common Function
//--------------------------------------------------------------------------
void jpeg_reg_dump(void);

#ifdef FPGA_VERSION

void jpeg_drv_dec_power_on(void)
{  
    JPEG_MSG("JPEG Decoder Power On\n");
}

void jpeg_drv_dec_power_off(void)
{  
    JPEG_MSG("JPEG Decoder Power Off\n");
}

void jpeg_drv_enc_power_on(void)
{  
    JPEG_MSG("JPEG Encoder Power On\n");
}

void jpeg_drv_enc_power_off(void)
{  
    JPEG_MSG("JPEG Encoder Power Off\n");
}

#else

static irqreturn_t jpeg_drv_isr(int irq, void *dev_id)
{
    JPEG_MSG("JPEG Codec Interrupt\n");
    //jpeg_reg_dump();
    
    if(irq == MT6577_JPEG_CODEC_IRQ_ID)
    {   
        if(jpeg_isr_dec_lisr() == 0)
        {
            wake_up_interruptible(&dec_wait_queue);
        }

        if(jpeg_isr_enc_lisr() == 0)
        {
            wake_up_interruptible(&enc_wait_queue);
        }         
    }
    
    return IRQ_HANDLED;
}

void jpeg_drv_dec_power_on(void)
{  
    BOOL ret;
    ret = enable_clock(MT65XX_PDN_MM_JPEG_DEC,"JPEG");
    NOT_REFERENCED(ret);
}

void jpeg_drv_dec_power_off(void)
{  
    BOOL ret;
    ret = disable_clock(MT65XX_PDN_MM_JPEG_DEC,"JPEG");
    NOT_REFERENCED(ret);
}

void jpeg_drv_enc_power_on(void)
{  
    BOOL ret;
    ret = enable_clock(MT65XX_PDN_MM_JPEG_ENC,"JPEG");
    NOT_REFERENCED(ret);
}

void jpeg_drv_enc_power_off(void)
{  
    BOOL ret;
    ret = disable_clock(MT65XX_PDN_MM_JPEG_ENC,"JPEG");
    NOT_REFERENCED(ret);
}

#endif

static int jpeg_drv_dec_init(void)
{
    int retValue;
    
    spin_lock(&jpeg_dec_lock);
    if(dec_status != 0)
    {
        JPEG_WRN("JPEG Decoder is busy\n");
        retValue = -EBUSY;
    }    
    else
    {
        dec_status = 1;
        retValue = 0;    
    }   
    spin_unlock(&jpeg_dec_lock);

    if(retValue == 0)
    {
        jpeg_drv_dec_power_on();
        jpeg_drv_dec_reset();
    }

    return retValue;
}

static void jpeg_drv_dec_deinit(void)
{
    if(dec_status != 0)
    {
#ifndef USE_SYSRAM        
        if(table_buffer_va != 0)
        {
            dma_free_coherent(0, TABLE_SIZE, table_buffer_va, table_buffer_pa);
            table_buffer_va = 0;
        }
#endif
        spin_lock(&jpeg_dec_lock);
        dec_status = 0;
        spin_unlock(&jpeg_dec_lock);
        jpeg_drv_dec_reset();
        jpeg_drv_dec_power_off();
    }
}


static int jpeg_drv_enc_init(void)
{
    int retValue;
    
    spin_lock(&jpeg_enc_lock);
    if(enc_status != 0)
    {
        JPEG_WRN("JPEG Encoder is busy\n");
        retValue = -EBUSY;
    }    
    else
    {
        enc_status = 1;
        retValue = 0;    
    }   
    spin_unlock(&jpeg_enc_lock);

    if(retValue == 0)
    {
        jpeg_drv_enc_power_on();
        jpeg_drv_enc_reset();
    }

    return retValue;
}

static void jpeg_drv_enc_deinit(void)
{
    if(enc_status != 0)
    {
        spin_lock(&jpeg_enc_lock);
        enc_status = 0;
        spin_unlock(&jpeg_enc_lock);

        jpeg_drv_enc_reset();
        jpeg_drv_enc_power_off();
    }
}


//--------------------------------------------------------------------------
// JPEG REG DUMP FUNCTION
//--------------------------------------------------------------------------
void jpeg_reg_dump(void)
{
    unsigned int reg_value = 0;
    unsigned int index = 0;

    JPEG_WRN("JPEG REG:\n ********************\n");
    for(index = 0 ; index < 0x168 ; index += 4){
        reg_value = ioread32(JPG_CODEC_BASE + index);
        JPEG_WRN("+0x%x 0x%x\n", index, reg_value);
    }
}
//--------------------------------------------------------------------------
// JPEG DECODER IOCTRL FUNCTION
//--------------------------------------------------------------------------

static int jpeg_dec_ioctl(unsigned int cmd, unsigned long arg, struct file *file)
{
    int retValue;
    unsigned int decResult, i;
    long timeout_jiff;
    JPEG_DEC_DRV_IN inParams;
    JPEG_DEC_DRV_OUT outParams;

    unsigned int *pStatus;
    unsigned int samplingFormat;
    
    pStatus = (unsigned int*)file->private_data;

    if(NULL == pStatus)
    {
        JPEG_WRN("Private data is null in flush operation. HOW COULD THIS HAPPEN ??\n");
        return -EFAULT;
    }
    
    switch(cmd)
    {
        case JPEG_DEC_IOCTL_INIT:
            JPEG_MSG("JPEG Decoder Initial and Lock\n");

            retValue = jpeg_drv_dec_init();
            if(retValue != 0)
            {
                return -EBUSY;   
            }  

            dec_src_va = 0;
            dec_src_pa = 0;
            table_buffer_va = 0;
            table_buffer_pa = 0;

            *pStatus = JPEG_DEC_PROCESS;
            
            break;
            
        case JPEG_DEC_IOCTL_CONFIG:
            JPEG_MSG("JPEG Decoder Configure Hardware\n");

            if(*pStatus != JPEG_DEC_PROCESS)
            {
                JPEG_WRN("Permission Denied! This process can not access decoder");
                return -EFAULT;
            }

            if(dec_status == 0)
            {
                JPEG_WRN("Decoder status is available, HOW COULD THIS HAPPEN ??");
                *pStatus = 0;
                return -EFAULT;
            }
            // copy input parameters
            if(copy_from_user(&inParams, (void *)arg, sizeof(JPEG_DEC_DRV_IN)))
            {
                JPEG_WRN("JPEG Decoder : Copy from user error\n");
                return -EFAULT;
            }

            JPEG_MSG("JPEG Decoder src addr : 0x%x\n", inParams.srcStreamAddr);
            JPEG_MSG("JPEG Decoder src Size : %d\n", inParams.srcStreamSize); 
            JPEG_MSG("JPEG Decoder src format : %d\n", inParams.samplingFormat);
            JPEG_MSG("JPEG Decoder mcu row : %d\n", inParams.mcuRow);
            JPEG_MSG("JPEG Decoder mcu column : %d\n", inParams.mcuColumn);
            
            dec_src_size = inParams.srcStreamSize;
            //now_size = inParams.srcStreamSize;

            dec_src_va = 0;
            dec_src_pa = inParams.srcStreamAddr;
                       
            // 0. reset    
            jpeg_drv_dec_reset();

            // 1. set source address
            jpeg_drv_dec_set_file_buffer(dec_src_pa , dec_src_size);

            // 2. set table address
#ifdef USE_SYSRAM
            //table_buffer_pa = alloc_internal_sram(INTERNAL_SRAM_JPEG_DECODER, 4096, 2048);
            //table_buffer_pa = MTxxx_SYSRAM_ALLOC(ESysramUser_JPEG_CODEC, TABLE_SIZE, 2048);
            table_buffer_pa = inParams.table_addr;
#else
            table_buffer_va = dma_alloc_coherent(0, TABLE_SIZE, &table_buffer_pa, GFP_KERNEL);
#endif

            if(table_buffer_pa == 0)
            {
                JPEG_WRN("JPEG Decoder : table pa == 0!!!\n");
                return -EFAULT;
            }
            jpeg_drv_dec_set_table_address(table_buffer_pa);

            // 3. set sampling factor
            samplingFormat = inParams.samplingFormat;
            if(samplingFormat == 422 && inParams.vSamplingFactor[0] == 2)
            {
                samplingFormat = 421;
            }
            if(1 != jpeg_drv_dec_set_sampling_factor_related(samplingFormat))
            {
                JPEG_WRN("JPEG Decoder : Sampling Factor Unsupported!!!\n");
                return -EFAULT;
            }

            // 4. set component id
            if(inParams.componentNum == 1)
            {
                jpeg_drv_dec_set_component_id(inParams.componentID[0], 0, 0);
            }
            else
            {
                jpeg_drv_dec_set_component_id(inParams.componentID[0], inParams.componentID[1], inParams.componentID[2]);
            }
            
            // 5. set tatal mcu number
            jpeg_drv_dec_set_total_mcu(inParams.mcuRow * inParams.mcuColumn);

            // set mcu number per row
            //jpeg_drv_dec_set_mcu_per_row();

            // 6. set each DU
            for(i = 0 ; i < inParams.componentNum ; i++)
            {
                jpeg_drv_dec_set_du(i, inParams.totalDU[i], inParams.dummyDU[i], inParams.duPerMCURow[i]);
            }
            
            // 7. set file size
            jpeg_drv_dec_set_file_size(dec_src_size + 16);

            // 8. set Q-table id
            if(inParams.componentNum == 1)
            {
                jpeg_drv_dec_set_q_table_id(inParams.qTableSelector[0], 0, 0);
            }
            else
            {
                jpeg_drv_dec_set_q_table_id(inParams.qTableSelector[0], inParams.qTableSelector[1], inParams.qTableSelector[2]);
            }

            break;
        
        case JPEG_DEC_IOCTL_START:
            // copy input parameters
            JPEG_MSG("JPEG Decoder : JPEG_DEC_IOCTL_START\n");

            if(*pStatus != JPEG_DEC_PROCESS)
            {
                JPEG_WRN("Permission Denied! This process can not access decoder");
                return -EFAULT;
            }
            if(dec_status == 0)
            {
                JPEG_WRN("Decoder status is available, HOW COULD THIS HAPPEN ??");
                *pStatus = 0;
                return -EFAULT;
            }
//            jpeg_reg_dump();
            jpeg_drv_dec_start();

            return 0;
            
            
        case JPEG_DEC_IOCTL_WAIT:
            if(*pStatus != JPEG_DEC_PROCESS)
            {
                JPEG_WRN("Permission Denied! This process can not access decoder");
                return -EFAULT;
            }
            if(dec_status == 0)
            {
                JPEG_WRN("Decoder status is available, HOW COULD THIS HAPPEN ??");
                *pStatus = 0;
                return -EFAULT;
            }           
            if(copy_from_user(&outParams, (void *)arg, sizeof(JPEG_DEC_DRV_OUT)))
            {
                JPEG_WRN("JPEG Decoder : Copy from user error\n");
                return -EFAULT;
            }

            //set timeout
            timeout_jiff = outParams.timeout* HZ / 1000;
            JPEG_MSG("JPEG Decoder Time Jiffies : %ld\n", timeout_jiff);   
#ifdef FPGA_VERSION
            JPEG_MSG("Polling JPEG Status");

            do
            {
                _jpeg_dec_int_status = REG_JPEG_DEC_INTERRUPT_STATUS;
            } while(_jpeg_dec_int_status == 0);
#else
            wait_event_interruptible_timeout(dec_wait_queue, _jpeg_dec_int_status, timeout_jiff);
#endif
            
            decResult = jpeg_drv_dec_get_result();
            _jpeg_dec_int_status = 0;

#ifndef USE_SYSRAM    
            if(table_buffer_va != 0)
            {
                dma_free_coherent(0, TABLE_SIZE, table_buffer_va, table_buffer_pa);
                table_buffer_va = 0;
            }
#endif

            JPEG_MSG("Decode Result : %d \n", decResult);
            if(decResult != 0)
            {
                jpeg_reg_dump();
            }
            if(copy_to_user(outParams.result, &decResult, sizeof(unsigned int)))
            {
                JPEG_WRN("JPEG Decoder : Copy to user error (result)\n");
                return -EFAULT;            
            }
            break;
            
        case JPEG_DEC_IOCTL_DEINIT:
            // copy input parameters
            if(*pStatus != JPEG_DEC_PROCESS)
            {
                JPEG_WRN("Permission Denied! This process can not access decoder");
                return -EFAULT;
            }
            if(dec_status == 0)
            {
                JPEG_WRN("Decoder status is available, HOW COULD THIS HAPPEN ??");
                *pStatus = 0;
                return -EFAULT;
            }
            jpeg_drv_dec_deinit();
            *pStatus = 0;
            
            break;
    }
    return 0;
}

static int jpeg_enc_ioctl(unsigned int cmd, unsigned long arg, struct file *file)
{
    int retValue;
    long timeout_jiff;
    unsigned int fileSize, encResult;
    JPEG_ENC_DRV_IN inParams;
    JPEG_ENC_DRV_OUT outParams;

    unsigned int *pStatus;

    pStatus = (unsigned int*)file->private_data;

    if(NULL == pStatus)
    {
        JPEG_WRN("Private data is null in flush operation. HOW COULD THIS HAPPEN ??\n");
        return -EFAULT;
    }
    
    switch(cmd)
    {       
        // initial and reset JPEG encoder
        case JPEG_ENC_IOCTL_INIT: 
            JPEG_MSG("JPEG Encoder Initial and Lock\n");
            
            retValue = jpeg_drv_enc_init();

            if(retValue == 0)
            {
                *pStatus = JPEG_ENC_PROCESS;
            }
            
            return retValue;   
     
            break;

        // Configure the register
        case JPEG_ENC_IOCTL_CONFIG:
            JPEG_MSG("JPEG Encoder Configure Hardware\n");
            if(*pStatus != JPEG_ENC_PROCESS)
            {
                JPEG_WRN("Permission Denied! This process can not access encoder");
                return -EFAULT;
            }

            if(enc_status == 0)
            {
                JPEG_WRN("Encoder status is available, HOW COULD THIS HAPPEN ??");
                *pStatus = 0;
                return -EFAULT;
            }
            
            // copy input parameters
            if(copy_from_user(&inParams, (void *)arg, sizeof(JPEG_ENC_DRV_IN)))
            {
                JPEG_WRN("JPEG Encoder : Copy from user error\n");
                return -EFAULT;
            }

            if(inParams.allocBuffer)
            {
                dstBufferSize = inParams.dstBufferSize;
                dstUserVA = inParams.dstBufferAddr;
                dstBufferVA = dma_alloc_coherent(0, dstBufferSize, &dstBufferPA, GFP_KERNEL);
            }
            else
            {
                dstBufferSize = 0;
                dstBufferPA = (unsigned int)inParams.dstBufferAddr;
            }

           
            JPEG_MSG("JPEG Encoder Buffer Address : %d\n", (unsigned int)inParams.dstBufferAddr);
            JPEG_MSG("JPEG Encoder Buffer Size : %d\n", inParams.dstBufferSize);
            JPEG_MSG("JPEG Encoder Buffer Width : %d\n", inParams.dstWidth);
            JPEG_MSG("JPEG Encoder Buffer Height : %d\n", inParams.dstHeight);
            JPEG_MSG("JPEG Encoder Buffer Format : %d\n", inParams.dstFormat);
            JPEG_MSG("JPEG Encoder Buffer Quality : %d\n", inParams.dstQuality);

            // 0. reset 
            jpeg_drv_enc_reset();

            // 1. set dst address
            jpeg_drv_enc_set_dst_buffer_info(dstBufferPA, inParams.dstBufferSize, 0);

            // 2. set file format
            jpeg_drv_enc_set_file_format(inParams.enableEXIF);

            // 3. set quality
            jpeg_drv_enc_set_quality(inParams.dstQuality);

            // 4. single run
            jpeg_drv_enc_set_mode(0, 0);

            // 5. set sampling factor
            if(jpeg_drv_enc_set_sample_format_related(inParams.dstWidth, inParams.dstHeight, inParams.dstFormat))
            {
                JPEG_WRN("JPEG Encoder : Unvalid YUV Format\n");
                jpeg_drv_enc_deinit();
                return -EINVAL;
            }

            // 6. set sync reset bit
            jpeg_drv_enc_set_sync_reset(inParams.enableSyncReset);
            
            break;

        case JPEG_ENC_IOCTL_START:
            JPEG_MSG("JPEG Encoder Start\n");
            if(*pStatus != JPEG_ENC_PROCESS)
            {
                JPEG_WRN("Permission Denied! This process can not access encoder");
                return -EFAULT;
            }

            if(enc_status == 0)
            {
                JPEG_WRN("Encoder status is available, HOW COULD THIS HAPPEN ??");
                *pStatus = 0;
                return -EFAULT;
            }
            //jpeg_reg_dump();
            jpeg_drv_enc_start();
            
            break;
            
        case JPEG_ENC_IOCTL_WAIT:
            if(*pStatus != JPEG_ENC_PROCESS)
            {
                JPEG_WRN("Permission Denied! This process can not access encoder");
                return -EFAULT;
            }

            if(enc_status == 0)
            {
                JPEG_WRN("Encoder status is available, HOW COULD THIS HAPPEN ??");
                *pStatus = 0;
                return -EFAULT;
            }
            if(copy_from_user(&outParams, (void *)arg, sizeof(JPEG_ENC_DRV_OUT)))
            {
                JPEG_WRN("JPEG Encoder : Copy from user error\n");
                return -EFAULT;
            }

#ifdef FPGA_VERSION
            JPEG_MSG("Polling JPEG Encoder Status");

            do
            {
                _jpeg_enc_int_status = REG_JPEG_ENC_INTERRUPT_STATUS;
            } while(_jpeg_enc_int_status == 0);
#else
            //set timeout
            timeout_jiff = outParams.timeout* HZ / 1000;
            JPEG_MSG("JPEG Encoder Time Jiffies : %ld\n", timeout_jiff);   
            wait_event_interruptible_timeout(enc_wait_queue, _jpeg_enc_int_status, timeout_jiff);
#endif
            encResult = jpeg_drv_enc_get_result(&fileSize);

            JPEG_MSG("Result : %d, Size : %u, addres : 0x%x\n", encResult, fileSize, ioread32(JPG_CODEC_BASE + 0x120));
            if(encResult != 0)
            {
                jpeg_reg_dump();
            }
            
            if(copy_to_user(outParams.fileSize, &fileSize, sizeof(unsigned int)))
            {
                JPEG_WRN("JPEG Encoder : Copy to user error (file size)\n");
                return -EFAULT;
            }
            
            if(copy_to_user(outParams.result, &encResult, sizeof(unsigned int)))
            {
                JPEG_WRN("JPEG Encoder : Copy to user error (result)\n");
                return -EFAULT;            
            }
            
            if(dstBufferSize != 0)
            {
                JPEG_MSG("Copy Data to User\n");
                if(copy_to_user(dstUserVA, dstBufferVA, fileSize))
                {
                    JPEG_WRN("JPEG Encoder : Copy to user error (dstbuffer)\n");
                    return -EFAULT; 
                }
                dma_free_coherent(0, dstBufferSize, dstBufferVA, dstBufferPA);
            }
            
            break;
            
        case JPEG_ENC_IOCTL_DEINIT:
            // copy input parameters
            if(*pStatus != JPEG_ENC_PROCESS)
            {
                JPEG_WRN("Permission Denied! This process can not access encoder");
                return -EFAULT;
            }

            if(enc_status == 0)
            {
                JPEG_WRN("Encoder status is available, HOW COULD THIS HAPPEN ??");
                *pStatus = 0;
                return -EFAULT;
            }
            jpeg_drv_enc_deinit();
            *pStatus = 0;

            return 0;
    }
    
    return 0;
}
//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
//static int jpeg_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
static long jpeg_unlocked_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch(cmd)
    {
        case JPEG_DEC_IOCTL_INIT:
        case JPEG_DEC_IOCTL_CONFIG:
        case JPEG_DEC_IOCTL_START:
        //case JPEG_DEC_IOCTL_RANGE:
        //case JPEG_DEC_IOCTL_RESUME:
        case JPEG_DEC_IOCTL_WAIT:
        //case JPEG_DEC_IOCTL_COPY:
        case JPEG_DEC_IOCTL_DEINIT:
            return jpeg_dec_ioctl(cmd, arg, file);

        case JPEG_ENC_IOCTL_INIT: 
        case JPEG_ENC_IOCTL_CONFIG:
        case JPEG_ENC_IOCTL_WAIT:
        case JPEG_ENC_IOCTL_DEINIT:
        case JPEG_ENC_IOCTL_START:
            return jpeg_enc_ioctl(cmd, arg, file);
            
        default :
            break; 
    }
    
    return -EINVAL;
}

static int jpeg_open(struct inode *inode, struct file *file)
{
    unsigned int *pStatus;
    //Allocate and initialize private data
    file->private_data = kmalloc(sizeof(unsigned int) , GFP_ATOMIC);

    if(NULL == file->private_data)
    {
        JPEG_WRN("Not enough entry for JPEG open operation\n");
        return -ENOMEM;
    }

    pStatus = (unsigned int *)file->private_data;
    *pStatus = 0;
    
    return 0;
}

static ssize_t jpeg_read(struct file *file, char __user *data, size_t len, loff_t *ppos)
{
    JPEG_MSG("jpeg driver read\n");
    return 0;
}

static int jpeg_release(struct inode *inode, struct file *file)
{
    if(NULL != file->private_data)
    {
        kfree(file->private_data);
        file->private_data = NULL;
    }
    return 0;
}

static int jpeg_flush(struct file * a_pstFile , fl_owner_t a_id)
{
    unsigned int *pStatus;

    pStatus = (unsigned int*)a_pstFile->private_data;

    if(NULL == pStatus)
    {
        JPEG_WRN("Private data is null in flush operation. HOW COULD THIS HAPPEN ??\n");
        return -EFAULT;
    }

    if (*pStatus == JPEG_DEC_PROCESS)
    {
        if(dec_status != 0) 
        {
            JPEG_WRN("Error! Enable error handling for jpeg decoder");
            jpeg_drv_dec_deinit();
        }
    }
    else if (*pStatus == JPEG_ENC_PROCESS)
    {
        if(enc_status != 0) 
        {
            JPEG_WRN("Error! Enable error handling for jpeg encoder");
            jpeg_drv_enc_deinit();
        }
    }

    return 0;
}

/* Kernel interface */
static struct file_operations jpeg_fops = {
	.owner		= THIS_MODULE,
	//.ioctl		= jpeg_ioctl,
	.unlocked_ioctl = jpeg_unlocked_ioctl,
	.open		= jpeg_open,
	.release	= jpeg_release,
	.flush		= jpeg_flush,
	.read       = jpeg_read,
};

static int jpeg_probe(struct platform_device *pdev)
{
    struct class_device;
    
	int ret;
    struct class_device *class_dev = NULL;
    
    JPEG_MSG("-------------jpeg driver probe-------\n");
	ret = alloc_chrdev_region(&jpeg_devno, 0, 1, JPEG_DEVNAME);

	if(ret)
	{
	    JPEG_ERR("Error: Can't Get Major number for JPEG Device\n");
	}
	else
	{
	    JPEG_MSG("Get JPEG Device Major number (%d)\n", jpeg_devno);
    }

	jpeg_cdev = cdev_alloc();
    jpeg_cdev->owner = THIS_MODULE;
	jpeg_cdev->ops = &jpeg_fops;

	ret = cdev_add(jpeg_cdev, jpeg_devno, 1);

    jpeg_class = class_create(THIS_MODULE, JPEG_DEVNAME);
    class_dev = (struct class_device *)device_create(jpeg_class, NULL, jpeg_devno, NULL, JPEG_DEVNAME);

    spin_lock_init(&jpeg_dec_lock);
    spin_lock_init(&jpeg_enc_lock);

    // initial codec, register codec ISR
    dec_status = 0;
    enc_status = 0;
    _jpeg_dec_int_status = 0;
    _jpeg_enc_int_status = 0;

#ifndef FPGA_VERSION
    init_waitqueue_head(&dec_wait_queue);
    init_waitqueue_head(&enc_wait_queue);

    enable_irq(MT6577_JPEG_CODEC_IRQ_ID);
    if(request_irq(MT6577_JPEG_CODEC_IRQ_ID, jpeg_drv_isr, IRQF_TRIGGER_LOW, "jpeg_driver" , NULL))
    {
        JPEG_ERR("JPEG Codec Driver request irq failed\n");
    }
#endif
	JPEG_MSG("JPEG Probe Done\n");

	NOT_REFERENCED(class_dev);
	return 0;
}

static int jpeg_remove(struct platform_device *pdev)
{
	JPEG_MSG("JPEG Codec remove\n");
	//unregister_chrdev(JPEGDEC_MAJOR, JPEGDEC_DEVNAME);
#ifndef FPGA_VERSION
	free_irq(MT6577_JPEG_CODEC_IRQ_ID, NULL);
#endif
	JPEG_MSG("Done\n");
	return 0;
}

static void jpeg_shutdown(struct platform_device *pdev)
{
	JPEG_MSG("JPEG Codec shutdown\n");
	/* Nothing yet */
}

/* PM suspend */
static int jpeg_suspend(struct platform_device *pdev, pm_message_t mesg)
{
    jpeg_drv_dec_deinit();
    jpeg_drv_enc_deinit();
    return 0;
}

/* PM resume */
static int jpeg_resume(struct platform_device *pdev)
{
    return 0;
}


static struct platform_driver jpeg_driver = {
	.probe		= jpeg_probe,
	.remove		= jpeg_remove,
	.shutdown	= jpeg_shutdown,
	.suspend	= jpeg_suspend,
	.resume		= jpeg_resume,
	.driver     = {
	              .name = JPEG_DEVNAME,
	},
};

static void jpeg_device_release(struct device *dev)
{
	// Nothing to release? 
}

static u64 jpegdec_dmamask = ~(u32)0;

static struct platform_device jpeg_device = {
	.name	 = JPEG_DEVNAME,
	.id      = 0,
	.dev     = {
		.release = jpeg_device_release,
		.dma_mask = &jpegdec_dmamask,
		.coherent_dma_mask = 0xffffffff,
	},
	.num_resources = 0,
};

static int __init jpeg_init(void)
{
    int ret;

    JPEG_MSG("JPEG Codec initialize\n");
	
    JPEG_MSG("Register the JPEG Codec device\n");
	if(platform_device_register(&jpeg_device))
	{
        JPEG_ERR("failed to register jpeg codec device\n");
        ret = -ENODEV;
        return ret;
	}

    JPEG_MSG("Register the JPEG Codec driver\n");    
    if(platform_driver_register(&jpeg_driver))
    {
        JPEG_ERR("failed to register jpeg codec driver\n");
        platform_device_unregister(&jpeg_device);
        ret = -ENODEV;
        return ret;
    }

    return 0;
}

static void __exit jpeg_exit(void)
{
    cdev_del(jpeg_cdev);
    unregister_chrdev_region(jpeg_devno, 1);
	//printk("Unregistering driver\n");
    platform_driver_unregister(&jpeg_driver);
	platform_device_unregister(&jpeg_device);
	
	device_destroy(jpeg_class, jpeg_devno);
	class_destroy(jpeg_class);
	
	JPEG_MSG("Done\n");
}

module_init(jpeg_init);
module_exit(jpeg_exit);
MODULE_AUTHOR("Tzu-Meng, Chung <Tzu-Meng.Chung@mediatek.com>");
MODULE_DESCRIPTION("MT6573 JPEG Codec Driver");
MODULE_LICENSE("GPL");
