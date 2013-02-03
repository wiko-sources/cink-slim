#include <linux/string.h>
#include <linux/time.h>
#include <linux/uaccess.h>
#include <linux/fb.h>

#include <linux/debugfs.h>


#include <mach/mt6577_typedefs.h>
#include <mach/m4u.h>



#include "mtk_mau_debug.h"
#include <mach/mtk_mau.h>

#include <linux/xlog.h>



#define MAU_DBG(fmt, arg...)      xlog_printk( ANDROID_LOG_DEBUG, "MAU/DBG", "[DBG]: %s() "fmt"\n", __FUNCTION__, ##arg)
#define MAU_INFO(fmt, arg...)     xlog_printk( ANDROID_LOG_INFO, "MAU/DBG", "[INFO]: %s() "fmt"\n", __FUNCTION__, ##arg)
#define MAU_WARN(fmt, arg...)     xlog_printk( ANDROID_LOG_WARN, "MAU/DBG", "[WARN]: %s() "fmt"\n", __FUNCTION__, ##arg)
#define MAU_ERROR(fmt, arg...)    xlog_printk( ANDROID_LOG_ERROR, "MAU/DBG", "[ERR]: %s(): %s@%d "fmt"\n", __FUNCTION__, __FILE__,__LINE__,##arg)
#define MAU_FATAL(fmt, arg...)    xlog_printk( ANDROID_LOG_FATAL, "MAU/DBG", "[FATAL]: %s(): %s@%d "fmt"\n", __FUNCTION__, __FILE__,__LINE__,##arg)



//extern char const* const mauPortName[MAU_MASK_ALL];
//extern char const* const mpuPortName[MPU_MASK_ALL];


static MTK_MAU_CONFIG mauConfig = {0};
//static MTK_MPU_CONFIG mpuConfig = {0};

static int print_whose_status = 0;  //0: MAU's stauts 1: MPU's status


static char debug_buffer[4096];


#ifdef MTK_M4U_SUPPORT
M4U_DEBUG_FUNCTION_STRUCT _m4u_debug_func;
EXPORT_SYMBOL(_m4u_debug_func);
#endif

static char STR_HELP[] =
    "\n"
    "USAGE\n"
    "        echo [ACTION]... > mau_dbg\n"
    "\n"
    "ACTION\n"
    "        module1|module2?R/W/RW(startPhyAddr,endPhyAddr)@MAU_Enty_ID\n"
    "             MAU will monitor specified module whether R/W specified range of memory\n"
    "             example: echo tvc|lcd_r?R(0,0x1000)@1 > mau_dbg\n"
    "             you can use [all] to specify all modules\n"
    "             example: echo all?W(0x2000,0x9000)@2 > mau_dbg\n"
    "\n"
    "        module1|module2@MAU_Enty_ID:off\n"
    "             Turn off specified module on specified MAU Entry\n"
    "             example: echo tvc|lcd_r@1:off > mau_dbg\n"
    "\n"
    "\n"
    "        all:off\n"
    "             Turn off all of modules\n"
    "             example: echo all:off > mau_dbg\n"
    "\n"
    "        list modules\n"
    "             list all module names MAU could monitor\n"
    "\n"
    "        reg:[MPU|MAU1|MAU2]\n"
    "             dump hw register values\n"
    "\n"
    "        regw:addr=val\n"
    "             write hw register\n"
    "\n"
    "        regr:addr\n"
    "             read hw register\n"
    "\n"
    "        m4u_log:on\n"
    "             start to print m4u translate miss rate every second \n"
    "\n"
    "        m4u_log:off\n"
    "             stop to print m4u translate miss rate every second \n"
    "\n"
    "        m4u_debug:[command] \n"
    "             input a command, used for debug \n"
    "\n"    
    "        m4u_monitor:on\n"
    "             start to print m4u translate miss rate every second \n"
    "\n"
    "        m4u_monitor:off\n"
    "             stop to print m4u translate miss rate every second \n";
// ---------------------------------------------------------------------------
//  Information Dump Routines
// ---------------------------------------------------------------------------



#if 0
static long int get_current_time_us(void)
{
    struct timeval t;
    do_gettimeofday(&t);
    return (t.tv_sec & 0xFFF) * 1000000 + t.tv_usec;
}

//Find a char in string, return its last position. -1 means no char in string
static int find_char(const char * str, char tar)
{
    int length = 0;
    int i = 0;
    if (str == NULL) return -1;
    length = strlen(str);
    DDBG("[MAU_DBG]%s str:%s len: %d tar:%c\n", __func__, str,length,tar );

    for (i=length-1; i>=0; i--) {
        if (str[i] == tar) {
            DDBG("[MAU_DBG]%s find str @ %d\n", __func__, i );
            return i;
        }
    }
    return -1;
}
#endif


#if 0
static bool parse_mau_cmd(const char *name)
{
    int i = 0;
    int n = 0;
    char strCmd[256];
    unsigned int _mau1_port_mask = 0;
    unsigned int _mau2_port_mask = 0;
    bool check_r = false;
    bool check_w = false;
    unsigned long startAddr;
    unsigned long endAddr;
    unsigned long entryId = 0;

    const int cmdLendth = sizeof(strCmd) - 1;
    memset(strCmd, 0, sizeof(strCmd));
    scnprintf(strCmd, cmdLendth, name);


    DDBG("[MAU_DBG]%s, %s\n", __func__, strCmd);
    if (0 == strncmp(strCmd, "all", 3)) {
        _mau1_port_mask = 0xffffffff;
        _mau2_port_mask = 0xffffffff;
       n += 4;
    }
    else {
        do {
            for (i = 1; i < MAU_MASK_ALL; i++) {
                if (i == MAU1_MASK_ALL) continue;
                if (0 == strncmp(strCmd+n, mauPortName[i], strlen(mauPortName[i]))) {
                    if (i < MAU1_MASK_ALL) {
                        _mau1_port_mask |= 1<<(i-1);
                    }
                    else if (i > MAU1_MASK_ALL) {
                        _mau2_port_mask |= 1<<(i-MAU1_MASK_ALL-1);
                    }
                    n += strlen(mauPortName[i])+1;
                    DDBG("[MAU_DBG]module math %s\n", mauPortName[i]);
                    break;
                }

            }
        } while (strCmd[n-1] == ',');
    }

    if (strCmd[n-1] == ':') {
        if (strCmd[n] == 'R' || strCmd[n] == 'r') {
            check_r = true;
            n++;
            if (strCmd[n] == 'W' || strCmd[n] == 'w') {
                check_w = true;
                n++;
            }
        } else if (strCmd[n] == 'W' || strCmd[n] == 'w') {
            check_w =true;
            n++;
            if (strCmd[n] == 'R' || strCmd[n] == 'r') {
                check_r = true;
                n++;
            }

        } else {
            goto Error;
        }

    } else {
        goto Error;
    }

    if (strCmd[n] == '[') {
        char *p = (char *)strCmd + n + 1;
        startAddr = simple_strtoul(p, &p, 16);
        endAddr  = simple_strtoul(p + 1, &p, 16);
        entryId = simple_strtoul(p + 1, &p, 16);
    } else {
        goto Error;
    }

    DDBG("[MAU_DBG] Parse result: port mask(%x), R(%d), W(%d), S(0x%x), E(0x%x)\n",
        _mau1_port_mask, check_r, check_w, (unsigned int)startAddr, (unsigned int)endAddr);
    mauConfig.InvalidMasterGMC1 = _mau1_port_mask;
    mauConfig.InvalidMasterGMC2 = _mau2_port_mask;
    mauConfig.ReadEn = check_r;
    mauConfig.WriteEn = check_w;
    mauConfig.StartAddr = startAddr;
    mauConfig.EndAddr = endAddr;
    mauConfig.Enable = true;
    if ( entryId == 0 || entryId == 1 || entryId == 2) {
        mauConfig.EntryID = entryId;
    }
    else {
        mauConfig.EntryID = 0;
    }
    return true;


Error:
    MAU_ERROR("parse command error! %s\n", strCmd);
    return false;

}
#endif
// ---------------------------------------------------------------------------
//  Command Processor
// ---------------------------------------------------------------------------

static void process_dbg_opt(const char *opt)
{
#ifdef MTK_M4U_SUPPORT
    if (0 == strncmp(opt, "m4u_monitor:", 12))
    {
        if (0 == strncmp(opt + 12, "on", 2)) {
            if(_m4u_debug_func.isInit)
            {
            	 _m4u_debug_func.m4u_perf_timer_on();
            }
            else
            {
            	 MAU_ERROR("m4u debug struct is invalid !");
            }
        } else if (0 == strncmp(opt + 12, "off", 3)) {
            if(_m4u_debug_func.isInit)
            {
            	 _m4u_debug_func.m4u_perf_timer_off();
            }
            else
            {
            	 MAU_ERROR("m4u debug struct is invalid !");
            }
        } else {
            goto Error;
        }
    }
    else if (0 == strncmp(opt, "m4u_log:", 8))
    {
        if (0 == strncmp(opt + 8, "on", 2)) {
            if(_m4u_debug_func.isInit)
            {
            	 _m4u_debug_func.m4u_log_on();
            }
            else
            {
            	 MAU_ERROR("m4u debug struct is invalid !");
            }
        } else if (0 == strncmp(opt + 8, "off", 3)) {
            if(_m4u_debug_func.isInit)
            {
            	 _m4u_debug_func.m4u_log_off();
            }
            else
            {
            	 MAU_ERROR("m4u debug struct is invalid !");
            }
        } else {
            goto Error;
        }
    }
    else if (0 == strncmp(opt, "m4u_debug:", 10))
    {
            if(_m4u_debug_func.isInit)
            {
		            unsigned int command;
		            char *p = (char *)opt + 10;
		            command = (unsigned int) simple_strtoul(p, &p, 10);
		            MAU_INFO("m4u_debug_command, command=%d ", command);
		            _m4u_debug_func.m4u_debug_command(command);
            }
            else
            {
            	 MAU_ERROR("m4u debug struct is invalid !");
            }
    }    
    else if (0 == strncmp(opt, "cat:", 4))
#else
    if (0 == strncmp(opt, "cat:", 4))
#endif    
    {
        if (0 == strncmp(opt + 4, "mau", 3)) {
            print_whose_status = 0;
        } else if (0 == strncmp(opt + 4, "mpu", 3)) {
            print_whose_status = 1;
        } else {
            goto Error;
        }
    }
    else if (0 == strncmp(opt, "log:", 4))
    {
        if (0 == strncmp(opt + 4, "on", 2)) {
            MAU_LogSwitch(true);
        } else if (0 == strncmp(opt + 4, "off", 3)) {
            MAU_LogSwitch(false);
        } else {
            goto Error;
        }
    }
    else if (0 == strncmp(opt, "reg:", 4))
    {
        if (0 == strncmp(opt + 4, "mau0", 4)) {
            MAU_DumpReg(MAU_ID_0);
        }
        else if (0 == strncmp(opt + 4, "mau1", 4)) {
            MAU_DumpReg(MAU_ID_1);
        }
        else if (0 == strncmp(opt + 4, "mau2", 4)) {
            MAU_DumpReg(MAU_ID_2);
        }
        else if (0 == strncmp(opt + 4, "mau3", 4)) {
            MAU_DumpReg(MAU_ID_3);
        }
        else if (0 == strncmp(opt + 4, "mauall", 6)) {
            MAU_DumpReg(MAU_ID_0);
            MAU_DumpReg(MAU_ID_1);
            MAU_DumpReg(MAU_ID_2);
            MAU_DumpReg(MAU_ID_3);
        }
        else {
            goto Error;
        }
    }
    else if (0 == strncmp(opt, "mau:stop", 8 ))
    {
        //memset(&mauConfig, 0, sizeof(mauConfig));
        mauConfig.EntryID = MAU_ENTRY_ALL;
        mauConfig.Enable = false;
        MAU_Config(&mauConfig);

        MAU_DBG("To stop all");
    }
    else if (0 == strncmp(opt, "mau:", 4 ))
    {
		unsigned int eID, rd, wt, mode, larb0, larb1, larb2, larb3, saddr, eaddr;
		char *p = (char *)opt + 4;
		eID = (unsigned int) simple_strtoul(p, &p, 16);
		p++;
		rd = (unsigned int) simple_strtoul(p, &p, 16);
		p++;
		wt = (unsigned int) simple_strtoul(p, &p, 16);
		p++;
		mode = (unsigned int) simple_strtoul(p, &p, 16);
		p++;
		larb0 = (unsigned int) simple_strtoul(p, &p, 16);
		p++;
		larb1 = (unsigned int) simple_strtoul(p, &p, 16);
        p++;
		larb2 = (unsigned int) simple_strtoul(p, &p, 16);
		p++;
		larb3 = (unsigned int) simple_strtoul(p, &p, 16);
        p++;
		saddr = (unsigned int) simple_strtoul(p, &p, 16);
		p++;
		eaddr = (unsigned int) simple_strtoul(p, &p, 16);

		MAU_DBG("eID=%d rd=%d wt=%d mode=%d \n"
            "larb0 =0x%x larb1 =0x%x larb2 =0x%x larb3 =0x%x\n"
            "saddr=0x%x eaddr=0x%x\n",
            eID, rd, wt, mode, larb0, larb1, larb2, larb3, saddr, eaddr);

        mauConfig.EntryID = eID;
        mauConfig.Enable = true;
        mauConfig.ReadEn = (rd==0) ? 0 : 1;
        mauConfig.WriteEn = (wt==0) ? 0 : 1;
        mauConfig.Mode = (mode==0) ? MAU_PA : MAU_MVA;
        mauConfig.InvalidMasterLARB0 = larb0;
        mauConfig.InvalidMasterLARB1 = larb1;
        mauConfig.InvalidMasterLARB2 = larb2;
        mauConfig.InvalidMasterLARB3 = larb3;
        mauConfig.StartAddr = saddr;
        mauConfig.EndAddr = eaddr;
        MAU_Config(&mauConfig);

    }

    else goto Error;

    return;
Error:
    MAU_ERROR("parse command error!\n\n%s", STR_HELP);
}


static void process_dbg_cmd(char *cmd)
{
    char *tok;

	MAU_DBG();

    while ((tok = strsep(&cmd, " ")) != NULL)
    {
        process_dbg_opt(tok);
    }
}


// ---------------------------------------------------------------------------
//  Debug FileSystem Routines
// ---------------------------------------------------------------------------

struct dentry *mau_dbgfs = NULL;


static ssize_t debug_open(struct inode *inode, struct file *file)
{
    file->private_data = inode->i_private;
    return 0;
}




static ssize_t debug_read(struct file *file,
                          char __user *ubuf, size_t count, loff_t *ppos)
{
    //const int debug_bufmax = sizeof(debug_buffer) - 1;
    int n = 0;
#if 0
    if (print_whose_status == 0) {
 	    MAU_PrintStatus(debug_buffer, debug_bufmax, &n);
    } else {
        MPU_PrintStatus(debug_buffer, debug_bufmax, &n);
    }
#endif

    return simple_read_from_buffer(ubuf, count, ppos, debug_buffer, n);
}


static ssize_t debug_write(struct file *file,
                           const char __user *ubuf, size_t count, loff_t *ppos)
{
    const int debug_bufmax = sizeof(debug_buffer) - 1;
	size_t ret;

	ret = count;

	if (count > debug_bufmax)
        count = debug_bufmax;

	if (copy_from_user(&debug_buffer, ubuf, count))
		return -EFAULT;

	debug_buffer[count] = 0;

    process_dbg_cmd(debug_buffer);

    return ret;
}


static struct file_operations debug_fops = {
	.read  = debug_read,
    .write = debug_write,
	.open  = debug_open,
};


void MAU_DBG_Init(void)
{
    mau_dbgfs = debugfs_create_file("mau",
        S_IFREG|S_IRUGO, NULL, (void *)0, &debug_fops);
	MAU_DBG();
}


void MAU_DBG_Deinit(void)
{
    debugfs_remove(mau_dbgfs);
	MAU_DBG();
}


