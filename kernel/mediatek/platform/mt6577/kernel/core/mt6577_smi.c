#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/earlysuspend.h>
#include <linux/mm.h>
#include <linux/slab.h>
#include <linux/proc_fs.h>  //For proc filesystem
#include <linux/jiffies.h>  //For timer
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/seq_file.h> // proc seq files



#include <asm/io.h>
#include <mach/sync_write.h>
#include <mach/irqs.h>

#include <mach/mt6577_reg_base.h>
#include <mach/mt6577_clock_manager.h>
#include <mach/mt6577_gpt.h>
#include <mach/mt6577_irq.h>

#include <mach/mt6577_smi.h>


static struct class *pSMI_CLASS = NULL;


static struct cdev * g_SMI_CharDrv = NULL;
static dev_t g_SMIdevno = MKDEV(SMI_DEV_MAJOR_NUMBER , 0);
static unsigned long g_Occupied = 0;
static inline int RegisterSMICharDrv(void);
static inline void UnregisterSMICharDrv(void);
unsigned long u4SMIBaseAddrArray[4];
#define MT6575SMI_MON_ENA(i) (i + 0x400)
#define MT6575SMI_MON_CLR(i) (i + 0x404)
#define MT6575SMI_MON_PORT(i) (i + 0x408)
#define MT6575SMI_MON_TYPE(i) (i + 0x40C)
#define MT6575SMI_MON_CON(i) (i + 0x410)
#define MT6575SMI_MON_ACT_CNT(i) (i + 0x420)
#define MT6575SMI_MON_REQ_CNT(i) (i + 0x424)
#define MT6575SMI_MON_IDL_CNT(i) (i + 0x428)
#define MT6575SMI_MON_BEA_CNT(i) (i + 0x42C)
#define MT6575SMI_MON_BYT_CNT(i) (i + 0x430)
#define MT6575SMI_MON_CP_CNT(i) (i + 0x434)
#define MT6575SMI_MON_DP_CNT(i) (i + 0x438)
#define MT6575SMI_MON_CDP_MAX(i) (i + 0x43C)
#define MT6575SMI_MON_COS_MAX(i) (i + 0x440)
#define MT6575SMI_MON_BUS_REQ0(i) (i + 0x450) 


static SMIBMResult *auto_trigger_result; //result matrix
static unsigned long hrtimer_smi_time_count;
/*-----------------------------------------------------------------------------
    Proc seq files OPs
  -----------------------------------------------------------------------------*/

//static char *sampleArray[3]={"string1","string2","string3"}; 
  
/** 
* This function is called at the beginning of a sequence. 
* ie, when: 
* 1.the /proc file is read (first time) 
* 2.after the function stop (end of sequence) 
* 
*/  
static void *my_seq_start(struct seq_file *s, loff_t *pos)  
{  
//DBG(); 
if (*pos>=hrtimer_smi_time_count) return NULL; 
return (void *)((unsigned long) *pos+1); 
}  
/** 
* This function is called after the beginning of a sequence. 
* It's called untill the return is NULL (this ends the sequence). 
* 
*/  
static void *my_seq_next(struct seq_file *s, void *v, loff_t *pos)  
{  
++*pos; 
return my_seq_start(s, pos); 
}  
/** 
* This function is called at the end of a sequence 
* 
*/  
static void my_seq_stop(struct seq_file *s, void *v)  
{  
//DBG(); 
/* nothing to do, we use a static value in start() */  
}  
/** 
* This function is called for each "step" of a sequence 
* 
*/  
static char ExtDestType[] = "Ext";
static char IntDestType[] = "Int";
static char RType[] = "R";
static char WType[] = "W";
static char GMC[] = "GMC";
static char AXI[] = "AXI";

static int my_seq_show(struct seq_file *s, void *v)  
{  
  
//unsigned long n; 
int n; 
char* destType;
char* rwType;
char* busType;

char** masterName = NULL;
SMIBMResult temp_proc_result;
 
//DBG(); 
n = (int)v-1; 

temp_proc_result = auto_trigger_result[n];


switch (temp_proc_result.cfg.u4Master)
{
    case 0:
        masterName = MT6575_SMI1_PORT_NAME;
        break;
    case 1:
        masterName = MT6575_SMI2_PORT_NAME;
        break;
    case 2:
        masterName = MT6575_SMI3_PORT_NAME;
        break;
    case 3:
        masterName = MT6575_SMI4_PORT_NAME;
        break;
}

if(temp_proc_result.cfg.bBusType == 0)
{
    busType = GMC;
}
else if(temp_proc_result.cfg.bBusType == 1)
{
    busType = AXI;
}
else
{
    busType = "NULL";	
}

if(temp_proc_result.cfg.bDestType == 0)
{
    destType = "EI";
}
else if(temp_proc_result.cfg.bDestType == 1)
{
    destType = ExtDestType;
}
else if(temp_proc_result.cfg.bDestType == 3)
{
    destType = IntDestType;	
}
else
{
		destType = "NULL";
}	

if(temp_proc_result.cfg.bRWType == 0)
{
    rwType = "RW";
}
else if(temp_proc_result.cfg.bRWType == 1)
{
    rwType = RType;
}
else if(temp_proc_result.cfg.bRWType == 2)
{
    rwType = WType;	
}
else
{
		rwType = "NULL";
}	


seq_printf(s, " %lu %s %s %s %s  %lu		%lu		%lu		%lu		%lu		%lu		%lu		%lu		%lu		%lu		%lu \n",temp_proc_result.cfg.u4Master,masterName[temp_proc_result.cfg.u4PortNo],busType,destType,rwType,temp_proc_result.u4ActiveCnt,temp_proc_result.u4RequestCnt,temp_proc_result.u4IdleCnt,temp_proc_result.u4BeatCnt,temp_proc_result.u4ByteCnt,temp_proc_result.u4CommPhaseAccum,temp_proc_result.u4DataPhaseAccum,temp_proc_result.u4MaxCommOrDataPhase,temp_proc_result.u4MaxOutTransaction,temp_proc_result.u4EndTimeSec,temp_proc_result.u4EndTimeMicroSec);
//seq_printf(s,"sampleArray[%d]:%s\n",n,sampleArray[n]); 
return 0; 
}  
/** 
* This structure gather "function" to manage the sequence 
* 
*/  
static struct seq_operations my_seq_ops = {  
.start = my_seq_start, 
.next = my_seq_next, 
.stop = my_seq_stop, 
.show = my_seq_show  
}; 
/** 
* This function is called when the /proc file is open. 
* 
*/  
static int my_open(struct inode *inode, struct file *file)  
{  
//DBG(); 
return seq_open(file, &my_seq_ops); 
}; 
/** 
* This structure gather "function" that manage the /proc file 
* 
*/  
static struct file_operations my_file_ops = {  
.owner = THIS_MODULE, 
.open = my_open, 
.read = seq_read, 
.llseek = seq_lseek, 
.release = seq_release  
}; 



/*-----------------------------------------------------------------------------
    SMI APIs
  -----------------------------------------------------------------------------*/

static void SMI_PowerOn(void)
{ 
    
    if(enable_clock(MT65XX_PDN_MM_SMI_LARB0, "SMI_LARB0")){printk("Enable SMI_LARB0 clock failed!\n");}
    if(enable_clock(MT65XX_PDN_MM_SMI_LARB0_EMI, "SMI_LARB0_EMI")){printk("Enable SMI_LARB0_EMI clock failed!\n");}

    if(enable_clock(MT65XX_PDN_MM_SMI_LARB1, "SMI_LARB1")){printk("Enable SMI_LARB1 clock failed!\n");}
    if(enable_clock(MT65XX_PDN_MM_SMI_LARB1_EMI, "SMI_LARB1_EMI")){printk("Enable SMI_LARB1_EMI clock failed!\n");}
    
    if(enable_clock(MT65XX_PDN_MM_SMI_LARB2_ACP_BUS, "SMI_LARB2_ACP_BUS")){printk("Enable SMI_LARB2_ACP_BUS clock failed!\n");}
    if(enable_clock(MT65XX_PDN_MM_SMI_LARB2_260MHZ, "SMI_LARB2_260MHZ")){printk("Enable SMI_LARB2_260MHZ clock failed!\n");}
    if(enable_clock(MT65XX_PDN_MM_SMI_LARB2_EMI, "SMI_LARB2_EMI")){printk("Enable SMI_LARB2_EMI clock failed!\n");}
    if(enable_clock(MT65XX_PDN_MM_SMI_LARB2_ACP_BUS_EMI, "SMI_LARB2_ACP_BUS_EMI")){printk("Enable SMI_LARB2_ACP_BUS_EMI clock failed!\n");}    
    if(enable_clock(MT65XX_PDN_MM_SMI_LARB2, "SMI_LARB2")){printk("Enable SMI_LARB2 clock failed!\n");}

    if(enable_clock(MT65XX_PDN_MM_SMI_LARB3_FULL, "SMI_LARB3_FULL")){printk("Enable SMI_LARB3_FULL clock failed!\n");}
    if(enable_clock(MT65XX_PDN_MM_SMI_LARB3_HALF, "MSMI_LARB3_HALF")){printk("Enable SMI_LARB3_HALF clock failed!\n");}

//printk("SMI clock on\n");    
}

static void SMI_PowerOff(void)
{
    if(disable_clock(MT65XX_PDN_MM_SMI_LARB3_HALF, "MSMI_LARB3_HALF")){printk("Enable SMI_LARB3_HALF clock failed!\n");}
    if(disable_clock(MT65XX_PDN_MM_SMI_LARB3_FULL, "SMI_LARB3_FULL")){printk("Enable SMI_LARB3_FULL clock failed!\n");}

    if(disable_clock(MT65XX_PDN_MM_SMI_LARB2, "SMI_LARB2")){printk("Enable SMI_LARB2 clock failed!\n");}    
    if(disable_clock(MT65XX_PDN_MM_SMI_LARB2_ACP_BUS_EMI, "SMI_LARB2_ACP_BUS_EMI")){printk("Enable SMI_LARB2_ACP_BUS_EMI clock failed!\n");}
    if(disable_clock(MT65XX_PDN_MM_SMI_LARB2_EMI, "SMI_LARB2_EMI")){printk("Enable SMI_LARB2_EMI clock failed!\n");}
    if(disable_clock(MT65XX_PDN_MM_SMI_LARB2_260MHZ, "SMI_LARB2_260MHZ")){printk("Enable SMI_LARB2_260MHZ clock failed!\n");}
    if(disable_clock(MT65XX_PDN_MM_SMI_LARB2_ACP_BUS, "SMI_LARB2_ACP_BUS")){printk("Enable SMI_LARB2_ACP_BUS clock failed!\n");}

    if(disable_clock(MT65XX_PDN_MM_SMI_LARB1_EMI, "SMI_LARB1_EMI")){printk("Enable SMI_LARB1_EMI clock failed!\n");}
    if(disable_clock(MT65XX_PDN_MM_SMI_LARB1, "SMI_LARB1")){printk("Enable SMI_LARB1 clock failed!\n");}

    if(disable_clock(MT65XX_PDN_MM_SMI_LARB0_EMI, "SMI_LARB0_EMI")){printk("Enable SMI_LARB0_EMI clock failed!\n");}
    if(disable_clock(MT65XX_PDN_MM_SMI_LARB0, "SMI_LARB0")){printk("Enable SMI_LARB0 clock failed!\n");}
    
//printk("SMI clock off\n");        
}


static int SMI_Open(struct inode * a_pstInode, struct file * a_pstFile)
{
    if(g_Occupied)
    {
        return -1;
    }

    g_Occupied = 1;

//printk("SMI Monitor opened!!\n");
		SMI_PowerOn();
    return 0;
}

static int SMI_Release(struct inode * a_pstInode, struct file * a_pstFile)
{
		if(NULL != auto_trigger_result)
    {
			kfree(auto_trigger_result);   		
    }

    g_Occupied = 0;
    
//printk("SMI Monitor closed!!\n");
		SMI_PowerOff();
    return 0;
}



 
//Manual Single Port  
  
void SMI_Manual_Trigger_Init(SMIBMCfg *cfg, SMIBMCfg_Ext *cfg_ex)
{
    unsigned long smi_master = cfg->u4Master;
    unsigned long u4SMIBaseAddr = u4SMIBaseAddrArray[smi_master]; 	
    unsigned long u4RegVal;

    
    //printk("SMI_master %lu , u4SMIBaseAddr 0x%lx \n",smi_master,u4SMIBaseAddr);
    
    //Disable monitor
    mt65xx_reg_sync_writel(0 , MT6575SMI_MON_ENA(u4SMIBaseAddr));

    //Clear counter
    mt65xx_reg_sync_writel(1 , MT6575SMI_MON_CLR(u4SMIBaseAddr));
    mt65xx_reg_sync_writel(0 , MT6575SMI_MON_CLR(u4SMIBaseAddr));
    
    //Set port
    mt65xx_reg_sync_writel(cfg->u4PortNo , MT6575SMI_MON_PORT(u4SMIBaseAddr));
    mt65xx_reg_sync_writel((((unsigned long)cfg->bRWType << 2) | cfg->bDestType) , MT6575SMI_MON_TYPE(u4SMIBaseAddr));
  
  	//printk("[%lu, %lu, %lu]\n", cfg->u4PortNo, cfg->bRWType, cfg->bDestType);
    
    if(cfg_ex != NULL)
    {	   
        u4RegVal = (((unsigned long)cfg_ex->uStarvationTime << 8) | ((unsigned long)cfg_ex->bStarvationEn << 6) | 
        ((unsigned long)cfg_ex->bMaxPhaseSelection << 5) | ((unsigned long)cfg_ex->bDPSelection << 4) | 
        ((unsigned long)cfg_ex->uIdleOutStandingThresh << 1) | cfg_ex->bIdleSelection);
        //printk("Ex configuration %lx\n", u4RegVal);
    }
    else
    {
    	  u4RegVal = (((unsigned long)8 << 8) | ((unsigned long)1 << 6) | 
        ((unsigned long)1 << 5) | ((unsigned long)1 << 4) | 
        ((unsigned long)3 << 1) | 1);
        //printk("default configuration %lx\n", u4RegVal);
    }
  
    mt65xx_reg_sync_writel(u4RegVal , MT6575SMI_MON_CON(u4SMIBaseAddr));

    // Enable it
    
    if(cfg->bBusType == 0) //GMC
    {
    		//printk("GMC\n");
        mt65xx_reg_sync_writel(0x1 , MT6575SMI_MON_ENA(u4SMIBaseAddr));//G2D    	
    }
    else //AXI
    {
    		//printk("AXI\n");
        mt65xx_reg_sync_writel(0x3 , MT6575SMI_MON_ENA(u4SMIBaseAddr));//GPU    	
    }

    // Cross Page Prevent
    *((volatile unsigned int *)0xF000882C) &= ~0x01;
    *((volatile unsigned int *)0xF000882C) |= 0x01;
    
}

void SMI_Manual_Trigger_Result(SMIBMCfg *cfg, SMIBMCfg_Ext *cfg_ex, SMIBMResult *result)
{
		unsigned long smi_master = cfg->u4Master;
	  unsigned long u4SMIBaseAddr = u4SMIBaseAddrArray[smi_master];
    struct timeval tv;

    //printk("SMI_Manual_Trigger_Result SMI_master %lu , u4SMIBaseAddr 0x%lx \n",smi_master,u4SMIBaseAddr);
    //Pause counter
    mt65xx_reg_sync_writel(0 , MT6575SMI_MON_ENA(u4SMIBaseAddr));

    //Get time mark
    do_gettimeofday(&tv);

    //Get SMI result
    result->u4ActiveCnt = ioread32(MT6575SMI_MON_ACT_CNT(u4SMIBaseAddr));
    result->u4RequestCnt = ioread32(MT6575SMI_MON_REQ_CNT(u4SMIBaseAddr));
    result->u4IdleCnt = ioread32(MT6575SMI_MON_IDL_CNT(u4SMIBaseAddr));
    result->u4BeatCnt = ioread32(MT6575SMI_MON_BEA_CNT(u4SMIBaseAddr));
    result->u4ByteCnt = ioread32(MT6575SMI_MON_BYT_CNT(u4SMIBaseAddr));
    result->u4CommPhaseAccum = ioread32(MT6575SMI_MON_CP_CNT(u4SMIBaseAddr));
    result->u4DataPhaseAccum = ioread32(MT6575SMI_MON_DP_CNT(u4SMIBaseAddr));
    result->u4MaxCommOrDataPhase = ioread32(MT6575SMI_MON_CDP_MAX(u4SMIBaseAddr));
    result->u4MaxOutTransaction = ioread32(MT6575SMI_MON_COS_MAX(u4SMIBaseAddr));
    result->u4EndTimeSec = tv.tv_sec;
    result->u4EndTimeMicroSec = tv.tv_usec;
    result->cfg = *cfg;
    //result->cfg_ex = *cfg_ex; 
    
   // printk("SMI_Manual_Trigger_Result [%x %x]\n",result->u4ActiveCnt,result->u4RequestCnt);

    //Clear counter
    mt65xx_reg_sync_writel(1 , MT6575SMI_MON_CLR(u4SMIBaseAddr));
    mt65xx_reg_sync_writel(0 , MT6575SMI_MON_CLR(u4SMIBaseAddr));

    //Resume counter

    if(cfg->bBusType == 0) //GMC
    {
        mt65xx_reg_sync_writel(0x1 , MT6575SMI_MON_ENA(u4SMIBaseAddr));//G2D    	
    }
    else //AXI
    {
        mt65xx_reg_sync_writel(0x3 , MT6575SMI_MON_ENA(u4SMIBaseAddr));//GPU    	
    }
	
}
  

//Auto Single Port

static struct hrtimer hrtimer_auto_trigger;
unsigned long hrtimer_smi_time_interval;
SMIBMCfg *hrtimer_cfg;
SMIBMCfg_Ext *hrtimer_cfg_ex;
unsigned long hrtimer_trigger_times;
SMIBMResult hrtimer_result;


enum hrtimer_restart hrtimer_auto_trigger_func(struct hrtimer *p_timer)
{ 

	//printk("The function SMI hrtimer_auto_trigger_func, times is %lu\n", hrtimer_trigger_times); 

	//before Linux 2.6.3 //hrtimer_auto_trigger.expires=ktime_add_ns(hrtimer_auto_trigger.expires,(u64)(hrtimer_smi_time_interval*1000000));
	hrtimer_set_expires(&hrtimer_auto_trigger, ktime_add_ns(hrtimer_get_expires(&hrtimer_auto_trigger),(u64)(hrtimer_smi_time_interval*1000000)));
	
	//Get SMI data
	SMI_Manual_Trigger_Result(hrtimer_cfg, hrtimer_cfg_ex, & hrtimer_result);
	auto_trigger_result[hrtimer_trigger_times] = hrtimer_result;
	
	//Update Conuter 
	hrtimer_trigger_times ++;
	
	if(hrtimer_trigger_times>=hrtimer_smi_time_count)
	{
		return HRTIMER_NORESTART;
	}
	else
	{		
		return HRTIMER_RESTART;
	}
}

void SMI_Auto_Trigger_Init(SMIBMCfg *cfg, SMIBMCfg_Ext *cfg_ex, unsigned long smi_time_count, unsigned long smi_time_interval)
{
    unsigned long smi_master = cfg->u4Master;
    unsigned long u4SMIBaseAddr = u4SMIBaseAddrArray[smi_master]; 	
    unsigned long u4RegVal;
    ktime_t ktime;  
    
//    printk("SMI_master %lu , u4SMIBaseAddr 0x%lx \n",smi_master,u4SMIBaseAddr);
    
    //Disable monitor
    mt65xx_reg_sync_writel(0 , MT6575SMI_MON_ENA(u4SMIBaseAddr));

    //Clear counter
    mt65xx_reg_sync_writel(1 , MT6575SMI_MON_CLR(u4SMIBaseAddr));
    mt65xx_reg_sync_writel(0 , MT6575SMI_MON_CLR(u4SMIBaseAddr));
    
    //Set port
    mt65xx_reg_sync_writel(cfg->u4PortNo , MT6575SMI_MON_PORT(u4SMIBaseAddr));
    mt65xx_reg_sync_writel((((unsigned long)cfg->bRWType << 2) | cfg->bDestType) , MT6575SMI_MON_TYPE(u4SMIBaseAddr));
  
//  	printk("[%lu, %lu, %lu]\n", cfg->u4PortNo, cfg->bRWType, cfg->bDestType);
    
    if(cfg_ex != NULL)
    {	   
        u4RegVal = (((unsigned long)cfg_ex->uStarvationTime << 8) | ((unsigned long)cfg_ex->bStarvationEn << 6) | 
        ((unsigned long)cfg_ex->bMaxPhaseSelection << 5) | ((unsigned long)cfg_ex->bDPSelection << 4) | 
        ((unsigned long)cfg_ex->uIdleOutStandingThresh << 1) | cfg_ex->bIdleSelection);
//        printk("Ex configuration %lx\n", u4RegVal);
    }
    else
    {
    	  u4RegVal = (((unsigned long)8 << 8) | ((unsigned long)1 << 6) | 
        ((unsigned long)1 << 5) | ((unsigned long)1 << 4) | 
        ((unsigned long)3 << 1) | 1);
//        printk("default configuration %lx\n", u4RegVal);
    }
  
    mt65xx_reg_sync_writel(u4RegVal , MT6575SMI_MON_CON(u4SMIBaseAddr));
    
    
    //prevent memory leak
    if(NULL != auto_trigger_result)
    {
//    	printk("auto_trigger_result free\n");
			kfree(auto_trigger_result);   		
    }
   
    //Init Space for result store
    auto_trigger_result = kmalloc((sizeof(SMIBMResult)*smi_time_count) , GFP_ATOMIC);
    
    if(NULL == auto_trigger_result)
    {
//        printk("MT6575 SMI Monitor -- Out of memory to hold SMI_Auto_Trigger_Init data\n");
        return;
    }
    
    // Enable it
    
    if(cfg->bBusType == 0) //GMC
    {
//    		printk("GMC\n");
        mt65xx_reg_sync_writel(0x1 , MT6575SMI_MON_ENA(u4SMIBaseAddr));//G2D    	
    }
    else //AXI
    {
//    		printk("AXI\n");
        mt65xx_reg_sync_writel(0x3 , MT6575SMI_MON_ENA(u4SMIBaseAddr));//GPU    	
    }

    // Cross Page Prevent
    *((volatile unsigned int *)0xF000882C) &= ~0x01;
    *((volatile unsigned int *)0xF000882C) |= 0x01;
    
    
    //Initial HRT Timer
	  
	  hrtimer_smi_time_count = smi_time_count;
    hrtimer_smi_time_interval = smi_time_interval;
    hrtimer_cfg = cfg;
    hrtimer_cfg_ex = cfg_ex;
    
	  ktime = ktime_set(smi_time_interval / 1000, (smi_time_interval % 1000) * 1000000); //ktime_set(long secs, long nanosecs);  //smi_time_interval: milli sec (1/1000)
	  hrtimer_init(&hrtimer_auto_trigger,CLOCK_MONOTONIC,HRTIMER_MODE_REL); 
	  hrtimer_auto_trigger.function = hrtimer_auto_trigger_func; 
	  hrtimer_trigger_times = 0;
	  hrtimer_start(&hrtimer_auto_trigger,ktime,HRTIMER_MODE_REL); 
    
}


//Auto Selected Port

SMIBMResult *auto_selected_result; //result matrix
unsigned long auto_selected_result_count; //result size

static struct hrtimer hrtimer_auto_selected;
unsigned long hrtimer_smi_sel_time_interval;
unsigned long hrtimer_selected_times;

SMIBMCfg *hrtimer_sel_cfg;
SMIBMCfg_Ext *hrtimer_sel_cfg_ex;
SMIBMResult hrtimer_sel_result;



enum hrtimer_restart hrtimer_auto_selected_func(struct hrtimer *p_timer)
{ 

	//printk("The function SMI hrtimer_auto_selected_func, times is %d\n", hrtimer_selected_times); 

	//before Linux 2.6.3 //hrtimer_auto_trigger.expires=ktime_add_ns(hrtimer_auto_trigger.expires,(u64)(hrtimer_smi_time_interval*1000000));
	hrtimer_set_expires(&hrtimer_auto_selected, ktime_add_ns(hrtimer_get_expires(&hrtimer_auto_selected),(u64)(hrtimer_smi_sel_time_interval*1000000)));
	
	//Get SMI data
	if(hrtimer_selected_times > 0) // Get Previous Result (No first time)
	{
		SMI_Manual_Trigger_Result(&hrtimer_sel_cfg[hrtimer_selected_times-1], &hrtimer_sel_cfg_ex[hrtimer_selected_times-1], & hrtimer_sel_result);
		auto_selected_result[hrtimer_selected_times-1] = hrtimer_sel_result;
	
	}
	
	if(hrtimer_selected_times < auto_selected_result_count) // Set Current Cfg (no last time)
	{	
		//Set SMI config
		if(hrtimer_sel_cfg_ex != NULL)
		{	
			SMI_Manual_Trigger_Init(&hrtimer_sel_cfg[hrtimer_selected_times], &hrtimer_sel_cfg_ex[hrtimer_selected_times]);
		}
		else
		{
			SMI_Manual_Trigger_Init(&hrtimer_sel_cfg[hrtimer_selected_times], 0);
		}		
	}
	//Update Conuter 
	hrtimer_selected_times ++;
	
	if(hrtimer_selected_times>auto_selected_result_count)
	{
		//Complete
		if(NULL != auto_trigger_result)
		{
			kfree(auto_trigger_result);
		}	
		auto_trigger_result = auto_selected_result; //result matrix for dump
    hrtimer_smi_time_count = auto_selected_result_count;
		
		return HRTIMER_NORESTART;
	}
	else
	{		
		return HRTIMER_RESTART;
	}
}




void SMI_Auto_Trigger_Selected(SMIBMCfg *cfg, SMIBMCfg_Ext *cfg_ex, unsigned long total_ports, unsigned long smi_time_interval)
{
		
		//Init Space for result store
		
		ktime_t ktime; 
		auto_selected_result_count = total_ports; // init result size
		
		if(NULL != auto_selected_result)
    {
			kfree(auto_selected_result);   		
    }
    auto_selected_result = kmalloc((sizeof(SMIBMResult)*auto_selected_result_count) , GFP_ATOMIC);
    
    if(NULL == auto_selected_result)
    {
//        printk("MT6575 SMI Monitor -- Out of memory to hold SMI_Auto_Trigger_Selected data\n");
        return;
    }
    
    //Initial HRT Timer
	  
    hrtimer_smi_sel_time_interval = smi_time_interval;
    hrtimer_sel_cfg = cfg;
    hrtimer_sel_cfg_ex = cfg_ex;
    
 
	  ktime = ktime_set(smi_time_interval / 1000, (smi_time_interval % 1000) * 1000000); //ktime_set(long secs, long nanosecs);  //smi_time_interval: milli sec (1/1000)
	  hrtimer_init(&hrtimer_auto_selected,CLOCK_MONOTONIC,HRTIMER_MODE_REL); 
	  hrtimer_auto_selected.function = hrtimer_auto_selected_func; 
	  hrtimer_selected_times = 0;
	  hrtimer_start(&hrtimer_auto_selected,ktime,HRTIMER_MODE_REL); 
	


}


//Manual Selected Port

SMIBMResult *manu_selected_result; //result matrix
unsigned long manu_selected_result_count; //result size
unsigned long manual_selected_times;
SMIBMCfg *manu_sel_cfg;
SMIBMCfg_Ext *manu_sel_cfg_ex;
SMIBMResult manu_sel_result;  

int SMI_Manu_Selected_Func(void)
{
	//printk("The function SMI manual_selected_func, times is %d\n", manual_selected_times); 	
	
	
	//Get SMI data
	if(manual_selected_times > 0) // Get Previous Result (No first time)
	{
		SMI_Manual_Trigger_Result(&manu_sel_cfg[manual_selected_times-1], &manu_sel_cfg_ex[manual_selected_times-1], & manu_sel_result);
		manu_selected_result[manual_selected_times-1] = manu_sel_result;
	}
	
	if(manual_selected_times < manu_selected_result_count) // Set Current Cfg (no last time)
	{	
		//Set SMI config
		if(manu_sel_cfg_ex != NULL)
		{	
			SMI_Manual_Trigger_Init(&manu_sel_cfg[manual_selected_times], &manu_sel_cfg_ex[manual_selected_times]);
		}
		else
		{
			SMI_Manual_Trigger_Init(&manu_sel_cfg[manual_selected_times], 0);
		}		
	}
	
	//Update Conuter 
	manual_selected_times ++;
	
	
	if(manual_selected_times>manu_selected_result_count)
	{
		//Complete
		if(NULL != auto_trigger_result)
		{
			kfree(auto_trigger_result);
		}	
		auto_trigger_result = manu_selected_result; //result matrix for dump
    hrtimer_smi_time_count = manu_selected_result_count;
	}
	
	return 1;
	
}

void SMI_Manu_Trigger_Selected(SMIBMCfg *cfg, SMIBMCfg_Ext *cfg_ex, unsigned long total_ports)
{
		manu_selected_result_count = total_ports;
		
		if(NULL != manu_selected_result)
    {
			kfree(manu_selected_result);   		
    }
    
    manu_selected_result = kmalloc((sizeof(SMIBMResult)*manu_selected_result_count) , GFP_ATOMIC);
    
		if(NULL == manu_selected_result)
    {
        printk("MT6575 SMI Monitor -- Out of memory to hold SMI_Manu_Trigger_Selected data\n");
        return;
    }
    
    manu_sel_cfg = cfg;
    manu_sel_cfg_ex = cfg_ex;
    manual_selected_times = 0;
	
}


/*-----------------------------------------------------------------------------
    /proc/driver/smi_test
  -----------------------------------------------------------------------------*/
 SMIBMCfg proc_cfg;
 unsigned long proc_smi_master, proc_u4PortNo, proc_bBusType, proc_bDestType, proc_bRWType;
 SMIBMResult proc_result;
 unsigned long proc_smi_time_count, proc_smi_time_interval;  
  
  
// Manual Single Port  
  
int SMIProc_smi_test_read(   char *page, char **start, off_t off,
                            int count,  int *eof, void *data )
{
    int len = 0;
    SMI_Manual_Trigger_Result(&proc_cfg, 0, &proc_result);

    //len += sprintf( page, "smi_test read %lu\n", smi_test);
    len += sprintf(page, " %lu		%lu		%lu		%lu		%lu		%lu		%lu		%lu		%lu		%lu		%lu \n",proc_result.u4ActiveCnt,proc_result.u4RequestCnt,proc_result.u4IdleCnt,proc_result.u4BeatCnt,proc_result.u4ByteCnt,proc_result.u4CommPhaseAccum,proc_result.u4DataPhaseAccum,proc_result.u4MaxCommOrDataPhase,proc_result.u4MaxOutTransaction,proc_result.u4EndTimeSec,proc_result.u4EndTimeMicroSec);

    *eof = 1;
    
    return len;
}


int SMIProc_smi_test_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{

    char            cmd_buf[64] = {'\0'}; 
    unsigned long   buf_size = (count < (sizeof(cmd_buf) - 1)) ? (count) : (sizeof(cmd_buf) - 1);

   //

    if (copy_from_user(cmd_buf, buffer, buf_size))
        return -EFAULT;

    sscanf(cmd_buf, "%lu %lu %lu %lu %lu",  &proc_smi_master, &proc_u4PortNo, &proc_bBusType, &proc_bDestType, &proc_bRWType);
    
    proc_cfg.u4Master = proc_smi_master;
    proc_cfg.u4PortNo = proc_u4PortNo;
    proc_cfg.bBusType = proc_bBusType;
    proc_cfg.bDestType = proc_bDestType;
    proc_cfg.bRWType = proc_bRWType;
    SMI_Manual_Trigger_Init(&proc_cfg, 0);

    return count;    
}

/*-----------------------------------------------------------------------------
    /proc/driver/smi_auto
  -----------------------------------------------------------------------------*/

//Auto Single Port

int SMIProc_smi_auto_read(   char *page, char **start, off_t off,
                            int count,  int *eof, void *data )
{
    int len = 0;
    
		len += sprintf(page + len, "Use /proc/driver/smi_dump to see result! Progressing %lu / %lu \n", hrtimer_trigger_times, hrtimer_smi_time_count);

    *eof = 1;
    
    return len;
}


int SMIProc_smi_auto_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{

    char            cmd_buf[64] = {'\0'}; 
    unsigned long   buf_size = (count < (sizeof(cmd_buf) - 1)) ? (count) : (sizeof(cmd_buf) - 1);

   //

    if (copy_from_user(cmd_buf, buffer, buf_size))
        return -EFAULT;

    sscanf(cmd_buf, "%lu %lu %lu %lu %lu %lu %lu",  &proc_smi_master, &proc_u4PortNo, &proc_bBusType, &proc_bDestType, &proc_bRWType, &proc_smi_time_count, &proc_smi_time_interval);
    
    proc_cfg.u4Master = proc_smi_master;
    proc_cfg.u4PortNo = proc_u4PortNo;
    proc_cfg.bBusType = proc_bBusType;
    proc_cfg.bDestType = proc_bDestType;
    proc_cfg.bRWType = proc_bRWType;
    SMI_Auto_Trigger_Init(&proc_cfg, 0, proc_smi_time_count, proc_smi_time_interval);

    return count;    
}


//Auto All Port

SMIBMCfg allPort[MT6575_SMI_ALLPORT_NUM*4];

int SMIProc_smi_all_read(   char *page, char **start, off_t off,
                            int count,  int *eof, void *data )
{
    int len = 0;
    //SMI_Manual_Trigger_Result(&proc_cfg, 0, proc_smi_master, &proc_result);
    len += sprintf(page + len, "Use /proc/driver/smi_dump to see result! Progressing %lu / %lu \n", (hrtimer_selected_times-1), auto_selected_result_count);
    
    *eof = 1;
    
    return len;
}




int SMIProc_smi_all_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{

    char            cmd_buf[64] = {'\0'}; 
    unsigned long   buf_size = (count < (sizeof(cmd_buf) - 1)) ? (count) : (sizeof(cmd_buf) - 1);
    int i;
    int m1=0,m2=0,m3=0,m4=0;
    
   //

    if (copy_from_user(cmd_buf, buffer, buf_size))
        return -EFAULT;

    sscanf(cmd_buf, "%lu", &proc_smi_time_interval);
    

    for (i=0;i<MT6575_SMI_ALLPORT_NUM*4;i++)
    {
    	if(i<MT6575_SMI1_PORT_NUM*4)
    	{
    		
    		allPort[i].u4Master = 0;
    		allPort[i].u4PortNo = (m1/4);
    		allPort[i].bBusType = 0;
    		allPort[i].bDestType = ((m1%2) ? 3 : 1);
    		allPort[i].bRWType = ((m1&0x2)? 2: 1);
    		m1++;
    	}
    	else if(i<(MT6575_SMI1_PORT_NUM+MT6575_SMI2_PORT_NUM)*4)
    	{
    		allPort[i].u4Master = 1;
    		allPort[i].u4PortNo = (m2/4);
    		allPort[i].bBusType = 0;
    		allPort[i].bDestType = ((m2%2) ? 3 : 1);
    		allPort[i].bRWType = ((m2&0x2)? 2: 1);
    		m2++;
    	}
    	else if(i<(MT6575_SMI1_PORT_NUM+MT6575_SMI2_PORT_NUM+MT6575_SMI3_PORT_NUM)*4)	
    	{
    		allPort[i].u4Master = 2;
    		allPort[i].u4PortNo = (m3/4);
    		allPort[i].bBusType = 0;
    		allPort[i].bDestType = ((m3%2) ? 3 : 1);
    		allPort[i].bRWType = ((m3&0x2)? 2: 1);	
    		m3++;
    	}
    	else if(i<MT6575_SMI_ALLPORT_NUM*4)
    	{
    		allPort[i].u4Master = 3;
    		allPort[i].u4PortNo = (m4/4);
    		allPort[i].bBusType = 1;
    		allPort[i].bDestType = ((m4%2) ? 3 : 1);
    		allPort[i].bRWType = ((m4&0x2)? 2: 1);
    		m4++;
    	}
    	else
    	{
    		printk("Error: SMI Index overbound");		
    	}		
    	
    }

    SMI_Auto_Trigger_Selected(allPort, 0, MT6575_SMI_ALLPORT_NUM*4, proc_smi_time_interval);

    return count;    
}


//Manual All Port

int SMIProc_smi_mall_read(   char *page, char **start, off_t off,
                            int count,  int *eof, void *data )
{
    int len = 0;

		SMI_Manu_Selected_Func();
	
    len += sprintf(page + len, "Use /proc/driver/smi_dump to see result! Progressing %lu / %lu \n", (manual_selected_times-1), manu_selected_result_count);
    
    *eof = 1;
    
    return len;
}




int SMIProc_smi_mall_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{

    int i;
    int m1=0,m2=0,m3=0,m4=0;
    
    for (i=0;i<MT6575_SMI_ALLPORT_NUM*4;i++)
    {
    	if(i<MT6575_SMI1_PORT_NUM*4)
    	{
    		
    		allPort[i].u4Master = 0;
    		allPort[i].u4PortNo = (m1/4);
    		allPort[i].bBusType = 0;
    		allPort[i].bDestType = ((m1%2) ? 3 : 1);
    		allPort[i].bRWType = ((m1&0x2)? 2: 1);
    		m1++;
    	}
    	else if(i<(MT6575_SMI1_PORT_NUM+MT6575_SMI2_PORT_NUM)*4)
    	{
    		allPort[i].u4Master = 1;
    		allPort[i].u4PortNo = (m2/4);
    		allPort[i].bBusType = 0;
    		allPort[i].bDestType = ((m2%2) ? 3 : 1);
    		allPort[i].bRWType = ((m2&0x2)? 2: 1);
    		m2++;
    	}
    	else if(i<(MT6575_SMI1_PORT_NUM+MT6575_SMI2_PORT_NUM+MT6575_SMI3_PORT_NUM)*4)	
    	{
    		allPort[i].u4Master = 2;
    		allPort[i].u4PortNo = (m3/4);
    		allPort[i].bBusType = 0;
    		allPort[i].bDestType = ((m3%2) ? 3 : 1);
    		allPort[i].bRWType = ((m3&0x2)? 2: 1);	
    		m3++;
    	}
    	else if(i<MT6575_SMI_ALLPORT_NUM*4)
    	{
    		allPort[i].u4Master = 3;
    		allPort[i].u4PortNo = (m4/4);
    		allPort[i].bBusType = 1;
    		allPort[i].bDestType = ((m4%2) ? 3 : 1);
    		allPort[i].bRWType = ((m4&0x2)? 2: 1);
    		m4++;
    	}
    	else
    	{
    		printk("Error: SMI Index overbound");		
    	}		
    	
    }

    SMI_Manu_Trigger_Selected(allPort, 0, MT6575_SMI_ALLPORT_NUM*4);

    return count;    
}


unsigned long smi_power = 0;

int SMIProc_smi_pow_read(   char *page, char **start, off_t off,
                            int count,  int *eof, void *data )
{
    int len = 0;

    len += sprintf( page, "SMI Power %lu\n", smi_power);
  
    *eof = 1;
    
    return len;
}


int SMIProc_smi_pow_write(struct file *file, const char __user *buffer, unsigned long count, void *data)
{

    char            cmd_buf[64] = {'\0'}; 
    unsigned long   buf_size = (count < (sizeof(cmd_buf) - 1)) ? (count) : (sizeof(cmd_buf) - 1);

   //

    if (copy_from_user(cmd_buf, buffer, buf_size))
        return -EFAULT;

    sscanf(cmd_buf, "%lu ",  &smi_power);
    
    if(smi_power > 0)
    {
        SMI_PowerOn();	
    }
    else
    {
        SMI_PowerOff();	
    }		
    
    return count;    
}


int SMIProc_init( void )
{
    int     ret_val = 0;
    struct proc_dir_entry *p_proc_entry;
        
    p_proc_entry = create_proc_entry("driver/smi_once", 0, NULL);
    if (p_proc_entry) {
        p_proc_entry->read_proc  = SMIProc_smi_test_read;
        p_proc_entry->write_proc = SMIProc_smi_test_write;
    }
    else {
        printk("add /proc/driver/smi_once entry fail \n");
        ret_val = -1;
    }
    
    p_proc_entry = create_proc_entry("driver/smi_auto", 0, NULL);
    if (p_proc_entry) {
        p_proc_entry->read_proc  = SMIProc_smi_auto_read;
        p_proc_entry->write_proc = SMIProc_smi_auto_write;
    }
    else {
        printk("add /proc/driver/smi_auto entry fail \n");
        ret_val = -1;
    }
    
    p_proc_entry = create_proc_entry("driver/smi_all", 0, NULL);
    if (p_proc_entry) {
        p_proc_entry->read_proc  = SMIProc_smi_all_read;
        p_proc_entry->write_proc = SMIProc_smi_all_write;
    }
    else {
        printk("add /proc/driver/smi_all entry fail \n");
        ret_val = -1;
    }
    
    p_proc_entry = create_proc_entry("driver/smi_mall", 0, NULL);
    if (p_proc_entry) {
        p_proc_entry->read_proc  = SMIProc_smi_mall_read;
        p_proc_entry->write_proc = SMIProc_smi_mall_write;
    }
    else {
        printk("add /proc/driver/smi_mall entry fail \n");
        ret_val = -1;
    }
    
    p_proc_entry = create_proc_entry("driver/smi_dump", 0, NULL);
    if (p_proc_entry) {
        p_proc_entry->proc_fops  = &my_file_ops;;
    }
    else {
        printk("add /proc/driver/smi_dump entry fail \n");
        ret_val = -1;
    }
    
    p_proc_entry = create_proc_entry("driver/smi_pow", 0, NULL);
    if (p_proc_entry) {
        p_proc_entry->read_proc  = SMIProc_smi_pow_read;
        p_proc_entry->write_proc = SMIProc_smi_pow_write;
    }
    else {
        printk("add /proc/driver/smi_mall entry fail \n");
        ret_val = -1;
    }

    return ret_val;
    
}

static int __init SMI_Init(void)
{
    static struct device* smimon_device = NULL;

//    printk("Insert MT6575 bus monitor module\n");
    
    if(RegisterSMICharDrv())
    {
        printk("Insert MT6575 SMI bus monitor driver module failed\n");
        return -1;
    }

    pSMI_CLASS = class_create(THIS_MODULE, "smimonitordrv");
    if (IS_ERR(pSMI_CLASS)) {
        int ret = PTR_ERR(pSMI_CLASS);
        printk("Unable to create class, err = %d\n", ret);
        return ret;            
    }    
    smimon_device = device_create(pSMI_CLASS , NULL, g_SMIdevno , NULL, "MT6575-SMIMonitor");
    
    u4SMIBaseAddrArray[0] = SMI_LARB0_BASE;
    u4SMIBaseAddrArray[1] = SMI_LARB1_BASE;
    u4SMIBaseAddrArray[2] = SMI_LARB2_BASE;
    u4SMIBaseAddrArray[3] = SMI_LARB3_BASE;
    
    //SMI Proc Init
    SMIProc_init();
    
    

    return 0;
}

static void __exit SMI_Exit(void)
{	
    UnregisterSMICharDrv();

    device_destroy(pSMI_CLASS , g_SMIdevno);

    class_destroy(pSMI_CLASS);

}

static long SMI_Ioctl(struct file * a_pstFile, unsigned int a_u4Command, unsigned long a_u4Param)
{
		long i4RetVal = 0;
		static stManualTriggerInitCfg mInitCfg;
		static stManualTriggerResultCfg mInitResult;
		
		static stAutoTriggerInitCfg aInitCfg;
		static SMIBMCfg *select_cfg;
		static SMIBMCfg_Ext *select_cfg_ext;
		static stAutoTriggerSelectTime aSeleTimes;
		static unsigned long mSeleTimes;
	
		switch (a_u4Command)
		{
  		case SMI_MANUAL_TRIGGER_INIT:
  			if(copy_from_user(&mInitCfg , (void *) a_u4Param, sizeof(stManualTriggerInitCfg)))
        {
        	printk("SMI_MANUAL_TRIGGER_INIT : copy from user failed\n");
        	return -1;
        }
        SMI_Manual_Trigger_Init(&mInitCfg.cfg, 0);
  		break;
  		
  		case SMI_MANUAL_TRIGGER_INIT_EXT:
  			if(copy_from_user(&mInitCfg , (void *) a_u4Param, sizeof(stManualTriggerInitCfg)))
        {
        	printk("SMI_MANUAL_TRIGGER_INIT_EXT : copy from user failed\n");
        	return -1;
        }
        SMI_Manual_Trigger_Init(&mInitCfg.cfg, &mInitCfg.cfg_ext);
  		break;
  		
  		case SMI_MANUAL_TRIGGER_RESULT:
  			if(copy_from_user(&mInitResult , (void *) a_u4Param, sizeof(stManualTriggerResultCfg)))
        {
        	printk("SMI_MANUAL_TRIGGER_RESULT : copy from user failed\n");
        	return -1;
        }
        SMI_Manual_Trigger_Result(&mInitResult.cfg, 0, &mInitResult.result);
        if(copy_to_user((void __user *) a_u4Param , &mInitResult , sizeof(stManualTriggerResultCfg)))
        {
        	printk("SMI_MANUAL_TRIGGER_RESULT : copy to user failed\n");
         	return -1;
				}
  		break;
  		
  		case SMI_AUTO_TRIGGER_INIT:
  			if(copy_from_user(&aInitCfg , (void *) a_u4Param, sizeof(stAutoTriggerInitCfg)))
        {
        	printk("SMI_AUTO_TRIGGER_INIT : copy from user failed\n");
        	return -1;
        }
        SMI_Auto_Trigger_Init(&aInitCfg.cfg, 0, aInitCfg.smi_time_count, aInitCfg.smi_time_interval);
  		break;
  		
  		case SMI_AUTO_TRIGGER_INIT_EXT:
  			if(copy_from_user(&aInitCfg , (void *) a_u4Param, sizeof(stAutoTriggerInitCfg)))
        {
        	printk("SMI_AUTO_TRIGGER_INIT_EXT : copy from user failed\n");
        	return -1;
        }
        SMI_Auto_Trigger_Init(&aInitCfg.cfg, &aInitCfg.cfg_ext, aInitCfg.smi_time_count, aInitCfg.smi_time_interval);
  		break;
  		
  		case SMI_AUTO_TRIGGER_RESULT:
  			if (hrtimer_trigger_times < hrtimer_smi_time_count)
  			{
//  				printk("SMI_AUTO_TRIGGER_RESULT : not done yet\n");
  				return -1;
  			}	
  			if(copy_to_user((void __user *) a_u4Param , auto_trigger_result , (sizeof(SMIBMResult)*hrtimer_smi_time_count)))
        {
            printk("SMI_AUTO_TRIGGER_RESULT : copy to user failed\n");
            return -1;
        }
  		break;	
  		
  		case SMI_AUTO_TRIGGER_SELECT_TIME:
  			if(copy_from_user(&aSeleTimes , (void *) a_u4Param, sizeof(stAutoTriggerSelectTime)))
        {
        	printk("SMI_AUTO_TRIGGER_SELECT_TIME : copy from user failed\n");
        	return -1;
        }
  		break;
  		
  		case SMI_AUTO_TRIGGER_SELECT_EXT:
  			select_cfg_ext = kmalloc((sizeof(SMIBMCfg_Ext)*aSeleTimes.total_ports) , GFP_ATOMIC);
  			if(copy_from_user(select_cfg_ext , (void *) a_u4Param, sizeof(SMIBMCfg_Ext)*aSeleTimes.total_ports))
        {
        	printk("SMI_AUTO_TRIGGER_SELECT_EXT : copy from user failed\n");
        	return -1;
        }
  			
  		break;
  		
  		case SMI_AUTO_TRIGGER_SELECT:
  			select_cfg = kmalloc((sizeof(SMIBMCfg)*aSeleTimes.total_ports) , GFP_ATOMIC);
  			if(copy_from_user(select_cfg , (void *) a_u4Param, sizeof(SMIBMCfg)*aSeleTimes.total_ports))
        {
        	printk("SMI_AUTO_TRIGGER_SELECT : copy from user failed\n");
        	return -1;
        }
        SMI_Auto_Trigger_Selected(select_cfg, select_cfg_ext, aSeleTimes.total_ports, aSeleTimes.smi_time_interval);
        
  		break;
  		
  		case SMI_AUTO_TRIGGER_SELECT_RESULT:
  			if(hrtimer_selected_times <= auto_selected_result_count)
  			{
            printk("SMI_AUTO_TRIGGER_SELECT_RESULT : not done yet\n");
            return -1;
  			}	
  			if(copy_to_user((void __user *) a_u4Param , auto_trigger_result , (sizeof(SMIBMResult)*auto_selected_result_count)))
        {
            printk("SMI_AUTO_TRIGGER_RESULT : copy to user failed\n");
            return -1;
        }
  		break;	
  		
  		case SMI_MANU_TRIGGER_SELECT_TIME:
  			if(copy_from_user(&mSeleTimes , (void *) a_u4Param, sizeof(unsigned long)))
        {
        	printk("SMI_MANU_TRIGGER_SELECT_TIME : copy from user failed\n");
        	return -1;
        }
  		break;
  		
  		case SMI_MANU_TRIGGER_SELECT_EXT:
  			select_cfg_ext = kmalloc((sizeof(SMIBMCfg_Ext)*mSeleTimes) , GFP_ATOMIC);
  			if(copy_from_user(select_cfg_ext , (void *) a_u4Param, sizeof(SMIBMCfg_Ext)*mSeleTimes))
        {
        	printk("SMI_MANU_TRIGGER_SELECT_EXT : copy from user failed\n");
        	return -1;
        }
  		break;
  		
  		case SMI_MANU_TRIGGER_SELECT:
  			select_cfg = kmalloc((sizeof(SMIBMCfg)*mSeleTimes) , GFP_ATOMIC);
  			if(copy_from_user(select_cfg , (void *) a_u4Param, sizeof(SMIBMCfg)*mSeleTimes))
        {
        	printk("SMI_MANU_TRIGGER_SELECT : copy from user failed\n");
        	return -1;
        }
        SMI_Manu_Trigger_Selected(select_cfg, select_cfg_ext, mSeleTimes);
  		break;
  		
  		case SMI_MANU_TRIGGER_SELECT_TRIG:
  			if(SMI_Manu_Selected_Func()!=1)
  			{
  				printk("SMI_MANU_TRIGGER_SELECT_TRIG : SMI_Manu_Selected_Func failed\n");
  				return -1;	
  			}	
  		break;
  		
  		case SMI_MANU_TRIGGER_SELECT_RESULT:
  			if(manual_selected_times<=manu_selected_result_count)
  			{
            printk("SMI_MANU_TRIGGER_SELECT_RESULT : not done yet\n");
            return -1;
  			}	
  			if(copy_to_user((void __user *) a_u4Param , auto_trigger_result , (sizeof(SMIBMResult)*manu_selected_result_count)))
        {
            printk("SMI_MANU_TRIGGER_SELECT_RESULT : copy to user failed\n");
            return -1;
        }
  		break;	
  		
  		default :
      	printk("MT6575 SMI bus monitor ioctl : No such command!!: %d\n" , a_u4Command);
        i4RetVal = -EINVAL;    	
  	
  	}
  	
  	return i4RetVal;
}

//file operations
static const struct file_operations g_SMI_fops = 
{
	.owner = THIS_MODULE,
	.open = SMI_Open,
	.release = SMI_Release,
	.unlocked_ioctl = SMI_Ioctl,
};



static inline int RegisterSMICharDrv(void)
{
    if( alloc_chrdev_region(&g_SMIdevno , 0, 1,"mt6575-smimon") )
    {
        printk("[MT6575 SMI Monitor] Allocate device no failed\n");

        return -EAGAIN;
    }

    //Allocate driver
    g_SMI_CharDrv = cdev_alloc();

    if(NULL == g_SMI_CharDrv)
    {
        unregister_chrdev_region(g_SMIdevno , 1);

        printk("[MT6575 SMI Monitor] Allocate mem for kobject failed\n");

        return -ENOMEM;
    }

    //Attatch file operation.
    cdev_init(g_SMI_CharDrv, &g_SMI_fops);

    g_SMI_CharDrv->owner = THIS_MODULE;

    //Add to system
    if(cdev_add(g_SMI_CharDrv , g_SMIdevno , 1))
    {
        printk("[MT6575 SMI Monitor] Attatch file operation failed\n");

        unregister_chrdev_region(g_SMIdevno, 1);

        return -EAGAIN;
    }

    return 0;

}

static inline void UnregisterSMICharDrv(void)
{
    //Release char driver
    cdev_del(g_SMI_CharDrv);

    unregister_chrdev_region(g_SMIdevno, 1);
}



module_init(SMI_Init);
module_exit(SMI_Exit);

MODULE_DESCRIPTION("MT6575/MT6577 SMI Monitor driver");
MODULE_AUTHOR("CVS <CVS.Chen@mediatek.com>");
MODULE_LICENSE("GPL");
