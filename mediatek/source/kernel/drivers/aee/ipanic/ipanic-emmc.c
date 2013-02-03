#ifdef MTK_EMMC_SUPPORT

#include <linux/module.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/aee.h>
#include "ipanic.h"
#include <linux/delay.h>

/* MTK Emmc exported functions to support panic dump */

extern int card_dump_func_read(unsigned char* buf, unsigned int len, unsigned int offset, unsigned int dev);

extern int card_dump_func_write(unsigned char* buf, unsigned int len, unsigned int offset, unsigned int dev);

#define EMMC_ID 0x10000
#define EMMC_BLOCK_SIZE 512

static u8 *emmc_bounce;

static int in_panic = 0;

char *emmc_allocate_and_read(int offset, int length)
{
	int size;
	char *buff = NULL;

	if (length == 0) {
		return NULL;
	}

	size = ALIGN(length, EMMC_BLOCK_SIZE);
	buff = kzalloc(size, GFP_KERNEL);
	if (buff != NULL) {
		if (card_dump_func_read(buff, size, offset, EMMC_ID) != 0) {
			kfree(buff);
			buff = NULL;
		}
	}
	else {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: Cannot allocate buffer to read(len:%d)\n", __FUNCTION__, length);
	}
	return buff;
}

static struct aee_oops *emmc_ipanic_oops_copy(void)
{
	struct aee_oops *oops = NULL;
	struct ipanic_header *hdr = NULL;
	int hdr_size = ALIGN(sizeof(struct ipanic_header), EMMC_BLOCK_SIZE);

	hdr = kzalloc(hdr_size, GFP_KERNEL);
	if (hdr == NULL) {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: Cannot allocate ipanic header memory\n", __FUNCTION__);
		return NULL;
	}

	if (card_dump_func_read((unsigned char *)hdr, hdr_size, 0, EMMC_ID) < 0) {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: emmc panic log header read failed\n", __func__);
		return NULL;
	}
	
	if (ipanic_header_check(hdr) != 0) {
		return NULL;
	}

	oops = aee_oops_create(AE_DEFECT_FATAL, AE_KE, IPANIC_MODULE_TAG);
	if (oops != NULL) {
		struct ipanic_oops_header *oops_header = (struct ipanic_oops_header *)emmc_allocate_and_read(hdr->oops_header_offset, hdr->oops_header_length);
		if (oops_header == NULL) { 
			xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: Can't read oops header(len:%d)\n", __FUNCTION__, hdr->oops_header_length);
			goto error_return;
		}
		aee_oops_set_process_path(oops, oops_header->process_path);
		aee_oops_set_backtrace(oops, oops_header->backtrace);
		kfree(oops_header);

		oops->detail = emmc_allocate_and_read(hdr->oops_detail_offset, hdr->oops_detail_length);
		oops->detail_len = hdr->oops_detail_length;
		if (oops->detail == NULL) {
			xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: read detail failed(len: %d)\n", __FUNCTION__, oops->detail_len);
			goto error_return;
		}

		oops->console = emmc_allocate_and_read(hdr->console_offset, hdr->console_length);
		oops->console_len = hdr->console_length;
		if (oops->console == NULL) {
			xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: read console failed(len: %d)\n", __FUNCTION__, oops->console_len);
			goto error_return;
		}

        /*If panic from kernel context, no user sapce info available. Shouldn't fail*/
        if (0 == hdr->userspace_info_length)
        {
            oops->userspace_info = NULL;
            oops->userspace_info_len = 0;
        }
        else
        {
            oops->userspace_info = emmc_allocate_and_read(hdr->userspace_info_offset, hdr->userspace_info_length);
            oops->userspace_info_len = hdr->userspace_info_length;
            if (oops->userspace_info == NULL) {
                xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: read usrespace info failed\n", __FUNCTION__);
                goto error_return;
            }
        }
	

		oops->android_main = emmc_allocate_and_read(hdr->android_main_offset, hdr->android_main_length);
		oops->android_main_len  = hdr->android_main_length;
		if (oops->android_main == NULL)	{
			xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: read android_main failed\n", __FUNCTION__);
			goto error_return;
		}
		
		oops->android_radio  = emmc_allocate_and_read(hdr->android_radio_offset, hdr->android_radio_length);
		oops->android_radio_len = hdr->android_radio_length;
		if (oops->android_radio == NULL) {
			xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: read android_radio failed\n", __FUNCTION__);
			goto error_return;
		}		    
		
		oops->android_system = emmc_allocate_and_read(hdr->android_system_offset, hdr->android_system_length);
		oops->android_system_len = hdr->android_system_length;
		if (oops->android_system == NULL) {
			xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: read android_system failed\n", __FUNCTION__);
			goto error_return;
		}		    
		
		xlog_printk(ANDROID_LOG_DEBUG, IPANIC_LOG_TAG, "ipanic_oops_copy return OK\n");
		kfree(hdr);
		return oops;
	}
	else {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: kmalloc failed at header\n", __FUNCTION__);
		kfree(hdr);
		return NULL;
	}
error_return:
	kfree(hdr);
	aee_oops_free(oops);
	return NULL;
}

static int emmc_ipanic_write(void *buf, int off, int len)
{
	int rem = len & (EMMC_BLOCK_SIZE - 1);
	len = len & ~(EMMC_BLOCK_SIZE - 1);

	if (len > 0) {
		if (card_dump_func_write((unsigned char *)buf, len, off, EMMC_ID))
			return -1;
	}
	if (rem != 0) {
		memcpy(emmc_bounce, buf + len, rem);
		memset(emmc_bounce + rem, 0, EMMC_BLOCK_SIZE - rem);
		if (card_dump_func_write(buf, EMMC_BLOCK_SIZE, off + len, EMMC_ID))
			return -1;
	}
	return len + rem;
}

static int ipanic_write_android_buf(unsigned int off, int type)
{
	unsigned int copy_count = 0;

	while (1) {
		int rc = panic_dump_android_log(emmc_bounce, PAGE_SIZE, type);
		BUG_ON(rc < 0);
		if (rc <= 0)
			break;
		if (rc < PAGE_SIZE) {
			memset(emmc_bounce + rc, 0, PAGE_SIZE - rc);
		}
		if (card_dump_func_write(emmc_bounce, PAGE_SIZE, off, EMMC_ID)) {
			xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "aee-ipanic-emmc(%s): android log %d write failed, offset %d\n", __FUNCTION__, type, off);
			return -1;
		}
		copy_count += rc;
		off += PAGE_SIZE;
	}
	xlog_printk(ANDROID_LOG_DEBUG, IPANIC_LOG_TAG, "%s: dump droid log type %d, count %d\n", __FUNCTION__, type, copy_count);
	return copy_count;
}

static int ipanic_write_all_android_buf(int offset, struct ipanic_header *hdr)
{
	int rc;

	// main buffer:
	offset = ALIGN(offset, EMMC_BLOCK_SIZE);
	rc = ipanic_write_android_buf(offset, 1);
	if (rc > 0) {
		hdr->android_main_offset = offset;
		hdr->android_main_length = rc;
		offset += rc;
	}
	else {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: Failed to write android main buffer\n", __func__);
	}

	// radio buffer:
	offset = ALIGN(offset, EMMC_BLOCK_SIZE);
	rc = ipanic_write_android_buf(offset, 3);
	if (rc > 0) {
		hdr->android_radio_offset = offset;
		hdr->android_radio_length = rc;
		offset += rc;
	}
	else {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: Failed to write android radio buffer\n", __func__);
	}

	// system buffer:
	offset = ALIGN(offset, EMMC_BLOCK_SIZE);
	rc = ipanic_write_android_buf(offset, 4) ; // system buffer.
	if (rc > 0) {
		hdr->android_system_offset = offset;
		hdr->android_system_length = rc;
		offset += rc;
	}
	else {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: Failed to write android system buffer\n", __func__);
	}
	return offset;
}

static int ipanic_write_log_buf(unsigned int off, int log_copy_start, int log_copy_end)
{
	int saved_oip;
	int rc, rc2;
	unsigned int last_chunk = 0, copy_count = 0;

	while (!last_chunk) {
		saved_oip = oops_in_progress;
		oops_in_progress = 1;
		rc = log_buf_copy2(emmc_bounce, PAGE_SIZE, log_copy_start, log_copy_end);
		BUG_ON(rc < 0);
		log_copy_start += rc;
		copy_count += rc;
		if (rc != PAGE_SIZE)
			last_chunk = rc;

		oops_in_progress = saved_oip;
		if (rc <= 0)
			break;

		rc2 = emmc_ipanic_write(emmc_bounce, off, rc);
		if (rc2 <= 0) {
			xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG,
			       "aee-ipanic: Flash write failed (%d)\n", rc2);
			return rc2;
		}
		off += rc2;
	}
	return copy_count;
}

static int panic_dump_user_info(char *buf, size_t size, unsigned copy_count)
{
	size_t distance = 0;
	size_t realsize = 0;
	
	distance = (strlen(NativeInfo) - copy_count);
	if(distance > size)
		realsize = size;
	else
		realsize = distance;
	memcpy(buf, NativeInfo + copy_count, realsize);
	return realsize;
}

static int ipanic_write_userspace(unsigned int off)
{
	int saved_oip, rc, rc2;
	unsigned int copy_count = 0;

	DumpNativeInfo();

	while (1) {
		saved_oip = oops_in_progress;
		oops_in_progress = 1;
		rc = panic_dump_user_info(emmc_bounce, PAGE_SIZE, copy_count);
		oops_in_progress = saved_oip;

		if (rc <= 0)
			break;

		copy_count += rc;
		rc2 = emmc_ipanic_write(emmc_bounce, off, rc);
		if (rc2 <= 0) {
			xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG,
				    "%s: Flash write failed (%d)\n", __func__, rc2);
			return rc2;
		}
		off += rc2;
	}
	xlog_printk(ANDROID_LOG_DEBUG, IPANIC_LOG_TAG, "%s: count %d, strlen(NativeInfo):%d, off:%d\n", __func__, copy_count, strlen(NativeInfo), off);
	return copy_count;
}

#ifdef CONFIG_LOCAL_WDT
enum wk_wdt_type {
        WK_WDT_LOC_TYPE,
        WK_WDT_EXT_TYPE
};
extern void mtk_wdt_restart(enum wk_wdt_type type);
#else
extern void mtk_wdt_restart(void);
#endif

static void ipanic_kick_wdt(void)
{
#ifdef CONFIG_LOCAL_WDT
    mtk_wdt_restart(WK_WDT_LOC_TYPE);
    mtk_wdt_restart(WK_WDT_EXT_TYPE);
#else
    mtk_wdt_restart();
#endif	
}
static spinlock_t ipanic_lock;
/*Note: for smp safe, any ensuing function call shouldn't enable irq*/
static int emmc_ipanic(struct notifier_block *this, unsigned long event,
		      void *ptr)
{
	int rc;
	struct ipanic_header iheader;

    aee_wdt_dump_info();

    /*In case many core run here concurrently*/
    spin_lock(&ipanic_lock);
	if (in_panic)
		return NOTIFY_DONE;

	in_panic = 1;

#ifdef CONFIG_PREEMPT
	/* Ensure that cond_resched() won't try to preempt anybody */
	add_preempt_count(PREEMPT_ACTIVE);
#endif

	memset(&iheader, 0, sizeof(struct ipanic_header));
	iheader.magic = AEE_IPANIC_MAGIC;
	iheader.version = AEE_IPANIC_PHDR_VERSION;

    ipanic_kick_wdt();
	/*
	 * Write out the console
	 * Section 0 is reserved for ipanic header, we start at section 1
	 */
	iheader.oops_header_offset = ALIGN(sizeof(struct ipanic_header), EMMC_BLOCK_SIZE);
	iheader.oops_header_length = emmc_ipanic_write(&oops_header, iheader.oops_header_offset, sizeof(struct ipanic_oops_header));
	if (iheader.oops_header_length < 0) {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: Error writing oops header to panic log! (%d)\n", __func__,
		       iheader.oops_header_length);
		iheader.oops_header_length = 0;
	}

	iheader.oops_detail_offset = ALIGN(iheader.oops_header_offset + iheader.oops_header_length,
					   EMMC_BLOCK_SIZE);
	iheader.oops_detail_length = ipanic_write_log_buf(iheader.oops_detail_offset, ipanic_detail_start, ipanic_detail_end);
	if (iheader.oops_detail_length < 0) {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: Error writing oops header to panic log! (%d)\n", __func__,
		       iheader.oops_detail_length);
		iheader.oops_detail_length  = 0;
	}

    ipanic_kick_wdt();
	iheader.console_offset = ALIGN(iheader.oops_detail_offset + iheader.oops_detail_length,
				       EMMC_BLOCK_SIZE);
#define __LOG_BUF_LEN	(1 << CONFIG_LOG_BUF_SHIFT)
	if (log_end > __LOG_BUF_LEN)
		iheader.console_length = ipanic_write_log_buf(iheader.console_offset, log_end-__LOG_BUF_LEN, log_end);
	else
		iheader.console_length = ipanic_write_log_buf(iheader.console_offset, 0, log_end);
	if (iheader.console_length < 0) {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "%s: Error writing console to panic log! (%d)\n", __func__,
		       iheader.console_length);
		iheader.console_length = 0;
	}

	iheader.userspace_info_offset = ALIGN(iheader.console_offset + iheader.console_length, EMMC_BLOCK_SIZE);
	iheader.userspace_info_length = ipanic_write_userspace(iheader.userspace_info_offset);
	if (iheader.userspace_info_length < 0) {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG,  "Error writing user space process to panic log (%d)\n", iheader.console_length);
		iheader.userspace_info_length = 0;
	}

	ipanic_write_all_android_buf(iheader.userspace_info_offset + iheader.userspace_info_length, &iheader);

    ipanic_kick_wdt();
	/*
	 * Finally write the ipanic header
	 */
	memset(emmc_bounce, 0, PAGE_SIZE);
	memcpy(emmc_bounce, &iheader, sizeof(struct ipanic_header));

	rc = emmc_ipanic_write(emmc_bounce, 0, ALIGN(sizeof(struct ipanic_header), EMMC_BLOCK_SIZE));
	if (rc <= 0) {
		xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "aee-ipanic: Header write failed (%d)\n", rc);
		goto out;
	}
    ipanic_kick_wdt();

	xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "aee-ipanic: Panic dump sucessfully written to emmc (detail len: %d, console len: %d)\n", 
	iheader.oops_detail_length, iheader.console_length);
	xlog_printk(ANDROID_LOG_ERROR, IPANIC_LOG_TAG, "android log : 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x \n", 
		    iheader.android_main_offset, iheader.android_main_length, 
		    iheader.android_event_offset, iheader.android_event_length, 
		    iheader.android_radio_offset, iheader.android_radio_length, 
		    iheader.android_system_offset, iheader.android_system_length);

out:

#ifdef CONFIG_PREEMPT
	sub_preempt_count(PREEMPT_ACTIVE);
#endif

	in_panic = 0;
	return NOTIFY_DONE;
}

static void emmc_ipanic_oops_free(struct aee_oops *oops, int erase)
{
	if (oops) {
		aee_oops_free(oops);
	}
	if (erase) {
		char *zero = kzalloc(PAGE_SIZE, GFP_KERNEL);
		emmc_ipanic_write(zero, 0, PAGE_SIZE);
		kfree(zero);
	}
}

static struct notifier_block panic_blk = {
	.notifier_call	= emmc_ipanic,
};

static struct ipanic_ops emmc_ipanic_ops = {
	.oops_copy = emmc_ipanic_oops_copy,
	.oops_free = emmc_ipanic_oops_free,
};

int __init aee_emmc_ipanic_init(void)
{
    spin_lock_init(&ipanic_lock);
	atomic_notifier_chain_register(&panic_notifier_list, &panic_blk);
	register_ipanic_ops(&emmc_ipanic_ops);
	emmc_bounce = (u8 *) __get_free_page(GFP_KERNEL);
	xlog_printk(ANDROID_LOG_INFO, IPANIC_LOG_TAG, "aee-emmc-ipanic: startup, partition assgined %s\n",
		       AEE_IPANIC_PLABEL);
	return 0;
}

module_init(aee_emmc_ipanic_init);

#endif

